
#include PULSE2D_HEADER
#include PULSE2D_GRAPHICS

#include "../include/dust-bg.h"
#include "../include/nebula-bg.h"
#include "../include/planet-bg.h"
#include "../include/stars-bg.h"

PULSE2D_START_PULSE();

PULSE2D_ENABLE_SEESAW_GAMEPAD();

// @TODO: Better error handling on malformed sizing
PULSE2D_DEFINE_SCENE(Level_One, 3, 6);

PULSE2D_GAME_SCENES(Level_One);

PULSE2D_ON_GAMESCENE_START(Level_One)
{
    PULSE2D_SPRITE(ship_sprite, "ship_1.bin", 44, 39);
    PULSE2D_SPRITE(laser_sprite, "fire_shot_2_2.bin", 35, 4);

    PULSE2D_SPRITE_FLASH(sprite_nebula, bg_1, 320, 240);
    PULSE2D_SPRITE_FLASH(sprite_stars, bg_2, 320, 240);
    PULSE2D_SPRITE_FLASH(sprite_planets, bg_3, 320, 240);
    PULSE2D_SPRITE_FLASH(sprite_dust, bg_4, 320, 240);

    PULSE2D_INIT_POOL(laser_ammo,
        {
            .position = { -10.0f, 0.0f },
            .velocity = { 0.0f,   0.0f },
            .width = { 1.0f,   0.5f },
            .friction = 0.0f
    });

    PULSE2D_CONTROLLED_BODY(ship_object,
        {
            .position = { -4.32f, 2.5111f },
            .velocity = { 0.0f,   0.0f    },
            .width = { 1.0f,   1.0f    },
            .friction = 0.0f
    });

    PULSE2D_ADD_PARALLAX_LAYER(sprite_nebula, 320.0f, 13.0f);
    PULSE2D_ADD_PARALLAX_LAYER(sprite_stars, 320.0f, 6.0f);
    PULSE2D_ADD_PARALLAX_LAYER(sprite_planets, 320.0f, 28.0f);
    PULSE2D_ADD_PARALLAX_LAYER(sprite_dust, 320.0f, 68.0f);
}

PULSE2D_ON_GAMESCENE(Level_One)
{
    PULSE2D_TICK_WORLD(Level_One);
    PULSE2D_POLL_SEESAW_GAMEPAD();

    PULSE2D_RENDER_BACKGROUNDS();

    auto& ship = PULSE2D_GET_BODY(ship_object);

    // @TODO: Prevent leaving the screen
    SEESAW_ARCADE_DIRECTIONAL_MOVEMENT_INVERTED(ship_object, 12.55f, true);

    if (SEESAW_BUTTON_INPUT(SEESAW_A)) {
        PULSE2D_SPAWN(laser_ammo,
            250,
            ship.position.x + 1.0f,
            ship.position.y,
            30.0f,
            0.0f);
    }

    PULSE2D_DRAW(ship_object, ship_sprite);

    PULSE2D_RENDER_POOL(laser_ammo, [&](auto* laser_object) {
        if (laser_object->position.x > 6.67f or
            laser_object->position.x < -10.0f) {
            PULSE2D_DESPAWN(laser_ammo, laser_object);
        } else {
            PULSE2D_DRAW_BODY(laser_object, laser_sprite);
        }
    });

    PULSE2D_RENDER(active_scene);
}

PULSE2D_ON_GAMESTART()
{
    Serial.begin(115200);

    // PULSE2D_POLL_SERIAL_CONNECTION();

    PULSE2D_REGISTER_ETL_ERROR_HANDLER();
    PULSE2D_INIT(0.0f, 0.0f, 10);
    PULSE2D_START_SEESAW_GAMEPAD();
    PULSE2D_SET_SCENE(Level_One);
}

PULSE2D_ON_GAMELOOP()
{
    PULSE2D_TICK_GAMESCENE();
}