#include "stm8s_conf.h"
#include "bsp_uart2.h"
#include "bsp_time2.h"
#include "bsp_ADC.h"
#include "bsp_task.h"

#define m_current_base_value 0 //?


void user_GPIO_init(void)
{
   // init LED bar ctrl pin : GPIOC, GPIO_PIN_4
   GPIO_Init(ADC_OPT_BAR_LED_CMD_PORT, ADC_OPT_BAR_LED_CMD_PIN, GPIO_MODE_OUT_PP_HIGH_FAST);
   GPIO_Init(OPEN_CLOSE_PORT, OPEN_CLOSE_PIN, GPIO_MODE_IN_PU_NO_IT);
   //GPIO_Init(GPIOC, GPIO_PIN_4, GPIO_MODE_OUT_PP_HIGH_FAST);  //for test
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


int main( void )
{
  device_init();

  while (1)
  {
    time_task_poll();
  }
}
