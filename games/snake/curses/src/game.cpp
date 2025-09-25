#include "game.h"
#include "input.h"
#include "terminal.h"
#include <algorithm>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <unistd.h>
#include <iostream>
#include <termios.h>
#include <thread>

using std::string;
using std::vector;
using std::deque;

const vector<Difficulty> DIFFICULTIES = {
    {"Easy",   4.0,  0.98, 1.4},
    {"Normal", 3.0,  0.96, 1.0},
    {"Hard",   2.4,  0.94, 0.82},
    {"Insane", 1.6,  0.92, 0.6}
};

Game::Game(int width, int height, int diffIndex)
    : width(width), height(height), diffIndex(diffIndex), score(0), alive(true), paused(false), dir(RIGHT) {
    reset();
    highscore = loadHighscore();
}

Game::Game(int width, int height, int diffIndex, std::deque<Point> initial_snake, Point initial_food)
    : width(width),
      height(height),
      diffIndex(diffIndex),
      score(0),
      highscore(0),
      alive(true),
      paused(false),
      dir(RIGHT),
      snake(std::move(initial_snake)),
      food(initial_food) {
    next_tick = clock::now();
}

void Game::reset() {
    int sx = width / 2;
    int sy = height / 2;
    snake.clear();
    snake.push_back({sx + 1, sy});
    snake.push_back({sx, sy});
    snake.push_back({sx - 1, sy});
    dir = RIGHT;
    food = randomFood();
    score = 0;
    alive = true;
    paused = false;
    next_tick = clock::now();
}

void Game::processInput(Input* input) {
    UserAction action = input->getUserAction(dir);
    switch (action) {
        case UserAction::UP: dir = UP; break;
        case UserAction::DOWN: dir = DOWN; break;
        case UserAction::LEFT: dir = LEFT; break;
        case UserAction::RIGHT: dir = RIGHT; break;
        case UserAction::PAUSE: paused = !paused; break;
        case UserAction::QUIT: alive = false; break;
        default: break;
    }
}

void Game::update() {
    if (!alive) return;

    if (paused) {
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        return;
    }

    auto now = clock::now();
    if (now < next_tick) {
        std::this_thread::sleep_for(next_tick - now);
    }
    next_tick += std::chrono::milliseconds(computeTickMs());

    Point newHead = snake.front();
    switch (dir) {
        case UP: newHead.y--; break;
        case DOWN: newHead.y++; break;
        case LEFT: newHead.x--; break;
        case RIGHT: newHead.x++; break;
    }

    if (newHead.x < 0 || newHead.x * 2 >= width * 2 || newHead.y < 0 || newHead.y >= height) {
        alive = false;
        return;
    }

    bool willGrow = (newHead == food);

    snake.push_front(newHead);

    if (!willGrow) {
        snake.pop_back();
    } else {
        score++;
        if (score > highscore) {
            highscore = score;
        }
        food = randomFood();
    }

    // Self-collision check after moving
    for (auto it = snake.begin() + 1; it != snake.end(); ++it) {
        if (*it == snake.front()) {
            alive = false;
            return;
        }
    }
}

void Game::resize(int& new_width, int& new_height) {
    width = new_width;
    height = new_height;
    reset();
}

Point Game::randomFood() {
    vector<Point> freeCells;
    freeCells.reserve(width * height);
    for (int yy = 0; yy < height; ++yy) {
        for (int xx = 0; xx < width; ++xx) {
            Point p{xx, yy};
            if (std::find(snake.begin(), snake.end(), p) == snake.end()) {
                freeCells.push_back(p);
            }
        }
    }
    if (freeCells.empty()) return {-1, -1};
    static bool seeded = false;
    if (!seeded) { srand(time(nullptr) ^ getpid()); seeded = true; }
    return freeCells[rand() % freeCells.size()];
}

int Game::computeTickMs() {
    const Difficulty& D = DIFFICULTIES[diffIndex];
    double target_cross = D.base_cross_time;
    target_cross = target_cross * sqrt((double)width / 40.0);
    double step_seconds = target_cross / std::max(1, width);
    step_seconds *= D.difficulty_speed_mul;
    step_seconds *= pow(D.score_accel, score);
    double min_step = 0.02;
    double max_step = 0.5;
    step_seconds = std::max(min_step, std::min(max_step, step_seconds));
    return (int)round(step_seconds * 1000.0);
}

bool Game::isGameOver() const {
    return !alive;
}

int Game::getScore() const {
    return score;
}

int Game::getHighscore() const {
    return highscore;
}

const deque<Point>& Game::getSnake() const {
    return snake;
}

const Point& Game::getFood() const {
    return food;
}

Direction Game::getDirection() const {
    return dir;
}

void Game::setDirection(Direction new_dir) {
    dir = new_dir;
}

bool Game::isPaused() const {
    return paused;
}

string highscorePath() {
    const char* home = getenv("HOME");
    if (!home) home = ".";
    return string(home) + "/.snake_highscore";
}

int loadHighscore() {
    string path = highscorePath();
    std::ifstream in(path);
    int v = 0;
    if (in.is_open()) in >> v;
    in.close();
    return v;
}

void saveHighscore(int v) {
    string path = highscorePath();
    std::ofstream out(path, std::ios::trunc);
    if (out.is_open()) out << v << "\n";
}

int promptDifficulty() {
    struct termios t;
    if (tcgetattr(STDIN_FILENO, &t) == 0) {
        t.c_lflag |= (ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &t);
    }

    std::cout << "\033[2J";
    std::cout << "Select difficulty:\n";
    for (size_t i = 0; i < DIFFICULTIES.size(); ++i) {
        std::cout << "  " << (i+1) << ") " << DIFFICULTIES[i].name << "\n";
    }
    std::cout << "\nChoose 1-" << DIFFICULTIES.size() << " (or press letter e/n/h/i): ";
    std::cout.flush();

    int choice = 1;
    char c = 0;
    while (true) {
        if (read(STDIN_FILENO, &c, 1) <= 0) continue;
        if (c >= '1' && c <= '0' + (int)DIFFICULTIES.size()) {
            choice = c - '1';
            break;
        } else {
            char lower = tolower((unsigned char)c);
            if (lower == 'e') { choice = 0; break; }
            if (lower == 'n') { choice = 1; break; }
            if (lower == 'h') { choice = 2; break; }
            if (lower == 'i') { choice = 3; break; }
        }
    }
    return choice;
}
