#pragma once

#include PULSE2D_HEADER
#include PULSE2D_GRAPHICS

#include <common.h>

namespace scenes::menus::gameover {

///////////////////////////////////////////////////////////////////////////////
// Lifecycle

/**
 * @brief
 * Game over
 *
 * @scope: PULSE_ON_GAMESCENE_START(Game_Over)
 */
PULSE_SCENE_FN void on_gameover_start(pulse2d_scene_runtime<Scenes...>& game)
{
    game.set_background_sprite("gameover_screen", gameover_1, 320, 240)
        .add_background_layer("gameover_screen", 320.0f);
}

/**
 * @brief
 * Per-frame Game over logic: Restart game on start button
 *
 * @scope: PULSE_ON_GAMESCENE(Game_Over)
 */
PULSE_SCENE_FN void on_gameover_tick(
    [[maybe_unused]] pulse2d_scene_runtime<Scenes...>& game,
    void (*on_reset)())
{
    PULSE_POLL_SEESAW_GAMEPAD();

    if (SEESAW_BUTTON_INPUT(SEESAW_START)) {
        on_reset();
    }
}

} // namespace scenes::menus::gameover
