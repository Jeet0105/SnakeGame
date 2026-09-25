| | |
|---|---|
| Repository | https://github.com/Jeet0105/SnakeGame |
| Base tag | `lab4-base` at commit `3437ac2` |
| Pull request | https://github.com/Jeet0105/SnakeGame/pull/6 |

---

## 1. Five rules — [5]

Written before opening the source. Behaviour, with an observable outcome.

| # | Rule |
|---|---|
| 1 | When the snake moves, its head shifts by one unit in its current direction. |
| 2 | The snake ignores direction changes that are exactly opposite to its current direction. |
| 3 | If the snake moves out of the board boundaries, the move fails and returns false. |
| 4 | When a new game starts, the player's initial score is exactly zero. |
| 5 | When the user inputs 'Q' while playing, the game saves the high score and immediately exits the process. |

If you could not state one of your own game's rules without going to look, say which and
why. It costs no marks.

None.

---

## 2. What you could test, and what stopped you — [10]

No source changes in this part. Every `file:line` below is a line in `lab4-base`.

| # | Rule | Test written? | Blocking dependency (`file:line` + what it is) |
|---|---|---|---|
| 1 | When the snake moves, its head shifts by one unit in its current direction. | Yes | |
| 2 | The snake ignores direction changes that are exactly opposite to its current direction. | Yes | |
| 3 | If the snake moves out of the board boundaries, the move fails and returns false. | Yes | |
| 4 | When a new game starts, the player's initial score is exactly zero. | No | `game.h:22` — `score` is a private member with no getter, blocking observation of its value from outside. |
| 5 | When the user inputs 'Q' while playing, the game saves the high score and immediately exits the process. | No | `game.cpp:150` — `std::exit(0)` terminates the test process immediately. |

> **Rules testable without modifying the source: 3 / 5**

---

## 3. Coverage, and what it missed — [6]

| | |
|---|---|
| Line coverage | 54.35 % |
| Branch coverage | 35.85 % |
| Command used | `g++ -O0 --coverage -c tests.cpp; g++ -O0 --coverage -c src/snake.cpp; g++ --coverage tests.o snake.o -o run_tests.exe; .\run_tests.exe; gcov -b snake.cpp` |

**One rule that is executed by the suite but not verified by it:**

| | |
|---|---|
| Rule | When a new snake is created, its length starts at 1. |
| Line that runs | `snake.cpp:6` (adds starting position to body) |
| The assertion that is missing | `CHECK(snake.getLength() == 1);` |

---

## 4. The seam — [10]

| | |
|---|---|
| Rule made testable | When the user presses 'p' during PLAYING, the game state changes to PAUSED. |
| Commit 1 (seam) | 3caf01a |
| Commit 2 (test) | 985de51 |
| Seam kind | object |
| Enabling point | `Game::handleInput()` calling `isKeyPressed()` and `getKey()` |
| What production code gave up | Slightly increased memory and CPU overhead due to virtual method table lookups for input checks. |

---

## 5. The double — [4]

| | |
|---|---|
| What you passed through the seam | stub |
| The method under test | `Game::handleInput()` |

Two sentences: was the collaborator asked a question or told to do something, and why does that decide the answer above?
The collaborator (the input mechanism) was asked a question about whether a key was pressed and which key it was. Because we provided predefined answers for it to return rather than verifying how it was called, it is a stub.

---

## 6. Two smells in your own tests — [5]

| | Smell | `file:line` | One-line fix |
|---|---|---|---|
| 1 | Magic Numbers | `tests.cpp:5` | `Snake snake(BOARD_W/2, BOARD_H/2, BOARD_W, BOARD_H);` |
| 2 | Code Duplication | `tests.cpp:17` | Extract `Snake snake(10, 10, 20, 20);` to a common setup method or fixture. |

