#pragma once

#include PULSE2D_HEADER
#include PULSE2D_GRAPHICS

#include <cmath> // sinf

#include <common.h>

#include <backgrounds/pause-no.h>
#include <backgrounds/pause-yes.h>
#include <enemy/boss-1.h>
#include <enemy/missile-0.h>

namespace scenes::levels::level_two {

inline constexpr int k_hits_per_health = 10;

struct State
{
    int current_health = 5;
    int enemy_health = 5;
    int enemy_hit_count = 0;
    elapsedMillis intro = 0;
    elapsedMillis fire_cooldown;
    bool boss_spawn = false;

    pulse2d::state::Draw_Fn draw = nullptr;
};

PULSE_DEFINE_SCENE_STATE(State);

///////////////////////////////////////////////////////////////////////////////
// Utilities

/**
 * @brief
 * Clear state
 *
 * @scope: on_level_two_tick
 */
PULSE2D_INLINE void reset()
{
    state.current_health = 5;
    state.enemy_health = 5;
    state.enemy_hit_count = 0;
    state.intro = 0;
    state.fire_cooldown = 0;
    state.boss_spawn = false;
}

/**
 * @brief
 * Dispatch impact event on player laser collision with the enemy.
 *
 * @scope: PULSE_ON_GAMESCENE
 */
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

///////////////////////////////////////////////////////////////////////////////
// Lifecycle

/**
 * @brief
 * Initialize all level-two bodies, sprites, and pools.
 * draw_fn must be a non-capturing lambda referencing the global runtime.
 *
 * @scope: PULSE_ON_GAMESCENE_START(Level_Two)
 */
PULSE_SCENE_FN void on_level_two_start(pulse2d_scene_runtime<Scenes...>& game,
    pulse2d::state::Draw_Fn draw_fn)
{
    state.draw = draw_fn;
    state.intro = 0;
    state.fire_cooldown = 0;

    set_red_background(game);
    set_static_walls(game);
    set_player_ship(game);
    set_blue_health_bar(game);
    set_red_health_bar(game);

    // clang-format off
    game
        .set_static_body("boss_enemy",
            {
                .position = { 20.0f, 0.0f },
                .velocity = {  0.0f, 0.0f },
                .width    = px_to_units(190.0f, 176.0f),
                .mass     = 5.0f,
                .is_sensor = true,
            })
        .set_background_sprite("pause_screen_yes", pause_yes, 320, 240)
        .set_background_sprite("pause_screen_no", pause_no, 320, 240)
        .set_sprite_embedded("enemy_sprite", boss_1, boss_1_width, boss_1_height)
        .set_sprite_embedded("enemy_laser_sprite", missile_0, 90, 26)
        .init_pool("enemy_laser",
            {
                .position  = { 25.0f, 0.0f },
                .velocity  = {  0.0f, 0.0f },
                .width     = {  2.0f, 0.5f },
                .is_sensor = true,
            });
    // clang-format on
}

/**
 * @brief
 * Per-frame level-two logic: input, boss AI, laser pools, collision.
 *
 * @scope: PULSE_ON_GAMESCENE(Level_Two)
 */
PULSE_SCENE_FN void on_level_two_tick(pulse2d_scene_runtime<Scenes...>& game,
    pulse2d_body& ship,
    void (*on_gameover)(),
    void (*on_next_level)())
{
    PULSE_POLL_SEESAW_GAMEPAD();

    auto& enemy = game.get_body("boss_enemy");

    if (state.intro < 9000) {
        game.draw_text("Defeat the Dreadnaught!",
            60,
            200,
            game.color(pulse2d_color::Maroon),
            1.6222f);
    }

    if (state.intro > 10000 and not state.boss_spawn) {
        enemy.set_position({ 4.0f, 0.0f });
        state.boss_spawn = true;
    }

    if (state.intro > 13000) {
        draw_red_health_bar(game, state.enemy_health);

        // Body stays fixed; sprite rotates on its axis.
        const float angle =
            sinf(static_cast<float>(state.intro) * 0.0012f) * 0.35f;
        game.draw("boss_enemy", "enemy_sprite", angle);

        if (state.fire_cooldown >= 1800) {
            // Rotate each gun's body-relative offset by the current angle
            // so spawned lasers come from the visual gun positions.
            const float ca = cosf(angle);
            const float sa = sinf(angle);
            const float ex = enemy.position.x;
            const float ey = enemy.position.y;

            // base offsets: dx = -2.8 (left side), dy = ±1.5 and ±0.75
            // clang-format off
            constexpr float dx = -2.8f;
            game.spawn("enemy_laser", 0, ex + dx * ca - (-1.5f) * sa, ey + dx * sa + (-1.5f) * ca, -25.0f, 0.0f);
            // game.spawn("enemy_laser", 0, ex + dx * ca - (-0.75f) * sa, ey + dx * sa + (-0.75f) * ca, -25.0f, 0.0f);
            // game.spawn("enemy_laser", 0, ex + dx * ca - ( 0.75f) * sa, ey + dx * sa + ( 0.75f) * ca, -25.0f, 0.0f);
            game.spawn("enemy_laser", 0, ex + dx * ca - ( 1.5f) * sa, ey + dx * sa + ( 1.5f) * ca, -25.0f, 0.0f);
            // clang-format on
            state.fire_cooldown = 0;
        }

        game.render_pool("enemy_laser", [&](pulse2d_body* el) {
            if (el->position.x < -7.0f) {
                game.despawn("enemy_laser", el);
            } else {
                game.draw_body(el, "enemy_laser_sprite");
            }
            game.on_collision_with_body(el, [&](pulse2d_body* hit) {
                if (hit == &ship) {
                    state.current_health--;
                    game.despawn("enemy_laser", el);
                }
            });
        });
    }

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

    if (state.current_health <= 0) {
        reset();
        on_gameover();
        return;
    }

    if (state.enemy_health <= 0) {
        reset();
        on_next_level();
    }
}

} // namespace scenes::levels::level_two
