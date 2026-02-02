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

void tilegpu_draw(uint8_t x, uint8_t y, uint16_t tile_id);

#define TILEGPU_NO_FX       0
#define TILEGPU_AND_COLOR   1
#define TILEGPU_NAND_COLOR  2
#define TILEGPU_XOR_COLOR   3
#define TILEGPU_OR_COLOR    4
#define TILEGPU_NOR_COLOR   5
void tilegpu_fxdraw(uint8_t x, uint8_t y, uint16_t tile_id, uint8_t fx_op, uint16_t fx_imm);
void tilegpu_clearscreen(void);
void tilegpu_puts(uint8_t x, uint8_t y, const char *str);
void tilegpu_print(const char *str);