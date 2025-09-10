from pickle import dumps
from hashlib import blake2b
from base64 import urlsafe_b64encode

def my_hash(obj, size=6):
    digest = blake2b(dumps(obj), digest_size=size).digest()
    return urlsafe_b64encode(digest).decode()

class MiniMax:
    def __init__(self):
        self.hash_dict = dict()

    def start(self):
        raise Exception("Start function doesn't have an implementation.")

    def game_over(self, state):
        raise Exception("Terminal function doesn't have an implementation.")

    def result(self, action, state):
        raise Exception("Result function doesn't have an implementation.")

    def score(self, state):
        raise Exception("Utility function doesn't have an implementation.")

    def actions(self, state):
        raise Exception("Actions function doesn't have an implementation.")

    def hash_state(self, state, score, MAX):
        state_hash = my_hash(state)
        if state_hash not in self.hash_dict.keys():
            self.hash_dict[state_hash] = [score if MAX else None, score if not MAX else None]
        elif MAX:
            self.hash_dict[state_hash][0] = score
        else:
            self.hash_dict[state_hash][1] = score

    def get_hashed_score(self, state, MAX):
        hashed_state = self.hash_dict.get(my_hash(state), None)
        if hashed_state:
            return hashed_state[0] if MAX else hashed_state[1]
        return None

    def minimax(self, state, is_max):
        if self.game_over(state):
            return self.score(state)

        best = self.get_hashed_score(state, MAX=is_max)
        if best is not None:
            return best

        if not is_max:
            best = float('inf')
            for action in self.actions(state):
                best = min(best, self.minimax(self.result(action, state), is_max=True))
        else:
            best = -float('inf')
            for action in self.actions(state):
                best = max(best, self.minimax(self.result(action, state), is_max=False))

        self.hash_state(state, best, MAX=is_max)
        return best

