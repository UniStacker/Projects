#!/bin/python
import sys, os, json, requests, re
from time import sleep
from datetime import datetime

GROQ_API_KEY = os.environ.get("GROQ_API_KEY")
GEMINI_API_KEY = os.environ.get("GEMINI_API_KEY")
GROQ_MODEL = "llama3-70b-8192"
GROQ_URL = "https://api.groq.com/openai/v1/chat/completions"
GEMINI_URL = f"https://generativelanguage.googleapis.com/v1/models/gemini-1.5-flash:generateContent?key={GEMINI_API_KEY}"
HISTORY_FILE = "chat_history.json"
MEMORY_FILE = "chat_memory.json"
USER = os.environ.get("USER") or "User"
MAX_HISTORY = 20
SAVE_CHAT_HISTORY = False
if "-c" in sys.argv or "--chat" in sys.argv:
    SAVE_CHAT_HISTORY = True

# ANSI Colors
RESET = "\033[0m"
BOLD = "\033[1m"
ITALIC = "\033[3m"
USER_COLOR = "\033[94m"
GROQ_COLOR = "\033[92m"
GEMINI_COLOR = "\033[96m"
MEM_COLOR = "\033[93m"

def println(line, speed=.01):
    for c in line:
        print(c, end='', flush=True)
        sleep(speed)
    print()

def now():
    return datetime.now().strftime("[%Y-%m-%d %H:%M:%S]")

def load_history():
    if os.path.exists(HISTORY_FILE):
        with open(HISTORY_FILE, 'r') as f:
            return json.load(f)
    return []

def save_history(history):
    with open(HISTORY_FILE, 'w') as f:
        json.dump(history, f, indent=2)

def trimmed(history):
    return history[-MAX_HISTORY:]

def load_memory():
    if os.path.exists(MEMORY_FILE):
        with open(MEMORY_FILE, 'r') as f:
            return json.load(f)
    return []

def save_memory(mem):
    with open(MEMORY_FILE, 'w') as f:
        json.dump(mem, f, indent=2)

def apply_memory_injection(history, memory):
    mem_text = "\n".join([f"- {m}" for m in memory])
    if mem_text:
        history.insert(0, {"role": "assistant", "content": f"Current memory bank:\n{mem_text}"})
    return history

def format_markdown_to_ansi(text):
    text = re.sub(r"\*\*(.*?)\*\*", f"{BOLD}\\1{RESET}", text)
    text = re.sub(r"\*(.*?)\*", f"{ITALIC}\\1{RESET}", text)
    return text

def display(name, role, message):
    color = {
        "user": USER_COLOR,
        "assistant": GROQ_COLOR if name == "Groq" else GEMINI_COLOR,
        "mem": MEM_COLOR
    }.get(role, RESET)
    prefix = f"{color}{BOLD}{name}{RESET}: "
    println(prefix + format_markdown_to_ansi(message))

def check_for_memory_ops(text, memory):
    mem_msgs = []
    added = re.findall(r"\$\{MEM.add\(\"(.*?)\"\)\}", text)
    removed = re.findall(r"\$\{MEM.remove\(\"(.*?)\"\)\}", text)
    for item in added:
        if item not in memory:
            memory.append(item)
            mem_msgs.append(f"\"{item}\" Saved.")
    for item in removed:
        if item in memory:
            memory.remove(item)
            mem_msgs.append(f"\"{item}\" Removed.")
    return memory, mem_msgs

def prepare_gemini_payload(history):
    trimmed = history[-MAX_HISTORY:]
    contents = []
    for msg in trimmed:
        role = "model" if msg["role"] == "assistant" else msg["role"]
        contents.append({
            "role": role,
            "parts": [{"text": msg["content"]}]
        })
    return {"contents": contents}

# Load chat history and memory
history = load_history() if SAVE_CHAT_HISTORY else []
memory = load_memory() if SAVE_CHAT_HISTORY else []

# Main loop
while True:
    prompt = input(f"{USER_COLOR}{USER}:{RESET} ")
    if prompt.strip().lower() == "exit":
        break

    history.append({"role": "user", "content": prompt})

    # Inject memory into history copy
    groq_history = apply_memory_injection(trimmed(history.copy()), memory.copy())
    data = {
        "model": GROQ_MODEL,
        "temperature": 0.7,
        "messages": groq_history
    }

    headers = {
        "Authorization": f"Bearer {GROQ_API_KEY}",
        "Content-Type": "application/json"
    }

    try:
        r = requests.post(GROQ_URL, headers=headers, json=data, timeout=15)
        r.raise_for_status()
        result = r.json()["choices"][0]["message"]["content"]
        timestamped_result = f"[{now()}] {result}"
        history.append({"role": "assistant", "content": timestamped_result})
        memory, mem_msgs = check_for_memory_ops(result, memory)
        display("Groq", "assistant", result)
        for msg in mem_msgs:
            display("MEM", "mem", msg)
    except:
        gdata = prepare_gemini_payload(apply_memory_injection(history.copy(), memory.copy()))
        try:
            r = requests.post(GEMINI_URL, headers={"Content-Type": "application/json"}, json=gdata, timeout=15)
            r.raise_for_status()
            result = r.json()["candidates"][0]["content"]["parts"][0]["text"]
            timestamped_result = f"{now()} {result}"
            history.append({"role": "assistant", "content": timestamped_result})
            memory, mem_msgs = check_for_memory_ops(result, memory)
            display("Gemini", "assistant", result)
            for msg in mem_msgs:
                display("MEM", "mem", msg)
        except Exception as e:
            println("[!] Both Groq and Gemini failed. Check connection or keys.")
            break

    if SAVE_CHAT_HISTORY:
        save_history(history)
        save_memory(memory)

