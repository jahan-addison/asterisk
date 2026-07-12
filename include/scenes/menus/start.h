#pragma once

#include PULSE2D_HEADER
#include PULSE2D_GRAPHICS

#include <common.h>

#include <backgrounds/start-screen.h>

namespace scenes::menus::start {

///////////////////////////////////////////////////////////////////////////////
// Lifecycle

/**
 * @brief
 * Start Game
 *
 * @scope: PULSE_ON_GAMESCENE_START(Start_Screen)
 */
PULSE_SCENE_FN void on_start(pulse2d_scene_runtime<Scenes...>& game)
{
    game.set_background_sprite("start_screen", start_screen, 320, 240);
}

/**
 * @brief
 * Per-frame Game Start logic
 *
 * @scope: PULSE_ON_GAMESCENE(Start_Screen)
 */
PULSE_SCENE_FN void on_start_tick(pulse2d_scene_runtime<Scenes...>& game,
    void (*on_reset)())
{
    PULSE_POLL_SEESAW_GAMEPAD();

    game.draw_sprite("start_screen", 0, 0);

    if (SEESAW_BUTTON_INPUT(SEESAW_START))
        on_reset();
}

} // namespace scenes::menus::gameover
