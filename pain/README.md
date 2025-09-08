# PAIN – Purely Abusive Instruction-less Notation

> Brainfuck was foreplay.  
> Malbolge was the trailer.  
> PAIN is the main feature.  

This is not a language. It’s a philosophical weapon disguised as one.  
Programs don’t run in PAIN. They survive. Maybe.  

---

## What is PAIN?

PAIN is a self-mutating, instruction-less esolang where:

- **Program = memory = tape = instruction set**
- Every pair of bytes `(C1, C2)` is read sequentially
- Mutation is inevitable. Output is a side effect
- Forward progress is an illusion

The program consumes itself as it runs, leaving behind a scarred, unrecognizable tape. Every run is unique unless you fight it into submission.

---

## Specification

### Memory Model
- The program text **is the memory**.
- Execution reads pairs `(C1, C2)` from left to right.
- A pointer `head` tracks the current pair.
- **Wrap-around applies**: jumping past program boundaries loops back.

---

### Rules

#### 1. SAME Pair (C1 == C2)

| Case                          | Effect                                          |
|-------------------------------|-------------------------------------------------|
| Printable (ASCII 32–126, 10)  | Print `C1`                                      |
| NULL (ASCII 0)                | Read **one char user input**, insert after `C2` |
| Non-printable & Non-NULL      | Move pointer backward by `C1` (wraps around)    |

```text
head = (head - C1) % program_length
```
---

#### 2. DIFFERENCE Pair (C1 != C2)

Mutate `C2` with:
```
diff = C2 - C1 + 1
new_C2 = (C2 + diff) % 256
```
Or expanded:
```
new_C2 = (2*C2 - C1 + 1) % 256
```
This mutation:

- Flips parity (odd ↔ even)

- Guarantees any ASCII is reachable

- Infects future execution with chaos

---

#### 3. Termination

Execution ends if:

- The pointer reaches the end of the program

- The pointer jumps to its current position

- The program cannibalizes itself into nothingness

---

## Special Notes

- Each byte participates in exactly two operations (as C1 and C2)

- Programs are one dense line unless \n is desired

- Bytes are single-byte values, wrapping around % 256

---

## Example: Hello, World!\n

Program:
```PAIN
HHVehllmoM,-' ;WXdoprsplmidB!\"0#\n
```
Output:
```
Hello, World!
```

---

## Philosophy

PAIN embodies:

> “Forward is an illusion. Mutation is inevitable. The program consumes itself.”



You don’t debug PAIN. You don’t control PAIN. You negotiate with it, byte by byte, while it rearranges its own body.


---

## PAIN isn’t Malbolge  

Yeah, you’ll see the resemblance. Self-modifying tape. Programs that feel alive. Absolute hostility. But PAIN isn’t Malbolge. It’s worse in its own way.  

| Feature                   | Malbolge                           | PAIN                              |
|---------------------------|------------------------------------|-----------------------------------|
| **Instruction set**       | 8 bizarre trit-encoded operations  | None. No opcodes. Just raw data   |
| **Mutation granularity**  | Single byte at a time              | Pairs of bytes (C1 mutates C2)    |
| **Mutation logic**        | “Crazy operation” encryption       | Simple but brutal math formula    |
| **User interaction**      | Minimal input support              | Code grows via NULL insertions    |
| **Idempotency**           | Non-idempotent but patchable       | Non-idempotent by design          |
| **Philosophy**            | Obfuscation and encryption hell    | Self-cannibalizing entropy        |
| **Approachability**       | Basically impossible               | Barely survivable                 |

Where Malbolge hides behind complexity, PAIN just stabs you in the face.  

Malbolge is a puzzle box.  
PAIN is a straight razor.  

The only reason PAIN hasn’t killed you yet is because you haven’t run it.

---

## Running PAIN

### Requirements
```
Python 3
```
Or implement it yourself. The spec is small enough to burn into your brain.


### Example
```
python pain.py [File]
```
OR
```
python pain.py [Code]
```

---

## Contributing

Want to write PAIN programs? Good luck.

Start small. “Hello” will break you. “Quine” will destroy you. Anything larger is asking for pain.

Pull requests welcome, but they’re probably doomed too.


---

## License

WTFPL – Do whatever the hell you want. PAIN doesn’t care. Neither do I.
