#include "game.h"
#include "utils.h"
#include <iostream>
#include <random>
#include <fstream>

Game::Game() 
    : snake(BOARD_WIDTH / 2, BOARD_HEIGHT / 2, BOARD_WIDTH, BOARD_HEIGHT),
      renderer(BOARD_WIDTH, BOARD_HEIGHT),
      score(0), highScore(0), state(PLAYING),
      frameController(5), // 5 FPS for snake game
      rng(std::random_device{}()) {
    loadHighScore();
    generateFood();
}

void Game::generateFood() {
    int attempts = 0;
    do {
        std::uniform_int_distribution<int> distX(0, BOARD_WIDTH - 1);
        std::uniform_int_distribution<int> distY(0, BOARD_HEIGHT - 1);
        food = Position(distX(rng), distY(rng));
        attempts++;
        // Prevent infinite loop
        if (attempts > BOARD_WIDTH * BOARD_HEIGHT) {
            // Try to find any available position
            for (int y = 0; y < BOARD_HEIGHT; y++) {
                for (int x = 0; x < BOARD_WIDTH; x++) {
                    if (!snake.isOnPosition(x, y)) {
                        food = Position(x, y);
                        return;
                    }
                }
            }
            // If no space found, place at (0,0) as fallback
            food = Position(0, 0);
            break;
        }
    } while (snake.isOnPosition(food.x, food.y));
}

void Game::handleInput() {
    if (!kbhit()) return;
    
    int key = getch();
    
    // Handle escape sequences for arrow keys (Linux/macOS)
    if (key == 27) { // Escape character
        if (!kbhit()) return;
        int key2 = getch();
        if (key2 == 91) { // [
            if (!kbhit()) return;
            int key3 = getch();
            if (state == PLAYING) {
                switch (key3) {
                    case 65: snake.changeDirection(UP); break;    // Up arrow
                    case 66: snake.changeDirection(DOWN); break;  // Down arrow
                    case 67: snake.changeDirection(RIGHT); break; // Right arrow
                    case 68: snake.changeDirection(LEFT); break;  // Left arrow
                }
            }
        }
        return;
    }
    
    // Handle regular keys - using if-else to avoid duplicate values
    if (key == 'w' || key == 'W' || key == 72) { // Up arrow (Windows)
        if (state == PLAYING) snake.changeDirection(UP);
    }
    else if (key == 's' || key == 'S' || key == 80) { // Down arrow (Windows)
        if (state == PLAYING) snake.changeDirection(DOWN);
    }
    else if (key == 'a' || key == 'A' || key == 75) { // Left arrow (Windows)
        if (state == PLAYING) snake.changeDirection(LEFT);
    }
    else if (key == 'd' || key == 'D' || key == 77) { // Right arrow (Windows)
        if (state == PLAYING) snake.changeDirection(RIGHT);
    }
    else if (key == 'p' || key == 'P') {
        if (state == PLAYING || state == PAUSED) {
            state = (state == PLAYING) ? PAUSED : PLAYING;
        }
    }
    else if (key == ' ' || key == 'r' || key == 'R') { // Space or R to restart
        if (state == GAME_OVER) {
            resetGame();
        } else if (state == PLAYING || state == PAUSED) {
            // Allow restart during gameplay too
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
    
    // Check if snake ate food
    if (snake.getHead().x == food.x && snake.getHead().y == food.y) {
        snake.grow();
        score += 10;
        
        // Increase speed slightly as score increases
        // Remove the FPS adjustment for now since we don't have getter/setter
        // if (score % 50 == 0 && frameController.getTargetFPS() < 20) {
        //     frameController.setTargetFPS(frameController.getTargetFPS() + 1);
        // }
        
        generateFood();
    }
}

void Game::resetGame() {
    snake = Snake(BOARD_WIDTH / 2, BOARD_HEIGHT / 2, BOARD_WIDTH, BOARD_HEIGHT);
    score = 0;
    state = PLAYING;
    // frameController.setTargetFPS(10); // Remove for now
    generateFood();
    
    // Clear any leftover input
    // clearInputBuffer(); // Remove for now - function doesn't exist
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
    
    // Clear any initial input buffer
    // clearInputBuffer(); // Remove for now
    
    while (running) {
        frameController.startFrame();
        
        handleInput();
        
        // Check if we should exit
        if (state == GAME_OVER) {
            // Check for quit command
            if (kbhit()) {
                int key = getch();
                if (key == 'q' || key == 'Q') {
                    running = false;
                    continue;
                }
            }
        }
        
        switch (state) {
            case PLAYING:
                update();
                renderer.render(snake, food, score, highScore, false);
                break;
            case PAUSED:
                renderer.render(snake, food, score, highScore, true);
                break;
            case GAME_OVER:
                renderer.renderGameOver(score, highScore);
                // Check for restart input with small delay to prevent instant restart
                sleepMs(600);
                if (kbhit()) {
                    int key = getch();
                    if (key == ' ' || key == 'r' || key == 'R') {
                        resetGame();
                    }
                }
                break;
        }
        
        frameController.endFrame();
    }
    
    // Save high score when exiting
    saveHighScore();
}