from functools import reduce

# ===== Colors =====
RESET = "\033[0m"
BOLD = "\033[1m"
B_RED = BOLD + "\033[31m"
B_GREEN = BOLD + "\033[32m"
B_YELLOW = BOLD + "\033[33m"
B_CYAN = BOLD + "\033[34m"
B_PURPLE = BOLD + "\033[35m"


def remove(s, c):
    l = list(s)
    while c in l:
        l.remove(c)
    return ''.join(l)

class Sentence:
    def evaluate(self, model):
        """Evaluates the Sentence according to Model."""
        raise Exception("Nothing to evaluate.")

    def formula(self):
        return ""

    def symbols(self):
        """Returns the set Symbols involved in the Sentence."""
        return set()

    @classmethod
    def validate(cls, sentence):
        """Validates if a satement is a Sentence."""
        if not isinstance(sentence, Sentence):
            raise Exception(f"Sentence '{sentence}' should be a prepositional Sentence.")

    @classmethod
    def parenthesize(cls, s):
        """Parenthesizes a string if it is not already."""
        def is_balanced(string):
            count = 0
            for c in string:
                if c == '(': count += 1
                elif c == ')':
                    if count <= 0: return False
                    count -= 1
            return count == 0

        if isinstance(s, str) and (not len(s) or s.isalnum() or (
                s[0] == '(' and s[-1] == ')' and is_balanced(s[1:-1])
            )):
            return s
        return f"({s})"


class Symbol(Sentence):
    def __init__(self, name):
        self.name = name

    def __eq__(self, other):
        return isinstance(other, Symbol) and self.name == other.name

    def __hash__(self):
        return hash(("Symbol", self.name))

    def __str__(self):
        return self.name

    def __len__(self):
        return len(self.name)

    def symbols(self):
        return {self.name}

    def formula(self):
        return self.name

    def evaluate(self, model):
        try:
            return bool(model[self.name])
        except:
            raise Exception(f"Variable {self.name} not in model.")


class Not(Sentence):
    def __init__(self, operand):
        Sentence.validate(operand)
        self.operand = operand

    def __eq__(self, other):
        return isinstance(other, Not) and self.operand == other.operand

    def __hash__(self):
        return hash(("NOT", self.operand))

    def __repr__(self):
        return f"NOT({self.operand})"

    def formula(self):
        return f"{B_RED}¬{RESET}" + Sentence.parenthesize(self.operand.formula())

    def evaluate(self, model):
        return not self.operand.evaluate(model)

    def symbols(self):
        return self.operand.symbols()


class And(Sentence):
    def __init__(self, *operands):
        for operand in operands:
            Sentence.validate(operand)
        self.operands = list(operands)

    def __eq__(self, other):
        return isinstance(other, And) and self.operands == other.oprands

    def __hash__(self):
        return hash(("And", tuple(hash(op) for op in self.oprands)))

    def __repr__(self):
        return f"AND({', '.join([str(op) for op in self.operands])})"

    def expand(self, expansion=None):
        out = expansion if expansion else And()
        for op in (out.operands if expansion else self.operands):
            if isinstance(op, And):
                out.operands += op.operands
                out = self.expand(out)
            else:
                if op not in out.operands: out.add(op)
        return out

    def formula(self):
        return f" {B_GREEN}∧{RESET} ".join([Sentence.parenthesize(op.formula()) for op in self.operands])

    def evaluate(self, model):
        result = True
        for operand in self.operands:
            result = result and operand.evaluate(model)
        return result

    def symbols(self):
        return set.union(*[op.symbols() for op in self.operands])

    def add(self, operand):
        self.operands.append(operand)


class Or(Sentence):
    def __init__(self, *operands):
        for operand in operands:
            Sentence.validate(operand)
        self.operands = list(operands)

    def __eq__(self, other):
        return isinstance(other, Or) and self.operands == other.operands

    def __hash__(self):
        return hash(("Or", tuple(hash(op) for op in self.oprands)))

    def __repr__(self):
        return f"OR({', '.join([str(op) for op in self.operands])})"

    def formula(self):
        return f" {B_YELLOW}∨{RESET} ".join([Sentence.parenthesize(op.formula()) for op in self.operands])

    def evaluate(self, model):
        result = False
        for operand in self.operands:
            result = result or operand.evaluate(model)
        return result

    def symbols(self):
        return set.union(*[op.symbols() for op in self.operands])

    def add(self, operand):
        self.operands.append(operand)


class Implies(Sentence):
    def __init__(self, *operands):
        for operand in operands:
            Sentence.validate(operand)
        self.operands = list(operands)

        if len(self.operands) != 2:
            raise Exception("Implication takes exactly 2 operands, but", len(self.operands), "was given.")

    def __eq__(self, other):
        return isinstance(other, Implies) and self.operands == other.oprands

    def __hash__(self):
        return hash(("Implies", tuple(hash(op) for op in self.oprands)))

    def __repr__(self):
        return f"Implies({', '.join([str(op) for op in self.operands])})"

    def formula(self):
        return f" {B_CYAN}->{RESET} ".join([Sentence.parenthesize(op.formula()) for op in self.operands])

    def evaluate(self, model):
        result = (
                (not self.operands[0].evaluate(model))
             or self.operands[1].evaluate(model)
        )
        return result

    def symbols(self):
        return set.union(*[op.symbols() for op in self.operands])


class Biconditional(Sentence):
    def __init__(self, *operands):
        for operand in operands:
            Sentence.validate(operand)
        self.operands = list(operands)

        if len(self.operands) != 2:
            raise Exception("Implication takes exactly 2 operands, but", len(self.operands), "was given.")

    def __eq__(self, other):
        return isinstance(other, Biconditional) and self.operands == other.operands

    def __hash__(self):
        return hash(("Biconditional", tuple(hash(op) for op in self.operands)))

    def __repr__(self):
        return f"Biconditional({', '.join([repr(op) for op in self.operands])})"

    def formula(self):
        return f" {B_PURPLE}<=>{RESET} ".join([Sentence.parenthesize(op.formula()) for op in self.operands])

    def evaluate(self, model):
        return self.operands[0].evaluate(model) == self.operands[1].evaluate(model)

    def symbols(self):
        return set.union(*[op.symbols() for op in self.operands])


# ===== Brute force model check =====
def model_check(knowledge, query):
    """Checks every possible world to see if knowledge entails query."""
    def check_model(symbols, model):
        if not symbols:
            if knowledge.evaluate(model) and not query.evaluate(model):
                return False
            return True
            #elif knowledge.evaluate(model) and not query.evaluate(model):
            #    return False
        else:
            symbols = symbols.copy()
            s = symbols.pop()

            model_true = model.copy()
            model_true[s] = True

            model_false = model.copy()
            model_false[s] = False

            return check_model(symbols, model_true) and check_model(symbols, model_false)

    symbols = set.union(knowledge.symbols(), query.symbols())

    return check_model(symbols, {})


# ===== Expansion logic =====
def _eliminate_iff(expr):
    if isinstance(expr, Biconditional):
        A, B = expr.operands
        return And(Implies(A, B), Implies(B, A))
    elif isinstance(expr, Not):
        return Not(_eliminate_iff(expr.operand))
    elif hasattr(expr, "operands"):
        return expr.__class__(*[_eliminate_iff(op) for op in expr.operands])
    return expr

def _eliminate_implies(expr):
    if isinstance(expr, Implies):
        A, B = expr.operands
        return Or(Not(A), B)
    elif isinstance(expr, Not):
        return Not(_eliminate_implies(expr.operand))
    elif hasattr(expr, "operands"):
        return expr.__class__(*[_eliminate_implies(op) for op in expr.operands])
    return expr

def _push_nots_inward(expr):
    if isinstance(expr, Not):
        operand = expr.operand
        if isinstance(operand, Not):
            return _push_nots_inward(operand.operand)
        elif isinstance(operand, And):
            return Or(*[_push_nots_inward(Not(op)) for op in operand.operands])
        elif isinstance(operand, Or):
            return And(*[_push_nots_inward(Not(op)) for op in operand.operands])
        else:
            return Not(_push_nots_inward(operand))
    elif hasattr(expr, "operands"):
        return expr.__class__(*map(_push_nots_inward, expr.operands))
    return expr

def _distribute_or_pairwise(a, b):
    if isinstance(a, And):
        return And(*[_distribute_or_pairwise(op, b) for op in a.operands])
    elif isinstance(b, And):
        return And(*[_distribute_or_pairwise(a, op) for op in b.operands])
    else:
        return Or(a, b)

def _distribute_or_nary(operands):
    return reduce(_distribute_or_pairwise, operands)

def _distribute_or_over_and(expr):
    if isinstance(expr, Or):
        # Recursively distribute over all operands
        operands = [_distribute_or_over_and(op) for op in expr.operands]

        # Reduce n-ary Or by pairwise distribution
        return _distribute_or_nary(operands)

    elif isinstance(expr, And):
        return And(*[_distribute_or_over_and(op) for op in expr.operands])

    elif isinstance(expr, Not):
        return Not(_distribute_or_over_and(expr.operand))

    else:
        return expr  # Symbols, literals

def _flatten(expr):
    if isinstance(expr, And) or isinstance(expr, Or):
        ops = []
        for op in expr.operands:
            if isinstance(op, expr.__class__):
                ops.extend(_flatten(op).operands)
            else:
                ops.append(_flatten(op))
        return expr.__class__(*ops)
    elif isinstance(expr, Not):
        return Not(_flatten(expr.operand))
    return expr

def to_cnf(clause):
    clause = _eliminate_iff(clause)
    clause = _eliminate_implies(clause)
    clause = _push_nots_inward(clause)
    clause = _distribute_or_over_and(clause)
    clause = _flatten(clause)
    return clause


# ===== Inferrence by Resolution =====
# (P v Q) ^ (R v ~P)
def resolve(kb):
    kb = to_cnf(kb)
    while kb.operands:
        c1 = kb.operands.pop()
        while kb.operands:
            c2 = kb.operands[0]
            if c1 != c2:
                c1_syms = [s for s in (c1.operands if hasattr(c1, "operands") else [c1.operand if isinstance(c1, Not) else c1])]
                c2_syms = [s for s in (c2.operands if hasattr(c2, "operands") else [c2.operand if isinstance(c2, Not) else c2])]
                new_clause = Or()
                for idx in range(len(c1_syms)):
                    sym = c1_syms.pop()
                    not_sym = _push_nots_inward(Not(sym))
                    if not_sym not in c2_syms:
                        new_clause.add(sym)
                    else:
                        c2_syms.remove(not_sym)
                        kb.operands.remove(c1)
                if new_clause != c1:
                    new_clause.operands += c2_syms
                    operands = len(new_clause.operands)
                    if operands == 1:
                        kb.add(new_clause.operands[0])
                    elif operands == 0:
                        return None
                    else:
                        kb.add(new_clause)
                    kb.operands.remove(c2)
                else:
                    if c1 not in kb.operands: new_kb.add(c1)
                    if c2 not in kb.operands: new_kb.add(c2)
        if not kb.operands and c1 not in new_kb.operands:
            new_kb.add(c1)

    return new_kb
