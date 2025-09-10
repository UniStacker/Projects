import numpy as np
import os
import re
import json

# ====== Hyperparameters ======
EMBED_DIM = 64
HIDDEN_DIM = 128
SEQ_LEN = 10
EPOCHS = 2000
LR = 0.1
LR_DECAY = 0.2
DECAY_COUNT = 40
CLIP_NORM = 5.0


# Read data
with open("data.json", "r") as f:
    data = json.load(f)


VOCAB = set()
for q, a in data:
    VOCAB.update(re.findall(r"\w+|[^\w\s]", q))
    VOCAB.update(re.findall(r"\w+|[^\w\s]", a))
VOCAB = sorted(list(VOCAB) + ["<PAD>"])
VOCAB_SIZE = len(VOCAB)

stoi = {word: i for i, word in enumerate(VOCAB)}
itos = {i: word for word, i in stoi.items()}


def tokenize(text):
    return [stoi[w] for w in re.findall(r"\w+|[^\w\s]", text)]

def detokenize(indices):
    return ' '.join([itos[i] for i in indices if i in itos and itos[i] != "<PAD>"])

def pad_or_truncate(seq, length):
    return seq[:length] + [0] * (length - len(seq))

# ====== Dataset ======
X = [pad_or_truncate(tokenize(q), SEQ_LEN) for q, a in data]
Y = [pad_or_truncate(tokenize(a), SEQ_LEN) for q, a in data]

# ====== Parameters ======
np.random.seed(0)

embed = np.random.randn(VOCAB_SIZE, EMBED_DIM) * 0.01
Wxh = np.random.randn(EMBED_DIM, HIDDEN_DIM) * 0.01
Whh = np.random.randn(HIDDEN_DIM, HIDDEN_DIM) * 0.01
Why = np.random.randn(HIDDEN_DIM, VOCAB_SIZE) * 0.01
bh = np.zeros((1, HIDDEN_DIM))
by = np.zeros((1, VOCAB_SIZE))

# ====== Adam Optimizer Setup ======
params = [embed, Wxh, Whh, Why, bh, by]
m = [np.zeros_like(p) for p in params]
v = [np.zeros_like(p) for p in params]
beta1, beta2 = 0.9, 0.999
eps = 1e-8

# ====== Utility Functions ======
def softmax(x):
    e_x = np.exp(x - np.max(x, axis=-1, keepdims=True))
    return e_x / e_x.sum(axis=-1, keepdims=True)

def one_hot(indices, depth):
    return np.eye(depth)[indices]

def clip_gradients(grads, clip_value):
    total_norm = np.sqrt(sum(np.sum(g ** 2) for g in grads))
    if total_norm > clip_value:
        scale = clip_value / (total_norm + 1e-6)
        return [g * scale for g in grads]
    return grads

def load_model(path="rnn_model.npz"):
    if not os.path.exists(path):
        raise FileNotFoundError(f"Model file {path} not found.")

    params = np.load(path)

    global embed, Wxh, Whh, Why, bh, by
    embed = params["embed"]
    Wxh = params["Wxh"]
    Whh = params["Whh"]
    Why = params["Why"]
    bh = params["bh"]
    by = params["by"]

# load_model()

# ====== Training ======
try:
    for epoch in range(1, EPOCHS + 1):
        total_loss = 0
        accuracy = 0

        if epoch % (EPOCHS // DECAY_COUNT) == 0:
            LR *= 1 - LR_DECAY

        for x_seq, y_seq in zip(X, Y):
            x_embed_seq = embed[x_seq]
            h_seq = np.zeros((SEQ_LEN + 1, HIDDEN_DIM))
            logits_seq = []
            loss = 0

            # Forward
            for t in range(SEQ_LEN):
                x_t = x_embed_seq[t:t+1]
                h_seq[t+1] = np.tanh(x_t @ Wxh + h_seq[t] @ Whh + bh)
                logits = h_seq[t+1] @ Why + by
                logits_seq.append(logits)
                probs = softmax(logits)
                loss -= np.log(probs[0, y_seq[t]] + 1e-9)
            total_loss += loss / SEQ_LEN
            accuracy = 1 - loss / SEQ_LEN

            # Backward
            dWxh = np.zeros_like(Wxh)
            dWhh = np.zeros_like(Whh)
            dWhy = np.zeros_like(Why)
            dbh = np.zeros_like(bh)
            dby = np.zeros_like(by)
            d_embed = np.zeros_like(embed)
            dh_next = np.zeros((1, HIDDEN_DIM))

            for t in reversed(range(SEQ_LEN)):
                probs = softmax(logits_seq[t])
                probs[0, y_seq[t]] -= 1
                # dWhy += h_seq[t+1].T @ probs
                dWhy += h_seq[t+1][None, :].T @ probs
                dby += probs
                dh = probs @ Why.T + dh_next
                dh_raw = (1 - h_seq[t+1] ** 2) * dh
                dbh += dh_raw
                dWxh += x_embed_seq[t:t+1].T @ dh_raw
                # dWhh += h_seq[t].T @ dh_raw
                dWhh += h_seq[t][None, :].T @ dh_raw
                dh_next = dh_raw @ Whh.T
                d_embed[x_seq[t]] += (dh_raw @ Wxh.T).flatten()

            grads = [d_embed, dWxh, dWhh, dWhy, dbh, dby]
            grads = clip_gradients(grads, CLIP_NORM)

            # Adam update
            for i, (p, g) in enumerate(zip(params, grads)):
                m[i] = beta1 * m[i] + (1 - beta1) * g
                v[i] = beta2 * v[i] + (1 - beta2) * (g ** 2)
                m_hat = m[i] / (1 - beta1 ** epoch)
                v_hat = v[i] / (1 - beta2 ** epoch)
                p -= LR * m_hat / (np.sqrt(v_hat) + eps)

        if epoch % 1 == 0:
            print(f"\r\033[KEpoch {epoch}, Total Loss: {total_loss:.4f}, Accuracy: {accuracy * 100:.2f}%", end='')

        if accuracy >= 0.90:
            print()
            print(f"Maximum accuracy reached, training complete.")
except KeyboardInterrupt:
    print()
    print("Training stopped.")

# ====== Save Model ======
np.savez("rnn_model.npz", embed=embed, Wxh=Wxh, Whh=Whh, Why=Why, bh=bh, by=by)

# ====== Inference ======
def answer_question(question):
    if os.path.exists("rnn_model.npz"):
        params = np.load("rnn_model.npz")
        embed[:] = params["embed"]
        Wxh[:] = params["Wxh"]
        Whh[:] = params["Whh"]
        Why[:] = params["Why"]
        bh[:] = params["bh"]
        by[:] = params["by"]

    x_seq = pad_or_truncate(tokenize(question), SEQ_LEN)
    x_embed_seq = embed[x_seq]
    h = np.zeros((1, HIDDEN_DIM))
    answer = []

    for t in range(SEQ_LEN):
        x_t = x_embed_seq[t:t+1]
        h = np.tanh(x_t @ Wxh + h @ Whh + bh)
        logits = h @ Why + by
        probs = softmax(logits)
        pred = np.argmax(probs)
        answer.append(pred)

    return detokenize(answer)

# ====== Test ======
print("\n--- Test Output ---")
for q, _ in data:
    print(f"Q: {q}")
    print(f"A: {answer_question(q)}\n")
