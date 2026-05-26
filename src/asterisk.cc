
#include PULSE2D_HEADER
#include PULSE2D_GRAPHICS

// #include "../include/dust-bg.h"
#include "../include/nebula-bg.h"
// #include "../include/planet-bg.h"
// #include "../include/stars-bg.h"

PULSE2D_START_PULSE();

PULSE2D_ENABLE_SEESAW_GAMEPAD();

PULSE2D_DEFINE_SCENE(Level_One, 1, 1);

PULSE2D_GAME_SCENES(Level_One);

PULSE2D_ON_GAMESCENE_START(Level_One)
{
    PULSE2D_SET_SPRITE_FLASH(sprite_nebula, bg_1, 320, 240);
    //  PULSE2D_SET_SPRITE_FLASH(sprite_stars, bg_2, 320, 240);
    //  PULSE2D_SET_SPRITE_FLASH(sprite_planets, bg_3, 320, 240);
    //   PULSE2D_SET_SPRITE_FLASH(sprite_dust, bg_4, 320, 240);

    // PULSE2D_ADD_SCROLLING_LAYER(sprite_nebula, 320.0f, 10.0f);
    //  PULSE2D_ADD_SCROLLING_LAYER(sprite_stars, 320.0f, 3.0f);
    //  PULSE2D_ADD_SCROLLING_LAYER(sprite_planets, 320.0f, 25.0f);
    //   PULSE2D_ADD_SCROLLING_LAYER(sprite_dust, 320.0f, 65.0f);

    // PULSE2D_SET_SPRITE(sprite_ship, "/ship.bin", 64, 64);

    // Spawn the player's physics body
    // Note: Locked to left side, middle of screen
    // PULSE2D_SPAWN_DYNAMIC_BODY(player,
    //     {
    //         .position = { 200.0f, 540.0f },
    //         .width = { 64.0f,  64.0f  },
    //         .mass = 1.0f
    // });
}

PULSE2D_ON_GAMESCENE(Level_One)
{
    PULSE2D_TICK_WORLD(Level_One);
    PULSE2D_POLL_SEESAW_GAMEPAD();
    PULSE2D_RENDER_BACKGROUNDS();
    PULSE2D_RENDER(active_scene);
}

PULSE2D_ON_GAMESTART()
{
    Serial.begin(115200);

    PULSE2D_POLL_SERIAL_CONNECTION();

    PULSE2D_INIT(0.0f, 0.0f, 10);

    PULSE2D_START_SEESAW_GAMEPAD();

    PULSE2D_SET_SCENE(Level_One);
}

PULSE2D_ON_GAMELOOP()
{
    PULSE2D_TICK_GAMESCENE();
}