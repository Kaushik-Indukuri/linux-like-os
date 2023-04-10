.text
    .globl syscall  
    syscall:        
        cmpl $0, %eax
        jbe syscallerror
        cmpl $10, %eax
        ja syscallerror                
        pushal
        pushfl
        pushl %edx
        pushl %ecx
        pushl %ebx
        sti
        call *syscalltable(,%eax,4)
        addl $12, %esp
        popfl
        popal
        jmp return

    syscallerror:
        movl $-1, %eax
    return:
        iret              

    syscalltable:
        .long 0x00000000
        .long halt, execute, read, write, open, close


