# Goblin — Roadmap

> **Status:** planned work, not implemented. This document describes possible future updates. The current game is a small Italian-language console RPG written in C. The source file `goblin.it.c` remains unchanged.
>
> **Last roadmap update:** 2026-10-23. Priorities are proposals, not release commitments.

## Project direction

The first goal is a stable, balanced, complete **console RPG**. Only after the core gameplay works reliably should the project grow into an optional 2D pixel-art RPG or a browser version.

Potential long-term direction: a small turn-based fantasy RPG with light roguelite elements — a village for preparation, short dungeon expeditions, encounters, loot, and a goblin boss. A run should have an achievable ending rather than a single, endlessly upgradeable encounter.

## Priority definitions

- **P0 — Correctness / safety:** defects and unbounded exploits that should be resolved before adding content.
- **P1 — Playable release:** systems required for a complete small game and maintainable code.
- **P2 — Depth and replayability:** new content and variety once the core is reliable.
- **P3 — Presentation / distribution:** graphics, sound, browser support, polish.

## Milestone 0.2 — Make the existing game reliable (P0)

- [ ] Replace unchecked string input with bounded input and validate names and promo codes.
- [ ] Handle non-numeric input, EOF, and invalid menu selections without getting stuck.
- [ ] Fix progression in Work(): it currently checks rest_count rather than a dedicated work counter, and can increase levels on every work action when rest_count is zero or divisible by three.
- [ ] Make temporary potions expire after the advertised five turns instead of permanently increasing boost_damage.
- [ ] Decide whether promo codes are one-use, developer-only, or removed from public gameplay; prevent unlimited gold/armor farming.
- [ ] Clamp received damage to zero and show the actual damage applied.
- [ ] Introduce max_health, healing limits, and reasonable stat caps; validate currency costs and prevent accidental overflows.
- [ ] Define critical-hit rules: crit_damage currently behaves as a fixed multiplier, not a random or conditional critical hit.
- [ ] Correct the effects and textual descriptions of shop products so that gameplay matches what is advertised.
- [ ] Make leveling depend on a coherent experience/progression mechanic rather than unrelated counters.
- [ ] Decide **turn-based vs. real-time** combat explicitly. Current time checks run between blocking console menu actions; they are not truly independent background events.
- [ ] Ensure victory, defeat, turn transitions, and enemy attacks all have consistent game-state rules.

**Acceptance criteria:** no unsafe unbounded input; every advertised consumable expires; progression and shops cannot be trivially farmed through the identified flaws; combat results and printed values agree.

## Milestone 0.3 — Complete the core console RPG (P1)

### Combat
- [ ] Introduce a deterministic, testable combat engine separate from UI.
- [ ] Add meaningful actions: attack, defend, heavy attack, and use a consumable.
- [ ] Add costs/trade-offs (e.g. stamina, cooldown, defense reduction, or limited item counts).
- [ ] Add readable enemy intent and/or patterns so the player's choices matter.
- [ ] Implement actual critical hits with explicit probability/conditions and damage rules.
- [ ] Add limited consumables and explicit buff/debuff duration tracking.
- [ ] Add at least **three enemy types** with distinct behavior and **one goblin boss**.
- [ ] Balance a starting run so that neither automatic clicking nor a single shop purchase dominates all other choices.

### Character, items, progression
- [ ] Replace accidental level-ups with XP or another deliberately defined reward system.
- [ ] Add a basic inventory (items owned, quantity, use, consume).
- [ ] Give gold meaningful uses and adjust prices/rewards through playtests.
- [ ] Add optional skills/perks and equipment upgrades without uncontrolled stat growth.
- [ ] Add save/load (versioned save format; safe error handling and recovery).
- [ ] Add a clear short-term goal, win condition, restart option, and accessible help/tutorial.

### Architecture and engineering
- [ ] Keep C as the core language while separating calculations, state updates, input, and presentation.
- [ ] Replace large monolithic source with small modules only where it reduces coupling. **Proposed**, not existing, layout:

        goblin/
        ├── include/
        │   ├── game.h
        │   ├── player.h
        │   ├── enemy.h
        │   ├── combat.h
        │   └── inventory.h
        ├── src/
        │   ├── main.c
        │   ├── game.c
        │   ├── combat.c
        │   ├── progression.c
        │   ├── inventory.c
        │   ├── save.c
        │   └── ui_console.c
        ├── tests/
        │   └── test_combat.c
        ├── README.md
        ├── Makefile
        └── .gitignore

- [ ] Use small, testable functions for damage, healing, XP, purchase validation, and enemy turns.
- [ ] Add compiler warnings and debug sanitizer builds where supported.
- [ ] Add unit and smoke tests; cover invalid input, zero damage, potion expiry, purchase boundaries, win/loss, and save/load.
- [ ] Add a reproducible Makefile or CMake build and CI checks.
- [ ] Keep user-facing strings together to simplify Italian corrections and optional localization.
- [ ] Create a tagged playable binary release with a short changelog and clear operating instructions.

**Acceptance criteria:** one complete run with several encounters and a boss, without editing source code; save/load works; tests run automatically; starting the game is documented.

## Milestone 0.4 — Replayability and world (P2)

- [ ] Introduce a village/hub for preparing before a run, managing equipment, and shopping.
- [ ] Add a sequence of dungeon rooms/encounters with a finish point.
- [ ] Add optional event rooms, treasures, and non-combat decisions.
- [ ] Add enemy variations, item combinations, and rewards that enable different strategies.
- [ ] Consider procedural generation of short room sequences (not an open world).
- [ ] Add limited run-based progression and decide what, if anything, persists after defeat.
- [ ] Balance through recorded playthroughs and basic analytics such as win rate, average turns, and item usage.
- [ ] Improve game feedback: status, buffs, action logs, enemy intent, and clearer instructions.

**Scope guard:** start small — 3 enemy types, 1 boss, roughly 5–8 useful items, and 3–4 core combat actions. Expand only when the base loop is fun.

## Milestone 1.0 — Optional presentation and platforms (P3)

These are **alternative directions**, not commitments to deliver every platform.

### Path A — Polished console game (recommended first)
- [ ] Clean terminal interface, readable menus, clear Italian text, optional other languages.
- [ ] Cross-platform packaging and versioned releases.
- [ ] End-to-end balance and robustness testing.

### Path B — 2D pixel-art edition
- [ ] Keep game rules separate from rendering.
- [ ] Evaluate **raylib** as a C-friendly graphics/audio/input library.
- [ ] Prototype one combat screen and one village screen before building a full map.
- [ ] Add distinct goblin sprites, animations, hit feedback, inventory UI, audio, accessibility settings, and performance checks.

### Path C — Browser edition
- [ ] Evaluate compiling a portable C core to WebAssembly or rewriting the presentation in JavaScript.
- [ ] Confirm input, file storage, and game-loop compatibility in browsers.
- [ ] Deploy a static web build via GitHub Pages **only when the game is ready**.
- [ ] Test keyboard, responsive layout, loading, and persistence.

## Known issues in the original prototype

This is a code-inspection checklist, not results from compiling or dynamically executing the game.

| Area | Observed in current source | Proposed solution |
|---|---|---|
| Player name / promo input | scanf("%s", ...) has no length bound | Bounded input and error handling |
| Work progression | Work() uses rest_count % 3; starts at 0 | Dedicated work/XP progression |
| Potion duration | Advertised 5 turns but permanently changes boost_damage | Timed-effect object/counter |
| Promo codes | Valid codes can be redeemed repeatedly | One-time validation or removal |
| Healing | HP has no max-health ceiling | max_health and capped healing |
| Crits | crit_damage starts at 1 without critical event logic | Explicit crit system |
| Negative damage | Damage printout can show a negative value | Clamp before apply and print |
| Timers | Evaluated between blocking console inputs | Deliberate turn clock or event loop |
| Game ending | One goblin; win/lose ends the process | Multiple encounters and replay |
| Persistence | No save/load | Versioned save files |
| Quality | No tests/build automation | Tests and reproducible builds |

## Decision order

1. Fix correctness and memory safety.
2. Choose a precise, turn-based combat model and test it.
3. Ship a complete console run with variety and a boss.
4. Add save/load and reliable packaging.
5. Evaluate 2D and browser versions after playtesting.

## Contributions and rights

This roadmap describes intentions only; it is **not** a grant of rights to use the current or planned implementation. See [LICENSE](LICENSE). No unsolicited code contributions are accepted without explicit prior agreement from the copyright holder.
