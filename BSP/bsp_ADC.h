#ifndef __BSP_ADC_H__
#define __BSP_ADC_H__
#include "stm8s.h"

#define ADC_OPT_BAR_LED_CMD_PORT    GPIOC
#define ADC_OPT_BAR_LED_CMD_PIN     GPIO_PIN_2
#define _hal_set_high_opt_led_bar_cmd() GPIO_WriteHigh(ADC_OPT_BAR_LED_CMD_PORT,\
                                                   ADC_OPT_BAR_LED_CMD_PIN)

#define _hal_set_low_opt_led_bar_cmd()  GPIO_WriteLow(ADC_OPT_BAR_LED_CMD_PORT,\
                                                   ADC_OPT_BAR_LED_CMD_PIN)

void user_ADC_init(void);
void user_start_ADC1_single_sample(void);
INTERRUPT void ADC1_IRQHandler(void); 
uint16_t get_adc_value(void);


#endif //   __BSP_ADC_H__