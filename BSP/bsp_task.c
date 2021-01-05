#include "bsp_task.h"
#include "bsp_time2.h"
#include "bsp_ADC.h"
#include "bsp_uart2.h"

//task time define
#define DELAY_TIMES_1MS       (100U)                  // cannot be 0
#define DELAY_TIMES_10MS      (10*DELAY_TIMES_1MS)
#define DELAY_TIMES_100MS     (100*DELAY_TIMES_1MS)
#define DELAY_TIMES_1S        (1000*DELAY_TIMES_1MS)

//////lightbar status ckeck param/////////
//#define MIN_DELTA_DETECTION    (200U)  // 800 / 4 = 200U
//#define MIN_THREASHOLD         (987U)  //3950 / 4 = 987U  //����ad������ֵ
#define IR_OPEN_MIN             500      //IR�򿪣�ͨ�����谭ʱAD��Сֵ
#define IR_CLOSE_MAX            200      //IR�رգ�����ͨ�����谭ʱ���AD���ֵ

#define THREASHOLD_RISING       2
#define THREASHOLD_FALLING      100

#define LIGHT_BAR_CTRL_OPEN     0
#define LIGHT_BAR_CTRL_CLOSE    1
#define LIGHT_BAR_CTRL_ERROR    2
uint8_t light_bar_status = 0;
uint8_t light_bar_old_status = 0;
uint8_t light_bar_debound_cnt = 0;

///////////time task param///////////////
uint32_t current_time = 0;
uint32_t task_mark_time_10ms = 0;
uint32_t task_mark_time_100ms = 0;
uint32_t task_mark_time_1s = 0;   //test

/////////////IR adc calc param///////////
#define FILTER_SIZE    16
uint16_t adc_current_base_value = 0;
uint8_t     adc_sample_start = 0;

uint16_t pwm_test_cnt = 0;

////////////close_open status///////////
#define     CLOSE    0
#define     OPEN     1

uint8_t open_close_status = OPEN;  //Ĭ��Ϊ��
/*
* ���㵱ǰir ����ADC�Ļ�������ֵ���ο�
* �� @10ms_task_mark_time(); ����
*/
void adc_current_base_value_calc(void)
{
  uint16_t the_current_ad_value = get_time2_sample_adc_value();
  adc_current_base_value = ((adc_current_base_value * (FILTER_SIZE-1))\
                                       +the_current_ad_value)/FILTER_SIZE;
}

/*
* ����IR����ܣ�����ʹ�ܻ�ȡ����
* �� @100ms_task_mark_time(); ����
*/
void IR_drv_adc_sample_ctrl(void)
{
  //��IR�����
  _hal_set_high_opt_led_bar_cmd();
  
  //_hal_set_low_opt_led_bar_cmd();
  //��ʼ�ɼ����ݣ�����bsp_ADC.c�б��õ�
  adc_sample_start = 1;
}

/*
* ��ȡ������������״̬
* ��timer_10ms_task()����
*/
void lightbar_get_status(void)
{
  uint16_t lightbar_current_ad_value = get_time2_sample_adc_value();
  printf("%d\r\n", lightbar_current_ad_value);
  //������AD�����̼������Ϊ3.3V��ĿǰΪ��ʱ�����ǳ����̣����Բ����ж�20210105
  //if((lightbar_current_ad_value - adc_current_base_value) >= MIN_DELTA_DETECTION) && (lightbar_current_ad_value >= MIN_THREASHOLD))  
  if(lightbar_current_ad_value > IR_OPEN_MIN)
  {
    light_bar_status = LIGHT_BAR_CTRL_OPEN; //ͻȻ���ߣ���Ϊ����
  }
  //else if((lightbar_current_ad_value >= MIN_THREASHOLD) && (adc_current_base_value >= MIN_THREASHOLD))
  else if(lightbar_current_ad_value  <= IR_CLOSE_MAX)
  {
    light_bar_status = LIGHT_BAR_CTRL_CLOSE;
  }
  else
  {
    light_bar_status = LIGHT_BAR_CTRL_ERROR;
  }
}

void handle_light_bar_status(void)
{
  //����״̬light_bar����״̬
  
}

/*
* �����adc�����������������������Ӵ���
* ��timer_10ms_task()����
*/
void light_bar_debound_sample(void)
{
  if(light_bar_old_status != light_bar_status)
  {
    light_bar_debound_cnt++;
    if(((light_bar_status == LIGHT_BAR_CTRL_OPEN)       //�����⵽��
         &&(light_bar_debound_cnt == THREASHOLD_RISING))//����������2��
       ||((light_bar_status != LIGHT_BAR_CTRL_OPEN)     //�����⵽�رջ����
         &&(light_bar_debound_cnt == THREASHOLD_FALLING)))//����������100��
    {
      light_bar_old_status = light_bar_status;
      light_bar_debound_cnt = 0;
      
      handle_light_bar_status();
    }
  }
  else
  {
    light_bar_debound_cnt = 0;
  }
}
/*
 *��ʼ��ʱ������Ĳ���
 */
void time_cnt_init(void)
{
  task_mark_time_10ms = get_current_time();
  task_mark_time_100ms = get_current_time();
  task_mark_time_1s= get_current_time();
}

//���������ӿ���״̬
void check_open_close_status(void)
{
   if(GPIO_ReadInputPin(OPEN_CLOSE_PORT, OPEN_CLOSE_PIN) == 0)
   {
     //printf_debug("close\r\n");
     open_close_status = CLOSE;
   }
   else
   {
     //printf_debug("open\r\n");
     open_close_status = OPEN;
   }
}

/*
 *��ȡ��ǰʱ����ʱ�����������old_time����¼������ʱ���ֵ
 */
uint32_t get_timer_interval(uint32_t old_time)
{
  if(current_time >= old_time)
  {
    return (current_time - old_time);
  }
  else
  {
    return (current_time + (0xffffffff - old_time));  
  }
}

/*
* 10ms����10msִ��һ��
*/

void timer_10ms_task(void)
{
  if(get_timer_interval(task_mark_time_10ms) > DELAY_TIMES_10MS)
  {
     task_mark_time_10ms = get_current_time();
    
     ////////������ִ����////////
     adc_current_base_value_calc(); 
     IR_drv_adc_sample_ctrl();
     lightbar_get_status();
     light_bar_debound_sample();
     check_open_close_status();
     ////////������ִ����////////
  }
}

/*
* 100ms����100msִ��һ��
*/
void timer_100ms_task(void)
{
  if(get_timer_interval(task_mark_time_100ms) > DELAY_TIMES_100MS)
  {
     task_mark_time_100ms = get_current_time();
     
     ////////������ִ����////////
     //����״̬����һ������Ϊ����״̬���ڶ���Ϊ�����״̬
     //printf("%d,%d\r\n", open_close_status, light_bar_status);
     ////////������ִ����////////
  }
}

/*
* 1s����1sִ��һ��
*/
void timer_1s_task(void)
{
  if(get_timer_interval(task_mark_time_1s) >= DELAY_TIMES_1S)
  {
     task_mark_time_1s = get_current_time();
     
     ////////������ִ����////////
     //uint16_t ad_value = get_adc_value();
     uint16_t ad_value = get_time2_sample_adc_value();
     //printf_debug("the value of %d",ad_value);
     ////////������ִ����////////
  }
}

void real_task_poll()
{
      pwm_test_cnt = pwm_test_cnt+1;
      if(pwm_test_cnt <= 2)
      {
        //GPIO_WriteHigh(GPIOC, GPIO_PIN_4);
        _hal_set_high_opt_led_bar_cmd();
      }
      
      if(pwm_test_cnt > 2)
      {
        //GPIO_WriteLow(GPIOC, GPIO_PIN_4);
        _hal_set_low_opt_led_bar_cmd();
      }
      
      if(pwm_test_cnt > 20)
      {
        pwm_test_cnt = 0;
      }
}

void time_task_poll(void)
{
  handle_adc1_sample_poll();
  current_time = get_current_time();//���µ�ǰʱ��
  //real_task_poll();        //���ڲ��Ե�PWM��
  timer_10ms_task();        //��ѯ����
  timer_100ms_task();
  timer_1s_task();
}

