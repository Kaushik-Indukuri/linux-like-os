#define ASM 1
#define KEYBOARD_LINK(name,func)   \
    .globl name                   ;\
    name:                         ;\
        call func  ;\
        iret                      ;\



KEYBOARD_LINK(kbd_link ,keyboard_ir_handler);
