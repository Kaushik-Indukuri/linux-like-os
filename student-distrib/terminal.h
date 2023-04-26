#ifndef terminal_H
#define terminal_H

#include "types.h"

extern int termBufPos[3];
extern int curr_terminal;
extern int scheduled_terminal;
extern char termLineBuffer[3][128];
extern int terminal_pid[3];

/*function to initialize terminal inputs and outputs*/
int terminal_open(const uint8_t* filename);
/*closes terminal for inputs and outputs*/
int terminal_close(int32_t fd);
/*reads an input of n bytes to buf*/
int terminal_read(int32_t fd, void* buf, int32_t n);
/*writes and input of n bytes to terminal*/
int terminal_write(int32_t fd, const void* buf, int32_t n);
/*moves cursor to x,y*/
void update_cursor(int x, int y);
/*scrolls terminal by one line*/
void terminal_scroll();
/*scrolls terminal screen by one line*/
void clear_termBuf(int terminal);
/*switches active terminal*/
void terminal_switch(int destination);

#endif
