#include "utils.h"
#include <iostream>
#include <chrono>
#include <thread>

// Basic sleep function
void sleepMs(int milliseconds) {
#ifdef _WIN32
    Sleep(milliseconds);
#else
    usleep(milliseconds * 1000);
#endif
}

// High-resolution sleep using std::this_thread
void highResSleepMs(int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

// Cross-platform keyboard hit detection
int kbhit() {
#ifdef _WIN32
    return _kbhit();
#else
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
#endif
}

// Cross-platform get character
int getch() {
#ifdef _WIN32
    return _getch();
#else
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
#endif
}

// Get terminal size
void getTerminalSize(int& width, int& height) {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#else
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    width = w.ws_col;
    height = w.ws_row;
#endif
}

// Clear screen function
void clearScreen() {
#ifdef _WIN32
    // On Windows, ensure UTF-8 output
    system("chcp 65001 > nul");
    system("cls");
#else
    system("clear");
#endif
}

// Input buffer clearing function
void clearInputBuffer() {
#ifdef _WIN32
    while (_kbhit()) {
        _getch();
    }
#else
    // For Linux/macOS, we flush the stdin buffer
    tcflush(STDIN_FILENO, TCIFLUSH);
#endif
}

// Non-blocking input check with timeout
int inputWithTimeout(int timeoutMs) {
    auto start = std::chrono::steady_clock::now();
    
    while (true) {
        if (kbhit()) {
            return getch();
        }
        
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
        
        if (elapsed.count() >= timeoutMs) {
            return -1; // Timeout
        }
        
        highResSleepMs(10); // Small sleep to prevent CPU spinning
    }
}

// FrameRateController implementation
FrameRateController::FrameRateController(int fps) 
    : targetFPS(fps), 
      frameTime(1000.0 / fps),
      lastFrame(std::chrono::steady_clock::now()),
      fpsStartTime(std::chrono::steady_clock::now()) {}

void FrameRateController::startFrame() {
    lastFrame = std::chrono::steady_clock::now();
}

void FrameRateController::endFrame() {
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastFrame);
    
    int sleepTime = static_cast<int>(frameTime) - elapsed.count();
    
    if (sleepTime > 0) {
        highResSleepMs(sleepTime);
    }
    
    // Update FPS calculation every second
    frameCount++;
    auto now = std::chrono::steady_clock::now();
    auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(now - fpsStartTime).count();
    
    if (elapsedSeconds >= 1) {
        currentFPS = frameCount;
        frameCount = 0;
        fpsStartTime = now;
    }
}

int FrameRateController::getFPS() const {
    return currentFPS;
}