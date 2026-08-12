# LLM Log
 
## Setup
 
| Item | Details |
|---|---|
| Model(s) | Claude Sonnet 5 |
| Tool / harness | Web chat (claude.ai) |
| IDE / editor | None (assistant used web tools directly; code was pasted manually into Visual Studio / VS Code) |
| Did you paste this assignment document into it? | Yes — the full assignment PDF was uploaded in the first prompt |
 
## Session
 
### Prompt 1 — Repository analysis and implementation plan
I uploaded the assignment document and gave the assistant the URL of my fork (`github.com/maitry4/SnakeGame`) and asked it to tell me what to do, step by step.
 
**Relevant response:** The assistant fetched the repository itself (it does not have my code pasted in — it pulled `game.h`, `game.cpp`, `snake.h`, `snake.cpp`, `renderer.h`, `renderer.cpp`, `utils.h` directly from GitHub via its own tools) and identified that `Snake`/`snake.h`/`snake.cpp` needed no changes at all, while `Game` (state, input, food/obstacle spawning, update loop) and `Renderer` (drawing, game-over screen) held the single-snake assumption. It proposed a full diff: add a second `Snake` member, add `score2`, split Player 1 to arrow-keys-only and add WASD for Player 2, rewrite `Game::update()` to move both snakes and check wall/self/obstacle/other-snake collisions, extend food/special-food spawning to avoid both snakes, and extend `Renderer::render`/`renderGameOver` to draw two snakes and report the losing player.
 
### Prompt 2 — Git workflow troubleshooting (branch)
After applying the code changes, I ran a PowerShell build/commit and discovered I had committed directly to `main` instead of a feature branch.
 
**Relevant response:** The assistant gave the recovery steps: create `feat/multiplayer` from the existing commit, hard-reset `main` back to `origin/main`, and switch to the new branch — without losing the completed work.
 
### Prompt 3 — Noisy diff (line-ending/whitespace)
`git diff --stat` showed 13 files changed and 1300+ lines, including files I never intentionally touched (`snake.cpp`, `utils.cpp`, `scores.txt`, the `.exe`, `readme.md`).
 
**Relevant response:** The assistant diagnosed this as CRLF/whitespace reformatting from the editor, had me set `core.autocrlf false`, restore the untouched files with `git checkout origin/main -- <files>`, and re-check with `git diff --stat -w` to isolate real changes from whitespace noise.
 
### Prompt 4 — Diff still noisy after cleanup, then pushed anyway
A follow-up screenshot still showed inflated line counts (702/412 lines in `game.cpp`/`renderer.cpp`), and a later push showed the noisy 13-file, 1318-insertion commit had gone to GitHub.
 
**Relevant response:** The assistant walked through resetting the four core files to the clean originals and reapplying only the specific edits by hand (rather than letting the editor reformat on save), and explained how to `git push --force-with-lease` to overwrite the noisy commit already on the remote with a clean one once the diff was verified small.
 
### Prompt 5 — Where the measurement table and analysis go
I asked where the Part D measurement table and Q1–Q4 answers should be submitted.
 
**Relevant response:** The assistant clarified both belong in the same place — the PR description on GitHub — not in `LLM-LOG.md` or a separate file, quoting the assignment's deliverables list.
 
### Prompt 6 — Draft analysis answers
I asked for answers to Q1, Q2, and Q4 (Q3 requires my own Lecture 3 prediction, which the assistant does not have).
 
**Relevant response:** The assistant drafted answers grounded in the actual diff: Q1 named `Renderer::render`, `Renderer::renderGameOver`, `Game::generateFood`, `Game::generateSpecialFood`, and `Game::generateObstacles` as non-snake functions reached because they depend on "every occupied cell," Q2 explained the one-snake assumption was implicit and spread across function signatures rather than centralized, and Q4 proposed replacing the single `Snake` member with `std::vector<Snake>` throughout as the one design change that would have made the task fast.
 
## Final result
 
The implementation builds and satisfies the multiplayer requirements: a second snake controlled by WASD, Player 1 restricted to arrow keys, shared fruit (first snake to reach it grows/scores), independent visible scores, and game-ending collision detection (wall, self, other snake, obstacle) with a losing-player announcement.
 
### What code was given to the LLM?
 
The assistant was not handed files manually — it fetched the **whole `src/` directory** itself directly from the public GitHub fork (`game.h`, `game.cpp`, `snake.h`, `snake.cpp`, `renderer.h`, `renderer.cpp`, `utils.h`, `utils.cpp`, `main.cpp`) using its own web-browsing tools, then worked from that.
 
### Total prompts to working code
 
6
 
