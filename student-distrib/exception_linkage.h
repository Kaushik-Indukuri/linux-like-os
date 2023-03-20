#ifndef EXCLINK
#define EXCLINK

//#include "idt.h"
void division_error();
void syscall();
    //void debug()
    // non_maskable_interrupt
    // breakpoint
    // overflow
    // bound_range_exceeded
    // invalid_opcode
    // device_not_available
    // double_fault
    // coprocessor_segment_overrun
    // invalid_Tss
    // segment_not_present
    // stack_segment_fault
    // general_protection_fault
    //void page_fault();
    // Reserved
    // x87_floating_point_Exception
    // Alignment_Check
    // Machine_Check
    // SIMD_floating_point_Exception
    // Virtualization_Exception
    // Control_Protection_Exception
#endif
