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
    game.set_static_body("health_blue",
            {
                .position = { -4.0f, 3.5f },
                .width = { 0.5f,  0.5f },
                .is_sensor = true
    })
        .set_static_body("blue_health_bar",
            { .position = { -2.0f, 3.5f },
                .width = { 3.0f, 0.5f },
                .is_sensor = true })
        .set_sprite_embedded("health-blue", health_blue, 57, 22)
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
    game.set_static_body("health_red",
            {
                .position = { 1.98f, -3.5f },
                .width = { 0.5f,  0.5f  },
                .is_sensor = true
    })
        .set_static_body("red_health_bar",
            { .position = { 3.98f, -3.5f },
                .width = { 3.0f, 0.5f },
                .is_sensor = true })
        .set_sprite_embedded("health-red", health_red, 57, 22)
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
    game.draw("health_blue", "health-blue");
    switch (health) {
        case 5:
            game.draw("blue_health_bar", "blue-health-bar-1");
            break;
        case 4:
            game.draw("blue_health_bar", "blue-health-bar-2");
            break;
        case 3:
            game.draw("blue_health_bar", "blue-health-bar-3");
            break;
        case 2:
            game.draw("blue_health_bar", "blue-health-bar-4");
            break;
        case 1:
            game.draw("blue_health_bar", "blue-health-bar-5");
            break;
        default:
            game.draw("blue_health_bar", "blue-health-bar-5");
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
    game.draw("health_red", "health-red");
    switch (health) {
        case 5:
            game.draw("red_health_bar", "red-health-bar-1");
            break;
        case 4:
            game.draw("red_health_bar", "red-health-bar-2");
            break;
        case 3:
            game.draw("red_health_bar", "red-health-bar-3");
            break;
        case 2:
            game.draw("red_health_bar", "red-health-bar-4");
            break;
        case 1:
            game.draw("red_health_bar", "red-health-bar-5");
            break;
        default:
            game.draw("red_health_bar", "red-health-bar-5");
            break;
    }
}