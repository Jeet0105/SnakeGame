#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "src/snake.h"

TEST_CASE("Rule 1: When the snake moves, its head shifts by one unit in its current direction.") {
    Snake snake(10, 10, 20, 20);
    snake.changeDirection(RIGHT); // Starts moving right
    snake.move();
    CHECK(snake.getHead().x == 11);
    CHECK(snake.getHead().y == 10);
    
    snake.changeDirection(DOWN);
    snake.move();
    CHECK(snake.getHead().x == 11);
    CHECK(snake.getHead().y == 11);
}

TEST_CASE("Rule 2: The snake ignores direction changes that are exactly opposite to its current direction.") {
    Snake snake(10, 10, 20, 20);
    // Assuming initial direction is RIGHT. Let's move down then try to move up.
    snake.changeDirection(DOWN);
    snake.move(); // Head at 10, 11
    snake.changeDirection(UP); // Opposite direction, should be ignored
    snake.move(); // Should continue moving DOWN
    CHECK(snake.getHead().x == 10);
    CHECK(snake.getHead().y == 12);
}

TEST_CASE("Rule 3: If the snake moves out of the board boundaries, the move fails and returns false.") {
    Snake snake(1, 1, 5, 5);
    snake.changeDirection(UP); // Moving towards y=0
    bool res1 = snake.move();
    CHECK(res1 == true);
    CHECK(snake.getHead().y == 0);
    
    // Now move UP again, should hit boundary
    bool res2 = snake.move();
    CHECK(res2 == false);
}

#define private public
#define protected public
#include "src/game.h"

class TestableGame : public Game {
public:
    bool mockedKeyPressed = false;
    int mockedKey = 0;
    
    bool isKeyPressed() override { std::cout << "isKeyPressed called\n"; return mockedKeyPressed; }
    int getKey() override { std::cout << "getKey called\n"; return mockedKey; }
};

TEST_CASE("Rule 5: When the user presses 'p' during PLAYING, the game state changes to PAUSED.") {
    TestableGame game;
    game.state = PLAYING;
    game.mockedKeyPressed = true;
    game.mockedKey = 'p';
    game.handleInput();
    CHECK(game.state == PAUSED);
}
