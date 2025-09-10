from minimax import MiniMax
from random import choice
from math import sqrt


# ===== Game Setup =====
PLAYER = 'X'
COMPUTER = 'O'

# ===== Color Setup =====
RESET = "\033[0m"
BOLD = "\033[1m"

B_GREEN = BOLD + "\033[32m"
B_RED = BOLD + "\033[31m"

P1 = B_GREEN + PLAYER + RESET
P2 = B_RED + COMPUTER + RESET

# ===== History Setup =====
import readline
import atexit

HISTORY_FILE = ".game_history"
HISTORY_LEN = 100

try:
    readline.set_history_length(HISTORY_LEN)
    readline.read_history_file(HISTORY_FILE)
except FileNotFoundError:
    ...

atexit.register(readline.write_history_file, HISTORY_FILE)


def is_int(s):
    return s and all('0' <= c <= '9' for c in s)

class Game(MiniMax):
    def __init__(self, turn='p1'):
        super().__init__()
        self.running = False
        self.turn = turn
        self.order = 3 * 3

        self.reset()

    def reset(self):
        self.board = [' '] * self.order
        self._reset_winner = 1 if self.running else 0
        self.winner = None

    def clear(self):
        for _ in range(6 + (self.winner != None) + self._reset_winner):
            print("\r\033[K\033[A", end='')
        self._reset_winner = 0

    def print_board(self, clear=False):
        if clear: self.clear()

        print("Type 'quit', 'exit' or 'stop' to stop the game.")
        print("Type 'reset' to reset the game.")
        for idx, cell in enumerate(self.board):
            if idx > 0 and idx % 3 == 0: print()
            if cell == ' ': print(f"[ {idx} ]\t", end='')
            else: print(f"[ {P1 if cell == PLAYER else P2} ]\t", end='')
        print()

    def update_board(self, idx, val):
        if 0 <= idx < self.order:
            self.board[idx] = val

    def get_input(self):
        inp =  input("<?> ").strip().lower()
        if is_int(inp) and not self.winner:
            inp = int(inp)
            if 0 <= inp < self.order and self.board[inp] == ' ': return inp
            return ""
        return inp

    def actions(self, board="self"):
        if board == "self": board = self.board
        return [i for i, cell in enumerate(board) if cell == ' ']

    def result(self, action, board="self", switch_turn=True):
        new_board = board
        if board == "self":
            new_board = self.board.copy()
        else:
            new_board = board.copy()
        if switch_turn:
            self.turn = "p2" if self.turn == "p1" else "p1"
        new_board[action] = PLAYER if self.turn == "p1" else COMPUTER

        return new_board

    def computer_turn(self):
        if self.winner: return

        best = (None, float('inf'))
        for action in self.actions():
            score = self.minimax(self.result(action, self.board), is_max=False)
            if score < best[1]:
                best = (action, score)
        self.update_board(best[0], COMPUTER)
        self.turn = "p1"
        #print(f"{best = }"); input()
        self.winner = None

    def game_over(self, board="self"):
        if board == "self": board = self.board
        row = sqrt(self.order)
        win_states = [
            [0, 1, 2], [3, 4, 5], [6, 7, 8],    # Horizontal checks
            [0, 3, 6], [1, 4, 7], [2, 5, 8],    # Vertical checks
            [0, 4, 8], [2, 4, 6]                # Diagonal checks
        ]

        for ws in win_states:
            if board[ws[0]] == board[ws[1]] == board[ws[2]] != ' ':
                self.winner = board[ws[0]]
                return True
        if ' ' not in board:
            self.winner = "D"
            return True

        return False

    def score(self, board="self"):
        if board == "self": board = self.board
        if self.winner is None: self.game_over(board)
        return 1 if self.winner == PLAYER else 0 if self.winner == "D" else -1

    def start(self):
        self.running = True

    def check(self):
        if self.game_over():
            score = self.score()
            if score == 1:
                print("Congrats! You won.")
            elif score == -1:
                print("Sorry, you lost.")
            else:
                print("It's a draw.")



def main():
    game = Game()

    game.start()
    start = True
    while game.running:
        if start: game.print_board(clear=False); start = False
        else: game.print_board(clear=True)

        game.check()

        inp = game.get_input()
        if inp in ("quit", "exit", "stop"): break
        elif inp == "reset": game.reset(); continue
        elif isinstance(inp, int):
            game.update_board(inp, PLAYER)
        else: continue

        game.check()

        game.computer_turn()


if __name__ == '__main__': main()
