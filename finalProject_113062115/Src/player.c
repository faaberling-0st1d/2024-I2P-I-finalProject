#include "player.h"
#include "game.h"
#include "utility.h"
#include "map.h"

#include <math.h>

static bool isCollision(Player* player, Map* map);

bool flag_dying;

Player create_player(char * path, int row, int col){
    Player player;
    memset(&player, 0, sizeof(player));
    
    player.coord = (Point){
        col * TILE_SIZE,
        row * TILE_SIZE
    };
    
    player.speed = 4;
    player.health = 100;
    
    player.image = al_load_bitmap(path);
    if(!player.image){
        game_abort("Error Load Bitmap with path : %s", path);
    }
    
    flag_dying = false;
    return player;
}


void update_player(Player* player, Map* map) {
    Point original = player->coord;

    // Knockback effect
    if (player->knockback_CD > 0) {

        player->knockback_CD--;
        int next_x = player->coord.x + player->speed * cos(player->knockback_angle);
        int next_y = player->coord.y + player->speed * sin(player->knockback_angle);
        Point next;
        next = (Point){ next_x, player->coord.y };

        if (!isCollision(player, map)) {
            player->coord = next;
        }

        next = (Point){ player->coord.x, next_y };
        if (!isCollision(player, map)) {
            player->coord = next;
        }
    }

    /* [TODO Hackathon 1-1] UP-DOWN MOVEMENT */

    if (!flag_dying) {
        // 'W': Goes up.
        if (keyState[ALLEGRO_KEY_W]) {
            player->coord.y -= (player->speed);    // Decrease (or change) the coord.y by the speed.
            player->direction = PLAYER_UP;          // According to the num keyboard.
        }

        // 'S': Goes down.
        if (keyState[ALLEGRO_KEY_S]) {
            player->coord.y += (player->speed);    // Increase (or change) the coord.y by the speed.
            player->direction = PLAYER_DOWN;        // According to the num keyboard.
        }

        // if Collide, snap to the grid to make it pixel perfect
        if (isCollision(player, map)) {
            player->coord.y = round((float)original.y / (float)TILE_SIZE) * TILE_SIZE;
        }
    }

    /* [TODO Hackathon 1-2] LEFT-RIGHT MOVEMENT */

    if (!flag_dying) {
            // 'A': Goes left.
        if (keyState[ALLEGRO_KEY_A]) {
            player->coord.x -= (player->speed);    // Decrease (or change) the coord.x by the speed.
            // printf("[LOG] Current Pos: (%d, %d)\n", player->coord.x, player->coord.y);
            player->direction = PLAYER_LEFT;        // According to the num keyboard.
        }

        // 'D': Goes right. 
        if (keyState[ALLEGRO_KEY_D]) {
            player->coord.x += (player->speed);    // Increase (or change) the coord.x by the speed.
            // printf("[LOG] Current Pos: (%d, %d)\n", player->coord.x, player->coord.y);
            player->direction = PLAYER_RIGHT;       // According to the num keyboard.
        }
    }
    
    if(isCollision(player, map)){
        player->coord.x = round((float)original.x / (float)TILE_SIZE) * TILE_SIZE;
    }
    
    /* [TODO Homework III] Player Animation */
    player->animation_tick = (player->animation_tick + 1) % 128;

    // Status Adjust - See if the player is walking / idle / or dead:
    if ((keyState[ALLEGRO_KEY_W] || keyState[ALLEGRO_KEY_A] || keyState[ALLEGRO_KEY_S] || keyState[ALLEGRO_KEY_D]) && !flag_dying) {
        player->status = PLAYER_WALKING;
    }
    else if (!flag_dying)
        player->status = PLAYER_IDLE;
    
    // Status Adjust - On the brink of dying:
    /*if (player->health <= 0 && !flag_dying) {
        player->status = PLAYER_DYING;
        player->animation_tick = 0;
        flag_dying = true;
    }*/

}

void draw_player(Player * player, Point cam){
    int dy = player->coord.y - cam.y; // destiny y axis
    int dx = player->coord.x - cam.x; // destiny x axis
    
    int flag = player->direction == PLAYER_RIGHT ? 1 : 0;     // Player flipping.
    int tint_to_red = (player->knockback_CD > 0) ? 0 : 255;   // Ture the player to red when attacked.

    /* [TODO Homework III] - Draw Animation of Dying, Walking, and Idle */

    // I. IDLE
    if (player->status == PLAYER_IDLE) {
        int idle_offset = 32 * (int)(player->animation_tick / 64);

        al_draw_tinted_scaled_rotated_bitmap_region(player->image, idle_offset, 0, 32, 32, al_map_rgb(255, tint_to_red, tint_to_red),
            0, 0, dx, dy, TILE_SIZE / 32, TILE_SIZE / 32,
            0, flag);
    }

    // II. WALKING
    else if (player->status == PLAYER_WALKING) {
        // `offest`: to "change" the animation to the next frame.
        int walk_offset = 32 * (int)(player->animation_tick / 32);

        al_draw_tinted_scaled_rotated_bitmap_region(player->image, walk_offset, 32, 32, 32, al_map_rgb(255, tint_to_red, tint_to_red),
            0, 0, dx, dy, TILE_SIZE / 32, TILE_SIZE / 32,
            0, flag);
    }

    // III. DYING
    else if (player->status == PLAYER_DYING) {
        printf("[LOG] DYING ANIMATION.\n");
        int dying_offset = 32 * (int)(player->animation_tick / 32);

        al_draw_tinted_scaled_rotated_bitmap_region(player->image, dying_offset, 64, 32, 32, al_map_rgb(255, 255, 255),
            0, 0, dx, dy, TILE_SIZE / 32, TILE_SIZE / 32,
            0, flag);
    }
    
#ifdef DRAW_HITBOX
    al_draw_rectangle(
        dx, dy, dx + TILE_SIZE, dy + TILE_SIZE,
        al_map_rgb(255, 0, 0), 1
    );
#endif
    
}

void delete_player(Player * player){
    al_destroy_bitmap(player->image);
}

static bool isCollision(Player* player, Map* map){

    if (player->coord.x < 0 ||
        player->coord.y < 0 ||
        (player->coord.x + TILE_SIZE - 1) / TILE_SIZE >= map->col ||
        (player->coord.y + TILE_SIZE - 1) / TILE_SIZE >= map->row)
        return true;

    /*
        [TODO Hackathon 2-1] 
    
        Check every corner of enemy if it's collide or not

        We check every corner if it's collide with the wall/hole or not
        You can activate DRAW_HITBOX in utility.h to see how the collision work

        Note: (1) The position of the avatars - the leftup most corner.
              (2) Notice the difference of x-y coord sys (btwn map and avatar).
    */
    
    if (!isWalkable(map->map[((player->coord.y)                 / TILE_SIZE)][((player->coord.x)                 / TILE_SIZE)])) return true;
    if (!isWalkable(map->map[((player->coord.y + TILE_SIZE - 1) / TILE_SIZE)][((player->coord.x)                 / TILE_SIZE)])) return true;
    if (!isWalkable(map->map[((player->coord.y)                 / TILE_SIZE)][((player->coord.x + TILE_SIZE - 1) / TILE_SIZE)])) return true;
    if (!isWalkable(map->map[((player->coord.y + TILE_SIZE - 1) / TILE_SIZE)][((player->coord.x + TILE_SIZE - 1) / TILE_SIZE)])) return true;
    
    return false;
}

void hitPlayer(Player * player, Point enemy_coord, int damage){
    if (player->knockback_CD == 0) {
        float dY = player->coord.y - enemy_coord.y;
        float dX = player->coord.x - enemy_coord.x;
        float angle = atan2(dY, dX);

        /* [TODO Homework I] GAME LOGIC WHEN BEING HIT OR DIED */

        player->knockback_angle = angle;
        player->knockback_CD = 40;

        player->health -= damage;

        if (player->health <= 0) {

            player->health = 0;
            player->status = PLAYER_DYING;
            player->knockback_CD = 0;

            if (!flag_dying) player->animation_tick = 0;    // Reset animation_tick to start dying animation.
            flag_dying = true;
        }   
    }
}
