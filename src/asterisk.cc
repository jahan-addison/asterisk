
#include PULSE2D_HEADER
#include PULSE2D_GRAPHICS

#include "asteroids.h"

#include "../include/dust-bg.h"
#include "../include/nebula-bg.h"
#include "../include/planet-bg.h"
#include "../include/stars-bg.h"

#include "../include/animated-laser-collision.h"
#include "../include/animated-ship.h"

PULSE2D_START_PULSE();

// @TODO: Pre-pass inventory management
PULSE_DEFINE_SCENE(Level_One, 6, 12);

PULSE_INIT_GAME(asterisk, Level_One);

PULSE_DEFINE_ANIMATOR(ship_animator);
PULSE_ANIMATION_DEFINITION(ship_thrust, animated_ship, 44, 39, 8, 60);

PULSE_DEFINE asteriods::Asteroid_Manager asteriods_{};

/**
 * @brief
 * Set the player ship object
 *
 * @scope: all
 * @allocate: 2 bodies, 2 sprites
 */
void set_player_ship()
{
    asterisk.set_sprite("ship_sprite", "ship_1.bin");
    asterisk.set_sprite("laser_sprite", "fire_shot_2_2.bin");

    asterisk.register_vfx(
        "laser_collision", animated_laser_collision, 15, 27, 2, 17);

    asterisk.set_controlled_body("ship_object",
        {
            .position = { -4.32f, 2.51f },
            .velocity = { 0.0f,   0.0f  },
            .width = { 1.0f,   1.0f  },
            .mass = 1.0f,
    });

    asterisk.init_pool("laser_gun",
        {
            .position = { -10.0f, 0.0f },
            .velocity = { 0.0f,   0.0f },
            .width = { 2.0f,   0.5f },
            .is_sensor = true
    });
}

/**
 * @brief
 * Set a blue parallax background
 *
 * @scope: all
 * @allocate: 0 bodies, 4 sprites
 */
void set_blue_background()
{
    asterisk.set_sprite_flash("sprite_nebula", bg_1, 320, 240);
    asterisk.set_sprite_flash("sprite_stars", bg_2, 320, 240);
    asterisk.set_sprite_flash("sprite_planets", bg_3, 320, 240);
    asterisk.set_sprite_flash("sprite_dust", bg_4, 320, 240);

    asterisk.add_parallax_layer("sprite_nebula", 320.0f, 15.0f);
    asterisk.add_parallax_layer("sprite_stars", 320.0f, 20.0f);
    asterisk.add_parallax_layer("sprite_planets", 320.0f, 40.0f);
    asterisk.add_parallax_layer("sprite_dust", 320.0f, 50.0f);
}

/**
 * @brief
 * Set the static walls objects
 *
 * @scope: all
 * @allocate: 2 bodies, 0 sprites
 */
void set_static_walls()
{
    asterisk.set_static_body("top_wall",
        {
            .position = { -4.5f, 4.0f },
            .width = { 2.0f,  0.5f },
    });

    asterisk.set_static_body("bottom_wall",
        {
            .position = { -4.5f, -4.0f },
            .width = { 2.0f,  0.5f  },
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
    set_blue_background();
    set_static_walls();
    set_player_ship();

    asterisk.set_sprite("meteor_1m_sprite", "meteors/meteor_1_m.bin");
    asterisk.set_sprite("meteor_1s_sprite", "meteors/meteor_1_s.bin");

    asterisk.set_sprite("meteor_5s_sprite", "meteors/meteor_5_s.bin");

    asterisk.set_sprite("meteor_6l_sprite", "meteors/meteor_6_l.bin");

    asterisk.set_sprite("meteor_2l_sprite", "meteors/meteor_6_l.bin");
    asterisk.set_sprite("meteor_2s_sprite", "meteors/meteor_6_s.bin");

    asterisk.set_dynamic_body("meteor_object_1",
        {
            .position = { 6.0f,    1.51f },
            .velocity = { -7.555f, 0.0f  },
            .width = px_to_units(65.0f, 65.0f),
            .mass = 1.0f,
            .is_sensor = true
    });

    asterisk.set_dynamic_body("meteor_object_2",
        {
            .position = { 3.32f, 1.51f },
            .velocity = { 0.0f,  0.0f  },
            .width = px_to_units(65.0f, 57.0f),
            .mass = 1.0f,
            .is_sensor = true
    });

    asterisk.set_dynamic_body("meteor_object_3",
        {
            .position = { 3.32f, 1.51f },
            .velocity = { 0.0f,  0.0f  },
            .width = px_to_units(62.0f, 65.0f),
            .mass = 3.0f,
            .is_sensor = true
    });

    // @TODO: hit counter in state machine
    asteriods_.objects["meteor_object_1"].start(
        asteriods::L_State{ &asterisk.get_body("meteor_object_1").width });
    asteriods_.objects["meteor_object_2"].start(
        asteriods::XL_State{ &asterisk.get_body("meteor_object_2").width });
    asteriods_.objects["meteor_object_3"].start(
        asteriods::L_State{ &asterisk.get_body("meteor_object_3").width });
}

PULSE_ON_GAMESCENE(Level_One)
{
    asterisk.tick();

    PULSE_POLL_SEESAW_GAMEPAD();

    asterisk.render_backgrounds();

    pulse2d_body& ship = asterisk.get_body("ship_object");

    register_animation(ship_animator, ship_thrust);

    asterisk.set_arcade_directional_inverted_control(
        "ship_object", 12.55f, true);

    if (SEESAW_BUTTON_INPUT(SEESAW_A)) {
        asterisk.spawn("laser_gun",
            250,
            ship.position.x + 1.0f,
            ship.position.y,
            30.0f,
            0.0f);
    }

    asterisk.draw("ship_object", "ship_sprite");
    asterisk.draw("meteor_object_1", "meteor_1m_sprite");

    asterisk.render_pool("laser_gun", [&](pulse2d_body* laser_object) {
        pulse2d_body& meteor = asterisk.get_body("meteor_object_1");

        if (laser_object->position.x > 6.67f or
            laser_object->position.x < -10.0f) {
            asterisk.despawn("laser_gun", laser_object);
        } else {
            asterisk.draw_body(laser_object, "laser_sprite");
        }

        asterisk.on_collision(laser_object, &meteor, [&] {
            auto coords = get_body_coordinates(laser_object);
            asterisk.play_vfx("laser_collision",
                to_int16(coords.x + 12),
                to_int16(coords.y - 12));
            asterisk.despawn("laser_gun", laser_object);
        });
    });

    asterisk.tick_animation(ship_animator, "ship_sprite");
    asterisk.tick_vfx();

    asterisk.render();
}

PULSE_ON_GAMESTART()
{
    Serial.begin(115200);
    // PULSE_POLL_SERIAL_CONNECTION();
    pulse_register_etl_error_handler();
    asterisk.init(0.0f, 0.0f, 10);
    PULSE_ENABLE_SEESAW_GAMEPAD();
    PULSE_SET_SCENE(asterisk, Level_One);
}

PULSE_ON_GAMELOOP()
{
    PULSE_TICK_GAMESCENE();
}