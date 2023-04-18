#ifndef keyboard_H
#define keyboard_H

#include "types.h"

extern char keyboardBuffer [128];
extern int kbdBufPos;
extern int kbdStart_x;
extern int kbdStart_y;
extern int kbdenable;

/*Initializes keyboard to recieve interrups*/
void keyboard_init();
/*Handles interrupts from keyboard*/
void keyboard_ir_handler();
/*Retuns key to be printed given code and state*/
char printKey(int code,int shift,int caps);
/*clears keyboard input buffer*/
void clear_kbdBuf();

#endif
