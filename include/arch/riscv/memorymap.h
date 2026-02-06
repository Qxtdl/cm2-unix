#pragma once

#include <stdint.h>

// POST
#define POST_SUCCESS 2
#define POST_FAILURE 1
#define POST_RESULT     ((volatile uint8_t *)0xFFF8)    // Result (0 = pass, 1 = fail)
#define POST_FAIL_CAUSE ((volatile uint8_t   *)0xFFF7)  // Encoded Fail Cause

// TTY Console
#define TTY_LOC     ((volatile char *)0xFFF6)           // TTY Console Location
#define TTY_CHAR    ((volatile char *)0xFFF5)           // TTY Console Location
#define TTY_WRITE   ((volatile uint8_t *)0xFFF4)        // Write to TTY Console
#define TTY_CLEAR   ((volatile uint8_t *)0xFFF3)        // Clear TTY Console
#define clear_tty *TTY_CLEAR = true; *TTY_LOC = 0;

// User Input
#define USER_READY  ((volatile uint8_t *)0xFFF2)        // User Input Ready Flag
#define USER_ASCII  ((volatile char *)0xFFF1)           // User Input ASCII Character

// (NOT IMPLEMENTED) Memory Mapped Math Units
#define MULT_X      ((volatile uint32_t *)0xFFF0)       // Divider X Input
#define MULT_Y      ((volatile uint32_t *)0xFFEC)       // Divider Y Input
#define MULT_LO     ((volatile uint32_t *)0xFFE8)       // Divider Result Output LOW 32 bits
#define MULT_HI     ((volatile uint32_t *)0xFFE4)       // Divider Result Output HI 32 bits

#define DIV_X      ((volatile uint32_t *)0xFFE0)        // Divider X Input
#define DIV_Y      ((volatile uint32_t *)0xFFDC)        // Divider Y Input
#define DIV_QUO    ((volatile uint32_t *)0xFFD8)        // Divider Quotient Output
#define DIV_REM    ((volatile uint32_t *)0xFFD4)        // Divider Remainder Output

// Random Byte
#define RANDOM_BYTE ((volatile uint8_t  *)0xFFD3)       // RNG 0-255

// Single Large RGB Display
#define RGB_X ((volatile uint8_t *)0xFFD2) 
#define RGB_Y ((volatile uint8_t *)0xFFD1) 
#define RGB_R ((volatile uint8_t *)0xFFD0) 
#define RGB_G ((volatile uint8_t *)0xFFCF) 
#define RGB_B ((volatile uint8_t *)0xFFCE) 
#define RGB_CLEARSCREEN ((volatile uint8_t *)0xFFCD) 
#define RGB_WRITE ((volatile uint8_t *)0xFFCC)

// Led Light
#define LED_LIGHT ((volatile uint8_t *)0xFFCB)

// Disk
#define DISK_ADDRESS ((volatile u16 *)0xFFC8)
#define DISK_DATA ((volatile uint8_t *)0xFFC6)
#define DISK_OUT ((volatile uint8_t *)0xFFC4)
#define DISK_WRITE ((volatile uint8_t *)0xFFC3)
