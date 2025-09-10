from structures import Node, State
from queue import PriorityQueue
import heapq
import itertools


# ------- Colors -------
RESET = "\033[0m"
BOLD = "\033[1m"
GREEN = "\033[32m"
RED = "\033[31m"
BG_X = "\033[7m"


class Maze:
    def __init__(self, fileName, search_alg="DFS"):
        self.search_alg = search_alg.lower()
        if self.search_alg not in ("dfs", "bfs", "gbfs", "a*"):
            raise Exception(f"Unknown search algorithm: {search_alg}")

        self.is_dfs = self.search_alg == "dfs"
        self.is_bfs = self.search_alg == "bfs"
        self.is_gbfs = self.search_alg == "gbfs"
        self.is_astar = self.search_alg == "a*"

        self.counter = itertools.count()
        self.frontier = []
        heapq.heapify(self.frontier)

        self.explored = set()
        self.solution = None
        self.steps = 0

        # Read maze file
        with open(fileName, "r") as f:
            contents = f.read()

        # Validate maze
        if contents.count("A") != 1:
            raise Exception("Maze can only have ONE starting point.")
        if contents.count("B") != 1:
            raise Exception("Maze can only have ONE goal.")

        # Parse maze
        contents = contents.splitlines()
        self.height = len(contents)
        self.width = max(len(line) for line in contents)
        self.walls = []
        for i in range(self.height):
            row = []
            for j in range(self.width):
                char = contents[i][j] if j < len(contents[i]) else ' '
                if char == 'A':
                    self.start = (i, j)
                    row.append(False)
                elif char == 'B':
                    self.goal = (i, j)
                    row.append(False)
                elif char == ' ':
                    row.append(False)
                else:
                    row.append(True)
            self.walls.append(row)

        # Start node
        self.add_neighbors(Node(self.start, None, None))

    def heuristic(self, state):
        # Manhattan distance
        return abs(state[0] - self.goal[0]) + abs(state[1] - self.goal[1])

    def priority(self, node):
        h = self.heuristic(node.state)
        g = self.steps
        if self.is_gbfs:
            return h
        elif self.is_astar:
            return g + h
        return g  # DFS and BFS don’t use heuristic

    def add_neighbors(self, node):
        for action, neighbor in self.neighbors(node.state):
            child = Node(state=neighbor, parent=node, action=action)
            cost = self.priority(child)
            heapq.heappush(self.frontier, (cost, next(self.counter), child))

    def neighbors(self, state):
        row, col = state
        candidates = [
            ("up", (row - 1, col)),
            ("down", (row + 1, col)),
            ("left", (row, col - 1)),
            ("right", (row, col + 1))
        ]
        valid = []
        for action, (i, j) in candidates:
            if (0 <= i < self.height and
                0 <= j < self.width and
                not self.walls[i][j] and
                (i, j) not in self.explored):
                valid.append((action, (i, j)))
        return valid

    def solve(self):
        while self.frontier:
            _, _, node = heapq.heappop(self.frontier)
            if node.state == self.goal:
                self.reconstruct_path(node)
                return

            self.explored.add(node.state)
            self.steps += 1
            self.add_neighbors(node)

        raise Exception("No solution found.")

    def reconstruct_path(self, node):
        actions = []
        states = []
        while node.parent:
            actions.append(node.action)
            states.append(node.state)
            node = node.parent
        actions.reverse()
        states.reverse()
        self.solution = (states, actions)

    def print(self, show_cost = False):
        print()
        print(f"Nodes explored: {len(self.explored)}")
        if self.solution:
            print(f"Solution moves: {len(self.solution[1])}")
            print("Solution:")
        else:
            print("No solution found.")
        print()

        solution_path = set(self.solution[0]) if self.solution else set()
        for i in range(self.height):
            for j in range(self.width):
                pos = (i, j)
                if self.walls[i][j]:
                    print(f"{BG_X}{'  '}{RESET}", end='')
                elif pos == self.start:
                    print(f"{BOLD}{RED}A {RESET}", end='')
                elif pos == self.goal:
                    print(f"{BOLD}{GREEN}B {RESET}", end='')
                elif pos in solution_path:
                    print(f"{GREEN}• {RESET}", end='')
                elif pos in self.explored:
                    print(f"{RED}· {RESET}", end='')
                else:
                    print("  ", end='')
            print()

        if self.solution:
            print("Actions:")
            print(", ".join(self.solution[1]))
