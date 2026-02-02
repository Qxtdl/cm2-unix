#pragma once

#include <stdint.h>

#define TILEGPU_X           ((volatile uint8_t  *)0xFFFF)
#define TILEGPU_Y           ((volatile uint8_t  *)0xFFFE)
#define TILEGPU_ADDR        ((volatile uint16_t *)0xFFFC)
#define TILEGPU_FX_OPCODE   ((volatile uint8_t  *)0xFFFB)
#define TILEGPU_FX_IMM      ((volatile uint16_t *)0xFFFA)
#define TILEGPU_DRAW        1
#define TILEGPU_CLEAR       2
#define TILEGPU_CONTROLS    ((volatile uint8_t  *)0xFFF9)

// Draw a tile at the specified (x, y) coordinates with the given tile ID.
// Coordinates are in pixel units.
void tilegpu_draw(uint8_t x, uint8_t y, uint16_t tile_id)
{
    *TILEGPU_X = x;
    *TILEGPU_Y = y;
    *TILEGPU_ADDR = tile_id;
    *TILEGPU_CONTROLS = TILEGPU_DRAW;
}

// Draw a tile at the specified (x, y) coordinates with the given tile ID.
// Coordinates are in pixel units.
// Apply special effects based on fx_op and fx_imm parameters.

// fx opcodes
#define NO_FX       0
#define AND_COLOR   1
#define NAND_COLOR  2
#define XOR_COLOR   3
#define OR_COLOR    4
#define NOR_COLOR   5

void tilegpu_fxdraw(uint8_t x, uint8_t y, uint16_t tile_id, uint8_t fx_op, uint16_t fx_imm)
{
    *TILEGPU_FX_OPCODE = fx_op;
    *TILEGPU_FX_IMM = fx_imm;
    tilegpu_draw(x, y, tile_id);
}

// Clear the entire screen.
void tilegpu_clearscreen(void)
{
    *TILEGPU_CONTROLS = TILEGPU_CLEAR;
}

// Print a null-terminated string at the specified (x, y) coordinates.
void tilegpu_puts(uint8_t x, uint8_t y, const char *str)
{   
    uint8_t x2 = x, y2 = y;
    for (uint16_t i = 0; str[i] != '\0'; i++) 
    {
        if (str[i] == '\n') y2+=2;
        tilegpu_draw(x2++, y2, str[i]);
        if (x2 >= 48 || str[i] == '\n') 
        {
            x2 = 0;
            y2+=2;
        }
    }
}

void tilegpu_print(const char *str) {
    static uint8_t y = 0;
    tilegpu_puts(0, y+=2, str);
    if (y == 64) {
        y = 0;
        *TILEGPU_CONTROLS = TILEGPU_CLEAR;
    }
}
