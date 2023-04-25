
#define lowmask 0x00FF
#define highmask 0xFF00
#define basefreq 1193182
#define channel0 0x40
#define command 0x43
#define pitmode 0x37

/*Function for initializing PIT interrupts at desired frequency*/
void init_pit();
/*Function that handles PIT interrupts*/
void pit_ir_handler();