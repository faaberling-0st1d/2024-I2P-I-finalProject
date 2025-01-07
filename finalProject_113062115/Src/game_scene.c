#include <string.h>
#include "game_scene.h"
#include "menu_scene.h"
#include "loading_scene.h"
#include "game.h"
#include "player.h"
#include "map.h"
#include "enemy.h"
#include "weapon.h"
#include "UI.h"

#include "losing_scene.h"
#include "winning_scene.h"

#include <math.h>

Player player;           // Player
Map map;                 // Map
enemyNode * enemyList;   // Enemy List
BulletNode * bulletList; // Bullet List

// Weapon
Weapon weapon1;          // normal gun (press 1)
Weapon weapon2;          // sniper (press 2)

// Coin
int coins_obtained;
extern int coin_counter;

static void init(void){

    /* ADVANCED UI - Quitting*/
    // QUITTING ICON
    /*static Button quitting_btn;
    quitting_btn = button_create(0, 0, 100, 100, "Assets/quitGame_Button.png", "Assets/quitGame_hoveredButton.png");
    printf("[DEBUGGER] QUITTING BOTTON INIT\n");*/
    
    initEnemy();
    
    map = create_map("Assets/map0.txt", 0);

    player = create_player("Assets/panda2.png", map.Spawn.x, map.Spawn.y);

    enemyList = createEnemyList();
    bulletList = createBulletList();

    weapon1 = create_weapon("Assets/guns.png", "Assets/yellow_bullet.png", 16, 8, 10);
    weapon2 = create_weapon("Assets/sniper.png", "Assets/orange_bullet.png", 50, 50, 50);
    
    for(int i=0; i<map.EnemySpawnSize; i++){
        Enemy enemy = createEnemy(map.EnemySpawn[i].x, map.EnemySpawn[i].y, map.EnemyCode[i]);
        insertEnemyList(enemyList, enemy);
    }

    game_log("coord x:%d \n coords y:%d \n", map.Spawn.x, map.Spawn.y);
    change_bgm("Assets/audio/game_bgm.mp3");
}

int weaponChange_CD = 100;
Weapon* cur_weapon = &weapon1;

static void update(void){
    weaponChange_CD--;
    /* [ADVANCED GAMEPLAY] Change Weapon */
    if (cur_weapon != &weapon2 && keyState[ALLEGRO_KEY_2] && weaponChange_CD < 0) {
        // Change to weapon 2 - sniper
        printf("[DEBUGGER] Change to weapon 2.\n");
        cur_weapon = &weapon2;
        weaponChange_CD = 100;
    }
    else if (cur_weapon != &weapon1 && keyState[ALLEGRO_KEY_1] && weaponChange_CD < 0) {
        // Change to weapon 1 - normal gun
        printf("[DEBUGGER] Change to weapon 1.\n");
        cur_weapon = &weapon1;
        weaponChange_CD = 100;
    }

    /* [TODO Homework II 12.25] Change the scene if winning/losing to win/lose scene */

    // I. LOSING
    if ((player.status == PLAYER_DYING) && (player.animation_tick == 128 - 1)) {                     // Player dying:
        change_scene(create_losing_scene());
    }
    // II. WINNING
    else if ( enemyList->next == NULL ) {  // Eneny List (linklist) is empty:
        change_scene(create_winning_scene());
    }
    // III. ELSE
    else {
        update_player(&player, &map);
        
        Point Camera;

        /* [TODO Hackathon 1-3] CAMERA POSITION SETUP */
        // Camera.pos = player.pos - screen_len / 2
        Camera.x = player.coord.x - SCREEN_W / 2;
        Camera.y = player.coord.y - SCREEN_H / 2;

        updateEnemyList(enemyList, &map, &player);
        update_weapon(cur_weapon, bulletList, player.coord, Camera);
        updateBulletList(bulletList, enemyList, &map);
        update_map(&map, player.coord, &coins_obtained);
    }
}

// Integer to String Function;
void int_to_str(char* goal_str, size_t goal_str_size, int target_int) {
    snprintf(goal_str, goal_str_size, "%d", target_int);
}

static void draw(void){
    Point Camera;

    /* [TODO Hackathon 1-4] CAMERA MOVEMENT */
    Camera.x = player.coord.x - SCREEN_W / 2;
    Camera.y = player.coord.y - SCREEN_H / 2;
    
    // Draw
    draw_map(&map, Camera);
    drawEnemyList(enemyList, Camera);
    drawBulletList(bulletList, Camera);
    draw_player(&player, Camera);
    draw_weapon(cur_weapon, player.coord, Camera);

    /* [TODO Homework V] DRAW HEALTH AND TOTAL COINS */

    // HP ICON
    static ALLEGRO_BITMAP* hp_icon;
    hp_icon = al_load_bitmap("Assets/heart.png");
    al_draw_scaled_rotated_bitmap(hp_icon, 0, 0, 0, 5, 2.0, 2.0, 0, 0);
    

    // HP VALUE LABEL
    char hp_value_txt[20]; // Set the max num of digits as 20
    int_to_str(hp_value_txt, sizeof(hp_value_txt), player.health);
    
    al_draw_text(
        P2_FONT,
        al_map_rgb(255, 255, 255),
        80,
        20,
        ALLEGRO_ALIGN_LEFT,
        hp_value_txt
    );

    // COIN ICON
    static ALLEGRO_BITMAP* coin_icon;
    coin_icon = al_load_bitmap("Assets/coin_icon.png");
    al_draw_scaled_rotated_bitmap(coin_icon, 0, 0, 0, 45, 4.0, 4.0, 0, 0);

    // COIN COUNT LABEL
    char coins_obtained_txt[20];
    int_to_str(coins_obtained_txt, sizeof(coins_obtained_txt), coins_obtained);

    al_draw_text(
        P2_FONT,
        al_map_rgb(255, 255, 255),
        80,
        64,
        ALLEGRO_ALIGN_LEFT,
        coins_obtained_txt
    );

    
    
}

static void destroy(void){
    delete_player(&player);
    delete_weapon(&weapon1);
    delete_weapon(&weapon2);
    destroy_map(&map);
    destroyBulletList(bulletList);
    destroyEnemyList(enemyList);
    terminateEnemy();
}


Scene create_game_scene(void){
    Scene scene;
    memset(&scene, 0, sizeof(Scene));
    
    scene.name = "game";
    scene.init = &init;
    scene.draw = &draw;
    scene.update = &update;
    scene.destroy = &destroy;
    
    return scene;
}
