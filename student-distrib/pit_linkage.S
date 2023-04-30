#define ASM 1
#define PIT_LINK(name,func)   \
    .globl name                   ;\
    name:                         ;\
        call func  ;\
        iret                      ;\



PIT_LINK(pit_link, pit_ir_handler);
