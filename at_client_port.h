
#ifndef _AT_CLIENT_PORT_H_
#define _AT_CLIENT_PORT_H_

#include "stdint.h"

#include <rtthread.h>
#include "app_board.h"

//extern unsigned int SysTick_ms;
#define AT_CLIENT_TICK_GET rt_tick_get()


static inline int at_client_tick_timeout(unsigned int start_tick, unsigned int interval){
  //去大于号 就不会存在MAX>MAX 于是永远阻塞
  if (AT_CLIENT_TICK_GET- start_tick > interval)
  {
      //这个算法要保证tick为满32位并自动溢出
      return 1;
  }
  return 0;
}

//查询tick剩余量
static inline unsigned int at_client_tick_Notimeout(unsigned int start_tick, unsigned int interval){
  //函数功能和以上结构重叠，但由于兼容非操作系统和历史原因不进行函数功能合并
  unsigned int pass = AT_CLIENT_TICK_GET- start_tick;
  if (pass > interval)
  {
    return 0;
  }else{
    return interval-pass;
  }
}

#define AT_CLIENT_UART_INIT()                             USART0_INIT();
#define AT_CLIENT_UART_SEND(data,datalen)                 USART0_send(data,datalen)
#define AT_CLIENT_UART_SET_RECEIVE_CALLBACK_FUNCTION(fn)  USART0_set_receive_callback_function(fn);
void at_client_port_input_loopQueue(uint8_t* data, int len);
int at_client_port_take_byte(uint8_t* byte, unsigned int timeout);
void at_client_port_output(uint8_t* data, int len);

void at_client_port_init(void);


void at_client_OFF(void);
void at_client_ON(void);

#endif /* _AT_CLIENT_PORT_H_ */
