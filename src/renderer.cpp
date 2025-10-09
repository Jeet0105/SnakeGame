#include "renderer.h"
#include "utils.h"
#include <iostream>
#include <sstream>

Renderer::Renderer(int width, int height) 
    : boardWidth(width), boardHeight(height) {
    screenBuffer.resize(boardHeight + 6);
}

void Renderer::clearBuffer() {
    for (auto& line : screenBuffer) {
        line.clear();
    }
}

void Renderer::drawToBuffer(int x, int y, const std::string& content) {
    if (y >= 0 && y < screenBuffer.size()) {
        if (x + content.length() > screenBuffer[y].length()) {
            screenBuffer[y].resize(x + content.length(), ' ');
        }
        screenBuffer[y].replace(x, content.length(), content);
    }
}

void Renderer::render(const Snake& snake, const Position& food, int score, int highScore, bool paused) {
    // Don't clear the screen - use cursor positioning instead
    std::cout << "\033[0;0H"; // Move cursor to top-left without clearing
    
    int bufferY = 0;
    
    // Game title with high score
    std::stringstream title;
    title << "🐍 SNAKE GAME 🐍 | 🏆 High: " << highScore << "                    "; // Extra spaces to clear previous content
    drawToBuffer(0, bufferY++, title.str());
    
    // Top border
    std::string topBorder = "🔶";
    for (int i = 0; i < boardWidth; i++) topBorder += "➖";
    topBorder += "🔶";
    drawToBuffer(0, bufferY++, topBorder);
    
    // Game board
    const auto& body = snake.getBody();
    Position head = snake.getHead();
    
    for (int y = 0; y < boardHeight; y++) {
        std::string line = "🔹";
        for (int x = 0; x < boardWidth; x++) {
            if (head.x == x && head.y == y) {
                line += "🐍";
            } else if (food.x == x && food.y == y) {
                line += "🍎";
            } else {
                bool isBodyPart = false;
                for (size_t i = 1; i < body.size(); i++) {
                    if (body[i].x == x && body[i].y == y) {
                        line += "🟢";
                        isBodyPart = true;
                        break;
                    }
                }
                if (!isBodyPart) {
                    line += "  ";
                }
            }
        }
        line += "🔹";
        drawToBuffer(0, bufferY++, line);
    }
    
    // Bottom border
    std::string bottomBorder = "🔶";
    for (int i = 0; i < boardWidth; i++) bottomBorder += "➖";
    bottomBorder += "🔶";
    drawToBuffer(0, bufferY++, bottomBorder);
    
    // UI information
    std::stringstream scoreInfo;
    scoreInfo << "📊 Score: " << score << " | 📏 Length: " << snake.getLength() << "               ";
    drawToBuffer(0, bufferY++, scoreInfo.str());
    
    std::string controls = "🎮 Controls: WASD/Arrow Keys | ⏸️  P | 🔄 R | ❌ Q          ";
    drawToBuffer(0, bufferY++, controls);
    
    if (paused) {
        drawToBuffer(0, bufferY, "⏸️  PAUSED - Press P to continue                    ");
    } else {
        // Clear the pause line if not paused
        drawToBuffer(0, bufferY, "                                                    ");
    }
    
    // Output the entire buffer
    for (const auto& line : screenBuffer) {
        if (!line.empty()) {
            std::cout << line << "\n";
        }
    }
    
    // Clear any remaining lines from previous render
    for (int i = bufferY + 1; i < screenBuffer.size(); i++) {
        std::cout << "                                                                  \n";
    }
    
    std::cout.flush();
}

void Renderer::renderGameOver(int score, int highScore) {
    // Clear screen only for game over (not every frame)
    clearScreen();
    std::cout << "\n";
    std::cout << "💀 GAME OVER! 💀\n";
    std::cout << "🏆 Final Score: " << score << " | High Score: " << highScore << "\n";
    std::cout << "\n";
    std::cout << "🔄 Press SPACE or R to restart\n";
    std::cout << "❌ Press Q to quit\n";
    std::cout.flush();
}

void Renderer::renderMenu() {
    clearScreen();
    std::cout << "========================================\n";
    std::cout << "           🐍 SNAKE GAME 🐍           \n";
    std::cout << "========================================\n";
    std::cout << "\n";
    std::cout << "1. 🎮 Start Game\n";
    std::cout << "2. ❌ Quit\n";
    std::cout << "\n";
    std::cout << "🎯 Select option: ";
    std::cout.flush();
}