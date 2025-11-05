#ifndef GAME_H
#define GAME_H

#include "snake.h"
#include "renderer.h"
#include "utils.h"
#include <random>
#include <vector>

enum GameState { MENU, PLAYING, PAUSED, GAME_OVER };

class Game {
private:
    static const int BOARD_WIDTH = 32;
    static const int BOARD_HEIGHT = 24;
    
    Snake snake;
    Renderer renderer;
    Position food;
    Position specialFood;  // New: Special food position
    std::vector<Position> obstacles; // New: Obstacles
    int score;
    int highScore;
    GameState state;
    FrameRateController frameController;
    std::mt19937 rng;
    
    bool specialFoodActive;  // New: Track if special food is active
    int specialFoodTimer;    // New: Timer for special food
    int specialFoodPoints;   // New: Points for special food
    int specialFoodMaxTimer; // New: Max timer for special food (for timeline)
    
    // New: player/high score names and rare food counter
    std::string playerName;
    std::string highScoreName;
    int specialFoodCount;
    
    void generateFood();
    void generateSpecialFood();  // New: Generate special food
    void generateObstacles(int count); // New: Generate obstacles
    void handleInput();
    void update();
    void resetGame();
    void saveHighScore();
    void loadHighScore();
    
    // New helpers
    void askPlayerName();
    void saveScoreEntry();
    
public:
    Game();
    void run();
};

#endif