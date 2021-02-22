#include <stdio.h>
#include <gb/gb.h>
#include "alpha.c"
#include "levels/levels.c"
#include "tiles.c"
#include "blankscreen.c"

int selected_level = 0;

void load_level() {
    set_bkg_tiles(0, 0, 20, 18, levels + level_index[selected_level]);
}

void init() {
    DISPLAY_ON;
    set_bkg_data(0, 4, tiles);
    load_level();
}

void check_input() {
    if (joypad() & J_B) {

    }
    if (joypad() & J_RIGHT) {
        selected_level++;
        if (selected_level >= level_count) {
            selected_level = level_count - 1;
        } else {
            load_level();
        }
    }
    if (joypad() & J_LEFT) {
        selected_level--;
        if (selected_level < 0) {
            selected_level = 0;
        } else {
            load_level();
        }
    }
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