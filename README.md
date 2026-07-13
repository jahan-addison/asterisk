<div align="center">
  <img src="assets/logo/Asterisk Game Logo.png" width="800" alt="asterisk"> </img>
</div>

<h5 align="center">
  The Asterisk Space-Shooter 2D Game 🎮
</h5>

---

> An asteroid space-shooter game for the Teensy 4.1 using the [Pulse2D Game Engine](https://github.com/jahan-addison/pulse2d).


<div align="center">
  <img src="assets/demo/V5/Asterisk Game Demo V5.gif" width="800" alt="asterisk"> </img>
</div>

## Overview

Asterisk is a feature-complete space shooter built with [Pulse2D](https://github.com/jahan-addison/pulse2d) and designed as the pilot game for the engine. It runs on a Teensy 4.1 with a 320x240 SPI display and a Seesaw gamepad, with no operating system and no heap allocation at runtime.

The game progresses through three levels of increasing difficulty, each with its own enemy, attack patterns, and parallax background art.

## Gameplay

The player controls a ship on the left side of the screen using arcade inverted directional input. The A button fires the laser gun. The SELECT button pauses the game. Health is displayed as a bar in the upper left and starts at 5.

### Level One - Asteroid Field

Four asteroid types spawn sequentially across the screen, each in three sizes: large, medium, and small. Each asteroid is managed by its own state machine that handles spawning, size-stepping on impact, and transitioning to an erased or evaded state.

Two consecutive evaded asteroids deal one point of damage to the player. A direct ship collision also deals one point of damage. Destroying twenty asteroids advances the player to Level Two.

### Level Two - The Dreadnaught

A large rotating boss ship occupies the right side of the screen. Its twin laser cannons are offset from the sprite center and rotate in sync with the body angle using a body-relative coordinate transform, so projectiles always emerge from the visual gun positions. Every fifty laser hits reduces the boss by one health point. Depleting all five health points advances the player to Level Three.

### Level Three - Final Boss

A three-stage boss fight with escalating attack patterns and a four-second transition window between each stage.

- **Stage 1** - the boss oscillates vertically and fires a laser every 1800 ms
- **Stage 2** - adds a rocket on a separate 2200 ms cooldown alongside the laser
- **Stage 3** - increases the laser interval to 1000 ms, the rocket interval to 1400 ms, and launches pairs of side rockets from off-screen at 2500 ms intervals

Each stage has five health points. Thirty hits per health point is required to advance. The player's health restores to five at stage three. Defeating the final stage ends the game.

## Hardware

The hardware is the [recommended set from Pulse2D](https://github.com/jahan-addison/pulse2d#hardware).

## Building

Asterisk uses the Pulse2D [Makefile.teensy](ext/pulse2d/Makefile.teensy) build system. The project Makefile is a thin wrapper that sets the source file, include path, and submodule root.

Install the ARM bare-metal toolchain:

```bash
# macOS
brew install --cask gcc-arm-embedded

# Ubuntu
sudo apt install gcc-arm-none-eabi
```

Clone the repository with submodules to pull in the Pulse2D engine:

```bash
git clone --recurse-submodules https://github.com/jahan-addison/asterisk
```

Build and flash:

```bash
make -j           # compile
make flash        # flash via teensy_loader_cli
make sections     # print the section map and memory layout
make bss_symbols  # list .bss symbols with size warnings
make clean        # remove build output
```

**Note:** Teensyduino must be installed alongside the Arduino IDE. See [pjrc.com/teensy/td_download.html](https://www.pjrc.com/teensy/td_download.html). The IDE itself is not needed for building.

## Project Structure

```
asterisk/
  src/
    asterisk.cc                 # scene graph, game lifecycle, pause logic
  include/
    common.h                    # shared utilities: ship, walls, health bars, backgrounds
    scenes/
      levels/
        level-one.h             # asteroid field
        level-two.h             # Dreadnaught boss
        level-three.h           # three-stage final boss
      menus/
        start.h                 # start screen
        gameover.h              # game over screen
        win.h                   # game won screen
    backgrounds/
      parallax/                 # RGB565 background headers per level
    sprites/
      asteroids/                # asteroid sprites (4 types x 3 sizes)
      health-bars/              # player and enemy health bar sprites
    animations/                 # spritesheet animation headers
    events/
      asteroids.h               # asteroid state machine and pool manager
    enemy/
      boss-1.h                  # Level Two boss sprite
      missile-0.h               # rocket sprite
  ext/
    pulse2d/                    # Pulse2D engine (git submodule)
  assets/                       # source artwork (not included, see Artwork below)
```

### `src/asterisk.cc`

The main file connects the scenes using the `PULSE_ON_GAMESCENE_START` and `PULSE_ON_GAMESCENE` macros from Pulse2D. It handles the start screen, loading transitions between levels, the pause screen, and gamepad polling. Pause duration is tracked by recording `millis()` at pause entry and "rewinding" all active `elapsedMillis` timers on resume so that timer-dependent intervals do not drift.

### `include/common.h`

Shared setup functions used by every level: `set_player_ship`, `set_static_walls`, per-level background helpers, and health bar drawing functions.

### `include/scenes/levels/`

Each level is a self-contained namespace with its own `State` struct, start function, per-frame tick function, collision handler, and `on_pause_resume` function. Scene state is a static variable managed by `PULSE_DEFINE_SCENE_STATE`.

### `include/events/asteroids.h`

Defines the `asteroid::Asteroid_Manager<N>` template and the `asteroid::Asteroid_Controller` type, which wraps the `asteroid_sm` boost.sml state machine with its configuration and live data. The manager handles sequential activation, respawning, and a Fisher-Yates shuffled selection pool so asteroids cycle without immediate repeats.

## Artwork

**Note:** Due to licensing restrictions, raw assets are not included in the repository. You must supply your own artwork and compile it for the hardware using the `png2header`, `png2bin`, and `animation2header` tools found in the Pulse2D engine.

Sprites loaded from the SD card at runtime use the `.bin` format produced by `png2bin`. Background and animation headers embedded directly in flash are produced by `png2header` and `animation2header` respectively. See the [Pulse2D tools documentation](ext/pulse2d/README.md) for usage details.

#### All assets, artwork, backgrounds, and audio are 100% human-created. No generative AI was used in this project.

## License

CC0 1.0 Universal
