
#ifndef _AT_CLIENT_PORT_H_
#define _AT_CLIENT_PORT_H_

#include "stdint.h"


extern unsigned int SysTick_ms;
#define AT_CLIENT_TICK_GET SysTick_ms

static inline int at_client_tick_timeout(unsigned int start_tick, unsigned int interval){
  //去大于号 就不会存在MAX>MAX 于是永远阻塞
    if (AT_CLIENT_TICK_GET- start_tick > interval)
    {
        //这个算法要保证tick为满32位并自动溢出
        return 1;
    }
    return 0;
}

#define AT_CLIENT_UART_INIT()                             uart1_init()
#define AT_CLIENT_UART_SEND(data,datalen)                 uart1_send(data,datalen)
#define AT_CLIENT_UART_SET_RECEIVE_CALLBACK_FUNCTION(fn)  uart1_set_receive_callback_function(fn);
void at_client_port_input_loopQueue(uint8_t* data, int len);
int at_client_port_take_byte(uint8_t* byte);
void at_client_port_output(uint8_t* data, int len);

void at_client_port_init(void);


void at_client_OFF(void);
void at_client_ON(void);

#endif /* _AT_CLIENT_PORT_H_ */
