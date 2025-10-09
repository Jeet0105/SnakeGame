#ifndef SNAKE_H
#define SNAKE_H

#include <deque>

struct Position {
    int x, y;
    Position(int x = 0, int y = 0) : x(x), y(y) {}
    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
    bool operator!=(const Position& other) const {
        return !(*this == other);
    }
};

enum Direction { UP, DOWN, LEFT, RIGHT };

class Snake {
private:
    std::deque<Position> body;
    Direction direction;
    int boardWidth, boardHeight;

public:
    Snake(int startX, int startY, int boardWidth, int boardHeight);
    
    void changeDirection(Direction newDir);
    bool move();
    void grow();
    bool checkCollision() const;
    bool isOnPosition(int x, int y) const;
    
    const std::deque<Position>& getBody() const { return body; }
    Position getHead() const { return body.front(); }
    Position getTail() const { return body.back(); }
    int getLength() const { return body.size(); }
};

#endif