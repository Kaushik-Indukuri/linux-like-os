/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

/* void i8259_init() 
 * 
 * Initializes 8259 PIC
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */
void i8259_init() 
{
  

    outb(0xFF, MASTER_8259_DATA); //mask all of 8259A-1 
    outb(0xFF, SLAVE_8259_DATA); //mask all of 8259A-2
  
    master_mask = inb(MASTER_8259_DATA);
    slave_mask = inb(SLAVE_8259_DATA);

    outb(ICW1, MASTER_8259_COMMAND); 
    outb(ICW1, SLAVE_8259_COMMAND); 

    outb(ICW2_MASTER, MASTER_8259_DATA);
    outb(ICW2_SLAVE, SLAVE_8259_DATA);

    outb(ICW3_MASTER, MASTER_8259_DATA); //ICW3 used because cascaded configuration is used
    outb(ICW3_SLAVE, SLAVE_8259_DATA);

    outb(ICW4, MASTER_8259_DATA);
    outb(ICW4, SLAVE_8259_DATA);

    outb(master_mask,MASTER_8259_DATA); //unmask part of 8259A-1
    outb(slave_mask,SLAVE_8259_DATA); //unmask part of 8259A-2
}

/* void enable_irq(uint32_t irq_num) 
 * 
 * Enable (unmask) the specified IRQ
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */
void enable_irq(uint32_t irq_num) 
{
    if(irq_num>15 || irq_num < 0)
    {
        return;
    }
    master_mask = inb(MASTER_8259_DATA); //Gets current mask
    slave_mask = inb(SLAVE_8259_DATA);
   if(irq_num<8)
   {
        master_mask &= ~(1<<irq_num);
        outb(master_mask,MASTER_8259_DATA); //If the mask is in previos 8 pushes master PIC
   }
   else
   {
        slave_mask &= ~(1<<(irq_num-8)); //If the mask is in previos top pushes slave PIC
        outb(slave_mask,SLAVE_8259_DATA);
   }        
}

/* void disable_irq(uint32_t irq_num) 
 * 
 * Disable (mask) the specified IRQ
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */
void disable_irq(uint32_t irq_num) 
{
    if(irq_num>15 || irq_num<0)
    {
        return;
    }
    master_mask = inb(MASTER_8259_DATA);
    slave_mask = inb(SLAVE_8259_DATA);
    if(irq_num<8)
    {
        master_mask |= (1<<irq_num);
        outb(master_mask,MASTER_8259_DATA); //If the mask is in previos 8 pushes master PIC
    }
    else
    {
        slave_mask |= (1<<(irq_num-8));
        outb(slave_mask,SLAVE_8259_DATA); //If the mask is in previos top pushes slave PIC
    }   
}

/* void send_eoi(uint32_t irq_num) 
 * 
 * Send end-of-interrupt signal for the specified IRQ
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */
void send_eoi(uint32_t irq_num) 
{
    if(irq_num>=8)
    {
        outb(EOI + irq_num,SLAVE_8259_COMMAND); //Ends EOI on respective PIC
    }
    outb(EOI + irq_num,MASTER_8259_COMMAND);
    //update slave eoi
}

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
