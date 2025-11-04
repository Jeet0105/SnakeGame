#ifndef GAME_H
#define GAME_H

#include "snake.h"
#include "renderer.h"
#include "utils.h"
#include <random>

enum GameState { MENU, PLAYING, PAUSED, GAME_OVER };

class Game {
private:
    static const int BOARD_WIDTH = 30;
    static const int BOARD_HEIGHT = 30;
    
    Snake snake;
    Renderer renderer;
    Position food;
    Position specialFood;  // New: Special food position
    int score;
    int highScore;
    GameState state;
    FrameRateController frameController;
    std::mt19937 rng;
    
    bool specialFoodActive;  // New: Track if special food is active
    int specialFoodTimer;    // New: Timer for special food
    int specialFoodDuration; // New: Total duration for special food (for timeline)
    int specialFoodPoints;   // New: Points for special food
    
    void generateFood();
    void generateSpecialFood();  // New: Generate special food
    void handleInput();
    void update();
    void resetGame();
    void saveHighScore();
    void loadHighScore();
    
public:
    Game();
    void run();
};

#endif