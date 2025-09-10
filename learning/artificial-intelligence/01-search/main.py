from maze import Maze
from os import system
import time
import argparse

parser = argparse.ArgumentParser(description='Solve a maze.')
parser.add_argument('maze_file', nargs='?', default='./mazes/maze1.txt', help='the maze file to solve')
parser.add_argument('-a', '--algorithm', default='dfs', help='the search algorithm to use (dfs, bfs, gbfs, a*)')
parser.add_argument('-c', '--cost', action='store_true', help='show the cost of each node')
parser.add_argument('-s', '--step', action='store_true', help='solve the maze step by step')
parser.add_argument('-l', '--live', action='store_true', help='solve the maze live')
parser.add_argument('-d', '--delay', type=float, default=1.0, help='the delay between steps in live mode')
args = parser.parse_args()

maze = Maze(args.maze_file, search_alg=args.algorithm)

def live():
    while not maze.solution:
        maze.print(show_cost=args.cost)
        time.sleep(args.delay)
        system("clear")
        maze.step()

if args.step:
    while not maze.solution:
        maze.print(show_cost=args.cost)
        if input().lower() in ("exit", "quit", "stop", "end"):
            break
        system("clear")
        maze.step()
elif args.live:
    live()
else:
    maze.solve()

maze.print(show_cost=args.cost)
