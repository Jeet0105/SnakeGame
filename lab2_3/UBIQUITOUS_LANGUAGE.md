# Ubiquitous Language

## Game Domain Terms

| Term | Definition | Aliases to avoid | In code |
| --- | --- | --- | --- |
| **Snake** | The controlled entity consisting of a head and body segments that moves across the board | Player object, worm, serpent | `Snake` — `src/snake.h:16` |
| **Fruit** | A collectible item on the board that increases score and snake length when eaten | Food, apple, item, collectible | `food` — `src/game.h:19` ⚠ |
| **Special Fruit** | A temporary bonus item that yields extra points and segment growth if collected before its timer expires | Special food, rare food, bonus item, super apple | `specialFood` — `src/game.h:20` ⚠ |
| **Board** | The bounded grid playing area where snakes, fruits, and obstacles exist | Grid, playing field, screen, map | `BOARD_WIDTH`, `BOARD_HEIGHT` — `src/game.h:14` |
| **Obstacle** | A static barrier entity on the board that causes game over upon collision | Wall, barrier, block | `obstacles` — `src/game.h:21` |
| **Tick** | A single execution cycle of the game update loop where state advances | Frame, step, loop iteration | `sleepMs` / `frameController` — `src/game.cpp:299` |
| **Score** | The total points accumulated by a player in the active session | Points, current score, player score | `score` — `src/game.h:22` |
| **High Score** | The top score recorded across all historical game sessions | Best score, record, top score | `highScore` — `src/game.h:23` |
| **Collision** | An event where a snake's head contacts its own body, an obstacle, or a board boundary | Crash, hit, death | `checkCollision` — `src/snake.h:28` |
| **Game State** | The current operational phase of the game session: Menu, Playing, Paused, or Game Over | Status, mode, screen state | `GameState` — `src/game.h:10` |

## Relationships

- A **Board** contains one or more **Snakes**, one **Fruit**, zero or one **Special Fruit**, and multiple **Obstacles**.
- Each **Tick** moves active **Snakes** one step in their current direction unless paused.
- Collecting a **Fruit** increments the player's **Score** and extends the **Snake** length.
- A **Collision** immediately transitions the **Game State** to Game Over.

## Example Dialogue

> **Dev:** "When a **Snake** moves during a **Tick**, how do we process eating a **Fruit** vs a **Special Fruit**?"
> **Domain expert:** "If the **Snake** head lands on the **Fruit**, score increases by 10 and the **Snake** grows by 1 segment. If it lands on a **Special Fruit**, score increases by 30 and it grows by 2 segments."
> **Dev:** "And if the **Snake** head hits an **Obstacle** or the **Board** boundary?"
> **Domain expert:** "That counts as a **Collision**, which ends the session and records the **Score**."

## Flagged Ambiguities

- **"Food" vs "Fruit"**: The codebase consistently uses `food` in code (`game.h:19`), whereas game documentation and UI context refer to "Fruit". "Fruit" is canonical.
- **"Special Food" vs "Rare Food"**: Comments in `game.h:33` refer to "rare food counter" while variables are named `specialFoodActive` and `specialFood`. "Special Fruit" is canonical.
- **"Score Entry" vs "High Score"**: `scores.txt` logs per-session history (`saveScoreEntry()`), whereas `highscore.txt` stores only the single global top score (`saveHighScore()`).

## Code Drift

| Canonical term | Called in code | Location | Note |
| --- | --- | --- | --- |
| **Fruit** | `food` | `src/game.h:19` | Code uses "food"; domain language specifies "Fruit" |
| **Special Fruit** | `specialFood` | `src/game.h:20` | Code uses "specialFood"; domain specifies "Special Fruit" |
| **Board Boundary** | `boardWidth`, `boardHeight` | `src/snake.h:20` | Primitive integers instead of a dedicated Board boundary entity |
