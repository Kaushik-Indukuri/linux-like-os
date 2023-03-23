#include "idt.h"
#include "exception_linkage.h"
#include "lib.h"
#include "x86_desc.h"
#include "keyboard_linkage.h"
#include "rtc_linkage.h"
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
    "","","","","","","", //Gap is reserved
    "Control Protection Exception",
    "Hypervisor Injection Exception",
    "VMM Communication Exception"
};

/*
 * init_idt
 *   DESCRIPTION: Initializes idt array for indicies 0 to 255. Present=1 unless exception is reserved 
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Initializes all idts, set seg_selector = KERNEL_CS, make sure desired x80 is present
 */
void init_idt()
{
    int i;
    for(i=0; i<256;i++)
    {
        //Used docs to see value for each bit
        idt[i].seg_selector = KERNEL_CS;
        idt[i].present = 1;
        idt[i].dpl = 0;
        idt[i].reserved0 = 0;
        idt[i].size = 1;
        idt[i].reserved1 = 1;
        idt[i].reserved2 = 1;
        idt[i].reserved3 = 0;
        idt[i].reserved4 = 0;
        if(i>31 || i==0x0F || i==0x09) // If >31. If 0xF, reserved so not present. If 0x9 not needed anymore, not present
        {
            idt[i].present = 0; //For time being until we deal w keyboard intr
        }
    }
    //Initalize each function. Connects each function to idt table
    SET_IDT_ENTRY(idt[0], division_error);
    SET_IDT_ENTRY(idt[1], debug);
    SET_IDT_ENTRY(idt[2], non_maskable_interrupt);
    SET_IDT_ENTRY(idt[3], breakpoint);
    SET_IDT_ENTRY(idt[4], overflow);
    SET_IDT_ENTRY(idt[5], bound_range_exceeded);
    SET_IDT_ENTRY(idt[6], invalid_opcode);
    SET_IDT_ENTRY(idt[7], device_not_available);
    SET_IDT_ENTRY(idt[8], double_fault);
    SET_IDT_ENTRY(idt[9], coprocessor_segment_overrun);
    SET_IDT_ENTRY(idt[10], invalid_Tss);
    SET_IDT_ENTRY(idt[11], segment_not_present);
    SET_IDT_ENTRY(idt[12], stack_segment_fault);
    SET_IDT_ENTRY(idt[13], general_protection_fault);
    SET_IDT_ENTRY(idt[14], page_fault);
    SET_IDT_ENTRY(idt[16], x_floating_point_exception);
    SET_IDT_ENTRY(idt[17], alignment_check);
    SET_IDT_ENTRY(idt[18], machine_check);
    SET_IDT_ENTRY(idt[19], simd_floating_point_exception);
    SET_IDT_ENTRY(idt[20], virtualization_exception);
    SET_IDT_ENTRY(idt[21], control_protection_exception);

    SET_IDT_ENTRY(idt[28], hypervisor_injection_exception);
    SET_IDT_ENTRY(idt[29], vmm_communication_exception);
    SET_IDT_ENTRY(idt[30], security_exception);

    SET_IDT_ENTRY(idt[33], kbd_link);
    idt[33].reserved3 = 1;
    idt[33].present = 1;

    SET_IDT_ENTRY(idt[0x28], rtc_link);
    idt[0x28].reserved3 = 1;
    idt[0x28].present = 1;

    // Set x80 sys call to respective idt table number
    SET_IDT_ENTRY(idt[syscall_vecnum], syscall);
    idt[syscall_vecnum].present = 1;
    idt[syscall_vecnum].dpl = 0x3;

}

/*
 * exception_idt
 *   DESCRIPTION: if exception is caught, print error message
 *   INPUTS: int32_t num: exception number, to match to respective error message
 *   OUTPUTS: Prints error message to screen / blue screen of death
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Clear screen and add blue screen with error message. Hard codes error message to screen, need to close vm to restart os
 */
void exception_idt(int32_t num) {
    clear();
    if (num == 0x80) {
        printf("Syscall Exception");
    }
    else {
        printf("%s", exception_strings[num]); //Print respective string for each exception
    }
    while(1){
    
    }
}
