#include "renderer.h"
#include "utils.h"
#include <iostream>
#include <sstream>

Renderer::Renderer(int width, int height) 
    : boardWidth(width), boardHeight(height) {
    screenBuffer.resize(boardHeight + 7);
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
                     bool specialFoodActive, int specialFoodTimer, int specialFoodMaxTimer,
                     int score, int highScore, bool paused,
                     const std::vector<Position>& obstacles) {
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
    
    // Game board with special food and obstacles
    for (int y = 0; y < boardHeight; y++) {
        std::string line = "🔹";
        for (int x = 0; x < boardWidth; x++) {
            bool isObstacle = false;
            for (const auto& ob : obstacles) {
                if (ob.x == x && ob.y == y) { isObstacle = true; break; }
            }
            if (snake.getHead().x == x && snake.getHead().y == y) {
                line += "🐍";
            } else if (snake.isOnPosition(x, y)) {
                line += "🟢";
            } else if (isObstacle) {
                line += "🧱"; // obstacle
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
    // Legend line to confirm obstacles are active
    drawToBuffer(0, bufferY++, std::string("Legend: 🐍 head 🟢 body 🍎 food 🌟 special 🧱 obstacle") + "    ");
    
    // Special food indicator + emoji timeline
    if (specialFoodActive && specialFoodTimer > 0) {
        std::string specialInfo = "🌟 SPECIAL FOOD ACTIVE! +30 points!";
        drawToBuffer(0, bufferY++, specialInfo + "      ");

        int barWidth = 30;
        int clampedMax = specialFoodMaxTimer > 0 ? specialFoodMaxTimer : 1;
        if (specialFoodTimer < 0) specialFoodTimer = 0;
        if (specialFoodTimer > clampedMax) specialFoodTimer = clampedMax;
        double fraction = static_cast<double>(specialFoodTimer) / static_cast<double>(clampedMax);
        int filled = static_cast<int>(fraction * barWidth + 0.5);
        if (filled < 0) filled = 0;
        if (filled > barWidth) filled = barWidth;

        std::string bar = "⏳[";
        for (int i = 0; i < filled; ++i) bar += "█";
        for (int i = 0; i < barWidth - filled; ++i) bar += " ";
        bar += "] ";
        std::stringstream remain;
        remain << bar << specialFoodTimer << "/" << clampedMax;
        drawToBuffer(0, bufferY++, remain.str() + "      ");
    } else {
        // Clear the special food lines (two lines) with plenty of spaces to overwrite any previous content
        drawToBuffer(0, bufferY++, "                                                                                                    ");
        drawToBuffer(0, bufferY++, "                                                                                                    ");
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