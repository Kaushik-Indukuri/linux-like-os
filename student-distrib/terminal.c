#include "i8259.h"
#include "lib.h"
#include "keyboard.h"
#include "terminal.h"

char termLineBuffer [128];
int screenWidth = 80;//Screen width = 80 chars
int screenHeight = 25;//Screen height = 25 chars
int termBufPos;


/*
 * terminal_open
 *   DESCRIPTION: Initalized Terminal/ opens it
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: 0 if success
 *   SIDE EFFECTS: Open terminal for input
 */
 
int terminal_open(const uint8_t* filename)
{
    clear();
    screen_x = 0;
    screen_y = 0;
    update_cursor(screen_x,screen_y);
    clear_termBuf();
    clear_kbdBuf();
    keyboard_init();
    return 0;
}
/*
 * terminal_close
 *   DESCRIPTION: Closes Terminal
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: 0 if success
 *   SIDE EFFECTS: Closes terminal for input
 */
int terminal_close(int32_t fd)
{
    update_cursor(screen_x,screen_y);
    clear_termBuf();
    clear_kbdBuf();
    return 0;
}
/*
 * terminal_read
 *   DESCRIPTION: Take in input from terminal when enter is pressed
 *   INPUTS: int32_t fd: file directory of curr file ,char* buf: Where string is outputted to, int n:num bytes to be read
 *   OUTPUTS: none
 *   RETURN VALUE: num bytes read
 *   SIDE EFFECTS: none
 */
int terminal_read(int32_t fd, void* buf, int32_t n)
{
    //char * buf2 = (char *)buf;
    while(termLineBuffer[termBufPos] != '\n'); //Waits until new line char is recieved
    termLineBuffer[termBufPos] = 0x00;
    int i;
    cli();
    for(i=0;i<n;i++)
    {
        ((char*)buf)[i] = termLineBuffer[i]; //Copies keyboard buffer to buf
    }
    clear_termBuf();
    sti();
    return n;
}
/*
 * terminal_write
 *   DESCRIPTION: Write input from  buf to terminal
 *   INPUTS: int32_t fd: file directory of curr file ,char* buf: String to be outputted, int n:num bytes to be written
 *   OUTPUTS: none
 *   RETURN VALUE: num bytes written
 *   SIDE EFFECTS: none
 */
int terminal_write(int32_t fd, const void* buf, int32_t n)
{
    int i;
    for(i = 0;i<n;i++) //Writes all chars to terminal
    {
        if(((char*)buf)[i] != 0x00)
        {
            if(screen_x == (screenWidth-1)) //goes to newline if char is at the end
            {
                if( ((char*)buf)[i] == '\t')
                {
                    terminal_write(2,"    ",4); //Prints 4 spaces for tab
                }
                else
                {
                    putc( ((char*)buf)[i]);
                }
                screen_y++;
                if(screen_y == screenHeight)
                {
                    terminal_scroll(); //Scrolls to newline if input is at bottom
                    screen_y=screenHeight-1;
                    kbdStart_y--;
                }
                screen_x = 0;   
            }
            else
            {
                if( ((char*)buf)[i] == '\t')
                {
                    terminal_write(2,"    ",4); //Prints 4 spaces for tab
                }
                else
                {
                    putc( ((char*)buf)[i]);
                }
                if(screen_y == screenHeight)
                {
                    terminal_scroll();
                    screen_y=screenHeight-1;
                }
            }
        }
        update_cursor(screen_x,screen_y);
    }
    return n; //Returns number of chars written
}
/*
 * update_cursor
 *   DESCRIPTION: Updates x and y pos of cursor
 *   INPUTS: int x: x pos, int y:pos y
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void update_cursor(int x, int y)
{
	uint16_t pos = y * screenWidth + x;
	outb(0x0F,0x3D4);
	outb((uint8_t) (pos & 0xFF),0x3D5);
	outb(0x0E,0x3D4);
	outb((uint8_t) ((pos >> 8) & 0xFF),0x3D5);
}
/*
 * terminal_scroll
 *   DESCRIPTION: Scrolls down one line
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: moves terminal screen down one
 */
void terminal_scroll()
{
    int i;
    for(i = 0;i<(screenWidth*2*(screenHeight-1));i++)
    {
        *(video_mem+i) = *(video_mem+i+(screenWidth*2));
    }
    for(i = (screenWidth*2*(screenHeight-1));i<(screenWidth*2*(screenHeight));i++)
    {
        *(video_mem+i) = 0x00;
    }
}
/*
 * clear_termBuf
 *   DESCRIPTION: Clear terminal buffer
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void clear_termBuf()
{
    int i;
    for(i = 0;i<128;i++)
    {
        termLineBuffer[i] = 0x00;
    }
}
