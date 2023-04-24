#ifndef keyboard_H
#define keyboard_H

#include "types.h"

extern char keyboardBuffer[3][128];
extern int kbdBufPos[3];
extern int kbdStart_x[3];
extern int kbdStart_y[3];
extern int kbdenable;

/*Initializes keyboard to recieve interrups*/
void keyboard_init();
/*Handles interrupts from keyboard*/
void keyboard_ir_handler();
/*Retuns key to be printed given code and state*/
char printKey(int code,int shift,int caps);
/*clears keyboard input buffer*/
void clear_kbdBuf(int terminal);

#endif
