#define ASM 1
#define syscall_vecnum (0x80)

#define EXCEPT_LINK(name,number)   \
    .globl name                   ;\
    name:                         ;\
        pushl $number             ;\
        call exception_idt        ;\
        addl $4, %esp             ;\
        iret                      ;\
# Declared umbrella function in idt.c which is called by every exception function
# and pushes a number associated with the exception to tell what the exception 
# handler has to do.

# Below, a linkage has been created with every exception and its associated idt num

EXCEPT_LINK(syscall, syscall_vecnum);

EXCEPT_LINK(division_error, 0);
EXCEPT_LINK(debug, 1);
EXCEPT_LINK(non_maskable_interrupt, 2);
EXCEPT_LINK(breakpoint, 3);
EXCEPT_LINK(overflow, 4);
EXCEPT_LINK(bound_range_exceeded, 5);
EXCEPT_LINK(invalid_opcode, 6);
EXCEPT_LINK(device_not_available, 7);
EXCEPT_LINK(double_fault, 8);
EXCEPT_LINK(coprocessor_segment_overrun, 9);
EXCEPT_LINK(invalid_Tss, 10);
EXCEPT_LINK(segment_not_present, 11);
EXCEPT_LINK(stack_segment_fault, 12);
EXCEPT_LINK(general_protection_fault, 13);
EXCEPT_LINK(page_fault, 14);

EXCEPT_LINK(x_floating_point_exception, 16);
EXCEPT_LINK(alignment_check, 17);
EXCEPT_LINK(machine_check, 18);
EXCEPT_LINK(simd_floating_point_exception, 19);
EXCEPT_LINK(virtualization_exception, 20);
EXCEPT_LINK(control_protection_exception, 21);

EXCEPT_LINK(hypervisor_injection_exception, 28);
EXCEPT_LINK(vmm_communication_exception, 29);
EXCEPT_LINK(security_exception, 30);
