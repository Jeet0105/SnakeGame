#include "snake.h"
#include <algorithm>

Snake::Snake(int startX, int startY, int width, int height) 
    : boardWidth(width), boardHeight(height) {
    body.push_back(Position(startX, startY));
    direction = RIGHT;
}

void Snake::changeDirection(Direction newDir) {
    // Prevent 180-degree turns
    if ((direction == UP && newDir != DOWN) ||
        (direction == DOWN && newDir != UP) ||
        (direction == LEFT && newDir != RIGHT) ||
        (direction == RIGHT && newDir != LEFT)) {
        direction = newDir;
    }
}

bool Snake::move() {
    Position newHead = body.front();
    
    switch (direction) {
        case UP:    newHead.y--; break;
        case DOWN:  newHead.y++; break;
        case LEFT:  newHead.x--; break;
        case RIGHT: newHead.x++; break;
    }
    
    // Check wall collision
    if (newHead.x < 0 || newHead.x >= boardWidth || 
        newHead.y < 0 || newHead.y >= boardHeight) {
        return false;
    }
    
    // Check self collision (skip head)
    for (auto it = body.begin() + 1; it != body.end(); ++it) {
        if (newHead == *it) {
            return false;
        }
    }
    
    // Add new head to front
    body.push_front(newHead);
    // Remove tail
    body.pop_back();
    
    return true;
}

void Snake::grow() {
    // Add a new segment at the tail position
    body.push_back(body.back());
}

bool Snake::checkCollision() const {
    const Position& head = body.front();
    
    // Wall collision
    if (head.x < 0 || head.x >= boardWidth || 
        head.y < 0 || head.y >= boardHeight) {
        return true;
    }
    
    // Self collision (skip head)
    for (auto it = body.begin() + 1; it != body.end(); ++it) {
        if (head == *it) {
            return true;
        }
    }
    
    return false;
}

bool Snake::isOnPosition(int x, int y) const {
    Position target(x, y);
    for (const auto& segment : body) {
        if (segment == target) {
            return true;
        }
    }
    return false;
}