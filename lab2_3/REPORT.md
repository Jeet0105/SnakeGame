# Lab 2_3 — Group A__

---

## 1. Tool and install route — [3]

| | |
|---|---|
| Agent used for run 2 | Antigravity IDE (Gemini 3.6 Flash) |
| `ubiquitous-language` install route | `it643-content/skills/ubiquitous-language` |
| `refactoring/` pack install route | `it643-content/skills/refactoring/` |

Installed skills from the course repository (`it643-content/skills/`) directly into the agent environment. The `ubiquitous-language` skill was explicitly invoked by name to extract domain terminology into `UBIQUITOUS_LANGUAGE.md`. The `refactoring/` skill pack (including `detect-code-smells` and `review-accuracy-calibration`) was loaded to conduct code smell detection and confidence calibration across commits.

---

## 2. What I changed in the glossary — [4]

The generated file is at `lab2_3/UBIQUITOUS_LANGUAGE.md`.

Corrected 'Food' to canonical domain term 'Fruit' across definitions and identified code drift in `src/game.h:19`. Resolved ambiguity between 'Special Food' and 'Rare Food' by standardizing on 'Special Fruit'. Removed invented generic terms like `drawGame` and `gotoxy` that do not represent domain concepts. Clarified the distinction between historical session score logging (`scores.txt`) and single top record tracking (`highscore.txt`).

---

## 3. Smell delta — [6]

Reports: `lab2_3/audits/main.md` (the code as you received it) and `lab2_3/audits/lab1-head.md` (after your Lab-1 PR).

| | count | representative site (`file:line`) |
|---|---|---|
| Smells my Lab-1 PR **introduced** | 3 | `src/game.cpp:157` |
| Smells my Lab-1 PR **left untouched** | 10 | `src/renderer.h:20` |
| Smells my Lab-1 PR **removed** | 0 | N/A |

---

## 4. Rejected candidates — [6]

At least three things the agent reported that are *not* real findings on this codebase.

| smell reported | `file:line` | why it does not hold |
|---|---|---|
| Lazy Class | `src/main.cpp:8` | `main.cpp` is the OS application entry point responsible solely for UTF-8 setup and instantiating `Game`; flagging it as a lazy class is a false positive as every executable requires an entry point. |
| Feature Envy | `src/renderer.cpp:40` | `Renderer::render()` reads `Snake` and `Position` data to draw the console screen buffer; moving rendering into domain entities would violate single responsibility and couple domain models to UI rendering. |
| Speculative Generality | `src/utils.h:27` | `FrameRateController` FPS getters and setters provide essential timing controls for pacing the game loop (10 FPS update speed) predictably across different CPU architectures. |

---

## 5. Commit map — [7]

Run `lab2_3/check-lab2_3.sh` and paste the table it prints.

| # | sha | subject | what it is |
|---|---|---|---|
| 1 | 3864676 | Add edited ubiquitous language glossary (UBIQUITOUS_LANGUAGE.md) | glossary |
| 2 | 9768e82 | Add code smell audit report for main branch (audits/main.md) | smell report |
| 3 | 0a99b77 | Refactor snake storage to std::vector<Snake> to centralize snake count representation | **the refactor, alone** |
| 4 | b0dc966 | Now add the second player | **the feature, alone** |

---

## 6. Two-run measurement — [4]

Run 1 is your Lab-1 branch — the numbers you already reported. Run 2 is commit 4 alone.

| | Run 1 (Lab 1) | Run 2 (commit 4) |
|---|---|---|
| Smells introduced | 3 | 0 |
| Lines changed, `git diff --shortstat -w` | 7 files changed, 146 insertions(+), 89 deletions(-) | 2 files changed, 56 insertions(+), 34 deletions(-) |
| Lines changed, **raw** (no `-w`) | 13 files changed, 1318 insertions(+), 1261 deletions(-) | 2 files changed, 56 insertions(+), 34 deletions(-) |
| Functions reached | 4 | 2 |
| Prompts to working code | 3 | 1 |
| Wall-clock time | 25 mins | 10 mins |

Commit 3 (the refactor) on its own: 6 files changed, 124 insertions(+), 92 deletions(-) lines `-w`, 6 files changed, 156 insertions(+), 124 deletions(-) raw.

---

## 7. Analysis Q1–Q2 — [5]

**Q1. Which smell did commit 3 actually fix?** Name it from your section 3 report. What was expensive before, what does it cost now.

Commit 3 fixed **Data Clumps** (`src/game.h:17`) and **Shotgun Surgery** (`src/game.cpp:12`). Before commit 3, supporting multiple snakes required declaring parallel fields (`snake2`, `score2`) and duplicating movement, collision, and food logic across `update()` and `handleInput()`. Adding a player meant editing 7 files across header declarations, rendering calls, and loop logic. Now, snake representation is centralized in `std::vector<Snake> snakes` and `std::vector<int> scores`. Adding a player requires zero structural code edits—simply pushing another `Snake` instance into the container.

**Q2. Compare commit 4 to your Lab-1 diff.** Same feature, same codebase. What changed in the cost and what did not? If it got worse, say so and explain — that marks the same.

In Lab 1, adding multiplayer required 235 modified lines (`-w`), duplicated collision/movement loops, and introduced 3 code smells. In Run 2 (commit 4), because the container abstraction was established in commit 3, adding the second player cost only 90 lines (`-w`) and 0 new smells. The cost of feature implementation dropped by over 60% in diff size and eliminated code smell introduction completely, because input routing and player initialization simply populated the existing vector.

---

## 8. Analysis Q3–Q4 — [5]

**Q3. Go back through your Lab-1 `LLM-LOG.md`. Did the assistant ever suggest restructuring before adding the feature?** Quote it if it did. If it did not, what would have had to be different in your prompt?

No, in Lab 1 the assistant immediately generated a direct, naive patch adding `snake2` and `score2` alongside `snake` and `score`. It did not suggest refactoring to a vector or single-sourced collection. For the assistant to propose pre-feature restructuring, the prompt would have needed an explicit design constraint such as: "Review the current domain model for extensibility first. Refactor snake collection storage so that player count is single-sourced before introducing multiplayer controls."

**Q4. How do you know commit 3 did not change behaviour?** Answer honestly. Most of you will find that you do not know. Say that plainly if it is true, and describe what you would have needed in order to actually know.

We verified behaviour manually by launching `snake_game.exe` and playing a single-player game session across menu navigation, WASD/arrow snake controls, food eating, special food timer countdowns, obstacle collisions, and high score saving. However, strictly speaking, manual play does not guarantee zero regression. To definitively guarantee non-breaking behavior, we would require automated unit and integration test suites (e.g., GoogleTest) covering `Snake` movement, collision edge cases, and game state transitions.

---

## If you did not finish

N/A — All tasks, skill installations, audits, refactoring, feature implementation, verification, and report analyses completed successfully.
