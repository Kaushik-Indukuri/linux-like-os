#define ASM 1
#define RTC_LINK(name,func)   \
    .globl name                   ;\
    name:                         ;\
        call func                 ;\
        iret                      ;\



RTC_LINK(rtc_link ,rtc_ir_handler);
