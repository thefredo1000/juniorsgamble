# Juniors (Butano GBA)

This project is a Butano-based GBA game that now combines:

- Intro screen
- Main menu
- Dream checkpoint scene
- Casino story menu
- Minigames submenu
- World map exploration/debug scene
- NPC conversation system
- Save-backed story progression

The recent work focused on gameplay flow, world map behavior, and architecture refactoring so systems are easier to maintain.

## Current Scene Flow

1. Title screen
2. Main menu with `Start new game`, `Continue`, `Minigames`, and `Config & extras`
3. Story flow routed by autosave stage:
4. Dream checkpoint scene on a new run
5. Casino story menu during the rescue phase
6. Story completion screen after the family is saved
7. Separate minigames submenu with all games available immediately
8. World map remains available as a debug entry from the title screen (`A`)

## Controls

- D-Pad: Move on world map (tile-by-tile movement)
- A: Confirm/select, and talk/interact on the world map
- B: Back/cancel (scene-specific)
- START: Confirm in shared input helper where supported

## World Map Gameplay Behavior

- Movement is locked to one tile at a time (Pokemon-like step movement).
- Facing updates immediately on direction change.
- If blocked by NPC/solid area, character turns but does not move.
- NPC interaction happens with the tile directly in front of the player.
- NPC rotates to face player when dialog starts.
- New game starts in the dream world map, and Continue resumes the story from its current point; minigames remain an explicit menu choice instead of the default story route.

## Refactored Module Structure

### High-level flow

- include/game_flow.h
- src/game_flow.cpp

Responsibilities:

- Owns scene state machine transitions.
- Runs title, main menu, dream checkpoint, casino story menu, world map, and minigame transitions.

### Input abstraction

- include/game_input.h
- src/game_input.cpp

Responsibilities:

- Shared confirm/back semantics.
- Prevents duplicated keypad condition logic across scenes.

### World map public API

- include/world_map_screen.h
- src/world_map_screen.cpp

Responsibilities:

- Keeps stable external entry point: world_map_screen().
- Orchestrates systems (movement, interaction, dialog, sprite syncing, scene return values).

### World map static configuration

- include/world_map_config.h

Responsibilities:

- Constants (tile step, speeds, cooldown).
- Initial world pivot.
- NPC data and dialog line arrays.

### World map pure rules/logic

- include/world_map_logic.h
- src/world_map_logic.cpp

Responsibilities:

- Generic helpers and pure game rules:
- clamp
- collision checks
- facing tile calculation
- direction frame mapping
- opposite direction mapping

### World map runtime mutable state

- include/world_map_state.h

Responsibilities:

- Central state object for map loop runtime.
- Tracks movement deltas, cooldown, facing, dialog/question status, active NPC indices.

### World map dialog system

- include/world_map_dialog.h
- src/world_map_dialog.cpp

Responsibilities:

- Dialog/question rendering.
- Dialog state transitions.
- Host question path that can return start_poker.

### World map movement system

- include/world_map_movement.h
- src/world_map_movement.cpp

Responsibilities:

- Movement input interpretation.
- Move-attempt start checks with bounds/collision.
- Axis stepping and step cooldown updates.

### World map interaction system

- include/world_map_interaction.h
- src/world_map_interaction.cpp

Responsibilities:

- Determine NPC in front of player.
- NPC lookup by world tile.
- Dialog-start state initialization.
- NPC frame orientation decision when dialog starts.

### World map player visual system

- include/world_map_player_visual.h
- src/world_map_player_visual.cpp

Responsibilities:

- Direction-based walking animation switching.
- Standing frame selection based on facing direction.

### World map NPC visual system

- include/world_map_npc_visual.h
- src/world_map_npc_visual.cpp

Responsibilities:

- NPC on-screen position sync from world coordinates.
- NPC visibility culling against current camera/pivot.

## Developer Quick Map

Use this table to quickly find where to change a behavior:

- Scene transitions (intro/menu/world map/poker): src/game_flow.cpp
- World map public entry point: include/world_map_screen.h
- Tile movement speed/cooldown/tile size: include/world_map_config.h
- Collision and facing rules: src/world_map_logic.cpp
- Movement input and axis stepping: src/world_map_movement.cpp
- NPC interaction target and dialog start state: src/world_map_interaction.cpp
- Player facing animation and standing frame: src/world_map_player_visual.cpp
- NPC screen sync and visibility: src/world_map_npc_visual.cpp
- Dialog/question flow and host poker trigger: src/world_map_dialog.cpp
- Shared confirm/back input logic: src/game_input.cpp

## Why This Structure

- Keeps the world map screen focused on orchestration, not low-level details.
- Moves pure logic and reusable decisions into focused modules.
- Reduces regression risk when tuning movement/dialog independently.
- Keeps the public world map API stable for callers.

## Build

From project root:

make -j4

Successful build produces and fixes the ROM output.

## Extension Tips

- Add collidable map regions by expanding solid_rects in src/world_map_screen.cpp.
- Add new NPCs in include/world_map_config.h.
- Adjust feel using tile_step, move_speed, and step_cooldown_frames.
- Keep new systems behind small modules and let world_map_screen.cpp orchestrate them.
