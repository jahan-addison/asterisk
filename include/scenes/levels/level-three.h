#pragma once

#include PULSE2D_HEADER
#include PULSE2D_GRAPHICS

#include <common.h>

#include <animations/animated-enemy.h>
#include <animations/enemy-laser-collision.h>

#include <enemy/missile-0.h>

PULSE_DEFINE_ANIMATOR(enemy_animator);
PULSE_ANIMATION_DEFINITION(enemy_thrust, animated_enemy, 65, 45, 8, 60);

namespace scenes::levels::level_three {

///////////////////////////////////////////////////////////////////////////////
// Constants

// clang-format off

inline constexpr int      k_hits_per_health    = 30;

inline constexpr float    k_stage1_speed       = 0.018f;  // units per tick, stage 1
inline constexpr float    k_stage2_speed       = 0.030f;  // units per tick, stage 2
inline constexpr float    k_stage3_speed       = 0.055f;  // units per tick, stage 3
inline constexpr float    k_enemy_y_max        = 3.2f;
inline constexpr float    k_enemy_y_min        = -3.2f;

inline constexpr uint32_t k_laser_fire_ms      = 1800;    // enemy laser interval, stage 1
inline constexpr uint32_t k_laser_fire_ms_3    = 1000;    // enemy laser interval, stage 3
inline constexpr uint32_t k_rocket_fire_ms     = 2200;    // enemy rocket interval, stage 2
inline constexpr uint32_t k_rocket_fire_ms_3   = 1400;    // enemy rocket interval, stage 3
inline constexpr uint32_t k_side_rocket_ms     = 2500;    // stage 3 side rocket wave interval
inline constexpr uint32_t k_transition_ms      = 4000;    // between-stage pause duration

///////////////////////////////////////////////////////////////////////////////
// State

struct State
{
    int current_health = 5;
    int enemy_health = 5;
    int enemy_stage = 1;              // current stage: 1, 2, or 3
    int enemy_hit_count = 0;
    float enemy_y = 0.0f;             // enemy y position for vertical oscillation
    float enemy_direction = 1.0f;           // oscillation direction: +1 or -1
    bool boss_spawn = false;
    bool in_transition = false;       // between-stage pause window
    bool player_laser_enabled = true;
    elapsedMillis intro = 0;
    elapsedMillis fire_cooldown;
    elapsedMillis rocket_cooldown;
    elapsedMillis transition_timer;
    elapsedMillis side_rocket_timer;
    pulse2d::state::Draw_Fn draw = nullptr;
};

// clang-format on

PULSE_DEFINE_SCENE_STATE(State);

///////////////////////////////////////////////////////////////////////////////
// Utilities

PULSE_SCENE_FN void set_enemy_ship(pulse2d_scene_runtime<Scenes...>& game)
{
    // clang-format off
    game.set_sprite("enemy_sprite", "ship_2.bin")
        .set_sprite("enemy_laser_sprite", "enemy_shot_1.bin")
        .set_sprite_embedded(
            "missile_sprite", missile_0, missile_0_width, missile_0_height)
        .register_vfx(
            "enemy_laser_collision", enemy_laser_collision, 15, 27, 2, 17)
        .set_controlled_body("enemy_ship",
            {
                .position = { 4.32f, 2.51f },
                .velocity = { 0.0f,  0.0f  },
                .width = pixels_to_units(65.0, 45.0),
                .mass = 1.0f,
                .is_sensor = true,
            })
        .init_pool("enemy_laser_gun",
            {
                .position = { 20.0f, 0.0f },
                .velocity = { 0.0f, 0.0f },
                .width = { 2.0f, 0.5f },
                .is_sensor = true,
            })
        .init_pool("enemy_rocket",
            {
                .position = { 20.0f, 0.0f },
                .velocity = { 0.0f, 0.0f },
                .width = { 2.0f, 0.5f },
                .is_sensor = true,
            })
        .init_pool("side_rocket",
            {
                .position = { 20.0f, 0.0f },
                .velocity = { 0.0f, 0.0f },
                .width = { 2.0f, 0.5f },
                .is_sensor = true,
            })
        .init_pool("side_rocket_2",
            {
                .position = { 20.0f, 0.0f },
                .velocity = { 0.0f, 0.0f },
                .width = { 2.0f, 0.5f },
                .is_sensor = true,
            });
    // clang-format on
}

PULSE2D_INLINE void reset()
{
    state.current_health = 5;
    state.enemy_health = 5;
    state.enemy_stage = 1;
    state.enemy_hit_count = 0;
    state.enemy_y = 0.0f;
    state.enemy_direction = 1.0f;
    state.boss_spawn = false;
    state.in_transition = false;
    state.player_laser_enabled = true;
    state.intro = 0;
    state.fire_cooldown = 0;
    state.rocket_cooldown = 0;
    state.side_rocket_timer = 0;
}

PULSE2D_INLINE void on_pause_resume(uint32_t duration)
{
    auto freeze = [duration](elapsedMillis& t) {
        t = ((uint32_t)t > duration) ? (uint32_t)t - duration : 0u;
    };
    freeze(state.intro);
    freeze(state.fire_cooldown);
    freeze(state.rocket_cooldown);
    freeze(state.side_rocket_timer);
    freeze(state.transition_timer);
}

///////////////////////////////////////////////////////////////////////////////
// Collision handlers

PULSE_SCENE_FN void kinematic_laser_collided_enemy(
    pulse2d_scene_runtime<Scenes...>& game,
    pulse2d_body* laser_object)
{
    auto coords = get_body_coordinates(laser_object);
    game.play_vfx(
        "laser_collision", to_int16(coords.x + 12), to_int16(coords.y - 12));
    game.despawn("laser_gun", laser_object);

    state.enemy_hit_count++;
    if (state.enemy_hit_count >= k_hits_per_health) {
        state.enemy_health--;
        state.enemy_hit_count = 0;
    }
}

PULSE_SCENE_FN void kinematic_enemy_laser_collided_ship(
    pulse2d_scene_runtime<Scenes...>& game,
    pulse2d_body* laser_object)
{
    auto coords = get_body_coordinates(laser_object);
    game.play_vfx("enemy_laser_collision",
        to_int16(coords.x + 12),
        to_int16(coords.y - 12));
    game.despawn("enemy_laser_gun", laser_object);
}

PULSE_SCENE_FN void kinematic_enemy_rocket_collided_ship(
    pulse2d_scene_runtime<Scenes...>& game,
    pulse2d_body* rocket)
{
    auto coords = get_body_coordinates(rocket);
    game.play_vfx("enemy_laser_collision",
        to_int16(coords.x + 12),
        to_int16(coords.y - 12));
    game.despawn("enemy_rocket", rocket);
}

PULSE_SCENE_FN void kinematic_side_rocket_collided_ship(
    pulse2d_scene_runtime<Scenes...>& game,
    pulse2d_body* rocket,
    const char* rocket_name)
{
    auto coords = get_body_coordinates(rocket);
    game.play_vfx("enemy_laser_collision",
        to_int16(coords.x + 12),
        to_int16(coords.y - 12));
    game.despawn(rocket_name, rocket);
}

///////////////////////////////////////////////////////////////////////////////
// Lifecycle

PULSE_SCENE_FN void on_level_three_start(pulse2d_scene_runtime<Scenes...>& game,
    pulse2d::state::Draw_Fn draw_fn)
{
    state.draw = draw_fn;
    state.intro = 0;
    state.fire_cooldown = 0;
    state.rocket_cooldown = 0;
    state.side_rocket_timer = 0;

    set_purple_background(game);
    set_static_walls(game);
    set_player_ship(game);
    set_enemy_ship(game);
    set_blue_health_bar(game);
    set_red_health_bar(game);

    game.set_background_sprite("pause_screen_yes", pause_yes, 320, 240)
        .set_background_sprite("pause_screen_no", pause_no, 320, 240)
        .set_background_sprite("loading_screen", loading_screen, 320, 240);
}

PULSE_SCENE_FN void on_level_three_tick(pulse2d_scene_runtime<Scenes...>& game,
    pulse2d_body& ship,
    void (*on_gameover)(),
    void (*on_next_level)())
{
    PULSE_POLL_SEESAW_GAMEPAD();

    register_animation(enemy_animator, enemy_thrust);

    auto& enemy = game.get_body("enemy_ship");

    ///////////////////
    // intro message //
    ///////////////////

    if (state.intro < 9000) {
        game.draw_text_centered("... protect venus from enemy invasion ...",
            200,
            game.color(pulse2d_color::Purple),
            1.2f);
    }

    ////////////////////////////////
    // spawn boss once intro ends //
    ////////////////////////////////

    if (state.intro > 10000 and not state.boss_spawn) {
        enemy.set_position({ 4.0f, state.enemy_y });
        state.boss_spawn = true;
        state.fire_cooldown = 0;
        state.rocket_cooldown = 0;
        state.side_rocket_timer = 0;
    }

    /////////////////////
    // player controls //
    /////////////////////

    game.set_arcade_directional_inverted_control("ship_object", 12.55f, true);

    if (state.player_laser_enabled and SEESAW_BUTTON_INPUT(SEESAW_A)) {
        game.spawn("laser_gun",
            250,
            ship.position.x + 1.0f,
            ship.position.y,
            30.0f,
            0.0f);
    }

    game.draw("ship_object", "ship_sprite");
    draw_blue_health_bar(game, state.current_health);

    ///////////////////////
    // player laser pool //
    ///////////////////////

    game.render_pool("laser_gun", [&](pulse2d_body* laser_object) {
        if (laser_object->position.x > 6.67f or
            laser_object->position.x < -6.0f) {
            game.despawn("laser_gun", laser_object);
        } else {
            game.draw_body(laser_object, "laser_sprite");
        }
        game.on_collision_with_body(laser_object, [&](pulse2d_body* hit) {
            if (hit == &enemy) {
                kinematic_laser_collided_enemy(game, laser_object);
            }
        });
    });

    if (not state.boss_spawn)
        return;

    //////////////////////////////
    // between-stage transition //
    //////////////////////////////

    if (state.in_transition) {
        const char* msg = (state.enemy_stage == 1)
                              ? "... saturn gives me strength ..."
                              : "... you cannot stop me ...";
        game.draw_text_centered(
            msg, 115, game.color(pulse2d_color::White), 1.25f);

        draw_red_health_bar(game, state.enemy_health);
        game.draw("enemy_ship", "enemy_sprite");
        game.tick_animation(enemy_animator, "enemy_sprite");

        if (state.transition_timer > k_transition_ms) {
            state.enemy_stage++;
            state.enemy_health = 5;
            state.enemy_hit_count = 0;
            state.in_transition = false;
            state.player_laser_enabled = true;
            state.fire_cooldown = 0;
            state.rocket_cooldown = 0;
            state.side_rocket_timer = 0;
            if (state.enemy_stage == 3) {
                state.current_health = 5;
            }
        }

        if (state.current_health <= 0) {
            reset();
            on_gameover();
        }
        return;
    }

    ///////////////////////////////////////
    // move enemy (vertical oscillation) //
    ///////////////////////////////////////

    float speed = (state.enemy_stage == 1)   ? k_stage1_speed
                  : (state.enemy_stage == 2) ? k_stage2_speed
                                             : k_stage3_speed;
    state.enemy_y += speed * state.enemy_direction;
    if (state.enemy_y >= k_enemy_y_max) {
        state.enemy_y = k_enemy_y_max;
        state.enemy_direction = -1.0f;
    } else if (state.enemy_y <= k_enemy_y_min) {
        state.enemy_y = k_enemy_y_min;
        state.enemy_direction = 1.0f;
    }
    enemy.set_position({ 4.0f, state.enemy_y });

    /////////////////
    // enemy fires //
    /////////////////

    // 3: laser
    uint32_t laser_ms =
        (state.enemy_stage == 3) ? k_laser_fire_ms_3 : k_laser_fire_ms;
    if ((state.enemy_stage == 1 or state.enemy_stage == 3) and
        state.fire_cooldown > laser_ms) {
        game.spawn("enemy_laser_gun",
            250,
            enemy.position.x - 1.0f,
            enemy.position.y,
            -28.0f,
            0.0f);
        state.fire_cooldown = 0;
    }

    // stages 2 and 3: rocket
    uint32_t rocket_ms =
        (state.enemy_stage == 3) ? k_rocket_fire_ms_3 : k_rocket_fire_ms;
    if ((state.enemy_stage == 2 or state.enemy_stage == 3) and
        state.rocket_cooldown > rocket_ms) {
        game.spawn("enemy_rocket",
            250,
            enemy.position.x - 1.0f,
            enemy.position.y,
            -20.0f,
            0.0f);
        state.rocket_cooldown = 0;
    }

    // stage 3: side rockets from off-screen right, near top and bottom
    if (state.enemy_stage == 3 and state.side_rocket_timer > k_side_rocket_ms) {
        game.spawn("side_rocket", 300, 7.5f, 3.0f, -16.0f, 0.0f);
        game.spawn("side_rocket_2", 300, 7.5f, -3.0f, -16.0f, 0.0f);
        state.side_rocket_timer = 0;
    }

    //////////////////
    // render enemy //
    //////////////////

    draw_red_health_bar(game, state.enemy_health);
    game.draw("enemy_ship", "enemy_sprite");
    game.tick_animation(enemy_animator, "enemy_sprite");

    //////////////////////
    // enemy laser pool //
    //////////////////////

    game.render_pool("enemy_laser_gun", [&](pulse2d_body* el) {
        if (el->position.x < -7.0f) {
            game.despawn("enemy_laser_gun", el);
        } else {
            game.draw_body(el, "enemy_laser_sprite");
        }
        game.on_collision_with_body(el, [&](pulse2d_body* hit) {
            if (hit == &ship) {
                state.current_health--;
                kinematic_enemy_laser_collided_ship(game, el);
            }
        });
    });

    ///////////////////////
    // enemy rocket pool //
    ///////////////////////

    game.render_pool("enemy_rocket", [&](pulse2d_body* rocket) {
        if (rocket->position.x < -7.0f) {
            game.despawn("enemy_rocket", rocket);
        } else {
            game.draw_body(rocket, "missile_sprite");
        }
        game.on_collision_with_body(rocket, [&](pulse2d_body* hit) {
            if (hit == &ship) {
                state.current_health--;
                kinematic_enemy_rocket_collided_ship(game, rocket);
            }
        });
    });

    //////////////////////
    // side rocket pool //
    //////////////////////

    game.render_pool("side_rocket", [&](pulse2d_body* rocket) {
        if (rocket->position.x < -7.0f) {
            game.despawn("side_rocket", rocket);
        } else {
            game.draw_body(rocket, "missile_sprite");
        }
        game.on_collision_with_body(rocket, [&](pulse2d_body* hit) {
            if (hit == &ship) {
                state.current_health--;
                kinematic_side_rocket_collided_ship(
                    game, rocket, "side_rocket");
            }
        });
    });

    game.render_pool("side_rocket_2", [&](pulse2d_body* rocket) {
        if (rocket->position.x < -7.0f) {
            game.despawn("side_rocket_2", rocket);
        } else {
            game.draw_body(rocket, "missile_sprite");
        }
        game.on_collision_with_body(rocket, [&](pulse2d_body* hit) {
            if (hit == &ship) {
                state.current_health--;
                kinematic_side_rocket_collided_ship(
                    game, rocket, "side_rocket_2");
            }
        });
    });

    ///////////////////
    // health checks //
    ///////////////////

    if (state.current_health <= 0) {
        reset();
        on_gameover();
        return;
    }

    if (state.enemy_health <= 0) {
        if (state.enemy_stage == 3) {
            reset();
            on_next_level();
        } else {
            state.in_transition = true;
            state.transition_timer = 0;
            state.player_laser_enabled = false;
        }
    }
}

} // namespace scenes::levels::level_three
