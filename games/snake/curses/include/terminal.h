#pragma once

#include <atomic>

class Terminal {
public:
    Terminal();
    ~Terminal();

    void enableRawMode();
    void disableRawMode();
    void getWindowSize(int& width, int& height);
    static bool isResized();
    static void setResized(bool value);

private:
    void setupSignalHandlers();
    static std::atomic<bool> resized;
};
