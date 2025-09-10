from logic import *

P = Symbol("P")
Q = Symbol("Q")
R = Symbol("R")
S = Symbol("S")
T = Symbol("T")
U = Symbol("U")

exp = And(Or(Not(P), Q), Not(P))#, Or(R, Not(Q)))

print(exp.formula(), ":")
resolved = resolve(exp)
print(resolved.formula() if resolved else "There is a contradiction in exp.")
