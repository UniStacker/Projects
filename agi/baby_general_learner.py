"""
Baby General Learner (BGL)
--------------------------
A tiny, dependency-free prototype that:
1) Recognizes patterns via incremental TF-IDF embeddings
2) Associates patterns via a term-term cooccurrence graph with PMI
3) Stores patterns in an append-only memory (JSONL) and an index

This is NOT state-of-the-art. It's a hackable scaffold you can extend:
- Swap in better encoders (character/word/subword models)
- Add a vector DB for faster retrieval (FAISS/Annoy if you want deps)
- Add a "meta-learner" that tunes token weights based on retrieval success
- Add replay buffers to avoid catastrophic forgetting
"""

import json, math, os, re, uuid, time
from collections import defaultdict, Counter
from typing import List, Dict, Tuple, Iterable

TOKEN_RE = re.compile(r"[A-Za-z0-9_]+")

def tokenize(text: str) -> List[str]:
    # lowercase, simple alnum/underscore tokens
    return [t.lower() for t in TOKEN_RE.findall(text)]

class SparseVec(dict):
    """Simple sparse vector with cosine similarity and basic operations."""
    def dot(self, other:"SparseVec")->float:
        if len(self) > len(other):
            self, other = other, self
        return sum(v*other.get(k,0.0) for k,v in self.items())

    def norm(self)->float:
        return math.sqrt(sum(v*v for v in self.values()))

    def cosine(self, other:"SparseVec")->float:
        a = self.norm()
        b = other.norm()
        if a==0 or b==0:
            return 0.0
        return self.dot(other)/(a*b)

    def add(self, other:"SparseVec") -> "SparseVec":
        res = SparseVec(self)
        for k,v in other.items():
            res[k] = res.get(k, 0.0) + v
        return res

    def sub(self, other:"SparseVec") -> "SparseVec":
        res = SparseVec(self)
        for k,v in other.items():
            res[k] = res.get(k, 0.0) - v
        return res

    def normalize(self) -> "SparseVec":
        res = SparseVec()
        n = self.norm()
        if n > 0:
            for k,v in self.items():
                res[k] = v/n
        return res

class BabyGeneralLearner:
    def __init__(self, store_dir="bgl_store", scaffold:str=None):
        self.store_dir = store_dir
        os.makedirs(store_dir, exist_ok=True)
        # --- Scaffolds ---
        self.scaffold = scaffold
        if self.scaffold == "text_classifier":
            self.labels = defaultdict(set) # label -> {doc_id, ...}
            self.doc_to_label = {}         # doc_id -> label
        elif self.scaffold == "qa":
            self.questions = {} # doc_id -> question text
            self.answers = {}   # doc_id -> answer text

        # Persistent files
        self.meta_path = os.path.join(store_dir, "meta.json")
        self.memory_path = os.path.join(store_dir, "memory.jsonl")
        self.index_path = os.path.join(store_dir, "index.json")
        # State
        self.N_docs = 0
        self.df = Counter()              # document frequency per token
        self.co = Counter()              # cooccurrence counts for token pairs (unordered tuple sorted)
        self.token_count = Counter()     # total token counts (for PMI)
        self.doc_entries = {}            # id -> {"text":..., "tags":[...]} (lightweight index)
        # Load if present
        self._load()

    # ---------- Persistence ----------
    def _load(self):
        if os.path.exists(self.meta_path):
            with open(self.meta_path, "r", encoding="utf-8") as f:
                meta = json.load(f)
                self.N_docs = meta.get("N_docs", 0)
                self.df = Counter(meta.get("df", {}))
                self.co = Counter({tuple(k.split("\t")): v for k,v in meta.get("co", {}).items()})
                self.token_count = Counter(meta.get("token_count", {}))
        if os.path.exists(self.index_path):
            with open(self.index_path, "r", encoding="utf-8") as f:
                self.doc_entries = json.load(f)
        # --- Scaffolds ---
        if self.scaffold == "text_classifier":
            self.labels = defaultdict(set)
            self.doc_to_label = {}
            for doc_id, entry in self.doc_entries.items():
                for tag in entry.get("tags",[]):
                    if tag.startswith("label:"):
                        label = tag.replace("label:","")
                        self.labels[label].add(doc_id)
                        self.doc_to_label[doc_id] = label
        elif self.scaffold == "qa":
            self.questions = {k:v['text'] for k,v in self.doc_entries.items() if v.get('tags',[]) and 'question' in v['tags']}
            self.answers = {k:v['text'] for k,v in self.doc_entries.items() if v.get('tags',[]) and 'answer' in v['tags']}

    def clear(self):
        """Reset the learner's state."""
        self.N_docs = 0
        self.df = Counter()
        self.co = Counter()
        self.token_count = Counter()
        self.doc_entries = {}
        if os.path.exists(self.meta_path): os.remove(self.meta_path)
        if os.path.exists(self.index_path): os.remove(self.index_path)
        if os.path.exists(self.memory_path): os.remove(self.memory_path)
        # --- Scaffolds ---
        if self.scaffold == "text_classifier":
            self.labels = defaultdict(set)
            self.doc_to_label = {}
        elif self.scaffold == "qa":
            self.questions = {}
            self.answers = {}
        print("Cleared BGL store.")

    def _save(self):
        meta = {
            "N_docs": self.N_docs,
            "df": self.df,
            "co": {"\t".join(k): v for k,v in self.co.items()},
            "token_count": self.token_count,
        }
        with open(self.meta_path, "w", encoding="utf-8") as f:
            json.dump(meta, f)
        with open(self.index_path, "w", encoding="utf-8") as f:
            json.dump(self.doc_entries, f)

    # ---------- Core: Embedding ----------
    def _tfidf(self, tokens: List[str]) -> SparseVec:
        # document-level tf
        tf = Counter(tokens)
        vec = SparseVec()
        for tok, f in tf.items():
            # idf with +1 smoothing
            idf = math.log((1 + self.N_docs) / (1 + self.df.get(tok, 0))) + 1.0
            vec[tok] = (f / len(tokens)) * idf
        return vec

    # ---------- Core: Cooccurrence & PMI ----------
    def _update_cooccurrence(self, tokens: List[str], window:int=5):
        # sliding window cooccurrence
        for i in range(len(tokens)):
            self.token_count[tokens[i]] += 1
            for j in range(i+1, min(i+1+window, len(tokens))):
                a, b = sorted((tokens[i], tokens[j]))
                self.co[(a,b)] += 1

    def pmi(self, a:str, b:str) -> float:
        # Pointwise Mutual Information (positive PMI)
        pair = tuple(sorted((a,b)))
        co = self.co.get(pair, 0)
        if co == 0: return 0.0
        pa = self.token_count[a] / max(1, sum(self.token_count.values()))
        pb = self.token_count[b] / max(1, sum(self.token_count.values()))
        pab = co / max(1, sum(self.co.values()))
        val = math.log2(pab / (pa*pb + 1e-12) + 1e-12)
        return max(0.0, val)

    def top_associations(self, term:str, k:int=10) -> List[Tuple[str,float]]:
        seen = set()
        scores = []
        for (a,b), c in self.co.items():
            if a == term and b not in seen:
                seen.add(b)
                scores.append((b, self.pmi(a,b)))
            elif b == term and a not in seen:
                seen.add(a)
                scores.append((a, self.pmi(a,b)))
        scores.sort(key=lambda x: x[1], reverse=True)
        return scores[:k]

    # ---------- Public API ----------
    def add_docs(self, texts: Iterable[str], tags: Iterable[List[str]] = None) -> List[str]:
        if tags is None:
            tags = [[] for _ in texts]
        ids = []
        for text, tgs in zip(texts, tags):
            doc_id = str(uuid.uuid4())
            tokens = tokenize(text)
            # update corpus stats
            self.N_docs += 1
            for tok in set(tokens):
                self.df[tok] += 1
            self._update_cooccurrence(tokens)
            # store episodic memory
            with open(self.memory_path, "a", encoding="utf-8") as f:
                f.write(json.dumps({"id": doc_id, "text": text, "tags": tgs, "ts": time.time()}) + "\n")
            # lightweight index (text + tags only; embeddings are computed on the fly with current idf)
            self.doc_entries[doc_id] = {"text": text, "tags": tgs}
            ids.append(doc_id)
        self._save()
        return ids

    def retrieve(self, query:str, topk:int=5) -> List[Tuple[str, float, Dict]]:
        q_tokens = tokenize(query)
        q_vec = self._tfidf(q_tokens)
        # brute-force cosine across all docs (fine for small demos; replace with ANN for scale)
        scored = []
        for doc_id, rec in self.doc_entries.items():
            d_vec = self._tfidf(tokenize(rec["text"]))
            sim = q_vec.cosine(d_vec)
            if sim > 0:
                scored.append((doc_id, sim, rec))
        scored.sort(key=lambda x: x[1], reverse=True)
        return scored[:topk]

    def get_doc(self, doc_id:str) -> Dict:
        return self.doc_entries.get(doc_id)

    def get_all_docs(self) -> Dict:
        return self.doc_entries

    def _get_doc_embedding(self, doc_id:str) -> SparseVec:
        doc = self.get_doc(doc_id)
        if not doc: return SparseVec()
        return self._tfidf(tokenize(doc['text']))

    def concept_embed(self, texts: Iterable[str]) -> SparseVec:
        # average tf-idf vector of given texts
        acc = defaultdict(float)
        n = 0
        for tx in texts:
            v = self._tfidf(tokenize(tx))
            for k, val in v.items():
                acc[k] += val
            n += 1
        if n == 0:
            return SparseVec()
        for k in list(acc.keys()):
            acc[k] /= n
        return SparseVec(acc)

    def explain(self, query:str, topk:int=10) -> Dict:
        """Return related terms via PMI and top docs via cosine for a query."""
        q_tokens = tokenize(query)
        # collect PMI neighbors for each token
        assoc = defaultdict(float)
        for tok in set(q_tokens):
            for nb, score in self.top_associations(tok, k=topk):
                assoc[nb] = max(assoc[nb], score)
        # top docs
        docs = self.retrieve(query, topk=topk)
        return {
            "query_tokens": q_tokens,
            "associations": sorted(assoc.items(), key=lambda x: x[1], reverse=True)[:topk],
            "docs": [{"id": did, "score": float(f"{s:.4f}"), "text": rec["text"], "tags": rec["tags"]} for did,s,rec in docs]
        }

    # ---------- Scaffolds ----------
    def train(self, doc_id:str, label:str):
        if self.scaffold != "text_classifier":
            raise Exception("Must init with scaffold='text_classifier'")
        self.labels[label].add(doc_id)
        self.doc_to_label[doc_id] = label
        # also add a tag for persistence
        if "label:"+label not in self.doc_entries[doc_id]['tags']:
            self.doc_entries[doc_id]['tags'].append("label:"+label)
        self._save()

    def predict(self, text:str, topk:int=1):
        if self.scaffold != "text_classifier":
            raise Exception("Must init with scaffold='text_classifier'")
        
        q_vec = self._tfidf(tokenize(text))
        
        # K-Nearest-Neighbors search (similarity-weighted)
        scored_labels = Counter()
        
        neighbors = []
        for doc_id, label in self.doc_to_label.items():
            d_vec = self._get_doc_embedding(doc_id)
            sim = q_vec.cosine(d_vec)
            if sim > 0:
                neighbors.append((sim, label))
        
        # No neighbors found
        if not neighbors:
            return []

        # Simple unweighted k-NN (or weighted if you prefer)
        # Here, we'll do a simple sum of similarities for each label
        for sim, label in neighbors:
            scored_labels[label] += sim
        
        return scored_labels.most_common(topk)

    def evaluate(self, test_data:List[Tuple[str,str]]):
        if self.scaffold != "text_classifier":
            raise Exception("Must init with scaffold='text_classifier'")
        
        correct = 0
        for text, true_label in test_data:
            preds = self.predict(text, topk=1)
            if preds and preds[0][0] == true_label:
                correct += 1
        return correct / len(test_data) if test_data else 0.0

    def add_qa(self, question:str, answer:str):
        if self.scaffold != "qa":
            raise Exception("Must init with scaffold='qa'")
        
        q_id = self.add_docs([question], tags=[['question']])[0]
        a_id = self.add_docs([answer], tags=[['answer']])[0]
        
        # Update live scaffold state
        self.questions[q_id] = question
        self.answers[a_id] = answer

        # Link them
        self.doc_entries[q_id]['tags'].append("answer_id:"+a_id)
        self.doc_entries[a_id]['tags'].append("question_id:"+q_id)
        self._save()
        return q_id, a_id

    def answer(self, query:str, topk:int=1):
        if self.scaffold != "qa":
            raise Exception("Must init with scaffold='qa'")
        
        # Find most similar questions
        q_tokens = tokenize(query)
        q_vec = self._tfidf(q_tokens)
        
        scored_questions = []
        for doc_id, text in self.questions.items():
            d_vec = self._get_doc_embedding(doc_id)
            sim = q_vec.cosine(d_vec)
            if sim > 0:
                scored_questions.append((doc_id, sim))
        
        scored_questions.sort(key=lambda x: x[1], reverse=True)
        
        # Retrieve answers
        results = []
        for q_id, score in scored_questions[:topk]:
            for tag in self.doc_entries[q_id].get('tags',[]):
                if tag.startswith("answer_id:"):
                    ans_id = tag.replace("answer_id:","")
                    ans_text = self.get_doc(ans_id).get('text')
                    results.append({"question": self.doc_entries[q_id]['text'], "answer": ans_text, "score": score})
        return results

# ------------------ Demo ------------------
if __name__ == "__main__":
    # --- Default Demo ---
    print("="*20, "Default BGL Demo", "="*20)
    bgl = BabyGeneralLearner(store_dir="bgl_demo_store")
    bgl.clear()
    # Seed with a tiny corpus
    docs = [
        "Apples are sweet red fruits. They grow on trees in orchards.",
        "Bananas are yellow and rich in potassium. Monkeys love bananas.",
        "Tomatoes are red but botanically fruits often used as vegetables.",
        "Neural networks learn patterns from data using many layers of weights.",
        "Transformers use attention to capture long-range dependencies in sequences.",
        "Reinforcement learning agents learn through rewards and exploration.",
        "Cats are playful animals that love chasing red laser dots.",
        "Dogs are loyal animals that love fetching balls in parks.",
    ]
    tags = [
        ["food","fruit"], ["food","fruit"], ["food","botany"],
        ["ai","ml"], ["ai","nlp"], ["ai","rl"],
        ["animals","pets"], ["animals","pets"]
    ]
    if bgl.N_docs == 0:
        bgl.add_docs(docs, tags)

    queries = [
        "red fruit",
        "how do transformers learn sequences",
        "pets that fetch",
        "botanical fruit used as vegetable",
    ]
    for q in queries:
        print("\n" + "-"*80)
        print("QUERY:", q)
        out = bgl.explain(q, topk=5)
        print("Tokens:", out["query_tokens"])
        print("Top associations:", out["associations"])
        print("Top docs:")
        for d in out["docs"]:
            print(f"  {d['score']} | {d['text']} | tags={d['tags']}")

    # --- Text Classifier Demo ---
    print("\n\n" + "="*20, "Text Classifier Demo", "="*20)
    bgl_class = BabyGeneralLearner(store_dir="bgl_class_demo", scaffold="text_classifier")
    bgl_class.clear()
    
    train_data = [
        ("I love this movie, it's fantastic!", "positive"),
        ("What a waste of time, terrible plot.", "negative"),
        ("The acting was superb and the story was gripping.", "positive"),
        ("I would not recommend this to anyone.", "negative"),
        ("A truly heartwarming and beautiful film.", "positive"),
        ("Simply awful, I want my money back.", "negative"),
    ]
    test_data = [
        ("The visuals were stunning, a real masterpiece.", "positive"),
        ("I was so bored I fell asleep.", "negative"),
    ]

    print("Training classifier...")
    for text, label in train_data:
        doc_id = bgl_class.add_docs([text])[0]
        bgl_class.train(doc_id, label)

    print("\nTesting classifier...")
    for text, label in test_data:
        preds = bgl_class.predict(text, topk=1)
        print(f"Text: '{text}'\n  -> Pred: {preds[0][0]} (True: {label})")

    accuracy = bgl_class.evaluate(test_data)
    print(f"\nAccuracy: {accuracy:.2f}")


    # --- QA Demo ---
    print("\n\n" + "="*20, "Q&A Demo", "="*20)
    bgl_qa = BabyGeneralLearner(store_dir="bgl_qa_demo", scaffold="qa")
    bgl_qa.clear()

    print("Adding Q&A pairs...")
    bgl_qa.add_qa("What is the capital of France?", "Paris is the capital of France.")
    bgl_qa.add_qa("What is the main component of air?", "Nitrogen is the main component of the air we breathe.")
    bgl_qa.add_qa("Who wrote 'Hamlet'?", "William Shakespeare wrote 'Hamlet'.")

    qa_queries = [
        "capital of france",
        "what is in the air",
        "author of hamlet",
    ]
    for q in qa_queries:
        print(f"\nQuery: '{q}'")
        answers = bgl_qa.answer(q)
        for ans in answers:
            print(f"  -> Answer: {ans['answer']} (Score: {ans['score']:.4f})")
