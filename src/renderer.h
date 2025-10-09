#ifndef RENDERER_H
#define RENDERER_H

#include "snake.h"
#include <string>
#include <vector>

class Renderer {
private:
    int boardWidth, boardHeight;
    std::vector<std::string> screenBuffer;
    
    void clearBuffer();
    void drawToBuffer(int x, int y, const std::string& content);
    
public:
    Renderer(int width, int height);
    
    // Make sure these match the implementation
    void render(const Snake& snake, const Position& food, int score, int highScore, bool paused);
    void renderGameOver(int score, int highScore);
    void renderMenu();
};

#endif