#include "i8259.h"
#include "lib.h"
#include "keyboard.h"
#include "terminal.h"

char termLineBuffer [128] = {'1','2','3'};
int cursorPos = 0;
int i;

void terminal_open()
{
    clear();
    cursorPos = 0;
    update_cursor(0,cursorPos);
    screen_x = 0;
    screen_y = 0;
    for(i = 0;i<128;i++)
    {
        termLineBuffer[i] = 0x00;
    }
}

void terminal_close()
{

}

void terminal_read()
{

}

void terminal_write(int32_t fd,char* buf,int n)
{
    if(n+screen_x > 128)
    {

    }
    else
    {
        for(i = 0;i<n;i++)
        {
            termLineBuffer[i+screen_x] = buf[i];
        }
        printf("%s",termLineBuffer);
        cursorPos+=n;
    }
}

void update_cursor(int x, int y)
{
	uint16_t pos = y * 128 + x;
	outb(0x0F,0x3D4);
	outb((uint8_t) (pos & 0xFF),0x3D5);
	outb(0x0E,0x3D4);
	outb((uint8_t) ((pos >> 8) & 0xFF),0x3D5);
}