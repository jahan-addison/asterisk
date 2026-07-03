#pragma once

#include PULSE2D_HEADER
#include PULSE2D_GRAPHICS

#include <events/asteroids.h>

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

void set_player_ship();

namespace scenes::levels::level_one {

struct State
{
    int current_asteroid = 0;
    float velocity_ratio = 1.0f;
    asteroid::Asteroid_Manager<4> asteroids{};
    pulse2d::state::Draw_Fn draw = nullptr;
};

PULSE_DEFINE_SCENE_STATE(State);

///////////////////
// Setup helpers //
///////////////////

/**
 * @brief
 * Set a blue parallax background
 *
 * @scope: PULSE_ON_GAMESCENE_START
 */
PULSE_SCENE_FN void set_blue_background(pulse2d_scene_runtime<Scene>& game)
{
    game.set_background_sprite("sprite_nebula", bg_1, 320, 240)
        .set_background_sprite("sprite_stars", bg_2, 320, 240)
        .set_background_sprite("sprite_planets", bg_3, 320, 240)
        .set_background_sprite("sprite_dust", bg_4, 320, 240)
        .add_parallax_layer("sprite_nebula", 320.0f, 15.0f)
        .add_parallax_layer("sprite_stars", 320.0f, 20.0f)
        .add_parallax_layer("sprite_planets", 320.0f, 40.0f)
        .add_parallax_layer("sprite_dust", 320.0f, 50.0f);
}

/**
 * @brief
 * Set the static walls objects
 *
 * @scope: PULSE_ON_GAMESCENE_START
 */
PULSE_SCENE_FN void set_static_walls(pulse2d_scene_runtime<Scene>& game)
{
    game.set_static_body("top_wall",
            {
                .position = { -4.5f, 4.0f },
                  .width = { 2.0f,  0.5f }
    })
        .set_static_body("bottom_wall",
            { .position = { -4.5f, -4.0f }, .width = { 2.0f, 0.5f } });
}

///////////////
// Collision //
///////////////

/**
 * @brief
 * Dispatch impact event on laser collision with an asteroid.
 *
 * @scope: on_level_one_tick
 */
PULSE_SCENE_FN void kinematic_laser_collided_asteroid(
    pulse2d_scene_runtime<Scene>& game,
    pulse2d_body* laser_object,
    size_t index)
{
    state.asteroids.get(index).dispatch(asteroid::Impact_Event{});
    auto coords = get_body_coordinates(laser_object);
    game.play_vfx(
        "laser_collision", to_int16(coords.x + 12), to_int16(coords.y - 12));
    game.despawn("laser_gun", laser_object);
}

/////////////////////
// Scene lifecycle //
/////////////////////

/**
 * @brief
 * Initialize all level-one bodies, sprites, and asteroid configs.
 * draw_fn must be a non-capturing lambda referencing the global runtime -
 * it is stored in state and forwarded to every Asteroid_Config.
 *
 * @scope: PULSE_ON_GAMESCENE_START(Level_One)
 */
PULSE_SCENE_FN void on_level_one_start(pulse2d_scene_runtime<Scene>& game,
    pulse2d::state::Draw_Fn draw_fn)
{
    state.draw = draw_fn;

    set_blue_background(game);
    set_static_walls(game);
    set_player_ship();

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
PULSE_SCENE_FN void on_level_one_tick(pulse2d_scene_runtime<Scene>& game,
    pulse2d_body& ship,
    void (*on_reset)())
{
    PULSE_POLL_SEESAW_GAMEPAD();

    game.set_arcade_directional_inverted_control("ship_object", 12.55f, true);

    if (SEESAW_BUTTON_INPUT(SEESAW_START)) {
        state.asteroids.reset_all();
        state.current_asteroid = 0;
        on_reset();
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

    etl::array<pulse2d_body*, 4> as_body = {
        &game.get_body("asteroid_object_1"),
        &game.get_body("asteroid_object_2"),
        &game.get_body("asteroid_object_3"),
        &game.get_body("asteroid_object_4"),
    };

    state.asteroids.get(state.current_asteroid)
        .dispatch(asteroid::Render_Event{});

    if (state.asteroids.get(state.current_asteroid).is<asteroid::Erased>()) {
        auto next = state.asteroids.pick_next(
            static_cast<size_t>(state.current_asteroid));
        state.current_asteroid = static_cast<int>(next);
        state.asteroids.respawn(next);
    }

    // @TODO: Explode ship on_collision_with_body(ship)

    game.render_pool("laser_gun", [&](pulse2d_body* laser_object) {
        if (laser_object->position.x > 6.67f or
            laser_object->position.x < -10.0f) {
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
}

} // namespace scenes::levels::level_one
