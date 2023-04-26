#include "i8259.h"
#include "lib.h"
#include "keyboard.h"
#include "terminal.h"
#include "syscall.h"

int screenWidth = 80;//Screen width = 80 chars
int screenHeight = 25;//Screen height = 25 chars
int saved_x[3] = {0,0,0};
int saved_y[3] = {0,0,0};
int terminal_pid[3] = {-1,-1,-1};

char termLineBuffer[3][128];
int termBufPos[3];
int curr_terminal = 0;
int scheduled_terminal = 0;
#define vidstart 0xB9000
#define KB4 4096

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
    int i;
    clear();
    screen_x = 0;
    screen_y = 0;
    for(i=0;i<3;i++)
    {
        clear_termBuf(i);
        clear_kbdBuf(i);
    }
    update_cursor(screen_x,screen_y);
    keyboard_init();
        vidmap(0);
        //     vidmap(vidstart+KB4);
        //         vidmap(vidstart+KB4*2);
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
    int i;
    update_cursor(screen_x,screen_y);
    for(i=0;i<3;i++)
    {
        clear_termBuf(i);
        clear_kbdBuf(i);
    }
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
    while(termLineBuffer[curr_terminal][termBufPos[curr_terminal]] != '\n'); //Waits until new line char is recieved
    termLineBuffer[curr_terminal][termBufPos[curr_terminal]] = 0x00;
    int i;
    cli();
    for(i=0;i<n;i++)
    {
        ((char*)buf)[i] = termLineBuffer[curr_terminal][i]; //Copies keyboard buffer to buf
    }
    clear_termBuf(curr_terminal);
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
                if(((char*)buf)[i] == '\t')
                {
                    terminal_write(2,"    ",4); //Prints 4 spaces for tab
                }
                else
                {
                    putc(((char*)buf)[i]);
                }
                if(((char*)buf)[i] != '\n')
                {
                    screen_y++;
                }
            
                if(screen_y == screenHeight)
                {
                    terminal_scroll(); //Scrolls to newline if input is at bottom
                    screen_y=24;
                    kbdStart_y[curr_terminal]--;
                }
                screen_x = 0;   
            }
            else
            {
                if(((char*)buf)[i] == '\t')
                {
                    terminal_write(2,"    ",4); //Prints 4 spaces for tab
                }
                else
                {
                    putc(((char*)buf)[i]);
                }
                if(screen_y == screenHeight)
                {
                    terminal_scroll();
                    screen_y=24;
                    screen_x=0;
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
void clear_termBuf(int terminal)
{
    int i;
    for(i = 0;i<128;i++)
    {
        termLineBuffer[terminal][i] = 0x00;
    }
}


void terminal_switch(int destination)
{
    if(destination>2||destination<0||destination==curr_terminal)
    {
        return;
    }
    saved_x[curr_terminal] = screen_x;
    saved_y[curr_terminal] = screen_y;
    screen_x = saved_x[destination];
    screen_y = saved_y[destination];
    update_cursor(screen_x,screen_y);
    memcpy(vidstart+curr_terminal*KB4,video_mem,4096);
    memcpy(video_mem,vidstart+destination*KB4,4096);
    flushtlb();
    curr_terminal = destination;
}
