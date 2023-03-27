#include "i8259.h"
#include "lib.h"

/* void keyboard_init() 
 * 
 * Initializes keyboard to send interrupts
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */
void keyboard_init()
{
    enable_irq(1); //Initializes keyboard with irq 1 on primary pic
}

/* void keyboard_ir_handler()
 * 
 * Handles interrupts from the keyboard and prints appropriate key
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */
void keyboard_ir_handler()
{
    char keystroke;
    char kbdnumvals [] = {'1','2','3','4','5','6','7','8','9','0'};
    char kbdletvals_1 [] = {'q','w','e','r','t','y','u','i','o','p'};
    char kbdletvals_2 [] = {'a','s','d','f','g','h','j','k','l'};
    char kbdletvals_3 [] = {'z','x','c','v','b','n','m'};

    sti();
    keystroke = inb(EOI);
    if(keystroke>=0x02 && keystroke <= 0x0B)//0x02 - start num row, 0x0B - end num row
    {
        putc(kbdnumvals[keystroke-0x02]);//offset by 0x02 - start num row
    }
    else if(keystroke>=0x10 && keystroke <= 0x19)//0x10 - start let row1, 0x19 - end let row1
    {
        putc(kbdletvals_1[keystroke-0x10]);//offset by 0x10 - start let row1 
    }
    else if(keystroke>=0x1E && keystroke <= 0x26)//0x1E - start let row2, 0x26 - end let row2
    {
        putc(kbdletvals_2[keystroke-0x1E]);//offset by 0x1E - start let row2
    }
    else if(keystroke>=0x2C && keystroke <= 0x32)//0x2C - start let row3, 0x32 - end let row3
    {
        putc(kbdletvals_3[keystroke-0x2C]);//offset by 0x2C - start let row3
    }
    else if(keystroke == 0x39)//0x39 - value for spacebar
    {
        putc(' ');
    }
    cli();
    send_eoi(1); //ends eoi at start
}