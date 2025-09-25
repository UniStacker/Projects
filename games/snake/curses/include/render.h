#pragma once

#include "game.h"
#include <string>
#include <deque>

class Renderer {
public:
    Renderer(int width, int height);
    void draw(const std::deque<Point>& snake, const Point& food, int score, int highscore);
    void drawGameOver(int score);
    void drawPaused();
    void clearMessage();
    void resize(int width, int height);
    void drawBorder();

private:
    void moveCursor(int row, int col);
    void drawCell(const Point& p, const std::string &color);
    void clearCell(const Point& p);
    void drawText(int row, int col, const std::string &s);
//    void drawBorder();

    int width;
    int height;
    std::deque<Point> last_snake;
    Point last_food;
    int last_score;
    int last_highscore;
    static const int WALL_OFFSET_X = 3;
    static const int WALL_OFFSET_Y = 2;
    static const int CELL_W = 2;
};
