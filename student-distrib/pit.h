#ifndef PIT_H
#define PIT_H
#define lowmask 0x00FF
#define highmask 0xFF00
#define basefreq 1193182
#define channel0 0x40
#define pit_command 0x43
#define pitmode 0x37


/*Function for initializing PIT interrupts at desired frequency*/
void init_pit();
/*Function that handles PIT interrupts*/
void pit_ir_handler();

void schedule();


#endif


