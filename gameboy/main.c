#include <stdio.h>
#include <gb/gb.h>
#include "alpha.c"
#include "levels/levels.c"
#include "tiles.c"
#include "sprites.c"
#include "blankscreen.c"

#define X 0
#define Y 1
#define BLANK 0
#define WALL 2
#define CRATE 2
#define SLOT 3
#define PLAYER 4

UINT8 selected_level = 0;
UINT8 player[2];
UINT8 crate[9][2];
UINT8 crate_count = 0;

void load_level() {
    crate_count = 0;
    set_bkg_tiles(0, 0, 20, 18, levels + level_index[selected_level]);
    for (UINT16 index = 0; index < 360; index++) {
        if (*(levels + level_index[selected_level] + index) == PLAYER) {
            player[X] = (index % 20) * 8 + 8;
            player[Y] = (index / 20) * 8 + 16;
            set_sprite_tile(0, 1);
            move_sprite(0, player[X], player[Y]);
        } else if (*(levels + level_index[selected_level] + index) == CRATE) {
            crate[crate_count][X] = (index % 20) * 8 + 8;
            crate[crate_count][Y] = (index / 20) * 8 + 16;
            crate_count++;
            set_sprite_tile(crate_count, 0);
            move_sprite(crate_count, crate[crate_count - 1][X], crate[crate_count - 1][Y]);
        }
    }
}

void init() {
    DISPLAY_ON;
    set_bkg_data(0, 5, tiles);
    set_sprite_data(0, 2, sprites);
    load_level();
}

void check_input() {
    switch (joypad())
    {
        case J_B:
            set_bkg_tiles(0, 0, 20, 18, blankscreen);
        break;
        case J_RIGHT:
            player[X] += 8;
            waitpadup();
        break;
        case J_LEFT:
            player[X] -= 8;
            waitpadup();
        break;
        case J_UP:
            player[Y] -= 8;
            waitpadup();
        break;
        case J_DOWN:
            player[Y] += 8;
            waitpadup();
        break;
        default:
            break;
    }
    move_sprite(0, player[X], player[Y]);
}

void update_switches() {
    HIDE_WIN;
    SHOW_SPRITES;
    SHOW_BKG;
}

UINT8 main() {
    init();
    while (1) {
        check_input();
        update_switches();
        wait_vbl_done();
    }
    return 0;
}