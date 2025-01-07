#include <allegro5/allegro.h>
#include <string.h>
#include "menu_scene.h"
#include "game_scene.h"
#include "setting_scene.h"
#include "utility.h"
#include "UI.h"
#include "game.h"

static int timer_countdown;
static ALLEGRO_BITMAP* player_bitmap;

static void init(void) {
    timer_countdown = 120;

    player_bitmap = al_load_bitmap("Assets/panda_lose.png");
    if (!player_bitmap) {
        game_abort("Failed to load player bitmap");
    }

    change_bgm("Assets/audio/marimba-lose-250960.mp3");
}

static void update(void) {
    timer_countdown--;
    if (timer_countdown == 0) {
        change_scene(create_menu_scene());
    }
}

static void draw(void) {

    al_draw_text(
        TITLE_FONT,
        al_map_rgb(146, 161, 185),
        SCREEN_W / 3 + 128,
        SCREEN_H / 3 - 100,
        ALLEGRO_ALIGN_CENTER,
        "YOU LOSE ..."
    );

    al_draw_scaled_bitmap(player_bitmap,
        0, 0, 64, 64,
        (SCREEN_W / 2) - (256 / 2), (SCREEN_H / 2) - (256 / 2), 256, 256, 0);
}

static void destroy(void) {
    al_destroy_bitmap(player_bitmap);
}


Scene create_losing_scene(void) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));

    scene.name = "losing";
    scene.init = &init;
    scene.draw = &draw;
    scene.update = &update;
    scene.destroy = &destroy;

    return scene;
}
