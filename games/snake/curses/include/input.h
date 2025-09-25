#pragma once

#include "game_fwd.h"

enum Direction { UP, DOWN, LEFT, RIGHT };

enum class UserAction {
    NONE,
    UP,
    DOWN,
    LEFT,
    RIGHT,
    PAUSE,
    QUIT,
    RESTART,
    CHANGE_DIFFICULTY
};

class Input {
public:
    UserAction getUserAction(Direction currentDirection);

private:
    bool kbhit();
    int readBytes(char* buf, int n);
};
