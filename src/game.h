#ifndef GAME_H
#define GAME_H

#include "snake.h"
#include "renderer.h"
#include "utils.h"
#include <random>

enum GameState { MENU, PLAYING, PAUSED, GAME_OVER };

class Game {
private:
    static const int BOARD_WIDTH = 12;
    static const int BOARD_HEIGHT = 8;
    
    Snake snake;
    Renderer renderer;
    Position food;
    int score;
    int highScore;
    GameState state;
    FrameRateController frameController;
    std::mt19937 rng;
    
    void generateFood();
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