#include "bsp_task.h"
#include "bsp_time2.h"
#include "bsp_ADC.h"

//task time define
#define DELAY_TIMES_1MS       (100U)                  // cannot be 0
#define DELAY_TIMES_10MS      (10*DELAY_TIMES_1MS)
#define DELAY_TIMES_100MS     (100*DELAY_TIMES_1MS)
#define DELAY_TIMES_1S        (1000*DELAY_TIMES_1MS)

//////lightbar status ckeck param/////////
#define MIN_DELTA_DETECTION    (200U)  // 800 / 4 = 200U
#define MIN_THREASHOLD         (987U)  //3950 / 4 = 987U  //采样ad的上限值
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

/*
* 计算当前ir 接收ADC的基础采样值做参考
* 被 @10ms_task_mark_time(); 调用
*/
void adc_current_base_value_calc(void)
{
  uint16_t the_current_ad_value = get_time2_sample_adc_value();
  adc_current_base_value = ((adc_current_base_value * (FILTER_SIZE-1))\
                                       +the_current_ad_value)/FILTER_SIZE;
}

/*
* 开启IR发射管，并且使能获取数据
* 被 @100ms_task_mark_time(); 调用
*/
void IR_drv_adc_sample_ctrl(void)
{
  //打开IR发射管
  _hal_set_high_opt_led_bar_cmd();
  
  //开始采集数据，将在bsp_ADC.c中被用到
  adc_sample_start = 1;
}

/*
* 获取挡光板控制器的状态
* 被timer_10ms_task()调用
*/
void lightbar_get_status(void)
{
  uint16_t lightbar_current_ad_value = get_time2_sample_adc_value();
  if((lightbar_current_ad_value - adc_current_base_value) >= MIN_DELTA_DETECTION
     && (lightbar_current_ad_value >= MIN_THREASHOLD))
  {
    light_bar_status = LIGHT_BAR_CTRL_OPEN; //突然升高，认为打开了
  }
  else if((lightbar_current_ad_value >= MIN_THREASHOLD)
          && (adc_current_base_value >= MIN_THREASHOLD))
  {
    light_bar_status = LIGHT_BAR_CTRL_ERROR;
  }
  else
  {
    light_bar_status = LIGHT_BAR_CTRL_CLOSE;
  }
}

void handle_light_bar_status(void)
{
  //处理状态light_bar各种状态
  
}

/*
* 仅针对adc采样的消抖，不做其它复杂处理
* 被timer_10ms_task()调用
*/
void light_bar_debound_sample(void)
{
  if(light_bar_old_status != light_bar_status)
  {
    light_bar_debound_cnt++;
    if(((light_bar_status == LIGHT_BAR_CTRL_OPEN)       //如果检测到打开
         &&(light_bar_debound_cnt == THREASHOLD_RISING))//并且消抖了2次
       ||((light_bar_status != LIGHT_BAR_CTRL_OPEN)     //如果检测到关闭或故障
         &&(light_bar_debound_cnt == THREASHOLD_FALLING)))//并且消抖了100次
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
 *初始化时间任务的参数
 */
void time_cnt_init(void)
{
  task_mark_time_10ms = get_current_time();
  task_mark_time_100ms = get_current_time();
  task_mark_time_1s= get_current_time();
}

/*
 *获取当前时间与时间任务参数（old_time）记录下来的时间差值
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
* 10ms任务，10ms执行一次
*/
void timer_10ms_task(void)
{
  if(get_timer_interval(task_mark_time_10ms) > DELAY_TIMES_10MS)
  {
     task_mark_time_10ms = get_current_time();
     
     ////////任务区执行区////////
     adc_current_base_value_calc(); 
     lightbar_get_status();
     light_bar_debound_sample();
     ////////任务区执行区////////
  }
}

/*
* 100ms任务，100ms执行一次
*/
void timer_100ms_task(void)
{
  if(get_timer_interval(task_mark_time_100ms) > DELAY_TIMES_100MS)
  {
     task_mark_time_100ms = get_current_time();
     
     ////////任务区执行区////////
     IR_drv_adc_sample_ctrl();
     ////////任务区执行区////////
  }
}

/*
* 1s任务，1s执行一次
*/
void timer_1s_task(void)
{
  if(get_timer_interval(task_mark_time_1s) >= DELAY_TIMES_1S)
  {
     task_mark_time_1s = get_current_time();
     
     ////////任务区执行区////////
     
     ////////任务区执行区////////
  }
}

void time_task_poll(void)
{
  current_time = get_current_time();//更新当前时间
  timer_10ms_task();        //轮询调度
  timer_100ms_task();
  timer_1s_task();
}

