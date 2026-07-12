#pragma once

#include PULSE2D_HEADER
#include PULSE2D_GRAPHICS

#include <common.h>

#include <backgrounds/win-no.h>
#include <backgrounds/win-yes.h>

namespace scenes::menus::game_win {

struct State
{
    bool start_over = false;
};

PULSE_DEFINE_SCENE_STATE(State);

///////////////////////////////////////////////////////////////////////////////
// Lifecycle

/**
 * @brief
 * Game over
 *
 * @scope: PULSE_ON_GAMESCENE_START(Game_Won)
 */
PULSE_SCENE_FN void on_game_win_start(pulse2d_scene_runtime<Scenes...>& game)
{
    state = {};
    game.set_background_sprite("game_win_yes", win_yes, 320, 240)
        .set_background_sprite("game_win_no", win_no, 320, 240);
}

/**
 * @brief
 * Per-frame Winning Scene Logic
 *
 * @scope: PULSE_ON_GAMESCENE(Game_Won)
 */
PULSE_SCENE_FN void on_game_win_tick(pulse2d_scene_runtime<Scenes...>& game,
    void (*on_reset)())
{
    PULSE_POLL_SEESAW_GAMEPAD();

    if (SEESAW_DIRECTION_IS_LEFT())
        state.start_over = false;
    if (SEESAW_DIRECTION_IS_RIGHT())
        state.start_over = true;

    if (state.start_over)
        game.draw_sprite("game_win_yes", 0, 0);
    else
        game.draw_sprite("game_win_no", 0, 0);

    if (SEESAW_BUTTON_INPUT(SEESAW_A) and state.start_over)
        on_reset();
}

} // namespace scenes::menus::game_win
