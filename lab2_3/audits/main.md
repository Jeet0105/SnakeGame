# Code Smell Audit Report — `main` Branch

This report documents structural code smells present in the repository at commit `3437ac2` (main branch).

## Findings

### Bloaters

- **Long Parameter List** (`src/renderer.h:20`) — C4 (Certain): `Renderer::render()` accepts 11 parameters (`snake`, `food`, `specialFood`, `specialFoodActive`, `specialFoodTimer`, `specialFoodMaxTimer`, `score`, `highScore`, `paused`, `obstacles`, `specialFoodCount`). Fix: `refactor-simplifying-method-calls` / `refactor-organizing-data`.
- **Primitive Obsession** (`src/game.h:28`) — C4 (Certain): Special food state is managed as 5 primitive fields (`specialFoodActive`, `specialFoodTimer`, `specialFoodPoints`, `specialFoodMaxTimer`, `specialFoodCount`) directly in `Game` rather than encapsulated in a object. Fix: `refactor-organizing-data`.
- **Long Method** (`src/game.cpp:80`) — C3 (High): `Game::handleInput()` is 73 lines long, handling menu selection, terminal escape sequences for arrow keys, WASD movement, pause toggling, game resets, and exit logic in a single function. Fix: `refactor-composing-methods`.
- **Long Method** (`src/game.cpp:154`) — C3 (High): `Game::update()` is 65 lines long, combining snake movement, self-collision checks, obstacle collision checks, food consumption, special food spawning, and special food timer updates. Fix: `refactor-composing-methods`.
- **Large Class** (`src/game.h:12`) — C3 (High): `Game` class manages 15+ member variables and is responsible for game loop control, entity management, rendering coordination, keyboard I/O, file persistence (`highscore.txt`, `scores.txt`), and frame rate regulation. Fix: `refactor-moving-features`.
- **Data Clumps** (`src/renderer.h:20`) — C3 (High): `specialFood`, `specialFoodActive`, `specialFoodTimer`, and `specialFoodMaxTimer` always travel together across `Game` and `Renderer` methods. Fix: `refactor-organizing-data`.

### OO Abusers & Change Preventers

- **Temporary Field** (`src/game.h:20`) — C2 (Medium): `specialFood` and `specialFoodTimer` are member variables of `Game` that hold meaningful values only when `specialFoodActive` is true. Fix: `refactor-organizing-data`.
- **Divergent Change** (`src/game.cpp:11`) — C3 (High): `Game` must be modified whenever changes are made to high score storage, terminal user interaction, collision rules, or special item behavior. Fix: `refactor-moving-features`.
- **Switch Statement** (`src/game.cpp:303`) — C3 (High): `Game::run()` uses a `switch (state)` statement to alter game loop behavior for `MENU`, `PLAYING`, `PAUSED`, and `GAME_OVER`. Fix: `refactor-simplifying-conditionals`.

### Dispensables & Couplers

- **Dead Code** (`src/utils.cpp:17`) — C2 (Medium): `highResSleepMs()` is defined in `utils.cpp` but is only referenced by `inputWithTimeout()`, which is never invoked in the main game flow. Fix: `refactor-composing-methods`.
