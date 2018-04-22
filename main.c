/* COMP2215 Task 7*/

#include <stdlib.h>
#include "lcd.h"
#include "ruota.h"
#include "rios.h"
#include <avr/interrupt.h>
// #include "main.h"

#define LCD_WIDTH    320
#define LCD_HEIGHT    240
#define BOARD_SIZE  240
#define BOARD_SQUARE_SIZE 30

#define BLACK 0x0000
#define BLUE  0x001F
#define LIGHT_BLUE 0x0EDC
#define LIGHT_BROWN 0XFE60
#define DARK_BROWN 0xD300

#define true 1
#define false 0

typedef int8_t bool;

typedef struct {
    int8_t x, y;
} position;

typedef struct {
    position pos;
    char type;
    bool player;
} piece;

void os_init(void);

void draw_initial_board(void);

void draw_square(int8_t, int8_t);

void draw_piece(int8_t, int8_t, unsigned char (*)[4], int8_t);

void draw_piece_at_position(piece *);

void init_game(void);

void perform_action(void);

piece *get_piece_at_position(int8_t, int8_t);

bool is_valid_move_position(void);

bool is_valid_move_pieces(void);

void move_cursor(int8_t, int8_t);

int check_switches(int);

// code from https://github.com/MKLOL/FortunaMultiplayer/blob/master/main.c
// binary representation of Textures for chess.
unsigned char rk[BOARD_SQUARE_SIZE][4] = {{255, 255, 255, 255},
                                          {255, 255, 255, 255},
                                          {254, 255, 255, 31},
                                          {254, 255, 255, 3},
                                          {254, 255, 255, 11},
                                          {254, 255, 127, 8},
                                          {226, 255, 127, 9},
                                          {34,  255, 63,  9},
                                          {174, 254, 47,  9},
                                          {238, 0,   32,  9},
                                          {238, 0,   32,  9},
                                          {238, 0,   32,  9},
                                          {226, 0,   32,  9},
                                          {226, 0,   32,  9},
                                          {226, 0,   32,  9},
                                          {226, 0,   32,  9},
                                          {226, 0,   32,  9},
                                          {226, 0,   32,  9},
                                          {238, 0,   32,  9},
                                          {238, 0,   32,  9},
                                          {238, 0,   32,  9},
                                          {174, 255, 63,  9},
                                          {162, 255, 63,  9},
                                          {226, 255, 127, 9},
                                          {254, 255, 127, 8},
                                          {254, 255, 255, 11},
                                          {254, 255, 255, 31},
                                          {255, 255, 255, 255},
                                          {255, 255, 255, 255},
                                          {255, 255, 255, 255}};
unsigned char qu[BOARD_SQUARE_SIZE][4] = {{255, 255, 255, 63},
                                          {31,  255, 255, 63},
                                          {31,  254, 255, 63},
                                          {31,  249, 255, 63},
                                          {255, 199, 255, 63},
                                          {255, 15,  254, 63},
                                          {231, 63,  248, 63},
                                          {227, 255, 240, 38},
                                          {3,   192, 248, 38},
                                          {231, 0,   216, 38},
                                          {255, 7,   216, 39},
                                          {255, 63,  216, 35},
                                          {251, 127, 88,  35},
                                          {241, 1,   88,  35},
                                          {1,   0,   88,  35},
                                          {241, 1,   88,  35},
                                          {255, 127, 88,  35},
                                          {255, 63,  216, 35},
                                          {255, 7,   216, 38},
                                          {231, 0,   216, 38},
                                          {7,   240, 216, 38},
                                          {199, 127, 0,   36},
                                          {231, 63,  248, 63},
                                          {255, 15,  255, 63},
                                          {127, 231, 255, 63},
                                          {63,  252, 255, 63},
                                          {63,  254, 255, 63},
                                          {63,  254, 255, 63},
                                          {255, 255, 255, 63},
                                          {255, 255, 255, 255}};
unsigned char bs[BOARD_SQUARE_SIZE][4] = {{255, 255, 255, 255},
                                          {255, 255, 255, 47},
                                          {255, 255, 255, 39},
                                          {255, 255, 255, 55},
                                          {255, 255, 255, 51},
                                          {255, 255, 255, 51},
                                          {255, 255, 255, 51},
                                          {255, 129, 255, 59},
                                          {255, 0,   255, 51},
                                          {127, 0,   156, 51},
                                          {63,  14,  144, 51},
                                          {31,  14,  120, 51},
                                          {13,  14,  120, 59},
                                          {129, 63,  120, 59},
                                          {128, 63,  120, 62},
                                          {129, 63,  120, 62},
                                          {31,  14,  120, 58},
                                          {31,  14,  88,  51},
                                          {63,  14,  88,  51},
                                          {127, 0,   144, 51},
                                          {255, 0,   254, 51},
                                          {255, 129, 255, 51},
                                          {255, 255, 255, 51},
                                          {255, 255, 255, 51},
                                          {255, 255, 255, 51},
                                          {255, 255, 255, 51},
                                          {255, 255, 255, 35},
                                          {255, 255, 255, 39},
                                          {255, 255, 255, 63},
                                          {255, 255, 255, 255}};
unsigned char pw[BOARD_SQUARE_SIZE][4] = {{255, 255, 255, 255},
                                          {255, 255, 255, 255},
                                          {255, 255, 255, 255},
                                          {255, 255, 255, 255},
                                          {255, 255, 255, 16},
                                          {255, 255, 31,  16},
                                          {255, 255, 15,  16},
                                          {255, 255, 7,   16},
                                          {255, 255, 3,   16},
                                          {255, 255, 1,   16},
                                          {255, 193, 0,   16},
                                          {127, 0,   0,   16},
                                          {119, 0,   0,   16},
                                          {33,  0,   0,   16},
                                          {1,   0,   0,   16},
                                          {1,   0,   0,   16},
                                          {33,  0,   0,   16},
                                          {127, 0,   0,   16},
                                          {255, 128, 0,   16},
                                          {255, 225, 0,   16},
                                          {255, 255, 1,   16},
                                          {255, 255, 1,   16},
                                          {255, 255, 3,   16},
                                          {255, 255, 15,  16},
                                          {255, 255, 63,  16},
                                          {255, 255, 255, 19},
                                          {255, 255, 255, 255},
                                          {255, 255, 255, 255},
                                          {255, 255, 255, 255},
                                          {255, 255, 255, 255}};
unsigned char kg[BOARD_SQUARE_SIZE][4] = {{255, 255, 255, 255},
                                          {255, 255, 255, 255},
                                          {255, 255, 255, 63},
                                          {255, 1,   255, 63},
                                          {255, 252, 252, 63},
                                          {127, 255, 251, 63},
                                          {127, 135, 247, 63},
                                          {191, 3,   14,  0},
                                          {191, 1,   28,  13},
                                          {255, 3,   152, 13},
                                          {127, 3,   152, 13},
                                          {255, 30,  152, 5},
                                          {31,  252, 153, 4},
                                          {237, 199, 223, 6},
                                          {48,  63,  192, 6},
                                          {253, 63,  223, 6},
                                          {237, 241, 159, 6},
                                          {255, 252, 152, 4},
                                          {127, 15,  152, 5},
                                          {127, 3,   152, 13},
                                          {191, 1,   152, 13},
                                          {191, 1,   156, 13},
                                          {191, 1,   14,  0},
                                          {191, 129, 247, 63},
                                          {127, 231, 249, 63},
                                          {127, 254, 252, 63},
                                          {255, 56,  255, 63},
                                          {255, 227, 255, 63},
                                          {255, 255, 255, 255},
                                          {255, 255, 255, 255}};
unsigned char hs[BOARD_SQUARE_SIZE][4] = {{255, 255, 255, 255},
                                          {255, 255, 255, 255},
                                          {255, 255, 249, 63},
                                          {255, 127, 246, 63},
                                          {255, 31,  230, 63},
                                          {255, 7,   224, 63},
                                          {255, 0,   224, 63},
                                          {127, 0,   224, 63},
                                          {63,  7,   248, 63},
                                          {135, 7,   252, 35},
                                          {143, 1,   252, 32},
                                          {31,  0,   60,  32},
                                          {1,   0,   30,  32},
                                          {3,   0,   14,  32},
                                          {15,  0,   2,   32},
                                          {127, 240, 0,   32},
                                          {255, 48,  0,   32},
                                          {255, 0,   0,   32},
                                          {191, 1,   0,   32},
                                          {127, 7,   0,   32},
                                          {255, 14,  0,   32},
                                          {255, 61,  0,   32},
                                          {255, 255, 0,   32},
                                          {255, 239, 31,  32},
                                          {255, 63,  255, 63},
                                          {255, 255, 195, 63},
                                          {255, 255, 255, 32},
                                          {255, 255, 255, 255},
                                          {255, 255, 255, 255},
                                          {255, 255, 255, 255}};

position current_cursor_position = {0, 0};

piece pieces[32] = {
        {{0, 0}, 'r', 0},
        {{1, 0}, 'h', 0},
        {{2, 0}, 'b', 0},
        {{3, 0}, 'q', 0},
        {{4, 0}, 'k', 0},
        {{5, 0}, 'b', 0},
        {{6, 0}, 'h', 0},
        {{7, 0}, 'r', 0},
        {{0, 1}, 'p', 0},
        {{1, 1}, 'p', 0},
        {{2, 1}, 'p', 0},
        {{3, 1}, 'p', 0},
        {{4, 1}, 'p', 0},
        {{5, 1}, 'p', 0},
        {{6, 1}, 'p', 0},
        {{7, 1}, 'p', 0},
        {{0, 7}, 'r', 1},
        {{1, 7}, 'h', 1},
        {{2, 7}, 'b', 1},
        {{3, 7}, 'q', 1},
        {{4, 7}, 'k', 1},
        {{5, 7}, 'b', 1},
        {{6, 7}, 'h', 1},
        {{7, 7}, 'r', 1},
        {{0, 6}, 'p', 1},
        {{1, 6}, 'p', 1},
        {{2, 6}, 'p', 1},
        {{3, 6}, 'p', 1},
        {{4, 6}, 'p', 1},
        {{5, 6}, 'p', 1},
        {{6, 6}, 'p', 1},
        {{7, 6}, 'p', 1}
};

piece *selected_piece = NULL;
position previous_selected_position = {-1, -1};


void main(void) {

    os_init();

    os_add_task(check_switches, 100, 1);

    draw_initial_board();

    sei();
    for (;;) {
    }

}

void os_init(void) {
    /* 8MHz clock, no prescaling (DS, p. 48) */
    CLKPR = (1 << CLKPCE);
    CLKPR = 0;

    // init_debug_uart1();
    init_lcd();
    os_init_scheduler();
    os_init_ruota();
}

/*
*	Drawing functions
*/
void draw_initial_board() {
    clear_screen();

    int i;
    int j;

    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            draw_square(i, j);
        }
    }
}

void draw_square(int8_t x, int8_t y) {
    uint16_t colour;

    if (current_cursor_position.x == x && current_cursor_position.y == y) {
        colour = LIGHT_BLUE;
    } else {
        if ((x + y) % 2 == 0) {
            colour = LIGHT_BROWN;
        } else {
            colour = DARK_BROWN;
        }
    }

    rectangle rect = {
            x * BOARD_SQUARE_SIZE,
            x * BOARD_SQUARE_SIZE + BOARD_SQUARE_SIZE,
            y * BOARD_SQUARE_SIZE,
            y * BOARD_SQUARE_SIZE + BOARD_SQUARE_SIZE,
    };

    fill_rectangle(rect, colour);

    piece *piece_at = get_piece_at_position(x, y);

    if (piece_at != NULL) {
        display_string_xy("drawing", 240, 10);
        draw_piece_at_position(piece_at);
    } else {
        display_string_xy("not    ", 240, 10);
    }

    // selected piece position debug
    char debug[256];
    sprintf(debug, "spp: %d %d  ", selected_piece->pos.x, selected_piece->pos.y);
    display_string_xy(debug, 240, 0);


    // cursor position debug
    sprintf(debug, "cp: %d %d  ", current_cursor_position.x, current_cursor_position.y);
    display_string_xy(debug, 240, 20);

    // previous selected piece position debug
    sprintf(debug, "pspp: %d %d  ", previous_selected_position.x, previous_selected_position.y);
    display_string_xy(debug, 240, 30);

    // position of all pieces debug
    int i = 0;
    for (i = 0; i < 16; i++) {
        sprintf(debug, "%d: %d %d  ", i, pieces[i].pos.x, pieces[i].pos.y);
        display_string_xy(debug, 240, i * 10 + 50);
    }
}

// code from https://github.com/MKLOL/FortunaMultiplayer/blob/master/main.c
void draw_piece(int8_t x, int8_t y, unsigned char vx[BOARD_SQUARE_SIZE][4], bool player_val) {
    rectangle rect;

    int8_t i;
    int8_t j;

    uint16_t colour;

    if (player_val == 0) {
        colour = WHITE;
    } else {
        colour = BLACK;
    }

    for (i = 0; i < BOARD_SQUARE_SIZE; i++) {
        for (j = 0; j < BOARD_SQUARE_SIZE; j++) {
            rect.top = j + y * BOARD_SQUARE_SIZE;
            rect.bottom = j + y * BOARD_SQUARE_SIZE;
            rect.left = 29 - i + x * BOARD_SQUARE_SIZE;
            rect.right = 29 - i + x * BOARD_SQUARE_SIZE;

            if (!(vx[i][j / 8] & (1 << (j % 8)))) {
                fill_rectangle(rect, colour);
            }
        }
    }

}

void draw_piece_at_position(piece *piece) {
    switch (piece->type) {
        case 'r':
            draw_piece(piece->pos.x, piece->pos.y, rk, piece->player);
            break;
        case 'h':
            draw_piece(piece->pos.x, piece->pos.y, hs, piece->player);
            break;
        case 'b':
            draw_piece(piece->pos.x, piece->pos.y, bs, piece->player);
            break;
        case 'k':
            draw_piece(piece->pos.x, piece->pos.y, kg, piece->player);
            break;
        case 'q':
            draw_piece(piece->pos.x, piece->pos.y, qu, piece->player);
        case 'p':
            draw_piece(piece->pos.x, piece->pos.y, pw, piece->player);
            break;
    }
}

/*
*	Functions for the chess game itself
*/
void init_game() {


}

void perform_action() {
    if (selected_piece == NULL) {
        piece *piece_at = get_piece_at_position(current_cursor_position.x, current_cursor_position.y);
        if (piece_at != NULL) {
            previous_selected_position.x = current_cursor_position.x;
            previous_selected_position.y = current_cursor_position.y;

            piece_at->pos.x = -1;
            piece_at->pos.y = -1;
            selected_piece = piece_at;

        }
    } else {
        if (is_valid_move_position() && is_valid_move_pieces()) {
            selected_piece->pos.x = current_cursor_position.x;
            selected_piece->pos.y = current_cursor_position.y;
        } else {
            selected_piece->pos.x = previous_selected_position.x;
            selected_piece->pos.y = previous_selected_position.y;
        }

        draw_square(selected_piece->pos.x, selected_piece->pos.y);

        selected_piece = NULL;
        previous_selected_position.x = -1;
        previous_selected_position.y = -1;
    }


    draw_square(current_cursor_position.x, current_cursor_position.y);

}

piece *get_piece_at_position(int8_t x, int8_t y) {
    int8_t i;

    for (i = 0; i < 32; i++) {
        if (pieces[i].pos.x == x && pieces[i].pos.y == y) {
            return &pieces[i];
        }
    }

    return NULL;
}


// check if the piece is allowed to go on that position from a movement's point of view
bool is_valid_move_position() {
    if (selected_piece->pos.x == -1 && selected_piece->pos.y == -1) {
        switch (selected_piece->type) {
            case 'p':
                if (abs(previous_selected_position.x - current_cursor_position.x) <= 1) {
                    if (selected_piece->player == 0
                        && previous_selected_position.y == (current_cursor_position.y - 1)) {
                        return true;
                    } else if (selected_piece->player == 1
                               && previous_selected_position.y == (current_cursor_position.y + 1)) {
                        return true;
                    }
                }
                break;
            case 'r':
                if (!(previous_selected_position.x != current_cursor_position.x
                      && previous_selected_position.y != current_cursor_position.y)) {
                    return true;
                }
                break;
            case 'h':
                if (abs(previous_selected_position.x - current_cursor_position.x) == 2
                    && abs(previous_selected_position.y - current_cursor_position.y) == 1) {
                    return true;
                } else if (abs(previous_selected_position.x - current_cursor_position.x) == 1
                           && abs(previous_selected_position.y - current_cursor_position.y) == 2) {
                    return true;
                }
                break;
            case 'b':
                if (abs(previous_selected_position.x - current_cursor_position.x) ==
                    abs(previous_selected_position.y - current_cursor_position.y)) {
                    return true;
                }
                break;
            case 'k':
                if (abs(previous_selected_position.x - current_cursor_position.x) <= 1
                    && abs(previous_selected_position.y - current_cursor_position.y) <= 1) {
                    return true;
                }
                break;
            case 'q':
                if (!(previous_selected_position.x != current_cursor_position.x
                      && previous_selected_position.y != current_cursor_position.y)) {
                    return true;
                } else if (abs(previous_selected_position.x - current_cursor_position.x) ==
                           abs(previous_selected_position.y - current_cursor_position.y)) {
                    return true;
                }
                break;
        }
    }
    return false;
}

// checks if the piece is allowed to move on a particular position from a movement's point of view
bool is_valid_move_pieces() {

    piece *piece_at;
    uint8_t i;
    int8_t min, max;
    int8_t mins, maxs;
    int8_t step_x, step_y;
    switch (selected_piece->type) {
        case 'p':
            if (previous_selected_position.x == current_cursor_position.x) {
                piece_at = get_piece_at_position(current_cursor_position.x, current_cursor_position.y);
                if (piece_at != NULL) {
                    return false;
                }
            } else {
                piece_at = get_piece_at_position(current_cursor_position.x, current_cursor_position.y);
                if (piece_at == NULL || piece_at->player == selected_piece->player) {
                    return false;
                }
            }
            break;
        case 'r':
            if(previous_selected_position.x == current_cursor_position.x){
                if(previous_selected_position.y <  current_cursor_position.y){
                    min = previous_selected_position.y;
                    max = current_cursor_position.y;
                } else {
                    min = current_cursor_position.y;
                    max = previous_selected_position.y;
                }

                for(i = min; i < max; i++){
                    piece_at = get_piece_at_position(current_cursor_position.x, i);
                    if(piece_at != NULL){
                        return false;
                    }
                }
            } else {
                if(previous_selected_position.x <  current_cursor_position.x){
                    min = previous_selected_position.x;
                    max = current_cursor_position.x;
                } else {
                    min = current_cursor_position.x;
                    max = previous_selected_position.x;
                }

                for(i = min; i < max; i++){
                    piece_at = get_piece_at_position(i, current_cursor_position.y);
                    if(piece_at != NULL){
                        return false;
                    }
                }
            }

            piece_at = get_piece_at_position(current_cursor_position.x, current_cursor_position.y);
            if(piece_at != NULL && piece_at->player == selected_piece->player){
                return false;
            }

            break;
        case 'h':
        case 'k':
            piece_at = get_piece_at_position(current_cursor_position.x, current_cursor_position.y);
            if(piece_at != NULL && piece_at->player == selected_piece->player){
                return false;
            }
            break;
        case 'b':
            if(previous_selected_position.x <  current_cursor_position.x){
                step_x = 1;
            } else {
                step_x = -1;
            }

            if(previous_selected_position.y <  current_cursor_position.y){
                step_y = 1;
            } else {
                step_y = -1;
            }

            min = previous_selected_position.x;
            max = current_cursor_position.x;
            mins = previous_selected_position.y;
            maxs = current_cursor_position.y;

            i = 0;
            while(min != max && mins != maxs){

                piece_at = get_piece_at_position(min, mins);
                if(piece_at != NULL){
                    return false;
                }
                min = min + step_x;
                mins = mins + step_y;
            }

            piece_at = get_piece_at_position(current_cursor_position.x, current_cursor_position.y);
            if(piece_at != NULL && piece_at->player == selected_piece->player){
                return false;
            }

            break;
        case 'q':
            if(previous_selected_position.x == current_cursor_position.x){
                if(previous_selected_position.y <  current_cursor_position.y){
                    min = previous_selected_position.y;
                    max = current_cursor_position.y;
                } else {
                    min = current_cursor_position.y;
                    max = previous_selected_position.y;
                }

                for(i = min; i < max; i++){
                    piece_at = get_piece_at_position(current_cursor_position.x, i);
                    if(piece_at != NULL){
                        return false;
                    }
                }
            } else {
                if(previous_selected_position.x <  current_cursor_position.x){
                    min = previous_selected_position.x;
                    max = current_cursor_position.x;
                } else {
                    min = current_cursor_position.x;
                    max = previous_selected_position.x;
                }

                for(i = min; i < max; i++){
                    piece_at = get_piece_at_position(i, current_cursor_position.y);
                    if(piece_at != NULL){
                        return false;
                    }
                }
            }

            if(previous_selected_position.x <  current_cursor_position.x){
                step_x = 1;
            } else {
                step_x = -1;
            }

            if(previous_selected_position.y <  current_cursor_position.y){
                step_y = 1;
            } else {
                step_y = -1;
            }

            min = previous_selected_position.x;
            max = current_cursor_position.x;
            mins = previous_selected_position.y;
            maxs = current_cursor_position.y;

            i = 0;
            while(min != max && mins != maxs){

                piece_at = get_piece_at_position(min, mins);
                if(piece_at != NULL){
                    return false;
                }
                min = min + step_x;
                mins = mins + step_y;
            }

            piece_at = get_piece_at_position(current_cursor_position.x, current_cursor_position.y);
            if(piece_at != NULL && piece_at->player == selected_piece->player){
                return false;
            }

            break;
    }

    return true;
}


/*
*  Functions for handling the movement of a piece
*/
void move_cursor(int8_t x, int8_t y) {
    bool changed = false;

    position previous_pos = {
            current_cursor_position.x,
            current_cursor_position.y
    };

    if (previous_pos.x + x >= 0 && previous_pos.x + x < 8) {
        current_cursor_position.x = previous_pos.x + x;
        changed = true;
    }

    if (previous_pos.y + y >= 0 && previous_pos.y + y < 8) {
        current_cursor_position.y = previous_pos.y + y;
        changed = true;
    }

    if (changed) {
        draw_square(previous_pos.x, previous_pos.y);
        draw_square(current_cursor_position.x, current_cursor_position.y);
    }
}


int check_switches(int state) {

    if (get_switch_press(_BV(SWN))) {
        move_cursor(0, -1);
    }

    if (get_switch_press(_BV(SWE))) {
        move_cursor(1, 0);
    }

    if (get_switch_press(_BV(SWS))) {
        move_cursor(0, 1);
    }

    if (get_switch_press(_BV(SWW))) {
        move_cursor(-1, 0);
    }

    if (get_switch_press(_BV(SWC))) {
        perform_action();
    }

    return state;
}
