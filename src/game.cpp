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
    : renderer(BOARD_WIDTH, BOARD_HEIGHT),
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
    snakes.push_back(Snake(BOARD_WIDTH / 2, BOARD_HEIGHT / 2, BOARD_WIDTH, BOARD_HEIGHT));
    scores.push_back(0);
    loadHighScore();
    generateFood();
    {
        std::uniform_int_distribution<int> obc(5, 7);
        generateObstacles(obc(rng));
    }
}

void Game::generateFood() {
    bool occupied = false;
    do {
        std::uniform_int_distribution<int> distX(0, BOARD_WIDTH - 1);
        std::uniform_int_distribution<int> distY(0, BOARD_HEIGHT - 1);
        food = Position(distX(rng), distY(rng));
        
        occupied = false;
        for (const auto& s : snakes) {
            if (s.isOnPosition(food.x, food.y)) { occupied = true; break; }
        }
        if (!occupied && std::find(obstacles.begin(), obstacles.end(), food) != obstacles.end()) {
            occupied = true;
        }
    } while (occupied);
}

void Game::generateSpecialFood() {
    if (!specialFoodActive) {
        int attempts = 0;
        bool occupied = false;
        do {
            std::uniform_int_distribution<int> distX(0, BOARD_WIDTH - 1);
            std::uniform_int_distribution<int> distY(0, BOARD_HEIGHT - 1);
            specialFood = Position(distX(rng), distY(rng));
            attempts++;
            
            if (attempts > BOARD_WIDTH * BOARD_HEIGHT) {
                break;
            }
            
            occupied = false;
            for (const auto& s : snakes) {
                if (s.isOnPosition(specialFood.x, specialFood.y)) { occupied = true; break; }
            }
            if (specialFood.x == food.x && specialFood.y == food.y) occupied = true;
            if (std::find(obstacles.begin(), obstacles.end(), specialFood) != obstacles.end()) occupied = true;
        } while (occupied);
        
        specialFoodActive = true;
        specialFoodTimer = specialFoodMaxTimer;
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
        
        bool occupied = false;
        for (const auto& s : snakes) {
            if (s.isOnPosition(p.x, p.y)) { occupied = true; break; }
        }
        if (occupied) continue;
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
            if (state == PLAYING && !snakes.empty()) {
                switch (key3) {
                    case 65: snakes[0].changeDirection(UP); break;
                    case 66: snakes[0].changeDirection(DOWN); break;
                    case 67: snakes[0].changeDirection(RIGHT); break;
                    case 68: snakes[0].changeDirection(LEFT); break;
                }
            }
        }
        return;
    }
    
    // Handle regular keys for snake[0]
    if (state == PLAYING && !snakes.empty()) {
        if (key == 'w' || key == 'W' || key == 72) {
            snakes[0].changeDirection(UP);
        }
        else if (key == 's' || key == 'S' || key == 80) {
            snakes[0].changeDirection(DOWN);
        }
        else if (key == 'a' || key == 'A' || key == 75) {
            snakes[0].changeDirection(LEFT);
        }
        else if (key == 'd' || key == 'D' || key == 77) {
            snakes[0].changeDirection(RIGHT);
        }
    }
    
    if (key == 'p' || key == 'P') {
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
    
    for (size_t i = 0; i < snakes.size(); ++i) {
        if (!snakes[i].move()) {
            state = GAME_OVER;
            if (scores[i] > highScore) {
                highScore = scores[i];
                highScoreName = playerName;
                saveHighScore();
            }
            score = scores[i];
            saveScoreEntry();
            return;
        }
        
        // Obstacle collision
        if (std::find(obstacles.begin(), obstacles.end(), snakes[i].getHead()) != obstacles.end()) {
            state = GAME_OVER;
            if (scores[i] > highScore) {
                highScore = scores[i];
                highScoreName = playerName;
                saveHighScore();
            }
            score = scores[i];
            saveScoreEntry();
            return;
        }
        
        // Check if snake ate regular food
        if (snakes[i].getHead().x == food.x && snakes[i].getHead().y == food.y) {
            snakes[i].grow();
            scores[i] += 10;
            if (scores[i] > score) score = scores[i];
            generateFood();
            
            if (scores[i] % 30 == 0 && scores[i] > 0) {
                generateSpecialFood();
            }
        }
        
        // Check if snake ate special food
        if (specialFoodActive && 
            snakes[i].getHead().x == specialFood.x && snakes[i].getHead().y == specialFood.y) {
            snakes[i].grow();
            snakes[i].grow();
            scores[i] += specialFoodPoints;
            if (scores[i] > score) score = scores[i];
            specialFoodActive = false;
            specialFoodTimer = 0;
            specialFoodCount++;
            
            if (scores[i] > highScore) {
                highScore = scores[i];
                highScoreName = playerName;
            }
        }
    }
    
    // Update special food timer
    if (specialFoodActive) {
        specialFoodTimer--;
        if (specialFoodTimer <= 0) {
            specialFoodActive = false;
            specialFoodTimer = 0;
        }
    }
}

void Game::resetGame() {
    snakes.clear();
    snakes.push_back(Snake(BOARD_WIDTH / 2, BOARD_HEIGHT / 2, BOARD_WIDTH, BOARD_HEIGHT));
    scores.clear();
    scores.push_back(0);
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
        std::string firstLine;
        if (std::getline(file, firstLine)) {
            try {
                size_t idx = 0;
                int val = std::stoi(firstLine, &idx);
                if (idx == firstLine.size()) {
                    highScore = val;
                    highScoreName = "Anonymous";
                } else {
                    highScoreName = firstLine;
                    int hs = 0;
                    file >> hs;
                    if (file) highScore = hs;
                }
            } catch (...) {
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
    renderer.renderMenu();
    
    while (running) {
        frameController.startFrame();
        
        handleInput();
        
        switch (state) {
            case MENU:
                break;
            case PLAYING:
                update();
                renderer.render(snakes, food, specialFood, specialFoodActive, specialFoodTimer, specialFoodMaxTimer, scores, highScore, false, obstacles, specialFoodCount);
                break;
            case PAUSED:
                renderer.render(snakes, food, specialFood, specialFoodActive, specialFoodTimer, specialFoodMaxTimer, scores, highScore, true, obstacles, specialFoodCount);
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
        sleepMs(150);
    }
    
    saveHighScore();
}