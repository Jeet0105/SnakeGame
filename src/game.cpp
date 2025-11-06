#include "game.h"
#include "utils.h"
#include <iostream>
#include <random>
#include <fstream>
#include <algorithm>
#include <limits>
#include <string>
#include <cstdlib>

Game::Game() 
    : snake(BOARD_WIDTH / 2, BOARD_HEIGHT / 2, BOARD_WIDTH, BOARD_HEIGHT),
      renderer(BOARD_WIDTH, BOARD_HEIGHT),
      score(0), highScore(0), state(MENU),
      frameController(10),
      specialFoodActive(false),
      specialFoodTimer(0),
      specialFoodPoints(30),
      specialFoodMaxTimer(50),
      rng(std::random_device{}()),
      playerName(""),
      highScoreName(""),
      specialFoodCount(0) {
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
    
    // Menu input
    if (state == MENU) {
        if (key == '1') {
            askPlayerName();
            resetGame();
            state = PLAYING;
        } else if (key == '2') {
            renderer.renderHighScore(highScoreName, highScore);
            (void)getch();
            renderer.renderMenu();
        } else if (key == '3' || key == 'q' || key == 'Q') {
            saveHighScore();
            std::exit(0);
        }
        return;
    }
    
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
            state = MENU;
            renderer.renderMenu();
        } else if (state == PLAYING || state == PAUSED) {
            resetGame();
        }
    }
    else if (key == 'q' || key == 'Q') {
        saveHighScore();
        state = GAME_OVER;
        std::exit(0);
    }
}

void Game::update() {
    if (state != PLAYING) return;
    
    if (!snake.move()) {
        state = GAME_OVER;
        if (score > highScore) {
            highScore = score;
            highScoreName = playerName;
            saveHighScore();
        }
        saveScoreEntry();
        return;
    }
    // Obstacle collision
    if (std::find(obstacles.begin(), obstacles.end(), snake.getHead()) != obstacles.end()) {
        state = GAME_OVER;
        if (score > highScore) {
            highScore = score;
            highScoreName = playerName;
            saveHighScore();
        }
        saveScoreEntry();
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
        specialFoodCount++;
        
        // Add bonus growth for special food
        snake.grow(); // Extra segment for special food
        
        // Update high score if needed
        if (score > highScore) {
            highScore = score;
            highScoreName = playerName;
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
    specialFoodCount = 0;
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
        file << (highScoreName.empty() ? std::string("Anonymous") : highScoreName) << "\n";
        file << highScore << "\n";
        file.close();
    }
}

void Game::loadHighScore() {
    std::ifstream file("highscore.txt");
    if (file.is_open()) {
        // Try to read name on first line, score on second; support legacy single-number file
        std::string firstLine;
        if (std::getline(file, firstLine)) {
            // Check if firstLine is an integer
            try {
                size_t idx = 0;
                int val = std::stoi(firstLine, &idx);
                if (idx == firstLine.size()) {
                    // legacy: only score present
                    highScore = val;
                    highScoreName = "Anonymous";
                } else {
                    highScoreName = firstLine;
                    int hs = 0;
                    file >> hs;
                    if (file) highScore = hs;
                }
            } catch (...) {
                // not a pure number => treat as name then read score
                highScoreName = firstLine;
                int hs = 0;
                file >> hs;
                if (file) highScore = hs;
            }
        }
        file.close();
    }
}

void Game::askPlayerName() {
    clearScreen();
    std::cout << "Enter your name: ";
    std::cout.flush();
    if (std::cin.peek() == '\n') std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::getline(std::cin, playerName);
    if (playerName.empty()) playerName = "Player";
}

void Game::saveScoreEntry() {
    std::ofstream log("scores.txt", std::ios::app);
    if (log.is_open()) {
        log << (playerName.empty() ? std::string("Player") : playerName) << "," << score << "\n";
        log.close();
    }
}

void Game::run() {
    bool running = true;
    
    // show menu initially
    renderer.renderMenu();
    
    while (running) {
        frameController.startFrame();
        
        handleInput();
        
        switch (state) {
            case MENU:
                // handled in input
                break;
            case PLAYING:
                update();
                renderer.render(snake, food, specialFood, specialFoodActive, specialFoodTimer, specialFoodMaxTimer, score, highScore, false, obstacles, specialFoodCount);
                break;
            case PAUSED:
                renderer.render(snake, food, specialFood, specialFoodActive, specialFoodTimer, specialFoodMaxTimer, score, highScore, true, obstacles, specialFoodCount);
                break;
            case GAME_OVER:
                renderer.renderGameOver(score, highScore);
                if (kbhit()) {
                    int key = getch();
                    if (key == ' ' || key == 'r' || key == 'R') {
                        state = MENU;
                        renderer.renderMenu();
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