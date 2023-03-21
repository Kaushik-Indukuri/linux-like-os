#ifndef IDTH
#define IDTH
#ifndef ASM
#include "lib.h"
#include "x86_desc.h"
#define numExceptions 30 //Total possible exceptions. From 
#define syscall_vecnum (0x80)

//Initializes idt array for indicies 0 to 255. Present=1 unless exception is reserved 
void init_idt();
//if exception is caught, print error message
void exception_idt(int32_t num);
#endif
#endif
