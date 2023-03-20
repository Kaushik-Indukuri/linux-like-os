#ifndef IDTH
#define IDTH
#ifndef ASM
#include "lib.h"
#include "x86_desc.h"
#define numExceptions 21
#define syscall_vecnum (0x80)
void init_idt();
void exception_idt(int32_t num);
#endif
#endif
