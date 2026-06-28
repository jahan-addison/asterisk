
#include PULSE2D_HEADER
#include PULSE2D_GRAPHICS

#include "../include/dust-bg.h"
#include "../include/nebula-bg.h"
#include "../include/planet-bg.h"
#include "../include/stars-bg.h"

#include "../include/animated-laser-collision.h"
#include "../include/animated-ship.h"

PULSE2D_START_PULSE();

// @TODO: Better error handling on malformed sizing
PULSE_DEFINE_SCENE(Level_One, 9, 8);

PULSE_INIT_GAME(asterisk, Level_One);

PULSE_DEFINE_ANIMATOR(ship_animator);
PULSE_ANIMATION_DEFINITION(ship_thrust, animated_ship, 44, 39, 8, 60);

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

PULSE_ON_GAMESCENE_START(Level_One)
{
    set_blue_background();
    set_static_walls();
    set_player_ship();

    asterisk.set_sprite("meteor_sprite", "meteor_2.bin");

    asterisk.set_static_body("meteor_object",
        {
            .position = { 3.32f, 1.51f },
            .velocity = { 0.0f,  0.0f  },
            .width = { 4.0f,  2.0f  },
            .is_sensor = true
    });
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
    asterisk.draw("meteor_object", "meteor_sprite");

    asterisk.render_pool("laser_gun", [&](pulse2d_body* laser_object) {
        pulse2d_body& meteor = asterisk.get_body("meteor_object");

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