class State:
    def __init__(self, coords, steps):
        self.coords = coords
        self.steps = steps

class Node:
    def __init__(self, state: State, parent, action):
        self.state = state
        self.parent = parent
        self.action = action

    def __str__(self):
        return f"Node(state={self.state}, parent={id(self.parent)}, action={self.action})"

    def __eq__(self, other):
        if isinstance(other, Node):
            return (
                self.state == other.state
                and self.parent == other.parent
                and self.action == other.action
            )

    def __lt__(self, _):
        return False

    def __gt__(self, _):
        return False


class StackFrontier:
    def __init__(self, frontier=None):
        if isinstance(frontier, StackFrontier):
            self.frontier = frontier.frontier
        elif isinstance(frontier, list) and all([isinstance(n, Node) for n in frontier]):
            self.frontier = frontier
        elif frontier is None:
            self.frontier = []
        else:
            raise TypeError(f"{self.__class__.__name__} only stores Nodes, not {type(frontier)}{f'[{type([n for n in frontier if not isinstance(n, Node)][0])}]' if isinstance(frontier, list) else ''}")

    def add(self, node: Node):
        if isinstance(node, Node):
            self.frontier.append(node)
            return self
        raise TypeError(f"{self.__class__.__name__} only stores Nodes, not {type(node)}")

    def remove(self, item=-1):
        if self.empty():
            raise Exception("Frontier is empty.")
        if isinstance(item, Node):
            for idx, node in enumerate(self):
                if node == item: return self.remove(idx)
        return self.frontier.pop(item)

    def contains_state(self, state):
        if isinstance(state, Node): state = node.state
        return any(n.state == state for n in self.frontier)

    def get_state(self, coords):
        if self.empty(): return None
        state = [n.state for n in self if coords == n.state[:2]]
        return state[0] if state else None

    def empty(self): return len(self) == 0

    def __str__(self):
        return f"{[str(n) for n in self.frontier]}"

    def __add__(self, other):
        out = StackFrontier()
        for n in self.frontier:
            out.add(n)

        if isinstance(other, StackFrontier):
            for n in other.frontier:
                out.add(n)
        elif isinstance(other, list):
            for n in other:
                out.add(n)

        return out

    def __iter__(self):
        for node in self.frontier: yield node

    def __len__(self): return len(self.frontier)

    def __getitem__(self, idx): return self.frontier[idx]


class QueueFrontier(StackFrontier):
    def __init__(self, frontier=None):
        super().__init__(frontier)

    def remove(self, item=0):
        if self.empty():
            raise Exception("Frontier is empty.")
        if isinstance(item, Node):
            for idx, node in enumerate(self):
                if node == item: return self.remove(idx)
        elif isinstance(item, int): return self.frontier.pop(item)
        else: return None
