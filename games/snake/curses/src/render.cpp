#include "render.h"
#include <iostream>

using std::string;
using std::deque;
using std::cout;
using std::flush;

Renderer::Renderer(int width, int height) : width(width), height(height), last_food(-1, -1), last_score(-1), last_highscore(-1) {
    drawBorder();
}

void Renderer::moveCursor(int row, int col) {
    cout << "\033[" << row << ";" << col << "H";
}

void Renderer::drawCell(const Point& p, const string &color) {
    int row = p.y + WALL_OFFSET_Y;
    int col = p.x * CELL_W + WALL_OFFSET_X;
    cout << "\033[" << row << ";" << col << "H" << color << "██" << "\033[0m";
}

void Renderer::clearCell(const Point& p) {
    int row = p.y + WALL_OFFSET_Y;
    int col = p.x * CELL_W + WALL_OFFSET_X;
    cout << "\033[" << row << ";" << col << "H" << "  ";
}

void Renderer::drawText(int row, int col, const string &s) {
    cout << "\033[" << row << ";" << col << "H" << s;
}

void Renderer::drawBorder() {
    cout << "\033[2J";
    const int top_y = 1;
    const int bottom_y = height + 2;
    const int left_x = 1;
    const int right_x = width * 2 + 3;

    // Top and bottom borders
    for (int x = left_x; x <= right_x; x += 2) {
        cout << "\033[" << top_y << ";" << x << "H██";
        cout << "\033[" << bottom_y << ";" << x << "H██";
    }

    // Left and right borders
    for (int y = top_y + 1; y < bottom_y; ++y) {
        cout << "\033[" << y << ";" << left_x << "H██";
        cout << "\033[" << y << ";" << right_x << "H██";
    }
}

void Renderer::draw(const deque<Point>& snake, const Point& food, int score, int highscore) {
    for (const auto& p : last_snake) {
        bool found = false;
        for (const auto& p2 : snake) {
            if (p == p2) {
                found = true;
                break;
            }
        }
        if (!found) {
            clearCell(p);
        }
    }

    if (!snake.empty()) {
        drawCell(snake.front(), "\033[1;32m");
        for (size_t i = 1; i < snake.size(); ++i) drawCell(snake[i], "\033[0;32m");
    }
    if (food.x >= 0) {
        if (last_food.x != food.x || last_food.y != food.y) {
            clearCell(last_food);
            drawCell(food, "\033[1;31m");
        }
    }
    if (score != last_score || highscore != last_highscore) {
        drawText(height + WALL_OFFSET_Y + 2, 1, "Score: " + std::to_string(score) + "    High: " + std::to_string(highscore) + "    ");
        last_score = score;
        last_highscore = highscore;
    }
    cout << flush;
    last_snake = snake;
    last_food = food;
}

void Renderer::drawGameOver(int score) {
    drawText(height + WALL_OFFSET_Y + 3, 1, "Game Over! Score: " + std::to_string(score) + "    ");
    drawText(height + WALL_OFFSET_Y + 4, 1, "Press 'r' to restart, 'd' to choose difficulty, or 'q' to quit.");
    cout << flush;
}

void Renderer::drawPaused() {
    drawText(height + WALL_OFFSET_Y + 3, 1, "PAUSED - press 'p' to resume.               ");
    cout << flush;
}

void Renderer::clearMessage() {
    drawText(height + WALL_OFFSET_Y + 3, 1, string(80, ' '));
    drawText(height + WALL_OFFSET_Y + 4, 1, string(80, ' '));
    cout << flush;
}

void Renderer::resize(int new_width, int new_height) {
    width = new_width;
    height = new_height;
    last_snake.clear();
    last_food = {-1, -1};
    drawBorder();
}
