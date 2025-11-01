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

void Renderer::render(const Snake& snake, const Position& food, const Position& specialFood, 
                     bool specialFoodActive, int score, int highScore, bool paused) {
    clearBuffer();
    
    int bufferY = 0;
    
    // Game title with high score
    std::stringstream title;
    title << "🐍 SNAKE GAME 🐍 | 🏆 High: " << highScore;
    drawToBuffer(0, bufferY++, title.str() + "        ");
    
    // Top border
    std::string topBorder = "🔶";
    for (int i = 0; i < boardWidth; i++) topBorder += "➖";
    topBorder += "🔶";
    drawToBuffer(0, bufferY++, topBorder);
    
    // Game board with special food
    for (int y = 0; y < boardHeight; y++) {
        std::string line = "🔹";
        for (int x = 0; x < boardWidth; x++) {
            if (snake.getHead().x == x && snake.getHead().y == y) {
                line += "🐍";
            } else if (snake.isOnPosition(x, y)) {
                line += "🟢";
            } else if (specialFoodActive && specialFood.x == x && specialFood.y == y) {
                line += "🌟";  // Special food - star emoji
            } else if (food.x == x && food.y == y) {
                line += "🍎";
            } else {
                line += "  ";
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
    scoreInfo << "📊 Score: " << score << " | 📏 Length: " << snake.getLength();
    drawToBuffer(0, bufferY++, scoreInfo.str() + "          ");
    
    // Special food indicator
    if (specialFoodActive) {
        std::string specialInfo = "🌟 SPECIAL FOOD ACTIVE! +30 points!";
        drawToBuffer(0, bufferY++, specialInfo + "      ");
    } else {
        // Clear the special food line
        drawToBuffer(0, bufferY++, "                                    ");
    }
    
    std::string controls = "🎮 Controls: WASD/Arrow Keys | ⏸️  P | 🔄 R | ❌ Q";
    drawToBuffer(0, bufferY++, controls + "    ");
    
    if (paused) {
        drawToBuffer(0, bufferY, "⏸️  PAUSED - Press P to continue                    ");
    } else {
        // Clear the pause line when not paused
        drawToBuffer(0, bufferY, "                                                    ");
    }
    
    // Move cursor to top instead of clearing screen (reduces blinking)
    std::cout << "\033[H";
    
    // Output the entire buffer
    for (const auto& line : screenBuffer) {
        if (!line.empty()) {
            std::cout << line << "\n";
        }
    }
    
    std::cout.flush();
}

void Renderer::renderGameOver(int score, int highScore) {
    // Clear screen for game over (not every frame)
    clearScreen();
    std::cout << "\n💀 GAME OVER! 💀\n";
    std::cout << "🏆 Final Score: " << score << " | High Score: " << highScore << "\n";
    std::cout << "🔄 Press SPACE or R to restart | ❌ Q to quit\n";
    std::cout.flush();
}

void Renderer::renderMenu() {
    clearScreen();
    std::cout << "🐍 === SNAKE GAME === 🐍\n";
    std::cout << "1. 🎮 Start Game\n";
    std::cout << "2. ❌ Quit\n";
    std::cout << "🎯 Select option: ";
    std::cout.flush();
}