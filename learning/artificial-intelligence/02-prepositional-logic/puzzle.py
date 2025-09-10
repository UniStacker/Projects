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

people = ["Gilderoy", "Pomona", "Minerva", "Horace"]
houses = ["Gryffindor", "Hufflepuff", "Ravenclaw", "Slytherine"]

symbols = []
knowledge = And()

# Adding symbols
for person in people:
    for house in houses:
        symbols.append(Symbol(f"{person}{house}"))

# Each person belongs to a house
for person in people:
    knowledge.add(Or(*[Symbol(f"{person}{house}") for house in houses]))

# A person belongs to only one house
for person in people:
    for h1 in houses:
        no_other_house = And()
        for h2 in houses:
            if h1 != h2:
                no_other_house.add(Not(Symbol(f"{person}{h2}")))
        knowledge.add(Implies(Symbol(person + h1), no_other_house))

# Everyone is belong to different houses
for house in houses:
    for p1 in people:
        no_other_person = And()
        for p2 in people:
            if p1 != p2:
                no_other_person.add(Not(Symbol(f"{p2}{house}")))
        knowledge.add(Implies(Symbol(p1 + house), no_other_person))


def check_knowledge(knowledge):
    for symbol in symbols:
        if model_check(knowledge, symbol):
            cprint(f"{symbol.name}: Yes", "Green", "Bold")
        elif not model_check(knowledge, Not(symbol)):
            cprint(f"{symbol.name}: Maybe", "Bold Yellow")


#print(knowledge.formula())
if __name__ == "__main__":
    # Gilderoy is either not in Gryffindor or Ravenclaw or both
    knowledge.add(Or(
        Symbol("GilderoyGryffindor"),
        Symbol("GilderoyRavenclaw")
    ))

    # Pomona is not in Slytherine
    knowledge.add(Not(Symbol("PomonaSlytherine")))

    # Minerava is in Gryffindor
    knowledge.add(Symbol("MinervaGryffindor"))

    check_knowledge(knowledge)
