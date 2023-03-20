#define ASM 1
#define syscall_vecnum (0x80)

#define EXCEPT_LINK(name,number)   \
    .globl name                   ;\
    name:                         ;\
        pushl $number             ;\
        pushfl                    ;\
        pushal                    ;\
        call  exception_idt       ;\
        popal                     ;\
        popfl                     ;\
        addl $4, %esp             ;\
        iret                      ;\



EXCEPT_LINK(division_error, 0);
EXCEPT_LINK(syscall, syscall_vecnum);
