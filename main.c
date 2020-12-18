#include "stm8s_conf.h"
#include "bsp_uart2.h"

void device_init(void)
{
  /*High speed internal clock prescaler: 1*/
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
  
  //uart 1 init
  uart2_init();
}

int main( void )
{
  device_init();
  char ans;
  while (1)
  {
    ans = getchar();
    printf_debug("%c", ans);  
  }
  //return 0;
}
