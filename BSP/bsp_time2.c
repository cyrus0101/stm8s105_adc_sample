#include "bsp_time2.h"
#include "bsp_ADC.h"
#include "bsp_uart2.h"

#define    LIGHT_BAR_ON_TIME     7     //7*10 = 70us
#define    SAMPLE_TIME           10    //tatol sample times

typedef struct {
  uint32_t  timer_cnt;          //new value = 1; old value = 1;
  uint8_t   cnt;                   //sample times
  uint16_t  max_adc1_sample_value; //max sample value
  uint16_t  adc1_sample_value;     //after 10 times,save the value of max
  uint8_t   time2_it_trigger_flag; //
  //uint8_t   adc_value_status;      //0 = adc value is not ready; 1 = adc value can use
}TIME2_SAMPLE_ADC1;

TIME2_SAMPLE_ADC1 TIM2_sample = {0,0,0,0,0};

extern uint32_t adc_sample_start;

void user_time2_init(void)
{
  /* Time base configuration */
  TIM2_TimeBaseInit(TIME2_PRESCALER, TIME2_Autoreload_VALUE);
    
  /* Prescaler configuration */
  TIM2_PrescalerConfig(TIME2_PRESCALER, TIM2_PSCRELOADMODE_IMMEDIATE);
  
  //enable ARR
  TIM2_ARRPreloadConfig(ENABLE);
  
  //enable time2 interrupt
  TIM2_ITConfig(TIME2_IT_TYPE, ENABLE);

  /* TIM1 counter enable */
  TIM2_Cmd(ENABLE);
}

/**
  * @brief  Timer2 Update/Overflow/Break Interrupt routine
  * @param  None
  * @retval None
  */

INTERRUPT_HANDLER(TIM2_UPD_OVF_BRK_IRQHandler, 13)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  TIM2_sample.timer_cnt++;
  TIM2_sample.time2_it_trigger_flag = 1;
  TIM2_ClearITPendingBit(TIM2_IT_UPDATE);
}

/*
获取ADC值的回调
10次一个周期，一次10ms，前面7次IR是开的，后面3次IR是关的
*/
void light_bar_sample_value_timer2_cb(void)
{
  //bar light on,sample 7 times
  if(TIM2_sample.cnt >= LIGHT_BAR_ON_TIME)
  {
    //设置IR发射口为低
    _hal_set_low_opt_led_bar_cmd();
    
    //if sample times >= 10, get the max value,init others params
    if(TIM2_sample.cnt >= SAMPLE_TIME)
    {
      TIM2_sample.cnt = 0;
      TIM2_sample.adc1_sample_value = TIM2_sample.max_adc1_sample_value;
      TIM2_sample.max_adc1_sample_value = 0;
      //采集到之后则停止，等下一次10ms
      adc_sample_start = 0;
    }
  }
  
  /*通过get_adc_value()获取ADC的值，并且根据之前保存的值比较，留下最大的值*/
  uint16_t ADC_sample_value = get_adc_value();
  if(ADC_sample_value > TIM2_sample.max_adc1_sample_value)
  {
    TIM2_sample.max_adc1_sample_value = ADC_sample_value;
  }
  //printf_debug("the value of ADC_sample_value is %d",ADC_sample_value);
  TIM2_sample.cnt++;

}
/*
定时器采样10次，取出10次最大值 = =很怀疑这样做的必要性，
先根据Venus代码实现，

* function:return the max value in 10 times ADC1 sample
*/
uint16_t get_time2_sample_adc_value(void)
{
   return TIM2_sample.adc1_sample_value;
}

uint32_t get_current_time(void)
{
  return TIM2_sample.timer_cnt;
}

#if 0
  /*
  delay_time = delay time + now time
  单位时间长度：10ns，与定时器有关
  */
  bool wait_timeout(uint32_t delay_time)  
  { 
    uint32_t deltatime = (uint32_t)(timeout - get_current_time())
    if((deltatime <= 0) && (delay_time > 0))
    {
       return true; //timeout is reached
    }
    else
    {
       return false;  
    }
  }
#endif


void handle_adc1_sample_poll(void)
{
  //如果中断还未触发，则忽略
  if(TIM2_sample.time2_it_trigger_flag == 0)  
    return;
  
  //如果10ms任务调度还没触发，则忽略
   if(adc_sample_start == 0)
     return;
   
  light_bar_sample_value_timer2_cb();//handle the value get from adc1 
  
  TIM2_sample.time2_it_trigger_flag = 0;
}
