#include <gb/gb.h>
#include "alpha.c"
#include "levels/levels.c"
#include "tiles.c"
#include "blankscreen.c"

void init() {
    DISPLAY_ON;
    set_bkg_data(0, 4, tiles);
    set_bkg_tiles(0,0,20,18, levels);
}

void check_input() {
    if (joypad() & J_B) {

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