#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gb/gb.h>
#include "alpha.c"
#include "levels/levels.c"
#include "tiles.c"
#include "sprites.c"
#include "blankscreen.c"

#define BLANK 0
#define WALL 1
#define CRATE 2
#define SLOT 3
#define PLAYER 4
#define UP 1
#define DOWN 2
#define LEFT 4
#define RIGHT 8

UINT8 selected_level = 0;
UINT8 crate_count = 0;
UINT8 slot_filled = 0;
UINT8 wall_mask[20][18];
UINT8 last_dir;

typedef struct entity {
    UINT8 x;
    UINT8 y;
    UINT8 move_mask;
} entity;
entity player, crates[10];

void load_level() {
    UINT16 index;
    UINT8 x, y;
    crate_count = 0;
    memset(wall_mask, 0, sizeof wall_mask);
    for (index = 0; index < 10; index++) {
        set_sprite_tile(index, 2);
    }
    set_bkg_tiles(0, 0, 20, 18, levels + level_index[selected_level]);
    for (index = 0; index < 360; index++) {
        if (*(levels + level_index[selected_level] + index) == PLAYER) {
            player.x = (index % 20) * 8 + 8;
            player.y = (index / 20) * 8 + 16;
            set_sprite_tile(0, 1);
            move_sprite(0, player.x, player.y);
        } else if (*(levels + level_index[selected_level] + index) == CRATE) {
            crates[crate_count].x = (index % 20) * 8 + 8;
            crates[crate_count].y = (index / 20) * 8 + 16;
            set_sprite_tile(crate_count + 1, 0);
            move_sprite(crate_count, crates[crate_count].x, crates[crate_count].y);
            crate_count++;
        } else if (*(levels + level_index[selected_level] + index) == WALL) {
            x = (index % 20);
            y = (index / 20);
            wall_mask[x][y] = 1;
        } else if (*(levels + level_index[selected_level] + index) == SLOT) {
            x = (index % 20);
            y = (index / 20);
            wall_mask[x][y] = 2;
        }
    }
}

void init() {
    DISPLAY_ON;
    set_bkg_data(0, 5, tiles);
    set_sprite_data(0, 2, sprites);
    load_level();
}

void update_crate_position(entity *p) {
    switch (last_dir)
    {
    case UP:
        p->y -= 8;
        break;
    case DOWN:
        p->y += 8;
        break;
    case LEFT:
        p->x -= 8;
    break;
    case RIGHT:
        p->x += 8;
    break;
    default:
        break;
    }
}

void check_input() {
    UINT8 index;
    last_dir = 0;
    switch (joypad())
    {
    case J_B:
        load_level();
    break;
    case J_RIGHT:
        if (!(player.move_mask & RIGHT)) {
            player.x += 8;
            last_dir = RIGHT;
            waitpadup();
        }
    break;
    case J_LEFT:
        if (!(player.move_mask & LEFT)) {
            player.x -= 8;
            last_dir = LEFT;
            waitpadup();
        }
    break;
    case J_UP:
        if (!(player.move_mask & UP)) {
            player.y -= 8;
            last_dir = UP;
            waitpadup();
        }
    break;
    case J_DOWN:
        if (!(player.move_mask & DOWN)) {
            player.y += 8;
            last_dir = DOWN;
            waitpadup();
        }
    break;
    default:
        break;
    }
    move_sprite(0, player.x, player.y);
    if (last_dir != 0) {
        for (index = 0; index < crate_count; index++) {
            if (player.x == crates[index].x && player.y == crates[index].y) {
                update_crate_position(&crates[index]);
            }
        }
    }
    for (index = 0; index < crate_count; index++) {
        move_sprite(index + 1, crates[index].x, crates[index].y);
    }
}

void update_switches() {
    HIDE_WIN;
    SHOW_SPRITES;
    SHOW_BKG;
}

void check_wall(entity *p) {
    UINT8 x, y;
    x = (p->x - 8) / 8;
    y = (p->y - 16) / 8;
    p->move_mask = 0;
    if (y > 0 && wall_mask[x][y - 1] == 1) {
        p->move_mask |= UP;
    }
    if (y < 17 && wall_mask[x][y + 1] == 1) {
        p->move_mask |= DOWN;
    }
    if (x > 0 && wall_mask[x - 1][y] == 1) {
        p->move_mask |= LEFT;
    }
    if (x < 19 && wall_mask[x + 1][y] == 1) {
        p->move_mask |= RIGHT;
    }
}

void check_slot(entity *p) {
    UINT8 x, y;
    x = (p->x - 8) / 8;
    y = (p->y - 16) / 8;
    slot_filled += wall_mask[x][y] == 2;
}

void check_player_wall() {
    check_wall(&player);
}

void check_crate_all() {
    slot_filled = 0;
    UINT8 i = 0, j = 0;
    for (i = 0; i < crate_count; i++) {    
        check_wall(&crates[i]);
        check_slot(&crates[i]);
    }
    for (i = 0; i < crate_count; i++) {
        for (j = 0; j < crate_count; j++) {
            if (i == j) continue;
            if (crates[i].x == crates[j].x) {
                if (crates[i].y - crates[j].y == 8) {
                    crates[j].move_mask |= DOWN;
                }
                if (crates[j].y - crates[i].y == 8) {
                    crates[j].move_mask |= UP;
                }
            }
            if (crates[i].y == crates[j].y) {
                if (crates[i].x - crates[j].x == 8) {
                    crates[j].move_mask |= RIGHT;
                }
                if (crates[j].x - crates[i].x == 8) {
                    crates[j].move_mask |= LEFT;
                }
            }
        }
    }
}

void check_player_crate() {
    UINT8 i = 0, j = 0;
    for (i = 0; i < crate_count; i++) { 
        if (crates[i].y == player.y) {
            if (crates[i].x - player.x == 8) {
                player.move_mask |= crates[i].move_mask & RIGHT;
            }
            if (player.x - crates[i].x == 8) {
                player.move_mask |= crates[i].move_mask & LEFT;
            }
        }
        if (crates[i].x == player.x) {
            if (crates[i].y - player.y == 8) {
                player.move_mask |= crates[i].move_mask & DOWN;
            }
            if (player.y - crates[i].y == 8) {
                player.move_mask |= crates[i].move_mask & UP;
            }
        }
    }
}

void check_movement() {
    check_crate_all();
    check_player_wall();
    check_player_crate();
}

UINT8 check_gameover() {
    return slot_filled == crate_count;
}

UINT8 main() {
    init();
    while (!check_gameover()) {
        check_movement();
        check_input();
        update_switches();
        wait_vbl_done();
    }
    printf("GAME OVER\n");
    wait_vbl_done();
    return 0;
}