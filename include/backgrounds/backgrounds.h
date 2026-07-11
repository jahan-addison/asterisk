#pragma once

#include PULSE2D_HEADER
#include PULSE2D_GRAPHICS

#include "parallax/blue-dust-bg.h"
#include "parallax/blue-nebula-bg.h"
#include "parallax/blue-planet-bg.h"
#include "parallax/blue-stars-bg.h"

// clang-format off
#include "parallax/red-nebula-l1.h"
#include "parallax/red-stars-l2.h"
#include "parallax/red-satellite01-l4.h"
#include "parallax/red-planet-l5.h"
#include "parallax/red-satellite02-l6.h"
// clang-format on

/**
 * @brief
 * Set a blue parallax background
 *
 * @scope: PULSE_ON_GAMESCENE_START
 */
PULSE_SCENE_FN void set_blue_background(pulse2d_scene_runtime<Scenes...>& game)
{
    game.set_background_sprite("blue_sprite_nebula", blue_bg_1, 320, 240)
        .set_background_sprite("blue_sprite_stars", blue_bg_2, 320, 240)
        .set_background_sprite("blue_sprite_planets", blue_bg_3, 320, 240)
        .set_background_sprite("blue_sprite_dust", blue_bg_4, 320, 240)
        .add_parallax_layer("blue_sprite_nebula", 320.0f, 15.0f)
        .add_parallax_layer("blue_sprite_stars", 320.0f, 20.0f)
        .add_parallax_layer("blue_sprite_planets", 320.0f, 40.0f)
        .add_parallax_layer("blue_sprite_dust", 320.0f, 50.0f);
}

/**
 * @brief
 * Set a red parallax background
 *
 * @scope: PULSE_ON_GAMESCENE_START
 */
PULSE_SCENE_FN void set_red_background(pulse2d_scene_runtime<Scenes...>& game)
{
    game.set_background_sprite("red_sprite_nebula", red_nebula_l1, 320, 240)
        .set_background_sprite("red_sprite_stars", red_stars_l2, 320, 240)
        .set_background_sprite(
            "red_sprite_satellite01", red_satellite01_l4, 320, 240)
        .set_background_sprite("red_sprite_planet", red_planet_l5, 320, 240)
        .set_background_sprite(
            "red_sprite_satellite02", red_satellite02_l6, 320, 240)
        .add_parallax_layer("red_sprite_nebula", 320.0f, 20.0f)
        .add_parallax_layer("red_sprite_stars", 320.0f, 30.0f)
        .add_parallax_layer("red_sprite_satellite01", 320.0f, 50.0f)
        .add_parallax_layer("red_sprite_planet", 320.0f, 65.0f)
        .add_parallax_layer("red_sprite_satellite02", 320.0f, 80.0f);
}