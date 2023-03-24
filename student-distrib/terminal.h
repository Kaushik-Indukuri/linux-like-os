#ifndef terminal_H
#define terminal_H

#include "types.h"

void terminal_open();
void terminal_close();
void terminal_read();
void terminal_write(int32_t fd,char* buf,int n);
void update_cursor(int x, int y);

#endif
