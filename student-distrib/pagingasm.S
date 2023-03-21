.text
.globl loadPageDirectory
.align 4
loadPageDirectory:
    pushl %ebp # stack build up
    movl %esp, %ebp
    movl 8(%esp), %eax # get param
    movl %eax, %cr3 # cr3<-eax , from osDev
    movl %ebp, %esp #stack teardown
    popl %ebp
    ret

.text
.globl enablePaging
enablePaging:
    movl %cr4, %eax # get param, cr4<-eax , from osDev
    orl $0x10, %eax # or eax with 0x10 to enable PSE/PCD
    movl %eax, %cr4 # get param, cr4<-eax

    movl %cr0, %eax # eax<-cr0
    orl $0x80000001, %eax # or eax with 0x01 to enable protection. 8 is for
    movl %eax, %cr0 # cr0<-eax , from osDev
    ret

# from descriptors.dvi page 6
# .align KB4
#    page_table:
#        .rept page_enteries
#        .long 0
#    .endr
# .align KB4
#    page_directory:
#        .rept page_enteries
#        .long 0
#    .endr


