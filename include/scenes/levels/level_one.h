#pragma once

#include PULSE2D_HEADER
#include PULSE2D_GRAPHICS

#include <common.h>

#include <events/asteroids.h>

#include <backgrounds/pause-no.h>
#include <backgrounds/pause-yes.h>

#include <sprites/asteroids/asteroid_1_l.h>
#include <sprites/asteroids/asteroid_1_m.h>
#include <sprites/asteroids/asteroid_1_s.h>
#include <sprites/asteroids/asteroid_2_l.h>
#include <sprites/asteroids/asteroid_2_m.h>
#include <sprites/asteroids/asteroid_2_s.h>
#include <sprites/asteroids/asteroid_5_l.h>
#include <sprites/asteroids/asteroid_5_m.h>
#include <sprites/asteroids/asteroid_5_s.h>
#include <sprites/asteroids/asteroid_7_l.h>
#include <sprites/asteroids/asteroid_7_m.h>
#include <sprites/asteroids/asteroid_7_s.h>

namespace scenes::levels::level_one {

struct State
{
    int current_health = 5;
    int current_asteroid = 0;
    int total_missed = 0;
    elapsedMillis timer = 0;
    float velocity_ratio = 1.485f;
    asteroid::Asteroid_Manager<4> asteroids{};

    pulse2d_body* collided_obj = nullptr;

    pulse2d::state::Draw_Fn draw = nullptr;
};

PULSE_DEFINE_SCENE_STATE(State);

///////////////////////////////////////////////////////////////////////////////
// Utilities

/**
 * @brief
 * Clear state
 *
 * @scope: on_level_one_tick
 */
PULSE2D_INLINE void reset()
{
    state.current_health = 4;
    state.current_asteroid = 0;
    state.total_missed = 0;
    state.timer = 0;
    state.velocity_ratio = 1.0f;
    state.asteroids.reset_all();
    state.collided_obj = nullptr;
    state.draw = nullptr;
}

/**
 * @brief
 * Dispatch impact event on laser collision with an asteroid.
 *
 * @scope: PULSE_ON_GAMESCENE
 */
PULSE_SCENE_FN void kinematic_laser_collided_asteroid(
    pulse2d_scene_runtime<Scenes...>& game,
    pulse2d_body* laser_object,
    size_t index)
{
    state.asteroids.get(index).dispatch(asteroid::Impact_Event{});
    auto coords = get_body_coordinates(laser_object);
    game.play_vfx(
        "laser_collision", to_int16(coords.x + 12), to_int16(coords.y - 12));
    game.despawn("laser_gun", laser_object);
}

///////////////////////////////////////////////////////////////////////////////
// Lifecycle

/**
 * @brief
 * Initialize all level-one bodies, sprites, and asteroid configs
 * draw_fn must be a non-capturing lambda referencing the global runtime
 *
 * @scope: PULSE_ON_GAMESCENE_START(Level_One)
 */
PULSE_SCENE_FN void on_level_one_start(pulse2d_scene_runtime<Scenes...>& game,
    pulse2d::state::Draw_Fn draw_fn)
{
    state.draw = draw_fn;

    set_blue_background(game);
    set_static_walls(game);
    set_player_ship(game);
    set_blue_health_bar(game);

    // clang-format off
    game
        .set_sprite_embedded("asteroid_1l_sprite", asteroid_1_l, 65, 65) // 1
        .set_sprite_embedded("asteroid_1m_sprite", asteroid_1_m, 45, 45)
        .set_sprite_embedded("asteroid_1s_sprite", asteroid_1_s, 25, 25)
        .set_sprite_embedded("asteroid_5l_sprite", asteroid_5_l, 65, 57) // 5
        .set_sprite_embedded("asteroid_5m_sprite", asteroid_5_m, 45, 39)
        .set_sprite_embedded("asteroid_5s_sprite", asteroid_5_s, 25, 22)
        .set_sprite_embedded("asteroid_7l_sprite", asteroid_7_l, 65, 65) // 7
        .set_sprite_embedded("asteroid_7m_sprite", asteroid_7_m, 45, 45)
        .set_sprite_embedded("asteroid_7s_sprite", asteroid_7_s, 25, 25)
        .set_sprite_embedded("asteroid_2l_sprite", asteroid_2_l, 65, 58) // 2
        .set_sprite_embedded("asteroid_2m_sprite", asteroid_2_m, 45, 40)
        .set_sprite_embedded("asteroid_2s_sprite", asteroid_2_s, 25, 22)
        .set_background_sprite("pause_screen_yes", pause_yes, 320, 240)
        .set_background_sprite("pause_screen_no", pause_no, 320, 240)
        .set_dynamic_body("asteroid_object_1",
            { .position = { 20.0f, 0.0f }, .velocity = { 0.0f, 0.0f },
              .width = px_to_units(65.0f, 65.0f), .mass = 1.0f, .is_sensor = true })
        .set_dynamic_body("asteroid_object_2",
            { .position = { 20.0f, 0.0f }, .velocity = { 0.0f, 0.0f },
              .width = px_to_units(65.0f, 57.0f), .mass = 1.0f, .is_sensor = true })
        .set_dynamic_body("asteroid_object_3",
            { .position = { 20.0f, 0.0f }, .velocity = { 0.0f, 0.0f },
              .width = px_to_units(65.0f, 65.0f), .mass = 1.0f, .is_sensor = true })
        .set_dynamic_body("asteroid_object_4",
            { .position = { 20.0f, 0.0f }, .velocity = { 0.0f, 0.0f },
              .width = px_to_units(65.0f, 58.0f), .mass = 1.0f, .is_sensor = true });

    state.asteroids
        .add({
            .body           = &game.get_body("asteroid_object_1"),
            .sprite_l       = "asteroid_1l_sprite",
            .sprite_m       = "asteroid_1m_sprite",
            .sprite_s       = "asteroid_1s_sprite",
            .width_l        = px_to_units(65.0f, 65.0f),
            .width_m        = px_to_units(45.0f, 45.0f),
            .width_s        = px_to_units(25.0f, 25.0f),
            .spawn_velocity = { -10.555f * state.velocity_ratio, 0.0f },
            .draw           = state.draw,
            .spawn_size     = asteroid::Size::L,
            .spawn_total    = 3,
        })
        .add({
            .body           = &game.get_body("asteroid_object_2"),
            .sprite_l       = "asteroid_5l_sprite",
            .sprite_m       = "asteroid_5m_sprite",
            .sprite_s       = "asteroid_5s_sprite",
            .width_l        = px_to_units(65.0f, 57.0f),
            .width_m        = px_to_units(45.0f, 39.0f),
            .width_s        = px_to_units(25.0f, 22.0f),
            .spawn_velocity = { -10.555f * state.velocity_ratio, 0.0f },
            .draw           = state.draw,
            .spawn_size     = asteroid::Size::L,
            .spawn_total    = 3,
        })
        .add({
            .body           = &game.get_body("asteroid_object_3"),
            .sprite_l       = "asteroid_7l_sprite",
            .sprite_m       = "asteroid_7m_sprite",
            .sprite_s       = "asteroid_7s_sprite",
            .width_l        = px_to_units(65.0f, 65.0f),
            .width_m        = px_to_units(45.0f, 45.0f),
            .width_s        = px_to_units(25.0f, 25.0f),
            .spawn_velocity = { -7.555f * state.velocity_ratio, 0.0f },
            .draw           = state.draw,
            .spawn_size     = asteroid::Size::L,
            .spawn_total    = 3,
        })
        .add({
            .body           = &game.get_body("asteroid_object_4"),
            .sprite_l       = "asteroid_2l_sprite",
            .sprite_m       = "asteroid_2m_sprite",
            .sprite_s       = "asteroid_2s_sprite",
            .width_l        = px_to_units(65.0f, 58.0f),
            .width_m        = px_to_units(45.0f, 40.0f),
            .width_s        = px_to_units(25.0f, 22.0f),
            .spawn_velocity = { -6.555f * state.velocity_ratio, 0.0f },
            .draw           = state.draw,
            .spawn_size     = asteroid::Size::L,
            .spawn_total    = 3,
        });
    // clang-format on
    state.asteroids.activate(0);
}

/**
 * @brief
 * Per-frame level-one logic: input, asteroid SM, laser pool, collision.
 *
 * @scope: PULSE_ON_GAMESCENE(Level_One)
 */
PULSE_SCENE_FN void on_level_one_tick(pulse2d_scene_runtime<Scenes...>& game,
    pulse2d_body& ship,
    void (*on_gameover)(),
    [[maybe_unused]] void (*on_next_level)())
{
    PULSE_POLL_SEESAW_GAMEPAD();

    game.set_arcade_directional_inverted_control("ship_object", 12.55f, true);

    if (SEESAW_BUTTON_INPUT(SEESAW_SELECT)) {
        reset();
        on_gameover();
        return;
    }

    if (SEESAW_BUTTON_INPUT(SEESAW_A)) {
        game.spawn("laser_gun",
            250,
            ship.position.x + 1.0f,
            ship.position.y,
            30.0f,
            0.0f);
    }

    game.draw("ship_object", "ship_sprite");

    draw_blue_health_bar(game, state.current_health);

    etl::array<pulse2d_body*, 4> as_body = {
        &game.get_body("asteroid_object_1"),
        &game.get_body("asteroid_object_2"),
        &game.get_body("asteroid_object_3"),
        &game.get_body("asteroid_object_4"),
    };

    auto& current = state.asteroids.get(state.current_asteroid);

    current.dispatch(asteroid::Render_Event{});

    if (current.config().body->position.x < -7.5f)
        current.dispatch(asteroid::Evaded_Event{});

    if (current.is<asteroid::Erased>() or current.is<asteroid::Evaded>()) {
        if (current.is<asteroid::Evaded>() and state.collided_obj == nullptr) {
            state.total_missed++;
        }
        if (state.collided_obj != nullptr) {
            state.collided_obj = nullptr;
        }
        auto next = state.asteroids.pick_next(
            static_cast<size_t>(state.current_asteroid));
        state.current_asteroid = static_cast<int>(next);
        state.asteroids.respawn(next);
    }

    game.on_collision_with("ship_object", [&](pulse2d_body* obj) {
        for (auto* i_obj : as_body) {
            if (state.collided_obj != nullptr and state.collided_obj == i_obj)
                break;
            if (obj == i_obj and state.current_health > 0) {
                state.collided_obj = i_obj;
                state.current_health--;
                break;
            }
        }
    });

    game.render_pool("laser_gun", [&](pulse2d_body* laser_object) {
        if (laser_object->position.x > 6.67f or
            laser_object->position.x < -6.0f) {
            game.despawn("laser_gun", laser_object);
        } else {
            game.draw_body(laser_object, "laser_sprite");
        }
        game.on_collision_with_body(
            laser_object, [&](pulse2d_body* collided_with) {
                if (collided_with == as_body[state.current_asteroid]) {
                    kinematic_laser_collided_asteroid(game,
                        laser_object,
                        static_cast<size_t>(state.current_asteroid));
                }
            });
    });

    if (state.current_health <= 0) {
        reset();
        on_gameover();
    }

    // if (state.timer >= 1000 * 60) {
    //     reset();
    //     on_next_level();
    // }
}

} // namespace scenes::levels::level_one
