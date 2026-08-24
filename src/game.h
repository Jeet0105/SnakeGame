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
    
    std::vector<Snake> snakes; // Centralized collection: number of snakes defined in exactly one place
    std::vector<int> scores;  // Scores per snake
    Renderer renderer;
    Position food;
    Position specialFood;  // Special food position
    std::vector<Position> obstacles; // Obstacles
    int score;
    int highScore;
    GameState state;
    FrameRateController frameController;
    std::mt19937 rng;
    
    bool specialFoodActive;  // Track if special food is active
    int specialFoodTimer;    // Timer for special food
    int specialFoodPoints;   // Points for special food
    int specialFoodMaxTimer; // Max timer for special food
    
    std::string playerName;
    std::string highScoreName;
    int specialFoodCount;
    
    void generateFood();
    void generateSpecialFood();  // Generate special food
    void generateObstacles(int count); // Generate obstacles
    void handleInput();
    void update();
    void resetGame();
    void saveHighScore();
    void loadHighScore();
    
    void askPlayerName();
    void saveScoreEntry();
    
public:
    Game();
    void run();
};

#endif