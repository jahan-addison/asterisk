
#include PULSE2D_HEADER
#include PULSE2D_GRAPHICS

#include <backgrounds/dust-bg.h>
#include <backgrounds/nebula-bg.h>
#include <backgrounds/planet-bg.h>
#include <backgrounds/stars-bg.h>

#include <animations/animated-laser-collision.h>
#include <animations/animated-ship.h>

#include <events/asteroids.h>

#include <scenes/levels/level_one.h>

namespace level_one = scenes::levels::level_one;

PULSE2D_START_PULSE();

PULSE_DEFINE_SCENE(Level_One, 7, 2);

PULSE_INIT_GAME(asterisk, Level_One);

PULSE_DEFINE_ANIMATOR(ship_animator);
PULSE_ANIMATION_DEFINITION(ship_thrust, animated_ship, 44, 39, 8, 60);

/**
 * @brief
 * Set the player ship object
 *
 * @scope: all
 */
void set_player_ship()
{
    asterisk.set_sprite("ship_sprite", "ship_1.bin")
        .set_sprite("laser_sprite", "fire_shot_2_2.bin")
        .register_vfx(
            "laser_collision", animated_laser_collision, 15, 27, 2, 17)
        .set_controlled_body("ship_object",
            {
                .position = { -4.32f, 2.51f },
                .velocity = { 0.0f,   0.0f  },
                .width = { 1.0f,   1.0f  },
                .mass = 1.0f,
    })
        .init_pool("laser_gun",
            {
                .position = { -10.0f, 0.0f },
                .velocity = { 0.0f, 0.0f },
                .width = { 2.0f, 0.5f },
                .is_sensor = true,
            });
}

/**
 * @brief
 * First level Scene Start
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
        asterisk, ship, [] { PULSE_SET_SCENE(asterisk, Level_One); });

    asterisk.tick_animation(ship_animator, "ship_sprite");
    asterisk.tick_vfx();
    asterisk.render();
}

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
