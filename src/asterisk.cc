
#include PULSE2D_HEADER
#include PULSE2D_GRAPHICS

#include <asteroids.h>

#include <backgrounds/dust-bg.h>
#include <backgrounds/nebula-bg.h>
#include <backgrounds/planet-bg.h>
#include <backgrounds/stars-bg.h>

#include <animations/animated-laser-collision.h>
#include <animations/animated-ship.h>

PULSE2D_START_PULSE();

// @TODO: Pre-pass inventory management
PULSE_DEFINE_SCENE(Level_One, 7, 12);

PULSE_INIT_GAME(asterisk, Level_One);

PULSE_DEFINE_ANIMATOR(ship_animator);
PULSE_ANIMATION_DEFINITION(ship_thrust, animated_ship, 44, 39, 8, 60);

PULSE_DEFINE int current_asteroid = 0;

PULSE_DEFINE asteroid::Asteroid_Manager<4> lvl_1_asteroids_{};

// @TODO: Split out level 1 code

/**
 * @brief
 * Set the player ship object
 *
 * @scope: all
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
 * Dispatch impact event on laser collision with an asteroid.
 *
 * @scope: PULSE_ON_GAMESCENE_START(Level_One)
 */
PULSE2D_INLINE void kinematic_laser_collided_asteriod(
    pulse2d_body* laser_object,
    const char* name)
{
    lvl_1_asteroids_.get(name).dispatch(asteroid::Impact_Event{});
    auto coords = get_body_coordinates(laser_object);
    asterisk.play_vfx(
        "laser_collision", to_int16(coords.x + 12), to_int16(coords.y - 12));
    asterisk.despawn("laser_gun", laser_object);
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

    // 1 - Size: 45 45
    asterisk.set_sprite("asteroid_1m_sprite", "asteroids/asteroid_1_m.bin");
    asterisk.set_sprite("asteroid_1s_sprite", "asteroids/asteroid_1_s.bin");
    // 2 - Size: 25 22
    asterisk.set_sprite("asteroid_5s_sprite", "asteroids/asteroid_5_s.bin");
    // 3 - Size: 62 65
    asterisk.set_sprite("asteroid_6l_sprite", "asteroids/asteroid_6_l.bin");
    // 4 - Size: 65 58
    asterisk.set_sprite("asteroid_2l_sprite", "asteroids/asteroid_6_l.bin");
    asterisk.set_sprite("asteroid_2m_sprite", "asteroids/asteroid_6_m.bin");

    asterisk.set_dynamic_body("asteroid_object_1",
        {
            .position = { 6.0f,                             pulse2d_math::trng_random(-2.5f, 2.5f) },
            .velocity = { pulse2d_math::trng_random(-10.0f, -7.5f),                          0.0f  },
            .width = px_to_units(65.0f, 65.0f),
            .mass = 1.0f,
            .is_sensor = true
    });

    asterisk.set_dynamic_body("asteroid_object_2",
        {
            .position = { 6.0f,     1.51f },
            .velocity = { -10.555f, 0.0f  },
            .width = px_to_units(25.0f, 22.0f),
            .mass = 1.0f,
            .is_sensor = true
    });

    asterisk.set_dynamic_body("asteroid_object_3",
        {
            .position = { 6.0f,    1.51f },
            .velocity = { -7.555f, 0.0f  },
            .width = px_to_units(62.0f, 65.0f),
            .mass = 1.0f,
            .is_sensor = true
    });

    asterisk.set_dynamic_body("asteroid_object_4",
        {
            .position = { 6.0f,    1.51f },
            .velocity = { -6.555f, 0.0f  },
            .width = px_to_units(65.0f, 58.0f),
            .mass = 1.0f,
            .is_sensor = true
    });

    lvl_1_asteroids_.add("asteroid_object_1", asteroid::Size::M, 2)
        .add("asteroid_object_2", asteroid::Size::S, 1)
        .add("asteroid_object_3", asteroid::Size::L, 1)
        .add("asteroid_object_4", asteroid::Size::L, 2);
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

    // Reset on start button
    if (SEESAW_BUTTON_INPUT(SEESAW_START)) {
        lvl_1_asteroids_.reset_all();
        current_asteroid = 0;
        PULSE_SET_SCENE(asterisk, Level_One);
    }

    if (SEESAW_BUTTON_INPUT(SEESAW_A)) {
        asterisk.spawn("laser_gun",
            250,
            ship.position.x + 1.0f,
            ship.position.y,
            30.0f,
            0.0f);
    }

    asterisk.draw("ship_object", "ship_sprite");

    auto& as_1 = asterisk.get_body("asteroid_object_1");
    auto& as_2 = asterisk.get_body("asteroid_object_2");
    auto& as_3 = asterisk.get_body("asteroid_object_3");
    auto& as_4 = asterisk.get_body("asteroid_object_4");

    // @TODO: Reset with faster velocity, random order asteroid selection
    switch (current_asteroid) {
        case 0: {
            lvl_1_asteroids_.execute_if_state<asteroid::Spawned_State>(
                "asteroid_object_1", [&](asteroid::Spawned_State const&) {
                    asterisk.draw("asteroid_object_1", "asteroid_1m_sprite");
                });
            lvl_1_asteroids_.execute_if_state<asteroid::Crashed_State>(
                "asteroid_object_1", [&](asteroid::Crashed_State const&) {
                    // gameover
                });
            lvl_1_asteroids_.execute_if_state<asteroid::Erased_State>(
                "asteroid_object_1", [&](asteroid::Erased_State const&) {
                    as_1.set_width({ 0.0f, 0.0f });
                    as_2.set_position(
                        { 6.0f, pulse2d_math::trng_random(-2.5f, 2.5f) });
                    current_asteroid++;
                });
            lvl_1_asteroids_.execute_if_state<asteroid::Live_State>(
                "asteroid_object_1", [&](asteroid::Live_State const& s) {
                    if (s.current_size == asteroid::Size::M) {
                        asterisk.draw(
                            "asteroid_object_1", "asteroid_1m_sprite");
                    }
                    if (s.current_size == asteroid::Size::S) {
                        as_1.set_width(asteroid::get_size_by_pixel(
                            asteroid::Size::S, 45, 45));
                        asterisk.draw(
                            "asteroid_object_1", "asteroid_1s_sprite");
                    }
                });
        } break;
        case 1: {
            lvl_1_asteroids_.execute_if_state<asteroid::Spawned_State>(
                "asteroid_object_2", [&](asteroid::Spawned_State const&) {
                    asterisk.draw("asteroid_object_2", "asteroid_5s_sprite");
                });
            lvl_1_asteroids_.execute_if_state<asteroid::Crashed_State>(
                "asteroid_object_2", [&](asteroid::Crashed_State const&) {
                    // gameover
                });
            lvl_1_asteroids_.execute_if_state<asteroid::Erased_State>(
                "asteroid_object_2", [&](asteroid::Erased_State const&) {
                    as_2.set_width({ 0.0f, 0.0f });
                    as_3.set_position(
                        { 6.0f, pulse2d_math::trng_random(-2.5f, 2.5f) });
                    current_asteroid++;
                });
            lvl_1_asteroids_.execute_if_state<asteroid::Live_State>(
                "asteroid_object_2", [&](asteroid::Live_State const& s) {
                    if (s.current_size == asteroid::Size::M) {
                        asterisk.draw(
                            "asteroid_object_1", "asteroid_1m_sprite");
                    }
                    if (s.current_size == asteroid::Size::S) {
                        asterisk.draw(
                            "asteroid_object_2", "asteroid_5s_sprite");
                    }
                });
        } break;
        case 2: {
            lvl_1_asteroids_.execute_if_state<asteroid::Spawned_State>(
                "asteroid_object_3", [&](asteroid::Spawned_State const&) {
                    asterisk.draw("asteroid_object_3", "asteroid_6l_sprite");
                });
            lvl_1_asteroids_.execute_if_state<asteroid::Crashed_State>(
                "asteroid_object_3", [&](asteroid::Crashed_State const&) {
                    // gameover
                });
            lvl_1_asteroids_.execute_if_state<asteroid::Erased_State>(
                "asteroid_object_3", [&](asteroid::Erased_State const&) {
                    as_3.set_width({ 0.0f, 0.0f });
                    as_4.set_position(
                        { 6.0f, pulse2d_math::trng_random(-2.5f, 2.5f) });
                    current_asteroid++;
                });
            lvl_1_asteroids_.execute_if_state<asteroid::Live_State>(
                "asteroid_object_3", [&](asteroid::Live_State const&) {
                    asterisk.draw("asteroid_object_3", "asteroid_6l_sprite");
                });
        } break;
        case 3: {

            lvl_1_asteroids_.execute_if_state<asteroid::Spawned_State>(
                "asteroid_object_4", [&](asteroid::Spawned_State const&) {
                    asterisk.draw("asteroid_object_4", "asteroid_2l_sprite");
                });
            lvl_1_asteroids_.execute_if_state<asteroid::Crashed_State>(
                "asteroid_object_4", [&](asteroid::Crashed_State const&) {
                    // gameover
                });
            lvl_1_asteroids_.execute_if_state<asteroid::Erased_State>(
                "asteroid_object_4", [&](asteroid::Erased_State const&) {
                    as_4.set_width({ 0.0f, 0.0f });
                    current_asteroid = 0;
                });
            lvl_1_asteroids_.execute_if_state<asteroid::Live_State>(
                "asteroid_object_4", [&](asteroid::Live_State const& s) {
                    if (s.current_size == asteroid::Size::L) {
                        asterisk.draw(
                            "asteroid_object_4", "asteroid_2l_sprite");
                    }
                    if (s.current_size == asteroid::Size::M) {
                        as_4.set_width(asteroid::get_size_by_pixel(
                            asteroid::Size::S, 65, 58));
                        asterisk.draw(
                            "asteroid_object_4", "asteroid_2m_sprite");
                    }
                });
        }
    }

    // @TODO: Explode ship on_collision_with_body(ship)

    asterisk.render_pool("laser_gun", [&](pulse2d_body* laser_object) {
        if (laser_object->position.x > 6.67f or
            laser_object->position.x < -10.0f) {
            asterisk.despawn("laser_gun", laser_object);
        } else {
            asterisk.draw_body(laser_object, "laser_sprite");
        }
        asterisk.on_collision_with_body(
            laser_object, [&](pulse2d_body* collided_with) {
                if (collided_with == &as_1 and current_asteroid == 0) {
                    kinematic_laser_collided_asteriod(
                        laser_object, "asteroid_object_1");
                }
                if (collided_with == &as_2 and current_asteroid == 1) {
                    kinematic_laser_collided_asteriod(
                        laser_object, "asteroid_object_2");
                }
                if (collided_with == &as_3 and current_asteroid == 2) {
                    kinematic_laser_collided_asteriod(
                        laser_object, "asteroid_object_3");
                }
                if (collided_with == &as_4 and current_asteroid == 3) {
                    kinematic_laser_collided_asteriod(
                        laser_object, "asteroid_object_4");
                }
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