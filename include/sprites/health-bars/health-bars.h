#pragma once

#include PULSE2D_HEADER
#include PULSE2D_GRAPHICS

#include "health-blue.h"
#include "health-red.h"

#include "blue-health-bar_1.h"
#include "blue-health-bar_2.h"
#include "blue-health-bar_3.h"
#include "blue-health-bar_4.h"
#include "blue-health-bar_5.h"

#include "red-health-bar_1.h"
#include "red-health-bar_2.h"
#include "red-health-bar_3.h"
#include "red-health-bar_4.h"
#include "red-health-bar_5.h"

/**
 * @brief
 * Set the blue (player) health bar
 *
 * @scope: PULSE_ON_GAMESCENE_START
 */
PULSE_SCENE_FN void set_blue_health_bar(pulse2d_scene_runtime<Scenes...>& game)
{
    game.set_sprite_embedded("health-blue", health_blue, 57, 22)
        .set_sprite_embedded("blue-health-bar-1", blue_health_bar_1, 100, 22)
        .set_sprite_embedded("blue-health-bar-2", blue_health_bar_2, 100, 22)
        .set_sprite_embedded("blue-health-bar-3", blue_health_bar_3, 100, 22)
        .set_sprite_embedded("blue-health-bar-4", blue_health_bar_4, 100, 22)
        .set_sprite_embedded("blue-health-bar-5", blue_health_bar_5, 100, 22);
}

/**
 * @brief
 * Set the red (enemy) health bar
 *
 * @scope: PULSE_ON_GAMESCENE_START
 */
PULSE_SCENE_FN void set_red_health_bar(pulse2d_scene_runtime<Scenes...>& game)
{
    game.set_sprite_embedded("health-red", health_red, 57, 22)
        .set_sprite_embedded("red-health-bar-1", red_health_bar_1, 100, 22)
        .set_sprite_embedded("red-health-bar-2", red_health_bar_2, 100, 22)
        .set_sprite_embedded("red-health-bar-3", red_health_bar_3, 100, 22)
        .set_sprite_embedded("red-health-bar-4", red_health_bar_4, 100, 22)
        .set_sprite_embedded("red-health-bar-5", red_health_bar_5, 100, 22);
}

/**
 * @brief
 * Draw the blue (player) health bar
 *
 * @scope: PULSE_ON_GAMESCENE
 */
PULSE_SCENE_FN void draw_blue_health_bar(pulse2d_scene_runtime<Scenes...>& game,
    int health = 5)
{
    auto [sx, sy] = units_to_pixels(-5.0f, 3.8f);
    auto [sx_1, sy_1] = units_to_pixels(-3.7f, 3.79f);
    game.draw_sprite("health-blue", sx, sy);
    switch (health) {
        case 5:
            game.draw_sprite("blue-health-bar-1", sx_1, sy_1);
            break;
        case 4:
            game.draw_sprite("blue-health-bar-2", sx_1, sy_1);
            break;
        case 3:
            game.draw_sprite("blue-health-bar-3", sx_1, sy_1);
            break;
        case 2:
            game.draw_sprite("blue-health-bar-4", sx_1, sy_1);
            break;
        case 1:
            game.draw_sprite("blue-health-bar-5", sx_1, sy_1);
            break;
        default:
            game.draw_sprite("blue-health-bar-5", sx_1, sy_1);
            break;
    }
}

/**
 * @brief
 * Draw the red (enemy) health bar
 *
 * @scope: PULSE_ON_GAMESCENE
 */
PULSE_SCENE_FN void draw_red_health_bar(pulse2d_scene_runtime<Scenes...>& game,
    int health = 5)
{
    auto [sx, sy] = units_to_pixels(0.5f, -3.0f);
    auto [sx_1, sy_1] = units_to_pixels(1.8f, -3.05f);
    game.draw_sprite("health-red", sx, sy);
    switch (health) {
        case 5:
            game.draw_sprite("red-health-bar-1", sx_1, sy_1);
            break;
        case 4:
            game.draw_sprite("red-health-bar-2", sx_1, sy_1);
            break;
        case 3:
            game.draw_sprite("red-health-bar-3", sx_1, sy_1);
            break;
        case 2:
            game.draw_sprite("red-health-bar-4", sx_1, sy_1);
            break;
        case 1:
            game.draw_sprite("red-health-bar-5", sx_1, sy_1);
            break;
        default:
            game.draw_sprite("red-health-bar-5", sx_1, sy_1);
            break;
    }
}