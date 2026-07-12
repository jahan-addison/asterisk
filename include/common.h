#pragma once
#include PULSE2D_HEADER
#include PULSE2D_GRAPHICS

#include <backgrounds/backgrounds.h>
#include <sprites/health-bars/health-bars.h>

#include <animations/animated-ship.h>
#include <animations/laser-collision.h>

#include <backgrounds/loading-screen.h>
#include <backgrounds/pause-no.h>
#include <backgrounds/pause-yes.h>

/**
 * @brief
 * Set the player ship object
 *
 * @scope: all
 */
PULSE_SCENE_FN void set_player_ship(pulse2d_scene_runtime<Scenes...>& game)
{
    game.set_sprite("ship_sprite", "ship_1.bin")
        .set_sprite("laser_sprite", "fire_shot_2_2.bin")
        .register_vfx(
            "laser_collision", animated_laser_collision, 15, 27, 2, 17)
        .set_controlled_body("ship_object",
            {
                .position = { -4.32f, 2.51f },
                .velocity = { 0.0f,   0.0f  },
                .width = { 1.0f,   1.0f  },
                .mass = 1.0f,
    })
        .init_pool("laser_gun",
            {
                .position = { -10.0f, 0.0f },
                .velocity = { 0.0f, 0.0f },
                .width = { 2.0f, 0.5f },
                .is_sensor = true,
            });
}

/**
 * @brief
 * Set the static walls objects
 *
 * @scope: PULSE_ON_GAMESCENE_START
 */
PULSE_SCENE_FN void set_static_walls(pulse2d_scene_runtime<Scenes...>& game)
{
    game.set_static_body("top_wall",
            {
                .position = { -4.5f, 4.0f },
                  .width = { 2.0f,  0.5f }
    })
        .set_static_body("bottom_wall",
            { .position = { -4.5f, -4.0f }, .width = { 2.0f, 0.5f } });
}
