#ifndef __BSP_TIME2_H__
#define __BSP_TIME2_H__
#include "stm8s_conf.h"

//TIM2 counter clock = TIME CLOCK / TIME2_PRESCALER = 16M /1 = 16M
#define TIME2_PRESCALER             TIM2_PRESCALER_1         //定时器全速跑
#define TIME2_Autoreload_VALUE      160                      //interrupt trigger time:ARR/TIME2 CLOCK = 160/16M = 10us
#define TIME2_IT_TYPE               TIM2_IT_UPDATE

void user_time2_init(void);
INTERRUPT void TIM2_UPD_OVF_BRK_IRQHandler(void);
void handle_adc1_sample_poll(void);
uint16_t get_time2_sample_adc_value(void);
uint32_t get_current_time(void);
void get_IR_adc_value(void);

#endif  // __BSP_TIME2_H__

