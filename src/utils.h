#ifndef UTILS_H
#define UTILS_H

#include <chrono>

#ifdef _WIN32
    #include <windows.h>
    #include <conio.h>
#else
    #include <termios.h>
    #include <unistd.h>
    #include <sys/ioctl.h>
    #include <fcntl.h>
#endif

// Basic utility functions
void sleepMs(int milliseconds);
void highResSleepMs(int milliseconds);
int kbhit();
int getch();
void getTerminalSize(int& width, int& height);
void clearScreen();
void clearInputBuffer();
int inputWithTimeout(int timeoutMs);

// Frame rate controller class
class FrameRateController {
private:
    int targetFPS;
    double frameTime; // in milliseconds
    std::chrono::steady_clock::time_point lastFrame;
    
    // FPS calculation
    int frameCount = 0;
    int currentFPS = 0;
    std::chrono::steady_clock::time_point fpsStartTime;
    
public:
    FrameRateController(int fps = 60);
    void startFrame();
    void endFrame();
    int getFPS() const;
    
    // Public getter and setter for targetFPS
    int getTargetFPS() const { return targetFPS; }
    void setTargetFPS(int fps) { 
        targetFPS = fps; 
        frameTime = 1000.0 / fps;
    }
};

#endif