#ifndef EXCLINK
#define EXCLINK

void division_error();
void syscall();
void debug();
void non_maskable_interrupt();
void breakpoint();
void overflow();
void bound_range_exceeded();
void invalid_opcode();
void device_not_available();
void double_fault();
void coprocessor_segment_overrun();
void invalid_Tss();
void segment_not_present();
void stack_segment_fault();
void general_protection_fault();
void page_fault();
void x_floating_point_exception();
void alignment_check();
void machine_check();
void simd_floating_point_exception();
void virtualization_exception();
void control_protection_exception();

#endif
