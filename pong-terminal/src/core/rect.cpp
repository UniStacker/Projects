#include <iostream>

enum Color {
  Blank = 0,
  Red = 31,
  Green = 32
};

struct Vec2 {
  int x, y;

  Vec2() {}
  Vec2(int x, int y) : x(x), y(y) {}
  bool operator== (Vec2 other) {
    return x == other.x && y == other.y;
  }
};

struct Side {
  Vec2 start, end;

  Side() {}
  Side(Vec2 start, Vec2 end) : start(start), end(end) {}
  bool operator== (Side other) {
    return start == other.start && end == other.end;
  }
};

class Rect {
private:
public:
  // Basic data
  int x{}, y{};
  size_t width{}, height{};
  Color background{};

  // Extra info
  Side left{}, right{}, top{}, bottom{};
  Vec2 topleft{}, bottomleft{}, topright{}, bottomright{};

  // Constructors
  Rect(int x, int y, size_t width, size_t height) : x(x), y(y), width(width), height(height) {
    background = Color::Blank;
    left = Side({x, y}, {x, static_cast<int>(y + height)});
    right = Side({static_cast<int>(x + width), y}, {static_cast<int>(x + width), static_cast<int>(y + height)});
    top = Side({x, y}, {static_cast<int>(x + width), y});
    bottom = Side({x, static_cast<int>(y + height)}, {static_cast<int>(x + width), static_cast<int>(y + height)});
    topleft = Vec2(x, y);
    bottomleft = Vec2(x, static_cast<int>(y + height));
    topright = Vec2(static_cast<int>(x + width), y);
    bottomright = Vec2(static_cast<int>(x + width), static_cast<int>(y + height));
  }
};
