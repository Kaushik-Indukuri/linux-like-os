#include "pit.h"
#include "terminal.h"
#include "i8259.h"
#include "lib.h"
#include "idt.h"
#include "paging.h"
#include "file_system.h"
#include "syscall.h"


#define user_program_start  0x48000
#define KERNEL_DS   0x0018
#define MB_8 (1<<23) // 8 MB
#define KB_8 (1<<13) // 8 KB
#define MB_4 (1<<22) // 4 MB
#define VIDEO       0xB8000

int int_freq = 20;/*18.2-1193182 Hz*/


void init_pit() 
{
    outb(pitmode, pit_command);
    outb(((basefreq/int_freq)&lowmask),channel0);
    outb(((basefreq/int_freq)&highmask)>>8,channel0); //Shift upper bits to lower bits (8 places)
    enable_irq(0);
}

// 
void pit_ir_handler() 
{
    // terminal_write(2,"hi",2);
    schedule();
    send_eoi(0);
}

void schedule() {
    // update_videomem(VIDEO+scheduled_terminal);
    int pid = (int)terminal_pid[scheduled_terminal%3]; // added mod 3q
    // if (pid != -1)
    // {
    //     video_mem = VIDEO+KB4*(curr_terminal+1);
    // } 
    if(pcb_ptr != 0)
    {
        register uint32_t ebpx asm("ebp");
        register uint32_t espx asm("esp");
        pcb_ptr->cur_esp = espx;
        pcb_ptr->cur_ebp = ebpx;
        // pcb_ptr->cur_tss = tss.esp0; // dont neeed
    }    
    if (pid == -1) 
    {
        terminal_shells[curr_terminal]++;
        terminal_switch((scheduled_terminal)%3);
        send_eoi(0);
        execute((uint8_t*) "shell");
    }

    scheduled_terminal++; // make unsigned???
    flushtlb();
    
    // pid = terminal_pid[scheduled_terminal];
    // pcb_ptr = pcb_array + pid;
    // // page_directory[32].addrlong = ((MB_8 + MB_4*pid) + user_program_start)>>22; //Add offset, removed 4mB aligb
    // page_directory[32].addrlong = (MB_8 + MB_4*pid) + user_program_start; //Add offset, removed 4mB aligb
    // asm volatile(" \n\
    //     movl %%cr3, %%eax \n\
    //     movl %%eax, %%cr3 \n\
    //     "
    //     :
    //     :
    //     : "memory"
    // );
    // uint32_t esp = pcb_ptr->cur_esp;
    // uint32_t ebp = pcb_ptr->cur_ebp;


    // tss.ss0 = KERNEL_DS; // dont neeed
    // tss.esp0 = pcb_ptr->cur_tss;
    // send_eoi(0);
    // asm volatile(" \n\
    //     movl %%eax, %%esp \n\
    //     movl %%ebx, %%ebp   \n\
    //     "
    //     :
    //     :"a"(esp),"b"(ebp)
    //     : "memory"
    // );
//    removed leave and ret after movls
}
