#include "pit.h"
#include "terminal.h"
#include "i8259.h"
#include "lib.h"
#include "idt.h"
#include "paging.h"
#include "file_system.h"
#include "syscall.h"

#define KERNEL_DS   0x0018
#define MB_8 (1<<23) // 8 MB
#define KB_8 (1<<13) // 8 KB
#define MB_4 (1<<22) // 4 MB

int int_freq = 20;/*18.2-1193182 Hz*/


void init_pit() 
{
    outb(pitmode, pit_command);
    outb(((basefreq/int_freq)&lowmask),channel0);
    outb(((basefreq/int_freq)&highmask)>>8,channel0); //Shift upper bits to lower bits (8 places)
    enable_irq(0);
}

void pit_ir_handler() 
{

    //terminal_write(2,"hi",2);
    schedule();
    send_eoi(0);
}

void schedule() {
    int pid = (int)terminal_pid[scheduled_terminal];
    if (pid != -1) {
        register uint32_t ebp asm("ebp");
        register uint32_t esp asm("esp");
        pcb_ptr->cur_esp = esp;
        pcb_ptr->cur_ebp = ebp;
        pcb_ptr->cur_tss = tss.esp0;
    }
    scheduled_terminal++;
    if (scheduled_terminal == 3) {
        scheduled_terminal = 0;
    } 
    if (pid == -1) {
        execute((uint8_t*)"shell");
    }
    pid = terminal_pid[scheduled_terminal];
    pcb_ptr = pcb_array + pid;
    page_directory[32].addrlong = (MB_8 + MB_4*pid) / KB4; //32nd index
    flushtlb();
    uint32_t esp = pcb_ptr->cur_esp;
    uint32_t ebp = pcb_ptr->cur_ebp;

    tss.ss0 = KERNEL_DS;
    tss.esp0 = pcb_ptr->cur_tss;
    asm volatile(" \n\
        movl %%eax, %%esp \n\
        movl %%ebx, %%ebp \n\
        leave   \n\
        ret    \n\
        "
        :
        :"a"(esp),"b"(ebp)
        : "memory"
    );
}
