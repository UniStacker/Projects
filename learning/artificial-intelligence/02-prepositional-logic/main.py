from logic import *

rain = Symbol("rain")
hagrid = Symbol("hagrid")
dumbledore = Symbol("dumbledore")

knowledge = And(
    Implies(Not(rain), hagrid),
    Or(hagrid, dumbledore),
    Not(And(hagrid, dumbledore)),
    dumbledore
)

print("Is it raining? ", end='')
print(model_check(knowledge, rain))

print("Harry visited hagrid? ", end='')
print(model_check(knowledge, hagrid))
