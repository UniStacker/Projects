from logic import *

mustard = Symbol("ColMustard")
plum = Symbol("ProfPlum")
scarlet = Symbol("MsScarlet")
characters = [mustard, plum, scarlet]

ballroom = Symbol("BallRoom")
kitchen = Symbol("Kitchen")
library = Symbol("Library")
rooms = [ballroom, kitchen, library]

knife = Symbol("Knife")
revolver = Symbol("Revolver")
wrench = Symbol("Wrench")
weapons = [knife, revolver, wrench]

symbols = characters + rooms + weapons

def check_knowledge(knowledge):
    print("=" * 20)
    for symbol in symbols:
        if model_check(knowledge, symbol):
            print(f"\033[32m{symbol}: \033[1mYES\033[0m")
        elif not model_check(knowledge, Not(symbol)):
            print(f"\033[33m{symbol}: \033[1mMAYBE\033[0m")
    print("=" * 20)


if __name__ == "__main__":
    # Basic Knowledge
    knowledge = And(
        Or(*characters),
        Or(*rooms),
        Or(*weapons)
    )

    # Aditional Knowledge from cards
    knowledge.add(Not(mustard))
    knowledge.add(Not(kitchen))
    knowledge.add(Not(revolver))

    knowledge.add(Or(Not(scarlet), Not(library), Not(wrench)))

    knowledge.add(Not(plum))
    knowledge.add(Not(ballroom))

    #print(knowledge.formula())
    check_knowledge(knowledge)
