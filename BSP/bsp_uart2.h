#ifndef __BSP_UART2_H__
#define __BSP_UART2_H__
#include "stdio.h"
void uart2_init(void);

#if defined(UART2_DEBUG)&&(UART2_DEBUG == ENABLE)
//#define printf_debug(_mod_name_, _fmt_, ...)    printf("[%s] " _fmt_ "\r\n", _mod_name_, ##__VA_ARGS__)
#define printf_debug(fmt, args...)    printf(fmt, ##args)
#else 
#define printf_debug
#endif  //end of UART2_DEBUG

#endif /* __BSP_UART2_H__*/
