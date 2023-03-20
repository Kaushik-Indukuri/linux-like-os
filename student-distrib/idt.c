#include "idt.h"
#include "exception_linkage.h"
#include "lib.h"
#include "x86_desc.h"
char exception_strings[numExceptions+1][30] = {
    "Division Error",
    "Debug",
    "Non-maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved",
    "x87 Floating-Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection Exception"
};

void init_idt()
{
    int i;
    for(i=0; i<256;i++)
    {
        idt[i].seg_selector = KERNEL_CS;
        idt[i].present = 1;
        idt[i].dpl = 0;
        idt[i].reserved0 = 0;
        idt[i].size = 1;
        idt[i].reserved1 = 1;
        idt[i].reserved2 = 1;
        idt[i].reserved3 = 0;
        idt[i].reserved4 = 0;
        if(i>31 || i==0x0F || i==0x09)
        {
            idt[i].present = 0; //For time being until we deal w keyboard intr
            //idt[i].reserved3 = 0;

        }
    }
    SET_IDT_ENTRY(idt[0], division_error);
    SET_IDT_ENTRY(idt[syscall_vecnum], syscall);
    idt[syscall_vecnum].present = 1;
    idt[syscall_vecnum].dpl = 0x3;
    // lidt(idt_desc_ptr);
}

void exception_idt(int32_t num) {
    clear();
    printf("Divide By Zero");
    while(1) {
    }
}
