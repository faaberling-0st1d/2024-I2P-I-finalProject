#include <allegro5/allegro.h>
#include <string.h>
#include "menu_scene.h"
#include "game_scene.h"
#include "setting_scene.h"
#include "loading_scene.h"
#include "utility.h"
#include "UI.h"
#include "game.h"

#include <math.h>

static Button settingButton;

//2 variables needed in `update()`
bool cur_mouse_press = false;
bool pre_mouse_press;

static void init(void) {
    settingButton = button_create(SCREEN_W / 2 - 200, 600, 400, 100, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");

    change_bgm("Assets/audio/menu_bgm.mp3");
    cur_mouse_press = true;
}

static void update(void) {
    update_button(&settingButton);            // Button update: hovered or not.
    if (keyState[ALLEGRO_KEY_ENTER]) {        // Key "Enter" update: change to menu if pressed.
        change_scene(create_loading_scene());
    }

    /* TODO Hackathon 4-3 */
    // Create 2 variables to make smooth mouse click
    pre_mouse_press = cur_mouse_press;                                   // boolean
    cur_mouse_press = (mouseState.buttons & 1) && settingButton.hovered; // boolean
    // mouseState is "3" in total. Use bitwise operation to see if the user pressed the left (aka 1)!
    
    if (!pre_mouse_press && cur_mouse_press) {
        // Mouse click
        change_scene(create_setting_scene());
    }
}

int welcome_text_tick = 0;
static void draw(void) {
    // Title Text
    al_draw_text(
        TITLE_FONT,
        al_map_rgb(216 - 20, 110 - 20, 204 - 20),
        SCREEN_W / 2,
        225,
        ALLEGRO_ALIGN_CENTER,
        "NTHU-RPG ADVENTURE"
    );
    al_draw_text(
        TITLE_FONT,
        al_map_rgb(216, 110, 204),
        SCREEN_W / 2,
        220,
        ALLEGRO_ALIGN_CENTER,
        "NTHU-RPG ADVENTURE"
    );

    al_draw_text(
        P2_FONT,
        al_map_rgb(200 + 50 * sin(0.125 * welcome_text_tick - 23), 200 + 20 * sin(0.125 * welcome_text_tick - 100), 0),
        SCREEN_W / 2,
        500 + 20 * sin(0.125 * welcome_text_tick),
        ALLEGRO_ALIGN_CENTER,
        "PRESS [ENTER] TO PLAY"
    );

    // button
    draw_button(settingButton);
    // button text
    al_draw_text(
        P2_FONT,
        al_map_rgb(66, 76, 110),
        SCREEN_W / 2,
        600 + 28 + settingButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "SETTING"
    );
    al_draw_text(
        P2_FONT,
        al_map_rgb(225, 225, 225),
        SCREEN_W / 2,
        600 + 31 + settingButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "SETTING"
    );

    welcome_text_tick++;
}

static void destroy(void) {
    destroy_button(&settingButton);
}


Scene create_menu_scene(void) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));

    scene.name = "menu";
    scene.init = &init;
    scene.draw = &draw;
    scene.update = &update;
    scene.destroy = &destroy;

    return scene;
}
