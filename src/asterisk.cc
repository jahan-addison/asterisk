
#include PULSE2D_HEADER
#include PULSE2D_GRAPHICS

#include <scenes/levels/level_one.h>
#include <scenes/menus/gameover/gameover.h>

namespace level_one = scenes::levels::level_one;
namespace gameover = scenes::menus::gameover;

PULSE2D_START_PULSE();

PULSE_DEFINE_SCENE(Menu, 4, 4);
PULSE_DEFINE_SCENE(Game_Over, 1, 2);
PULSE_DEFINE_SCENE(Level_One, 7, 2);
PULSE_DEFINE_SCENE(Level_Two, 7, 2);
PULSE_DEFINE_SCENE(Level_Three, 7, 2);

PULSE_INIT_GAME(asterisk, Menu, Game_Over, Level_One, Level_Two, Level_Three);

PULSE_DEFINE_ANIMATOR(ship_animator);
PULSE_ANIMATION_DEFINITION(ship_thrust, animated_ship, 44, 39, 8, 60);

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

    level_one::on_level_one_tick(asterisk, ship, [] {
        PULSE_DEFER_SCENE(asterisk, Game_Over);
    }); // on_gameover

    asterisk.tick_animation(ship_animator, "ship_sprite");
    asterisk.tick_vfx();
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
        asterisk, [] { PULSE_SET_SCENE(asterisk, Level_One); }); // on_reset
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
    PULSE_SET_SCENE(asterisk, Level_One);
}

PULSE_ON_GAMELOOP()
{
    PULSE_TICK_GAMESCENE();
}
