
#include PULSE2D_HEADER
#include PULSE2D_GRAPHICS

#include "../include/dust-bg.h"
#include "../include/nebula-bg.h"
#include "../include/planet-bg.h"
#include "../include/stars-bg.h"

PULSE2D_START_PULSE();

PULSE2D_ENABLE_SEESAW_GAMEPAD();

// @TODO: Better error handling on malformed sizing
PULSE2D_DEFINE_SCENE(Level_One, 2, 6);

PULSE2D_GAME_SCENES(Level_One);

PULSE2D_DEFINE bool gun_fired = false;

PULSE2D_ON_GAMESCENE_START(Level_One)
{
    ////////////////
    // Ship setup //
    ////////////////

    PULSE2D_SPAWN_STATIC_BODY("ship",
        {
            .position = { -4.32f, 2.5111f },
            .velocity = { 0.0f,   0.0f    },
            .width = { 1.0f,   1.0f    },
            .friction = 0.0f,
            .mass = 1.0f
    });

    PULSE2D_SET_SPRITE(ship_sprite, "ship_1.bin", 44, 39);

    PULSE2D_SET_SPRITE(laser_sprite, "fire_shot_2_2.bin", 35, 4);

    ////////////////
    // Ammo setup //
    ////////////////

    PULSE2D_SPAWN_STATIC_BODY("laser",
        {
            .position = { -10.0f, 0.0f },
            .velocity = { 0.0f,   0.0f },
            .width = { 1.0f,   0.5f },
            .friction = 0.0f,
            .mass = 1.0f
    });

    //////////////////////
    // Background setup //
    //////////////////////

    PULSE2D_SET_SPRITE_FLASH(sprite_nebula, bg_1, 320, 240);
    PULSE2D_SET_SPRITE_FLASH(sprite_stars, bg_2, 320, 240);
    PULSE2D_SET_SPRITE_FLASH(sprite_planets, bg_3, 320, 240);
    PULSE2D_SET_SPRITE_FLASH(sprite_dust, bg_4, 320, 240);

    PULSE2D_ADD_PARALLAX_LAYER(sprite_nebula, 320.0f, 10.0f);
    PULSE2D_ADD_PARALLAX_LAYER(sprite_stars, 320.0f, 3.0f);
    PULSE2D_ADD_PARALLAX_LAYER(sprite_planets, 320.0f, 25.0f);
    PULSE2D_ADD_PARALLAX_LAYER(sprite_dust, 320.0f, 65.0f);
}

PULSE2D_ON_GAMESCENE(Level_One)
{
    PULSE2D_TICK_WORLD(Level_One);
    PULSE2D_POLL_SEESAW_GAMEPAD();

    auto& ship = PULSE2D_GET_BODY("ship");
    auto& laser = PULSE2D_GET_BODY("laser");

    // @TODO: Prevent leaving the screen
    SEESAW_ARCADE_DIRECTIONAL_MOVEMENT_INVERTED("ship", 12.55f, true);

    if (SEESAW_BUTTON_INPUT(SEESAW_A)) {
        if (!gun_fired) {
            gun_fired = true;
            laser.position = { ship.position.x, ship.position.y };
            laser.velocity = { 25.0f, 0.0f };
        }
    }

    PULSE2D_RENDER_BACKGROUNDS();
    PULSE2D_DRAW("ship", ship_sprite);

    if (gun_fired) {
        // laser shot went off the screen, reset it
        if (laser.position.x > 6.67f) {
            gun_fired = false;
            laser.position = { -10.0f, 0.0f };
            laser.velocity = { 0.0f, 0.0f };
        }
        PULSE2D_DRAW("laser", laser_sprite);
    }

    PULSE2D_RENDER(active_scene);
}

PULSE2D_ON_GAMESTART()
{
    Serial.begin(115200);

    PULSE2D_POLL_SERIAL_CONNECTION();

    PULSE2D_REGISTER_ETL_ERROR_HANDLER();

    PULSE2D_INIT(0.0f, 0.0f, 10);

    PULSE2D_START_SEESAW_GAMEPAD();

    PULSE2D_SET_SCENE(Level_One);
}

PULSE2D_ON_GAMELOOP()
{
    PULSE2D_TICK_GAMESCENE();
}