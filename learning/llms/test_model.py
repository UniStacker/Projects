# Mini QnA LLM using NumPy (teacher forcing + full output prediction)
# Slightly smarter version: predicts each token in the answer sequence

import numpy as np

# ====== Hyperparameters ======
EMBED_DIM = 32
SEQ_LEN = 16
VOCAB = list("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789?.,:;'-_ ")
VOCAB_SIZE = len(VOCAB)
EPOCHS = 100000
LR = 0.2
LR_DECAY = 0.2
DECAY_TIMES = 30

# ====== Tokenizer ======
stoi = {ch: i for i, ch in enumerate(VOCAB)}
itos = {i: ch for ch, i in stoi.items()}

def tokenize(text):
    return [stoi[c] for c in text if c in stoi]

def detokenize(indices):
    return ''.join([itos[i] for i in indices])

# ====== Dataset ======
data = [
    ("What is your name?", "My name is Aeon."),
    ("How are you?", "I am fine."),
    ("What is 2 + 2?", "4"),
    ("Who created you?", "Shan."),
]

def pad_or_truncate(seq, length):
    return seq[:length] + [0]*(length - len(seq))

X = np.stack([pad_or_truncate(tokenize(q), SEQ_LEN) for q, a in data])
Y = np.stack([pad_or_truncate(tokenize(a), SEQ_LEN) for q, a in data])

# ====== Model Parameters ======
embed = np.random.randn(VOCAB_SIZE, EMBED_DIM) * 0.01
linear1 = np.random.randn(EMBED_DIM, EMBED_DIM) * 0.01
linear2 = np.random.randn(EMBED_DIM, VOCAB_SIZE) * 0.01

# ====== Utility Functions ======
def softmax(x):
    e_x = np.exp(x - np.max(x, axis=-1, keepdims=True))
    return e_x / e_x.sum(axis=-1, keepdims=True)

def one_hot(indices, depth):
    return np.eye(depth)[indices]

# ====== Training Loop ======
for epoch in range(EPOCHS): 
    total_loss = 0
    accuracy = 0
    if epoch % (EPOCHS // DECAY_TIMES) == 0:
        LR *= 1 - LR_DECAY
    for i in range(len(X)):
        x = X[i]                 # (seq_len,)
        y = Y[i]                 # (seq_len,)

        # Embed input sequence
        x_embed = embed[x]      # (seq_len, embed_dim)
        h = np.maximum(0, x_embed @ linear1) # (seq_len, embed_dim)
        logits = h @ linear2    # (seq_len, vocab_size)
        probs = softmax(logits) # (seq_len, vocab_size)

        # Cross-entropy loss over all time steps
        y_onehot = one_hot(y, VOCAB_SIZE)# (seq_len, vocab_size)
        loss = -np.sum(y_onehot * np.log(probs + 1e-9)) / SEQ_LEN
        total_loss += loss
        accuracy = 1 - loss

        # Backprop manually
        dlogits = (probs - y_onehot) / SEQ_LEN# (seq_len, vocab_size)
        dlinear2 = h.T @ dlogits    # (embed_dim, vocab_size)
        dh = dlogits @ linear2.T    # (seq_len, embed_dim)
        dh[h <= 0] = 0              # ReLU backprop
        dlinear1 = x_embed.T @ dh   # (embed_dim, embed_dim)

        # Update
        linear2 -= LR * dlinear2
        linear1 -= LR * dlinear1
        for t in range(SEQ_LEN):
            embed[x[t]] -= LR * dh[t] @ linear1.T

    if epoch % 10 == 0:
        print(f"\r\033[KEpoch {epoch}, Total Loss: {total_loss:.4f} Accuracy: {accuracy*100:.2f}%", end='')

    if total_loss < 0.0001:
        print()
        print(f"Max accuracy reached, training complete.")
        break

print()

# ====== Inference ======
def answer_question(question):
    x = pad_or_truncate(tokenize(question), SEQ_LEN)
    x_embed = embed[x]
    h = np.maximum(0, x_embed @ linear1)
    logits = h @ linear2
    probs = softmax(logits)
    preds = np.argmax(probs, axis=-1)
    return detokenize(preds)

# ====== Test ======
print("--- Test Output ---")
test_qs = [
    "What is your name?",
    "Who created you?",
    "What is 2 + 2?",
    "How are you?"
]

for q in test_qs:
    print(f"Q: {q}")
    print(f"A: {answer_question(q)}\n")


