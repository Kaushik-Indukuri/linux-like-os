.text
    .globl syscall  
    syscall:        
        cmpl $0, %eax
        jbe syscallerror
        cmpl $10, %eax
        ja syscallerror              
        pushl %edi
        pushl %esi
        pushl %ebp
        pushl %esp
        pushl %ebx
        pushl %edx
        pushl %ecx
        pushfl
        pushl %edx
        pushl %ecx
        pushl %ebx
        sti
        call *syscalltable(,%eax,4)
        addl $12, %esp
        popfl
        popl %ecx
        popl %edx
        popl %ebx
        popl %esp
        popl %ebp
        popl %esi
        popl %edi
        jmp return

    syscallerror:
        movl $-1, %eax
    return:
        iret          

    syscalltable:
        .long 0x00000000
        .long halt, execute, read, write, open, close


