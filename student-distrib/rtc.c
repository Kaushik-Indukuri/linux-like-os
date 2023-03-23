//rtc.c
#include "rtc.h"
#include "lib.h"
#include "i8259.h"
#include "lib.h"

#define rtcIO 0x70
#define cmosIO 0x71

#define cmosREGA 0x8A
#define cmosREGB 0x8B
#define cmosREGC 0x8C


/*Initialize RTC*/
void rtc_init(void)
{
    enable_irq(8);
    cli();
    //outportb(0x70, 0x8A); //Disable NMIs on reg A
    //outportb(0x71, 0x20);
    outb(0x8B,0x70); //Disable NMIs on reg B
    char prev=inb(0x71); //Read current value of reg B
    outb(0x8B,0x70);		// set the index again (a read will reset the index to register D)
    outb(prev | 0x40,0x71);	// write the previous value ORed with 0x40. This turns on bit 6 of register B
    sti();
}

/*Handles interrupts from keyboard*/
void rtc_ir_handler()
{
    outb(0x0C,0x70);	// select register C
    inb(0x71);
    //test_interrupts();
    send_eoi(8);
}
