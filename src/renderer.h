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
    
    // Updated to include special food
    void render(const Snake& snake, const Position& food, const Position& specialFood, 
                bool specialFoodActive, int score, int highScore, bool paused = false,
                const std::vector<Position>& obstacles = {});
    void renderGameOver(int score, int highScore);
    void renderMenu();
};

#endif