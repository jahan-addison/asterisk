
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
    asterisk.set_sprite("meteor_1m_sprite", "meteors/meteor_1_m.bin");
    asterisk.set_sprite("meteor_1s_sprite", "meteors/meteor_1_s.bin");
    // 2 - Size: 25 22
    asterisk.set_sprite("meteor_5s_sprite", "meteors/meteor_5_s.bin");
    // 3 - Size: 62 65
    asterisk.set_sprite("meteor_6l_sprite", "meteors/meteor_6_l.bin");
    // 4 - Size: 65 58
    asterisk.set_sprite("meteor_2l_sprite", "meteors/meteor_6_l.bin");
    asterisk.set_sprite("meteor_2m_sprite", "meteors/meteor_6_m.bin");

    asterisk.set_dynamic_body("meteor_object_1",
        {
            .position = { 6.0f,    -2.5f },
            .velocity = { -7.555f, 0.0f  },
            .width = px_to_units(65.0f, 65.0f),
            .mass = 1.0f,
            .is_sensor = true
    });

    asterisk.set_dynamic_body("meteor_object_2",
        {
            .position = { 6.0f,     1.51f },
            .velocity = { -10.555f, 0.0f  },
            .width = px_to_units(25.0f, 22.0f),
            .mass = 1.0f,
            .is_sensor = true
    });

    asterisk.set_dynamic_body("meteor_object_3",
        {
            .position = { 6.0f,    1.51f },
            .velocity = { -7.555f, 0.0f  },
            .width = px_to_units(62.0f, 65.0f),
            .mass = 2.0f,
            .is_sensor = true
    });

    asterisk.set_dynamic_body("meteor_object_4",
        {
            .position = { 6.0f,    1.51f },
            .velocity = { -6.555f, 0.0f  },
            .width = px_to_units(65.0f, 58.0f),
            .mass = 2.0f,
            .is_sensor = true
    });

    lvl_1_asteroids_.objects.at(0).dispatch(
        asteroid::Spawn_Event{ asteroid::Size::M, 2 });
    lvl_1_asteroids_.objects.at(1).dispatch(
        asteroid::Spawn_Event{ asteroid::Size::S, 1 });
    lvl_1_asteroids_.objects.at(2).dispatch(
        asteroid::Spawn_Event{ asteroid::Size::L, 1 });
    lvl_1_asteroids_.objects.at(3).dispatch(
        asteroid::Spawn_Event{ asteroid::Size::L, 2 });
}

PULSE2D_INLINE void kinematic_laser_collided_asteriod(
    pulse2d_body* laser_object,
    size_t index)
{
    lvl_1_asteroids_.objects[index].dispatch(asteroid::Impact_Event{});
    auto coords = get_body_coordinates(laser_object);
    asterisk.play_vfx(
        "laser_collision", to_int16(coords.x + 12), to_int16(coords.y - 12));
    asterisk.despawn("laser_gun", laser_object);
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

    auto& as_1 = asterisk.get_body("meteor_object_1");
    auto& as_2 = asterisk.get_body("meteor_object_2");
    auto& as_3 = asterisk.get_body("meteor_object_3");
    auto& as_4 = asterisk.get_body("meteor_object_4");

    const auto& as_1_state = lvl_1_asteroids_.objects.at(0).get_current_state();
    const auto& as_2_state = lvl_1_asteroids_.objects.at(1).get_current_state();
    const auto& as_3_state = lvl_1_asteroids_.objects.at(2).get_current_state();
    const auto& as_4_state = lvl_1_asteroids_.objects.at(3).get_current_state();

    // @TODO: Reset with faster velocity, random order asteroid selection
    switch (current_asteroid) {
        case 0: {
            std::visit(
                pulse2d_util::overload{
                    [&](asteroid::Spawned_State const&) {
                        asterisk.draw("meteor_object_1", "meteor_1m_sprite");
                    },
                    [](asteroid::Crashed_State const&) {
                        // game over
                    },
                    [&](asteroid::Erased_State const&) {
                        as_1.set_width({ 0.0f, 0.0f });
                        as_2.set_position(
                            { 6.0f, pulse2d_math::trng_random(-2.5f, 2.5f) });
                        current_asteroid++;
                    },
                    [&](asteroid::Live_State const& s) {
                        if (s.current_size == asteroid::Size::M) {
                            asterisk.draw(
                                "meteor_object_1", "meteor_1m_sprite");
                        }
                        if (s.current_size == asteroid::Size::S) {
                            as_1.set_width(asteroid::get_size_by_pixel(
                                asteroid::Size::S, 45, 45));
                            asterisk.draw(
                                "meteor_object_1", "meteor_1s_sprite");
                        }
                    },
                },
                as_1_state);
        } break;
        case 1: {
            std::visit(
                pulse2d_util::overload{
                    [&](asteroid::Spawned_State const&) {
                        asterisk.draw("meteor_object_2", "meteor_5s_sprite");
                    },
                    [](asteroid::Crashed_State const&) {
                        // game over
                    },
                    [&](asteroid::Erased_State const&) {
                        as_2.set_width({ 0.0f, 0.0f });
                        as_3.set_position(
                            { 6.0f, pulse2d_math::trng_random(-2.5f, 2.0f) });
                        current_asteroid++;
                    },
                    [&](asteroid::Live_State const&) {
                        asterisk.draw("meteor_object_2", "meteor_5s_sprite");
                    },
                },
                as_2_state);
        } break;
        case 2: {
            std::visit(
                pulse2d_util::overload{
                    [&](asteroid::Spawned_State const&) {
                        asterisk.draw("meteor_object_3", "meteor_6l_sprite");
                    },
                    [](asteroid::Crashed_State const&) {
                        // game over
                    },
                    [&](asteroid::Erased_State const&) {
                        as_3.set_width({ 0.0f, 0.0f });
                        as_4.set_position(
                            { 6.0f, pulse2d_math::trng_random(-2.5f, 2.5f) });
                        current_asteroid++;
                    },
                    [&](asteroid::Live_State const&) {
                        asterisk.draw("meteor_object_3", "meteor_6l_sprite");
                    },
                },
                as_3_state);
        } break;
        case 3: {
            std::visit(pulse2d_util::overload{
                           [&](asteroid::Spawned_State const&) {
                               asterisk.draw(
                                   "meteor_object_4", "meteor_2l_sprite");
                           },
                           [](asteroid::Crashed_State const&) {
                               // game over
                           },
                           [&](asteroid::Erased_State const&) {
                               as_4.set_width({ 0.0f, 0.0f });
                               current_asteroid = 0;
                           },
                           [&](asteroid::Live_State const& s) {
                               if (s.current_size == asteroid::Size::L) {
                                   asterisk.draw(
                                       "meteor_object_4", "meteor_2l_sprite");
                               }
                               if (s.current_size == asteroid::Size::M) {
                                   as_4.set_width(asteroid::get_size_by_pixel(
                                       asteroid::Size::S, 65, 58));
                                   asterisk.draw(
                                       "meteor_object_4", "meteor_2m_sprite");
                               }
                           },
                       },
                as_4_state);
        }
    }

    // @TODO: on_collision_with_body(ship)

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
                    kinematic_laser_collided_asteriod(laser_object, 0);
                }
                if (collided_with == &as_2 and current_asteroid == 1) {
                    kinematic_laser_collided_asteriod(laser_object, 1);
                }
                if (collided_with == &as_3 and current_asteroid == 2) {
                    kinematic_laser_collided_asteriod(laser_object, 2);
                }
                if (collided_with == &as_4 and current_asteroid == 3) {
                    kinematic_laser_collided_asteriod(laser_object, 3);
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