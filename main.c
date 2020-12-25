#include "stm8s_conf.h"
#include "bsp_uart2.h"
#include "bsp_time2.h"
#include "bsp_ADC.h"

#define m_current_base_value 0 //?


void user_GPIO_init(void)
{
   // init LED bar ctrl pin : GPIOC, GPIO_PIN_4
   GPIO_Init(ADC_OPT_BAR_LED_CMD_PORT, ADC_OPT_BAR_LED_CMD_PIN, GPIO_MODE_OUT_PP_HIGH_FAST);
}

void device_init(void)
{
  /*High speed internal clock prescaler: 1*/
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);  //main clock: 16M / 1 = 16M
  
  user_GPIO_init();     //GPIO init

  uart2_init();         //uart 1 init

  user_time2_init();    //time 2 init. after adc init
  user_ADC_init();      //ADC 1 init
  /* Enable general interrupts */  
  enableInterrupts();
}

void handle_lightbar_get_status(void)
{
  uint16_t currut_sample_value = get_time2_sample_adc_value();
  if(currut_sample_value == 0)
    return;
  
  printf_debug("currut_sample_value is %d\r\n", currut_sample_value);
}

void task_poll(void)
{
  handle_lightbar_get_status();
  handle_adc1_sample_poll();
}

void ADC_get_value_text()
{
  uint16_t a_val = get_adc_value();
  if(a_val == 0)
    return;
  
  //printf_debug("adc sample normal, get value is %d\r\n", a_val);
}


int main( void )
{
  device_init();


  while (1)
  {
    //task_poll();
    //ADC_get_value_text();
  }
}
