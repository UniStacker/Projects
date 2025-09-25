#include "input.h"
#include "game.h"
#include <unistd.h>
#include <sys/select.h>

bool Input::kbhit() {
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    timeval timeout = {0, 0};
    return select(STDIN_FILENO + 1, &readfds, NULL, NULL, &timeout) > 0;
}

int Input::readBytes(char* buf, int n) {
    ssize_t r = read(STDIN_FILENO, buf, n);
    if (r <= 0) return 0;
    return (int)r;
}

UserAction Input::getUserAction(Direction currentDirection) {
    if (!kbhit()) {
        return UserAction::NONE;
    }

    char buf[32];
    int n = readBytes(buf, sizeof(buf));
    for (int i = 0; i < n; ++i) {
        char c = buf[i];
        if (c == '\033') { // escape seq
            char b1 = 0, b2 = 0;
            if (i + 2 < n) {
                b1 = buf[i + 1];
                b2 = buf[i + 2];
                i += 2;
            } else {
                char tmp[2];
                int r = readBytes(tmp, 2);
                if (r >= 1) b1 = tmp[0];
                if (r >= 2) b2 = tmp[1];
            }
            if (b1 == '[') {
                switch (b2) {
                    case 'A': if (currentDirection != DOWN) return UserAction::UP; break;
                    case 'B': if (currentDirection != UP) return UserAction::DOWN; break;
                    case 'C': if (currentDirection != LEFT) return UserAction::RIGHT; break;
                    case 'D': if (currentDirection != RIGHT) return UserAction::LEFT; break;
                }
            }
        } else {
            if (c == 'q' || c == 'Q') return UserAction::QUIT;
            if (c == 'p' || c == 'P' || c == ' ') return UserAction::PAUSE;
            if (c == 'r' || c == 'R') return UserAction::RESTART;
            if (c == 'd' || c == 'D') return UserAction::CHANGE_DIFFICULTY;
            if ((c == 'w' || c == 'W') && currentDirection != DOWN) return UserAction::UP;
            if ((c == 's' || c == 'S') && currentDirection != UP) return UserAction::DOWN;
            if ((c == 'd' || c == 'D') && currentDirection != LEFT) return UserAction::RIGHT;
            if ((c == 'a' || c == 'A') && currentDirection != RIGHT) return UserAction::LEFT;
        }
    }
    return UserAction::NONE;
}