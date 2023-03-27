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
#define standardFreq 2

volatile int rtc_interrupt_flag=0;



/*
 * rtc_init
 *   DESCRIPTION: Initializes rtc, enables interrupts, 
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Initializes   *********************FINISH THIS
 */

/*Initialize RTC*/
void rtc_init(void)
{
    enable_irq(8); //secondary pic to enable rtc interrupt
    cli();
    //outportb(0x70, 0x8A); //Disable NMIs on reg A
    //outportb(0x71, 0x20);
    outb(cmosREGB,rtcIO); //Disable NMIs on reg B
    char prev=inb(cmosIO); //Read current value of reg B
    outb(cmosREGB,rtcIO);		// set the index again (a read will reset the index to register D)
    outb(prev | 0x40,cmosIO);	// write the previous value ORed with 0x40. This turns on bit 6 of register B
    sti();
}

/*
 * rtc_ir_handler
 *   DESCRIPTION: Handles interrupts from keyboard 
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Initializes etc, sets interruput flag to 1
 */

void rtc_ir_handler()
{
    outb(0x0C,rtcIO);	// select register C
    inb(cmosIO);
    //test_interrupts();
    send_eoi(8);
    rtc_interrupt_flag=1;
}


/*
 * rtc_open
 *   DESCRIPTION: Initializes rtc, enables interrupts, sets freq to 2Hz using
            helper function to get rate. Then sets rate of rtc. From osdev
 *   INPUTS: const uint8_t* filename, name of file being modified
 *   OUTPUTS: none
 *   RETURN VALUE: 0 if success, -1 if fail
 *   SIDE EFFECTS: Set freq to 2 Hz
 */
int32_t rtc_open(const uint8_t* filename)
{
    int rate=rtc_logbase2(2); //2 is base freq
    if(rate==-1)
    {
        return rate;
    }
    rate &=0x0F;			// rate must be above 2 and not over 15
    cli();
    outb(0x8A,0x70);		// set index to register A, disable NMI
    char prev=inb(0x71);	// get initial value of register A
    outb(0x8A,0x70);		// reset index to A
    outb((prev & 0xF0) | rate, 0x71); //write only our rate to A. Note, rate is the bottom 4 bits.
    sti();
    return 0;
}

/*
 * rtc_close
 *   DESCRIPTION: Does nothing as RTC not virtualized
 *   INPUTS: int32_t fd, file directory of current file
 *   OUTPUTS: none
 *   RETURN VALUE: 0 always
 *   SIDE EFFECTS: none
 */
int32_t rtc_close(int32_t fd)
{
    return 0;
}

/*
 * rtc_read
 *   DESCRIPTION:  should block until the next interrupt
 *   INPUTS: int32_t fd, file directory of current file, void* buf buffer, buffer
                        with data about file, int32_t nbytes, num bytes
 *   OUTPUTS: none
 *   RETURN VALUE: 0 always
 *   SIDE EFFECTS: blocks interrupts until another recieved
 */
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes)
{

    rtc_interrupt_flag=0;
    while(rtc_interrupt_flag==0)
    {
        //while no interrupt recieved, keep reading
    }
    return 0;
}


/*
 * rtc_write
 *   DESCRIPTION: change frequency to desired one. Uses helper fxn to get rate
 *   INPUTS: int32_t fd, file directory of current file, void* buf buffer, buffer
                        with data about file, int32_t nbytes, num bytes
 *   OUTPUTS: none
 *   RETURN VALUE: 0 if success, -1 if fail
 *   SIDE EFFECTS: Set freq to desired
 */
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes)
{
    if(buf==NULL)
    {
        return -1;
    }
    int rate=rtc_logbase2( *(int *)buf);
    if(rate==-1)
    {
        return rate;
    }
    rate &=0x0F;			// rate must be above 2 and not over 15
    cli();
    outb(0x8A,0x70);		// set index to register A, disable NMI
    char prev=inb(0x71);	// get initial value of register A
    outb(0x8A,0x70);		// reset index to A
    outb((prev & 0xF0) | rate, 0x71); //write only our rate to A. Note, rate is the bottom 4 bits.
    sti();
    return 0;
}

/*
 * rtc_logbase2
 *   DESCRIPTION: Get rate for rtc from given freq
 *   INPUTS: int freq, desired freq
 *   OUTPUTS: none
 *   RETURN VALUE: rate used for pic
 *   SIDE EFFECTS: none
 */
int rtc_logbase2(int freq)
{
    //freq=2^15>>(rate-1)
    //freq=2^(15-rate)
    //15-log2(freq)=rate
    int temp=2;
    int i=1;
    for(i=1;i<17;i++) //up to 16 iterations, since #16 means not found
    {
        if(temp==freq)
        {
            break;
        }
        temp*=2;
    }
    if(i==16) // if 16 not mult of 2
    {
        return -1;
    }
    return (15-i+1); //from os dev formula

}
