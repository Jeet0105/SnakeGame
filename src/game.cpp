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
      snake2(3 * BOARD_WIDTH / 4, BOARD_HEIGHT / 2, BOARD_WIDTH, BOARD_HEIGHT),
      renderer(BOARD_WIDTH, BOARD_HEIGHT),
      score(0), score2(0), highScore(0), state(MENU),
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
    } while (snake.isOnPosition(food.x, food.y) || snake2.isOnPosition(food.x, food.y) ||
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
                snake2.isOnPosition(specialFood.x, specialFood.y) ||
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
        if (snake.isOnPosition(p.x, p.y) || snake2.isOnPosition(p.x, p.y)) continue;
        if (p == food || (specialFoodActive && p == specialFood)) continue;
        if (std::find(obstacles.begin(), obstacles.end(), p) != obstacles.end()) continue;
        obstacles.push_back(p);
    }
}

void Game::handleInput() {
    if (!kbhit()) return;

    int key = getch();

    if (state == MENU) {
        if (key == '1') {
            askPlayerName();
            resetGame();
            state = PLAYING;
        }
        else if (key == '2') {
            renderer.renderHighScore(highScoreName, highScore);
            (void)getch();
            renderer.renderMenu();
        }
        else if (key == '3' || key == 'q' || key == 'Q') {
            saveHighScore();
            std::exit(0);
        }

        return;
    }

    if (key == 27) {
        if (!kbhit()) return;

        int key2 = getch();

        if (key2 == 91) {
            if (!kbhit()) return;

            int key3 = getch();

            if (state == PLAYING) {
                switch (key3) {
                    case 65:
                        snake.changeDirection(UP);
                        break;

                    case 66:
                        snake.changeDirection(DOWN);
                        break;

                    case 67:
                        snake.changeDirection(RIGHT);
                        break;

                    case 68:
                        snake.changeDirection(LEFT);
                        break;
                }
            }
        }

        return;
    }

    if (key == 72) {
        if (state == PLAYING)
            snake.changeDirection(UP);
    }
    else if (key == 80) {
        if (state == PLAYING)
            snake.changeDirection(DOWN);
    }
    else if (key == 75) {
        if (state == PLAYING)
            snake.changeDirection(LEFT);
    }
    else if (key == 77) {
        if (state == PLAYING)
            snake.changeDirection(RIGHT);
    }

    else if (key == 'w' || key == 'W') {
        if (state == PLAYING)
            snake2.changeDirection(UP);
    }
    else if (key == 's' || key == 'S') {
        if (state == PLAYING)
            snake2.changeDirection(DOWN);
    }
    else if (key == 'a' || key == 'A') {
        if (state == PLAYING)
            snake2.changeDirection(LEFT);
    }
    else if (key == 'd' || key == 'D') {
        if (state == PLAYING)
            snake2.changeDirection(RIGHT);
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
        }
        else if (state == PLAYING || state == PAUSED) {
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

    bool p1Alive = snake.move();
    bool p2Alive = snake2.move();

    if (p1Alive && std::find(obstacles.begin(), obstacles.end(), snake.getHead()) != obstacles.end())
        p1Alive = false;
    if (p2Alive && std::find(obstacles.begin(), obstacles.end(), snake2.getHead()) != obstacles.end())
        p2Alive = false;

    if (p1Alive && p2Alive &&
        snake.getHead().x == snake2.getHead().x && snake.getHead().y == snake2.getHead().y) {
        p1Alive = false;
        p2Alive = false; 
    } else {
        if (p1Alive && snake2.isOnPosition(snake.getHead().x, snake.getHead().y)) p1Alive = false;
        if (p2Alive && snake.isOnPosition(snake2.getHead().x, snake2.getHead().y)) p2Alive = false;
    }

    if (!p1Alive || !p2Alive) {
        state = GAME_OVER;
        if (!p1Alive && !p2Alive) loserMessage = "Both players crashed!";
        else if (!p1Alive) loserMessage = "Player 1 lost!";
        else loserMessage = "Player 2 lost!";

        int bestScore = std::max(score, score2);
        if (bestScore > highScore) { highScore = bestScore; highScoreName = playerName; saveHighScore(); }
        saveScoreEntry();
        return;
    }

    bool p1AteFood = (snake.getHead().x == food.x && snake.getHead().y == food.y);
    bool p2AteFood = (snake2.getHead().x == food.x && snake2.getHead().y == food.y);
    if (p1AteFood) { snake.grow(); score += 10; }
    if (p2AteFood) { snake2.grow(); score2 += 10; }
    if (p1AteFood || p2AteFood) {
        generateFood();
        int bestScore = std::max(score, score2);
        if (bestScore % 30 == 0 && bestScore > 0) generateSpecialFood();
    }

    if (specialFoodActive) {
        bool p1AteSpecial = (snake.getHead().x == specialFood.x && snake.getHead().y == specialFood.y);
        bool p2AteSpecial = (snake2.getHead().x == specialFood.x && snake2.getHead().y == specialFood.y);
        if (p1AteSpecial) { snake.grow(); snake.grow(); score += specialFoodPoints; specialFoodCount++; }
        if (p2AteSpecial) { snake2.grow(); snake2.grow(); score2 += specialFoodPoints; specialFoodCount++; }
        if (p1AteSpecial || p2AteSpecial) { specialFoodActive = false; specialFoodTimer = 0; }
        int bestScore = std::max(score, score2);
        if (bestScore > highScore) { highScore = bestScore; highScoreName = playerName; }
    }

    if (specialFoodActive) {
        specialFoodTimer--;
        if (specialFoodTimer <= 0) { specialFoodActive = false; specialFoodTimer = 0; }
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
                renderer.render(snake, snake2, food, specialFood, specialFoodActive, specialFoodTimer,specialFoodMaxTimer, score, score2, highScore, false, obstacles, specialFoodCount);
            case PAUSED:
                renderer.render(snake, snake2, food, specialFood, specialFoodActive, specialFoodTimer,specialFoodMaxTimer, score, score2, highScore, true, obstacles, specialFoodCount);
                break;
            case GAME_OVER:
                renderer.renderGameOver(score, score2, highScore, loserMessage);
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