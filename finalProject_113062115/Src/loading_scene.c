#include <allegro5/allegro.h>
#include <string.h>
#include "menu_scene.h"
#include "loading_scene.h"
#include "game_scene.h"
#include "setting_scene.h"
#include "utility.h"
#include "UI.h"
#include "game.h"

#include <math.h>

static int timer_countdown;
static ALLEGRO_BITMAP* player_bitmap;

static void init(void) {
    timer_countdown = 60;

    player_bitmap = al_load_bitmap("Assets/panda2.png");
    if (!player_bitmap) {
        game_abort("Failed to load player bitmap");
    }

    change_bgm("None");
}

int hint_text_tick = 0;
static void update(void) {
    timer_countdown--;
    if (timer_countdown == 0) {
        change_scene(create_game_scene());
    }

    hint_text_tick++;
}


static void draw(void) {

    al_draw_text(
        TITLE_FONT,
        al_map_rgb(216, 110, 204),
        SCREEN_W / 2 + 40,
        SCREEN_H / 2,
        ALLEGRO_ALIGN_CENTER,
        "LOADING..."
    );

    al_draw_scaled_bitmap(player_bitmap,
        0, 0, 32, 32,
        SCREEN_W / 4, SCREEN_H / 2, 64, 64, 0);

    // Reminder
    al_draw_text(
        P2_FONT,
        al_map_rgb(160 + 50 * sin(0.125 * hint_text_tick - 23), 195 + 20 * sin(0.125 * hint_text_tick), 0),
        SCREEN_W / 2,
        640 + 20 * sin(0.125 * hint_text_tick),
        ALLEGRO_ALIGN_CENTER,
        "Hint: Kill all the enemy to win!"
    );
}

static void destroy(void) {
    al_destroy_bitmap(player_bitmap);
}


Scene create_loading_scene(void) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));

    scene.name = "loading";
    scene.init = &init;
    scene.draw = &draw;
    scene.update = &update;
    scene.destroy = &destroy;

    return scene;
}
