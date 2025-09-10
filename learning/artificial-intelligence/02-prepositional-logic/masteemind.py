from logic import *

def cprint(out, *stylings):
    stylings = list(stylings)
    if len(stylings) == 1 and ' ' in stylings[0]:
        stylings = stylings[0].split(' ')

    reset = "\033[0m"
    colors_and_styles = {
        "red": "\033[31m",
        "green": "\033[32m",
        "yellow": "\033[33m",
        "bold": "\033[1m",
        "italic": "\033[2m",
        "underline": "\033[3m",
        "invert": "\033[7m"
    }

    def get_styling(key):
        styling = colors_and_styles.get(key.lower(), None)
        if styling: return styling
        raise Exception(f"Invalid styling: {key}")

    print(''.join([get_styling(k) for k in stylings]) + out + reset)

colors = ["Red", "Blue", "Green", "Yellow"]

symbols = []
knowledge = And()

# Adding symbols
for pos in range(1, len(colors) + 1):
    for color in colors:
        symbols.append(Symbol(f"{color}{pos}"))

# Eaxh color is at a position
for color in colors:
    knowledge.add(
        Or(
            *[Symbol(f"{color}{i}") for i in range(1, len(colors) + 1)]
        )
    )

# One color holds a single position
for color in colors:
    for p1 in range(1, len(colors) + 1):
        no_other_pos = And()
        for p2 in range(1, len(colors) + 1):
            if p1 != p2:
                no_other_pos.add(Not(Symbol(f"{color}{p2}")))
        knowledge.add(Implies(Symbol(f"{color}{p1}"), no_other_pos))

# Every color holds a different position
for pos in range(1, len(colors) + 1):
    for c1 in colors:
        no_other_color = And()
        for c2 in colors:
            if c1 != c2:
                no_other_color.add(Not(Symbol(f"{c2}{pos}")))
        knowledge.add(Implies(Symbol(f"{c1}{pos}"), no_other_color))


def check_knowledge(knowledge):
    for symbol in symbols:
        if model_check(knowledge, symbol):
            cprint(f"{symbol.name}: Yes", "Green", "Bold")
        elif not model_check(knowledge, Not(symbol)):
            cprint(f"{symbol.name}: Maybe", "Bold Yellow")


#print(knowledge.formula())
if __name__ == "__main__":
    check_knowledge(knowledge)
