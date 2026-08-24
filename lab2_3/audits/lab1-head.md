# Code Smell Audit Report — `lab1-head` (Lab-1 PR Head)

This report documents structural code smells present in the repository at commit `8b63a76` (Lab-1 branch head `feat/multiplayer`).

## Findings

### Smells Retained from `main`

- **Long Parameter List** (`src/renderer.h:20`) — C4 (Certain): `Renderer::render()` parameter count expanded from 11 to 13 parameters to accept `const Snake& snake2` and `int score2`. Fix: `refactor-simplifying-method-calls` / `refactor-organizing-data`.
- **Primitive Obsession** (`src/game.h:29`) — C4 (Certain): Special food primitives (`specialFoodActive`, `specialFoodTimer`, etc.) remain unencapsulated in `Game`. Fix: `refactor-organizing-data`.
- **Long Method** (`src/game.cpp:80`) — C3 (High): `Game::handleInput()` grew further to process controls for both Player 1 and Player 2. Fix: `refactor-composing-methods`.
- **Long Method** (`src/game.cpp:154`) — C3 (High): `Game::update()` grew to 180+ lines to process movement, body collision, wall collision, and food eating logic for two snakes sequentially. Fix: `refactor-composing-methods`.
- **Large Class** (`src/game.h:12`) — C3 (High): `Game` class added `snake2`, `score2`, and `loserMessage`, increasing class responsibility and field count. Fix: `refactor-moving-features`.
- **Data Clumps** (`src/renderer.h:20`) — C3 (High): `specialFood` timer/active variables continue to be passed together with scores and snake references. Fix: `refactor-organizing-data`.
- **Temporary Field** (`src/game.h:22`) — C2 (Medium): `loserMessage` and `specialFoodTimer` are set and read only under specific state transitions. Fix: `refactor-organizing-data`.
- **Divergent Change** (`src/game.cpp:11`) — C3 (High): `Game` combines physics, input routing for multiple players, game over messaging, and file I/O. Fix: `refactor-moving-features`.
- **Switch Statement** (`src/game.cpp:303`) — C3 (High): `Game::run()` switch on `state` remains un-refactored. Fix: `refactor-simplifying-conditionals`.
- **Dead Code** (`src/utils.cpp:17`) — C2 (Medium): Unused `highResSleepMs()` helper function remains present. Fix: `refactor-composing-methods`.

### Smells Introduced in Lab-1 PR

- **Duplicate Code** (`src/game.cpp:157`) — C4 (Certain): `update()` duplicates collision checks, movement updates, and food consumption logic for `snake` and `snake2` in parallel blocks rather than iterating over a collection. Fix: `refactor-composing-methods`.
- **Data Clumps** (`src/game.h:17`) — C3 (High): `snake` and `snake2`, alongside `score` and `score2`, exist as distinct parallel member fields in `Game` instead of a unified player/snake structure. Fix: `refactor-organizing-data`.
- **Shotgun Surgery** (`src/game.cpp:12`) — C3 (High): Adding player 2 required coordinated edits across `game.cpp`, `game.h`, `renderer.cpp`, `renderer.h`, `utils.cpp`, `scores.txt`, and executable rebuilds. Fix: `refactor-moving-features`.
