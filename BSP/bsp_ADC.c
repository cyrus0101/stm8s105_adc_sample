#include "bsp_ADC.h"
#include "stm8s_conf.h"
#include "bsp_uart2.h"

typedef struct {
  uint8_t value_status;  //new value == 1; old value == 0;
  uint8_t ADC1_data_h;   //ADC value high
  uint8_t ADC1_data_l;   //ADC value low
}ADC1_Value;
ADC1_Value ADC1_sample_value ={0,0,0};

void user_ADC_init(void)
{
  /*  Init GPIO for ADC1 */
  GPIO_Init(GPIOB, GPIO_PIN_0, GPIO_MODE_IN_FL_NO_IT);
  
  ADC1_DeInit();
  
  /*
  * ADC1_ConversionMode = ADC1_CONVERSIONMODE_SINGLE         指定ADC1转换模式：单次转换
  * ADC1_Channel = ADC1_CHANNEL_0                            指定ADC1通道
  * ADC1_PrescalerSelection =  ADC1_PRESSEL_FCPU_D2          指定ADC1分频器, fclk/2 = 8M
  * ADC1_ExtTrigger = ADC1_EXTTRIG_TIM                       指定ADC1外部触发器
  * ADC1_ExtTriggerState = DISABLE                           指定外部触发器状态
  * ADC1_Align = ADC1_ALIGN_RIGHT                            指定ADC1右对齐
  * ADC1_SchmittTriggerChannel = ADC1_SCHMITTTRIG_CHANNEL9   指定斯密特触发器
  * ADC1_SchmittTriggerState = DISABLE                       指定斯密特触发器状态
  */
  ADC1_Init(ADC1_CONVERSIONMODE_CONTINUOUS,
            ADC1_CHANNEL_0,
            ADC1_PRESSEL_FCPU_D8,
            ADC1_EXTTRIG_TIM,
            DISABLE,
            ADC1_ALIGN_RIGHT,
            ADC1_SCHMITTTRIG_CHANNEL9,
            DISABLE);
  
  /* Enable EOC interrupt */
  //ADC1_ITConfig(ADC1_IT_EOCIE, ENABLE); 
  
  /*Start Conversion */
  ADC1_StartConversion();
}

//enable ADC1 sample
void user_start_ADC1_single_sample(void)
{
  /*Start Conversion */
  ADC1_StartConversion();
}

/**
  * @brief  ADC1 interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(ADC1_IRQHandler, 22)
 {
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
       为了在开发过程中检测意外事件，建议在以下指令上设置断点。
    */
   ADC1_sample_value.value_status = 1;  //new adc value
   
   ADC1_ClearITPendingBit(ADC1_IT_EOC);
 }

uint8_t zero_cnt = 0;
uint16_t get_adc_value(void)
{
  return ADC1_GetConversionValue();
  /*
  if(ADC1_sample_value.value_status == 1)
  {
    ADC1_sample_value.value_status = 0;
    //return ((ADC1_sample_value.ADC1_data_h<<8) + ADC1_sample_value.ADC1_data_l);
    return ADC1_GetConversionValue();
  }
  else
  {
    zero_cnt++;
    if(zero_cnt == 100)
    {
      zero_cnt = 0;
      printf_debug("ADC GET 0!\r\n");
    }
    return 0;
  }
  */
}