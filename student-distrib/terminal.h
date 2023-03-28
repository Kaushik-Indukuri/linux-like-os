#ifndef terminal_H
#define terminal_H

#include "types.h"

extern int termBufPos;


extern char termLineBuffer [128];
/*function to initialize terminal inputs and outputs*/
int terminal_open();
/*closes terminal for inputs and outputs*/
int terminal_close();
/*reads an input of n bytes to buf*/
int terminal_read(int32_t fd,char* buf,int n);
/*writes and input of n bytes to terminal*/
int terminal_write(int32_t fd,char* buf,int n);
/*moves cursor to x,y*/
void update_cursor(int x, int y);
/*scrolls terminal by one line*/
void terminal_scroll();
/*scrolls terminal screen by one line*/
void clear_termBuf();

#endif
