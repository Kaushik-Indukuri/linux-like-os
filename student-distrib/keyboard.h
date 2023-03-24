#ifndef keyboard_H
#define keyboard_H

#include "types.h"

/*Initializes keyboard to recieve interrups*/
void keyboard_init();
/*Handles interrupts from keyboard*/
void keyboard_ir_handler();
/*Retuns key to be printed given code and state*/
char printKey(int code,int shift,int caps);

#endif
