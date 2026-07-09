
#include PULSE2D_HEADER
#include PULSE2D_GRAPHICS

#include <backgrounds/pause-no.h>
#include <backgrounds/pause-yes.h>

#include <scenes/levels/level_one.h>
#include <scenes/menus/gameover.h>
#include <scenes/menus/start.h>

namespace level_one = scenes::levels::level_one;

namespace gameover = scenes::menus::gameover;
namespace start = scenes::menus::start;

PULSE2D_START_PULSE();

PULSE_DEFINE_SCENE(Start_Screen, 1, 1);
PULSE_DEFINE_SCENE(Pause_Screen, 1, 2);
PULSE_DEFINE_SCENE(Game_Over, 1, 4);
PULSE_DEFINE_SCENE(Level_One, 7, 4);
PULSE_DEFINE_SCENE(Level_Two, 7, 4);
PULSE_DEFINE_SCENE(Level_Three, 7, 4);

PULSE_INIT_GAME(asterisk,
    Start_Screen,
    Pause_Screen,
    Game_Over,
    Level_One,
    Level_Two,
    Level_Three);

PULSE_DEFINE_ANIMATOR(ship_animator);
PULSE_ANIMATION_DEFINITION(ship_thrust, animated_ship, 44, 39, 8, 60);

struct State
{
    bool game_paused = false;
    bool resume_play = false;
};

PULSE_DEFINE_SCENE_STATE(State);

///////////////////////////////////////////////////////////////////////////////
// Level One

/**
 * @brief
 * First level Start
 *
 * @scope: Level_One
 */
PULSE_ON_GAMESCENE_START(Level_One)
{
    level_one::on_level_one_start(
        asterisk, [](pulse2d_body* body, const char* sprite) {
            asterisk.draw_body(body, sprite);
        });
}

PULSE_ON_GAMESCENE(Level_One)
{
    asterisk.tick();
    asterisk.render_backgrounds();

    pulse2d_body& ship = asterisk.get_body("ship_object");

    register_animation(ship_animator, ship_thrust);

    level_one::on_level_one_tick(
        asterisk,
        ship,
        [] { PULSE_DEFER_SCENE(asterisk, Game_Over); },
        [] { PULSE_DEFER_SCENE(asterisk, Level_One); }); // next level

    asterisk.tick_animation(ship_animator, "ship_sprite");
    asterisk.tick_vfx();
    asterisk.render();
}

///////////////////////////////////////////////////////////////////////////////
// Start game

/**
 * @brief
 * Start Game
 *
 * @scope: Start_Screen
 */
PULSE_ON_GAMESCENE_START(Start_Screen)
{
    start::on_start(asterisk);
}

PULSE_ON_GAMESCENE(Start_Screen)
{
    asterisk.tick();
    start::on_start_tick(
        asterisk, [] { PULSE_SET_SCENE(asterisk, Level_One); });
    asterisk.render();
}

///////////////////////////////////////////////////////////////////////////////
// Game Over

/**
 * @brief
 * Game Over
 *
 * @scope: Game_Over
 */
PULSE_ON_GAMESCENE_START(Game_Over)
{
    gameover::on_gameover_start(asterisk);
}

PULSE_ON_GAMESCENE(Game_Over)
{
    asterisk.tick();
    gameover::on_gameover_tick(
        asterisk, [] { PULSE_SET_SCENE(asterisk, Start_Screen); }); // on_reset
    asterisk.render();
}

///////////////////////////////////////////////////////////////////////////////
// Lifecycle

PULSE_ON_GAMESTART()
{
    Serial.begin(115200);
    // PULSE_POLL_SERIAL_CONNECTION();
    pulse2d_math::init_trng_engine_random();
    pulse_register_etl_error_handler();
    asterisk.init(0.0f, 0.0f, 10);
    PULSE_ENABLE_SEESAW_GAMEPAD();
    PULSE_SET_SCENE(asterisk, Start_Screen);
}

PULSE_ON_GAMELOOP()
{
    PULSE_POLL_SEESAW_GAMEPAD();

    //////////////////
    // Pause screen //
    //////////////////

    if (state.game_paused or
        (SEESAW_BUTTON_INPUT(SEESAW_SELECT) and
            not std::holds_alternative<Start_Screen>(asterisk.current_scene))) {
        state.game_paused = true;

        if (state.game_paused) {
            if (SEESAW_DIRECTION_IS_LEFT())
                state.resume_play = false;
            if (SEESAW_DIRECTION_IS_RIGHT())
                state.resume_play = true;

            if (state.resume_play)
                asterisk.draw_sprite("pause_screen_yes", 0, 0);
            else
                asterisk.draw_sprite("pause_screen_no", 0, 0);

            if (state.resume_play and SEESAW_BUTTON_INPUT(SEESAW_A))
                state.game_paused = false;
        }
        asterisk.render();
        return;
    }
    PULSE_TICK_GAMESCENE();
}
