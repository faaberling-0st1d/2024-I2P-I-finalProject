#include <allegro5/allegro.h>
#include "setting_scene.h"
#include "menu_scene.h"
#include "utility.h"
#include "UI.h"
#include "game.h"

#include <math.h>

// Back Button
static Button backButton;

// Volume Stick
static ALLEGRO_BITMAP* volumeStick;
const float stick_len = 492.84;                                          // Length of the Slider.
const float stick_height = 16;
const float stick_pos_x = 153.58;                                       // The left-up pos x.
const float stick_pos_y = 200;      // The height of slider (for slider).

// Volume Slider
static ALLEGRO_BITMAP* volumeSlider;
float slider_pos_x = 376;
float slider_pos_y = 184;
const float slider_size = 48;
float x_projection = 400;      // Projection for slider.
float altered_volume;    // The volume newly changed.

// Player Aspect Icon Buttons
static Button singleBtn;
static Button multiBtn;

static void init(void) {
    backButton = button_create(SCREEN_W / 2 - 200, 700, 400, 100, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");
    volumeStick = al_load_bitmap("Assets/volumeStick.png");
    volumeSlider = al_load_bitmap("Assets/volumeSlider.png");

    x_projection = stick_pos_x + (BGM_VOLUME / 1.0f) * stick_len;

    singleBtn = button_create(SCREEN_W / 4, 400, 100, 100, "Assets/panda_win.png", "Assets/panda_win.png");
    multiBtn = button_create(SCREEN_W * 3 / 4, 400, 100, 100, "Assets/multiplayer_icon.png", "Assets/multiplayer_icon.png");
}

bool isClicking(float target_x, float target_y, float target_w, float target_h) {

    if ((mouseState.buttons & 1) &&                                        // If mouse pressed (left click),
        (mouseState.x > target_x && mouseState.x < target_x + target_w) && // mouse within x range,
        (mouseState.y > target_y && mouseState.y < target_y + target_h)) { // mouse within y range,

        return true;
    }

    return false;

}

bool prev_flag_drag = false;
bool cur_flag_drag = false;
static void update(void) {

    update_button(&backButton);

    if (mouseState.buttons && backButton.hovered == true) {
        change_scene(create_menu_scene());
    }

    // Dragging the slider
    cur_flag_drag = mouseState.buttons & 1;
    if (isClicking(slider_pos_x, slider_pos_y, slider_size, slider_size)) {
        if (mouseState.x < stick_pos_x)                  x_projection = stick_pos_x;
        else if (mouseState.x > stick_pos_x + stick_len) x_projection = stick_pos_x + stick_len;
        else                                             x_projection = mouseState.x;
    }
    else if (cur_flag_drag && prev_flag_drag) {
        if (mouseState.x < stick_pos_x)                  x_projection = stick_pos_x;
        else if (mouseState.x > stick_pos_x + stick_len) x_projection = stick_pos_x + stick_len;
        else                                             x_projection = mouseState.x;
    }
    else {
        cur_flag_drag = false;
    }

    // Adjusting the volume
    SFX_VOLUME = BGM_VOLUME = 1.0f * ((x_projection - stick_pos_x) / stick_len);

    prev_flag_drag = cur_flag_drag;

}

int text_tick = 0;
static void draw(void) {
    // button
    draw_button(backButton);
    //button text
    al_draw_text(
        P2_FONT,
        al_map_rgb(66, 76, 110),
        SCREEN_W / 2,
        700 + 28 + backButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "BACK"
    );
    al_draw_text(
        P2_FONT,
        al_map_rgb(225, 225, 225),
        SCREEN_W / 2,
        700 + 31 + backButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "BACK"
    );

    // Volume Setting Area
    al_draw_text(
        P1_FONT,
        al_map_rgb(216, 110, 204),
        SCREEN_W / 2,
        (SCREEN_H / 2) - 256 - 40,
        ALLEGRO_ALIGN_CENTER,
        "Volume Setting"
    );
    
    // Volume Stick
    al_draw_scaled_bitmap(volumeStick,
        0, 0, 2495, 81,
        stick_pos_x, stick_pos_y, stick_len, stick_height, 0);
    
    // Volume Slider
    slider_pos_x = (x_projection - slider_size / 2);
    al_draw_scaled_bitmap(volumeSlider,
        0, 0, 550, 550,
        slider_pos_x, slider_pos_y, slider_size, slider_size, 0);

    // Aspect Setting Area
    al_draw_text(
        P1_FONT,
        al_map_rgb(216, 110, 204),
        SCREEN_W / 2,
        SCREEN_H / 2 - 80,
        ALLEGRO_ALIGN_CENTER,
        "Player Aspect"
    );

    al_draw_text(
        P2_FONT,
        al_map_rgb(255, 255, 255),
        SCREEN_W / 4,
        SCREEN_H / 2,
        ALLEGRO_ALIGN_CENTER,
        "Single Player"
    );
    
    al_draw_text(
        P2_FONT,
        al_map_rgb(255, 255, 255),
        SCREEN_W * 3 / 4,
        SCREEN_H / 2,
        ALLEGRO_ALIGN_CENTER,
        "Multiplayer"
    );

    // Overall Reminder: Press BACK button to reset!
    al_draw_text(
        P2_FONT,
        al_map_rgb(200 + 50 * sin(0.125 * text_tick - 23), 200 + 20 * sin(0.125 * text_tick - 100), 0),
        SCREEN_W / 2,
        640 + 20 * sin(0.125 * text_tick),
        ALLEGRO_ALIGN_CENTER,
        "Press BACK button to reset!"
    );

    text_tick++;
}

static void destroy(void) {
    destroy_button(&backButton);
}

Scene create_setting_scene(void) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));

    scene.name = "setting";
    scene.init = &init;
    scene.draw = &draw;
    scene.update = &update;
    scene.destroy = &destroy;

    return scene;
}
