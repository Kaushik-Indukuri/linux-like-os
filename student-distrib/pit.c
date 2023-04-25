#include "pit.h"
#include "terminal.h"
#include "i8259.h"
#include "lib.h"

int int_freq = 20;/*18.2-1193182 Hz*/

void init_pit() 
{
    outb(pitmode, command);
    outb(((basefreq/int_freq)&lowmask),channel0);
    outb(((basefreq/int_freq)&highmask)>>8,channel0); //Shift upper bits to lower bits (8 places)
    enable_irq(0);
}

void pit_ir_handler() 
{

    //terminal_write(2,"hi",2);
    send_eoi(0);
}