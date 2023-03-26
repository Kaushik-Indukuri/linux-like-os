//rtc.h
#ifndef RTC_H
#define RTC_H

#include "types.h"



/*Initialize RTC*/
void rtc_init();
/*Handles interrupts from keyboard*/
void rtc_ir_handler();


//should block until the next interrupt, return 0 
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes);
//must be able to change frequency, return 0 or -1
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes);
//initializes RTC frequency to 2HZ, return 0
int32_t rtc_open(const uint8_t* filename);
//probably does nothing, unless you virtualize RTC, return 0
int32_t rtc_close(int32_t fd);

// Get rate given freq
int rtc_logbase2(int freq);

#endif
