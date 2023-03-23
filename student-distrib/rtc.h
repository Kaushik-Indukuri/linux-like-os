//rtc.h
#ifndef RTC_H
#define RTC_H

#include "types.h"



/*Initialize RTC*/
void rtc_init();
/*Handles interrupts from keyboard*/
void rtc_ir_handler();

#endif
