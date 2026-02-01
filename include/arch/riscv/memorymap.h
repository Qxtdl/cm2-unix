#ifndef MEMORYMAP_H
#define MEMORYMAP_H

// Note that the post interface is just a indicator to the user,
// you can decide how to encode the fail cause.
#define POST_SUCCESS 2
#define POST_FAILURE 1
#define POST_RESULT     ((volatile bool *)0xFFF8) // Power-On Self Test Result (0 = pass, 1 = fail)
#define POST_FAIL_CAUSE ((volatile u8   *)0xFFF7) // Power-On Self Test Fail Cause

// TTY Console
#define TTY_LOC     ((volatile char *)0xFFF6) // TTY Console Location
#define TTY_CHAR    ((volatile char *)0xFFF5) // TTY Console Location
#define TTY_WRITE   ((volatile bool *)0xFFF4) // Write to TTY Console
#define TTY_CLEAR   ((volatile bool *)0xFFF3) // Clear TTY Console
#define clear_tty *TTY_CLEAR = true; *TTY_LOC = 0;

// User Input
#define USER_READY  ((volatile bool *)0xFFF2) // User Input Ready Flag !!! READING RESETS THIS TO 0 IF IT'S 1 !!!
#define USER_ASCII  ((volatile char *)0xFFF1) // User Input ASCII Character

// Memory Mapped Math Units
#define MULT_X      ((volatile u32 *)0xFFF0) // Divider X Input
#define MULT_Y      ((volatile u32 *)0xFFEC) // Divider Y Input
#define MULT_LO     ((volatile u32 *)0xFFE8) // Divider Result Output LOW 32 bits
#define MULT_HI     ((volatile u32 *)0xFFE4) // Divider Result Output HI 32 bits

#define DIV_X      ((volatile u32 *)0xFFE0) // Divider X Input
#define DIV_Y      ((volatile u32 *)0xFFDC) // Divider Y Input
#define DIV_QUO    ((volatile u32 *)0xFFD8) // Divider Quotient Output
#define DIV_REM    ((volatile u32 *)0xFFD4) // Divider Remainder Output

#define RANDOM_BYTE ((volatile u8  *)0xFFD3) // RNG 0-255

#define RGB_X ((volatile u8 *)0xFFD2) 
#define RGB_Y ((volatile u8 *)0xFFD1) 
#define RGB_R ((volatile u8 *)0xFFD0) 
#define RGB_G ((volatile u8 *)0xFFCF) 
#define RGB_B ((volatile u8 *)0xFFCE) 
#define RGB_CLEARSCREEN ((volatile u8 *)0xFFCD) 
#define RGB_WRITE ((volatile u8 *)0xFFCC)


#define LED_LIGHT ((volatile u8 *)0xFFCB)

#define DISK_ADDRESS ((volatile u16 *)0xFFC8)
#define DISK_DATA ((volatile u8 *)0xFFC6)
#define DISK_OUT ((volatile u8 *)0xFFC4)
#define DISK_WRITE ((volatile bool *)0xFFC3)

#endif // MEMORYMAP_H