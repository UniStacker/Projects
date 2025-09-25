#include "terminal.h"
#include <termios.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <sys/ioctl.h>
#include <signal.h>

std::atomic<bool> Terminal::resized(false);
static struct termios orig_term;
static bool raw_enabled = false;

static void handle_exit_signal(int) {
    if (raw_enabled) {
        tcsetattr(STDIN_FILENO, TCSANOW, &orig_term);
        raw_enabled = false;
    }
    write(STDOUT_FILENO, "\033[?25h", 6);
    _exit(0);
}

void Terminal::setupSignalHandlers() {
    signal(SIGINT, handle_exit_signal);
    signal(SIGTERM, handle_exit_signal);
    signal(SIGWINCH, [](int){ Terminal::setResized(true); });
}

Terminal::Terminal() {
    setupSignalHandlers();
    enableRawMode();
    atexit([](){
        if (raw_enabled) {
            tcsetattr(STDIN_FILENO, TCSANOW, &orig_term);
            raw_enabled = false;
            write(STDOUT_FILENO, "\033[?25h", 6);
        }
    });
}

Terminal::~Terminal() {
    disableRawMode();
}

void Terminal::enableRawMode() {
    if (tcgetattr(STDIN_FILENO, &orig_term) == -1) {
        perror("tcgetattr");
        exit(1);
    }
    struct termios raw = orig_term;
    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_iflag &= ~(IXON);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) == -1) {
        perror("tcsetattr");
        exit(1);
    }
    raw_enabled = true;
    write(STDOUT_FILENO, "\033[?25l", 6);
}

void Terminal::disableRawMode() {
    if (raw_enabled) {
        tcsetattr(STDIN_FILENO, TCSANOW, &orig_term);
        raw_enabled = false;
        write(STDOUT_FILENO, "\033[?25h", 6);
    }
}

void Terminal::getWindowSize(int& width, int& height) {
    winsize wsize;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &wsize) == -1) {
        perror("ioctl");
        width = -1;
        height = -1;
    } else {
        width = wsize.ws_col;
        height = wsize.ws_row;
    }
}

bool Terminal::isResized() {
    return resized.load();
}

void Terminal::setResized(bool value) {
    resized.store(value);
}
