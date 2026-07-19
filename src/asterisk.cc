
#include PULSE2D_HEADER
#include PULSE2D_GRAPHICS

#include <backgrounds/pause-no.h>
#include <backgrounds/pause-yes.h>

#include <scenes/levels/level-one.h>
#include <scenes/levels/level-three.h>
#include <scenes/levels/level-two.h>
#include <scenes/menus/gameover.h>
#include <scenes/menus/start.h>
#include <scenes/menus/win.h>

////////////
// Scenes //
////////////

namespace level_one = scenes::levels::level_one;
namespace level_two = scenes::levels::level_two;
namespace level_three = scenes::levels::level_three;

namespace gameover = scenes::menus::gameover;
namespace game_win = scenes::menus::game_win;
namespace start = scenes::menus::start;

PULSE2D_START_PULSE();

PULSE_DEFINE_SCENE(Start_Screen, 1, 1);
PULSE_DEFINE_SCENE(Pause_Screen, 1, 2);
PULSE_DEFINE_SCENE(Game_Won, 1, 2);
PULSE_DEFINE_SCENE(Game_Over, 1, 2);
PULSE_DEFINE_SCENE(Level_One, 7, 5);
PULSE_DEFINE_SCENE(Level_Two, 7, 7);
PULSE_DEFINE_SCENE(Level_Three, 9, 7);

PULSE_INIT_GAME(asterisk,
    Start_Screen,
    Pause_Screen,
    Game_Over,
    Game_Won,
    Level_One,
    Level_Two,
    Level_Three);

///////////
// State //
///////////

PULSE_DEFINE_ANIMATOR(ship_animator);
PULSE_ANIMATION_DEFINITION(ship_thrust, animated_ship, 44, 39, 8, 60);

struct State
{
    bool game_paused = false;
    bool game_loading = false;
    bool resume_play = false;
    elapsedMillis load_timer = 0;
    uint32_t pause_start_ms = 0;
};

PULSE_DEFINE_SCENE_STATE(State);

#define PULSE_LOAD_NEXT_SCENE(game, scene) \
    pending_transition = []() {            \
        state.load_timer = 0;              \
        state.game_loading = true;         \
        PULSE_SET_SCENE(game, scene);      \
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
        asterisk, [] { PULSE_LOAD_NEXT_SCENE(asterisk, Level_One); });
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
// Winning screen

/**
 * @brief
 * Game Won
 *
 * @scope: Game Won
 */
PULSE_ON_GAMESCENE_START(Game_Won)
{
    game_win::on_game_win_start(asterisk);
}

PULSE_ON_GAMESCENE(Game_Won)
{
    asterisk.tick();
    game_win::on_game_win_tick(
        asterisk, [] { PULSE_SET_SCENE(asterisk, Start_Screen); }); // on_reset
    asterisk.render();
}

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
        [] { PULSE_LOAD_NEXT_SCENE(asterisk, Level_Two); }); // next level

    asterisk.tick_animation(ship_animator, "ship_sprite");
    asterisk.tick_vfx();
    asterisk.render();
}

///////////////////////////////////////////////////////////////////////////////
// Level Two

/**
 * @brief
 * Second level Start
 *
 * @scope: Level_Two
 */
PULSE_ON_GAMESCENE_START(Level_Two)
{
    level_two::on_level_two_start(
        asterisk, [](pulse2d_body* body, const char* sprite) {
            asterisk.draw_body(body, sprite);
        });
}

PULSE_ON_GAMESCENE(Level_Two)
{
    asterisk.tick();
    asterisk.render_backgrounds();

    pulse2d_body& ship = asterisk.get_body("ship_object");

    register_animation(ship_animator, ship_thrust);

    level_two::on_level_two_tick(
        asterisk,
        ship,
        [] { PULSE_DEFER_SCENE(asterisk, Game_Over); },
        [] { PULSE_LOAD_NEXT_SCENE(asterisk, Level_Three); }); // next level

    asterisk.tick_animation(ship_animator, "ship_sprite");
    asterisk.tick_vfx();
    asterisk.render();
}

///////////////////////////////////////////////////////////////////////////////
// Level Three

/**
 * @brief
 * Third level Start
 *
 * @scope: Level_Three
 */
PULSE_ON_GAMESCENE_START(Level_Three)
{
    level_three::on_level_three_start(
        asterisk, [](pulse2d_body* body, const char* sprite) {
            asterisk.draw_body(body, sprite);
        });
}

PULSE_ON_GAMESCENE(Level_Three)
{
    asterisk.tick();
    asterisk.render_backgrounds();

    pulse2d_body& ship = asterisk.get_body("ship_object");

    register_animation(ship_animator, ship_thrust);

    level_three::on_level_three_tick(
        asterisk,
        ship,
        [] { PULSE_DEFER_SCENE(asterisk, Game_Over); },
        [] { PULSE_DEFER_SCENE(asterisk, Game_Won); });

    asterisk.tick_animation(ship_animator, "ship_sprite");
    asterisk.tick_vfx();
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
    // asterisk.enable_audio();
    PULSE_ENABLE_SEESAW_GAMEPAD();
    PULSE_SET_SCENE(asterisk, Level_One);
}

PULSE_ON_GAMELOOP()
{
    PULSE_POLL_SEESAW_GAMEPAD();

    /////////////////
    // Load screen //
    /////////////////

    if (state.game_loading) {
        if (state.load_timer <= 3500) {
            asterisk.draw_sprite("loading_screen", 0, 0);
            asterisk.render();
            return;
        } else {
            state.game_loading = false;
        }
    }

    //////////////////
    // Pause screen //
    //////////////////

    if (state.game_paused or
        (SEESAW_BUTTON_INPUT(SEESAW_SELECT) and
            not std::holds_alternative<Start_Screen>(asterisk.current_scene))) {

        if (not state.game_paused)
            state.pause_start_ms = millis();

        state.game_paused = true;

        if (SEESAW_DIRECTION_IS_LEFT())
            state.resume_play = false;
        if (SEESAW_DIRECTION_IS_RIGHT())
            state.resume_play = true;

        if (state.resume_play)
            asterisk.draw_sprite("pause_screen_yes", 0, 0);
        else
            asterisk.draw_sprite("pause_screen_no", 0, 0);

        if (state.resume_play and SEESAW_BUTTON_INPUT(SEESAW_A)) {
            state.game_paused = false;
            const uint32_t duration = millis() - state.pause_start_ms;
            level_one::on_pause_resume(duration);
            level_two::on_pause_resume(duration);
            level_three::on_pause_resume(duration);
        }

        asterisk.render();
        return;
    }

    ///////////////
    // Game tick //
    ///////////////

    PULSE_TICK_GAMESCENE();
}
