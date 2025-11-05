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
                     const std::vector<Position>& obstacles,
                     int specialFoodCount) {
    clearBuffer();
    
    static bool first = true;
    if (first) {
        std::cout << "\033[2J\033[?25l"; // clear once and hide cursor
        first = false;
    }
    
    int bufferY = 0;
    const int leftPad = 2;
    const std::string pad(leftPad, ' ');
    
    // Game title with high score (padded)
    std::stringstream title;
    title << pad << "🐍 S N A K E  G A M E 🐍    🏆 High: " << highScore;
    drawToBuffer(0, bufferY++, title.str() + "        ");
    
    // Top border
    std::string topBorder = pad + "🔶";
    for (int i = 0; i < boardWidth; i++) topBorder += "➖";
    topBorder += "🔶";
    drawToBuffer(0, bufferY++, topBorder);
    
    // Game board with special food and obstacles
    for (int y = 0; y < boardHeight; y++) {
        std::string line = pad + "🔹";
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
        // Right-side panel content aligned with board rows
        std::string sidebar = "  ";
        if (y == 0) {
            std::stringstream ss; ss << "📊 Score: " << score; sidebar += ss.str();
        } else if (y == 1) {
            std::stringstream ss; ss << "📏 Length: " << snake.getLength(); sidebar += ss.str();
        } else if (y == 2) {
            std::stringstream ss; ss << "🌟 Rare foods: " << specialFoodCount; sidebar += ss.str();
        } else if (y == 3) {
            sidebar += "🎮 WASD/Arrows";
        } else if (y == 4) {
            sidebar += "Legend:";
        } else if (y == 5) {
            sidebar += "🐍 head  🟢 body";
        } else if (y == 6) {
            sidebar += "🍎 food  🌟 special";
        } else if (y == 7) {
            sidebar += "🧱 obstacle";
        } else if (y == 9 && specialFoodActive && specialFoodTimer > 0 && specialFoodMaxTimer > 0) {
            int barWidth = 16;
            int t = specialFoodTimer; if (t < 0) t = 0; if (t > specialFoodMaxTimer) t = specialFoodMaxTimer;
            int filled = static_cast<int>((static_cast<double>(t) / specialFoodMaxTimer) * barWidth + 0.5);
            if (filled < 0) filled = 0; if (filled > barWidth) filled = barWidth;
            std::string bar = "⏳[";
            for (int i = 0; i < filled; ++i) bar += "█";
            for (int i = 0; i < barWidth - filled; ++i) bar += " ";
            bar += "]";
            sidebar += bar;
        }
        line += sidebar;
        drawToBuffer(0, bufferY++, line);
    }
    
    // Bottom border
    std::string bottomBorder = pad + "🔶";
    for (int i = 0; i < boardWidth; i++) bottomBorder += "➖";
    bottomBorder += "🔶";
    drawToBuffer(0, bufferY++, bottomBorder);
    
    // Footer info (padded)
    std::stringstream scoreInfo;
    scoreInfo << pad << "📊 Score: " << score << " | 📏 Length: " << snake.getLength();
    drawToBuffer(0, bufferY++, scoreInfo.str() + "          ");
    
    // Special food indicator + emoji timeline
    if (specialFoodActive && specialFoodTimer > 0) {
        std::string specialInfo = pad + "🌟 SPECIAL FOOD ACTIVE! +30 points!";
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
        drawToBuffer(0, bufferY++, pad + remain.str() + "      ");
    } else {
        // Clear the special food lines (two lines) with plenty of spaces to overwrite any previous content
        drawToBuffer(0, bufferY++, pad + "                                                                                                  ");
        drawToBuffer(0, bufferY++, pad + "                                                                                                  ");
    }
    
    std::string controls = pad + "🎮 Controls: WASD/Arrow Keys | ⏸️  P | 🔄 R | ❌ Q";
    drawToBuffer(0, bufferY++, controls + "    ");
    
    if (paused) {
        drawToBuffer(0, bufferY, pad + "⏸️  PAUSED - Press P to continue                    ");
    } else {
        // Clear the pause line when not paused
        drawToBuffer(0, bufferY, pad + "                                                  ");
    }
    
    // Move cursor to top instead of clearing screen (reduces blinking)
    std::cout << "\033[H";
    
    // Output the entire buffer, ensuring we erase leftovers when a line becomes shorter
    static std::vector<size_t> lastWidths;
    if (lastWidths.size() != screenBuffer.size()) lastWidths.assign(screenBuffer.size(), 0);
    for (size_t i = 0; i < screenBuffer.size(); ++i) {
        std::string out = screenBuffer[i];
        if (lastWidths[i] > out.length()) out.append(lastWidths[i] - out.length(), ' ');
        std::cout << out << "\n";
        lastWidths[i] = out.length();
    }
    
    std::cout.flush();
}

void Renderer::renderGameOver(int score, int highScore) {
    // Clear screen for game over (not every frame)
    clearScreen();
    std::cout << "\n💀 GAME OVER! 💀\n";
    std::cout << "🏆 Final Score: " << score << " | High Score: " << highScore << "\n";
    std::cout << "🔄 Press SPACE or R to return to menu | ❌ Q to quit\n";
    std::cout.flush();
}

void Renderer::renderMenu() {
    clearScreen();
    std::cout << "🐍 === SNAKE GAME === 🐍\n";
    std::cout << "1. 🎮 Start Game\n";
    std::cout << "2. 🏆 Highest Score\n";
    std::cout << "3. ❌ Exit\n";
    std::cout << "🎯 Select option: ";
    std::cout.flush();
}

void Renderer::renderHighScore(const std::string& name, int highScore) {
    clearScreen();
    std::cout << "🏆 Highest Score\n";
    std::cout << "👤 Player: " << (name.empty() ? std::string("Anonymous") : name) << "\n";
    std::cout << "📊 Score : " << highScore << "\n\n";
    std::cout << "↩️  Press any key to return to menu...";
    std::cout.flush();
}