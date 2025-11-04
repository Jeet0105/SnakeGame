#include "game.h"
#include "utils.h"
#include <iostream>
#include <random>
#include <fstream>
#include <algorithm>

Game::Game() 
    : snake(BOARD_WIDTH / 2, BOARD_HEIGHT / 2, BOARD_WIDTH, BOARD_HEIGHT),
      renderer(BOARD_WIDTH, BOARD_HEIGHT),
      score(0), highScore(0), state(PLAYING),
      frameController(10),
      specialFoodActive(false),
      specialFoodTimer(0),
      specialFoodPoints(30),
      specialFoodMaxTimer(50),
      rng(std::random_device{}()) {
    loadHighScore();
    generateFood();
    // Place 5-7 obstacles
    {
        std::uniform_int_distribution<int> obc(5, 7);
        generateObstacles(obc(rng));
    }
}

void Game::generateFood() {
    do {
        std::uniform_int_distribution<int> distX(0, BOARD_WIDTH - 1);
        std::uniform_int_distribution<int> distY(0, BOARD_HEIGHT - 1);
        food = Position(distX(rng), distY(rng));
    } while (snake.isOnPosition(food.x, food.y) ||
             std::find(obstacles.begin(), obstacles.end(), food) != obstacles.end());
}

void Game::generateSpecialFood() {
    // Only generate special food if it's not already active
    if (!specialFoodActive) {
        int attempts = 0;
        do {
            std::uniform_int_distribution<int> distX(0, BOARD_WIDTH - 1);
            std::uniform_int_distribution<int> distY(0, BOARD_HEIGHT - 1);
            specialFood = Position(distX(rng), distY(rng));
            attempts++;
            
            // Prevent infinite loop
            if (attempts > BOARD_WIDTH * BOARD_HEIGHT) {
                break;
            }
        } while (snake.isOnPosition(specialFood.x, specialFood.y) || 
                (specialFood.x == food.x && specialFood.y == food.y) ||
                std::find(obstacles.begin(), obstacles.end(), specialFood) != obstacles.end());
        
        specialFoodActive = true;
        specialFoodTimer = specialFoodMaxTimer; // 5 seconds at ~10 FPS
    }
}

void Game::generateObstacles(int count) {
    obstacles.clear();
    std::uniform_int_distribution<int> distX(0, BOARD_WIDTH - 1);
    std::uniform_int_distribution<int> distY(0, BOARD_HEIGHT - 1);
    int attempts = 0;
    while ((int)obstacles.size() < count && attempts < count * 20) {
        Position p(distX(rng), distY(rng));
        attempts++;
        if (snake.isOnPosition(p.x, p.y)) continue;
        if (p == food || (specialFoodActive && p == specialFood)) continue;
        if (std::find(obstacles.begin(), obstacles.end(), p) != obstacles.end()) continue;
        obstacles.push_back(p);
    }
}

void Game::handleInput() {
    if (!kbhit()) return;
    
    int key = getch();
    
    // Handle escape sequences for arrow keys (Linux/macOS)
    if (key == 27) {
        if (!kbhit()) return;
        int key2 = getch();
        if (key2 == 91) {
            if (!kbhit()) return;
            int key3 = getch();
            if (state == PLAYING) {
                switch (key3) {
                    case 65: snake.changeDirection(UP); break;
                    case 66: snake.changeDirection(DOWN); break;
                    case 67: snake.changeDirection(RIGHT); break;
                    case 68: snake.changeDirection(LEFT); break;
                }
            }
        }
        return;
    }
    
    // Handle regular keys
    if (key == 'w' || key == 'W' || key == 72) {
        if (state == PLAYING) snake.changeDirection(UP);
    }
    else if (key == 's' || key == 'S' || key == 80) {
        if (state == PLAYING) snake.changeDirection(DOWN);
    }
    else if (key == 'a' || key == 'A' || key == 75) {
        if (state == PLAYING) snake.changeDirection(LEFT);
    }
    else if (key == 'd' || key == 'D' || key == 77) {
        if (state == PLAYING) snake.changeDirection(RIGHT);
    }
    else if (key == 'p' || key == 'P') {
        if (state == PLAYING || state == PAUSED) {
            state = (state == PLAYING) ? PAUSED : PLAYING;
        }
    }
    else if (key == ' ' || key == 'r' || key == 'R') {
        if (state == GAME_OVER) {
            resetGame();
        } else if (state == PLAYING || state == PAUSED) {
            resetGame();
        }
    }
    else if (key == 'q' || key == 'Q') {
        saveHighScore();
        state = GAME_OVER;
    }
}

void Game::update() {
    if (state != PLAYING) return;
    
    if (!snake.move()) {
        state = GAME_OVER;
        if (score > highScore) {
            highScore = score;
            saveHighScore();
        }
        return;
    }
    // Obstacle collision
    if (std::find(obstacles.begin(), obstacles.end(), snake.getHead()) != obstacles.end()) {
        state = GAME_OVER;
        if (score > highScore) {
            highScore = score;
            saveHighScore();
        }
        return;
    }
    
    // Check if snake ate regular food
    if (snake.getHead().x == food.x && snake.getHead().y == food.y) {
        snake.grow();
        score += 10;
        generateFood();
        
        // Check if we should spawn special food (every 30 points)
        if (score % 30 == 0 && score > 0) {
            generateSpecialFood();
        }
    }
    
    // Check if snake ate special food
    if (specialFoodActive && 
        snake.getHead().x == specialFood.x && snake.getHead().y == specialFood.y) {
        snake.grow();
        score += specialFoodPoints;
        specialFoodActive = false;
        specialFoodTimer = 0;
        
        // Add bonus growth for special food
        snake.grow(); // Extra segment for special food
        
        // Update high score if needed
        if (score > highScore) {
            highScore = score;
        }
    }
    
    // Update special food timer
    if (specialFoodActive) {
        specialFoodTimer--;
        if (specialFoodTimer <= 0) {
            specialFoodActive = false; // Special food disappears
            specialFoodTimer = 0;
        }
    }
}

void Game::resetGame() {
    snake = Snake(BOARD_WIDTH / 2, BOARD_HEIGHT / 2, BOARD_WIDTH, BOARD_HEIGHT);
    score = 0;
    state = PLAYING;
    specialFoodActive = false;
    specialFoodTimer = 0;
    generateFood();
    {
        std::uniform_int_distribution<int> obc(5, 7);
        generateObstacles(obc(rng));
    }
}

void Game::saveHighScore() {
    std::ofstream file("highscore.txt");
    if (file.is_open()) {
        file << highScore;
        file.close();
    }
}

void Game::loadHighScore() {
    std::ifstream file("highscore.txt");
    if (file.is_open()) {
        file >> highScore;
        file.close();
    }
}

void Game::run() {
    bool running = true;
    
    while (running) {
        frameController.startFrame();
        
        handleInput();
        
        switch (state) {
            case PLAYING:
                update();
                renderer.render(snake, food, specialFood, specialFoodActive, specialFoodTimer, specialFoodMaxTimer, score, highScore, false, obstacles);
                break;
            case PAUSED:
                renderer.render(snake, food, specialFood, specialFoodActive, specialFoodTimer, specialFoodMaxTimer, score, highScore, true, obstacles);
                break;
            case GAME_OVER:
                renderer.renderGameOver(score, highScore);
                if (kbhit()) {
                    int key = getch();
                    if (key == ' ' || key == 'r' || key == 'R') {
                        resetGame();
                    } else if (key == 'q' || key == 'Q') {
                        running = false;
                    }
                }
                break;
        }
        
        frameController.endFrame();
        
        // Game speed
        sleepMs(150);
    }
    
    saveHighScore();
}