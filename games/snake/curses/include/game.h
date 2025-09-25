#pragma once

#include <deque>
#include <string>
#include <vector>
#include <chrono>
#include "input.h"

struct Point {
    int x = 0;
    int y = 0;
    Point(int x, int y) : x(x), y(y) {}
    Point() = default;
    bool operator==(const Point &o) const { return x == o.x && y == o.y; }
};

struct Difficulty {
    std::string name;
    double base_cross_time;
    double score_accel;
    double difficulty_speed_mul;
};

extern const std::vector<Difficulty> DIFFICULTIES;

class Game {
public:
    Game(int width, int height, int diffIndex);
    // Constructor for testing
    Game(int width, int height, int diffIndex, std::deque<Point> initial_snake, Point initial_food);

    void processInput(Input* input);
    void update();
    void resize(int& new_width, int& new_height);

    bool isGameOver() const;
    bool isPaused() const;
    int getScore() const;
    int getHighscore() const;
    const std::deque<Point>& getSnake() const;
    const Point& getFood() const;
    Direction getDirection() const;
    void setDirection(Direction new_dir);

private:
    void reset();
    Point randomFood();
    int computeTickMs();

    int width, height;
    int diffIndex;
    int score;
    int highscore;
    bool alive;
    bool paused;
    Direction dir;
    std::deque<Point> snake;
    Point food;

    using clock = std::chrono::steady_clock;
    std::chrono::time_point<clock> next_tick;
};

int promptDifficulty();
int loadHighscore();
void saveHighscore(int v);
