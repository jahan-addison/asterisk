#pragma once

#include PULSE2D_HEADER
#include PULSE2D_GRAPHICS

#include "blue-dust-bg.h"
#include "blue-nebula-bg.h"
#include "blue-planet-bg.h"
#include "blue-stars-bg.h"
#include "gameover-1.h"

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
