#include <stdio.h>

#include "at_client_port.h"
#include "at_client_cfg.h"

#include "at_client.h"






static uint8_t qdata[AT_CLIENT_PORT_INPUT_LOOPQUEUE_SIZE];
static int qhead = 0;
static int qend = 0;
//使用接收信号量
#if USE_RTOS
#define QSEM 1
#else
#define QSEM 0
#endif
#if QSEM
static struct rt_semaphore qsem;
#endif /* #if QSEM */
void at_client_port_input_loopQueue(uint8_t* data, int len){
    for (int i = 0; i < len; i++)
    {
        qdata[qend] = data[i];
        qend = (qend+1)%AT_CLIENT_PORT_INPUT_LOOPQUEUE_SIZE;
        if (qend == qhead)
        {
            qhead = (qhead+1)%AT_CLIENT_PORT_INPUT_LOOPQUEUE_SIZE;
        }
    }
#if QSEM
    rt_sem_release(&qsem);
#endif /* #if QSEM */
}

int at_client_port_take_byte(uint8_t* byte, unsigned int timeout){
#if QSEM
    if (qend == qhead)
    {
        rt_sem_take(&qsem, timeout);
    }
#endif /* #if QSEM */


    int len = 0;
    if (qend >= qhead)
    {
        len = qend - qhead;
    }else{
        len = AT_CLIENT_PORT_INPUT_LOOPQUEUE_SIZE - qhead + qend;
    }
    
    if (len > 0)
    {
        byte[0] = qdata[qhead];
        qhead = (qhead+1)%AT_CLIENT_PORT_INPUT_LOOPQUEUE_SIZE;
        return 1;
    }else{
        return 0;
    }
}


void at_client_port_output(uint8_t* data, int len){
    if (USE_RELAY_STATE && RELAY_STATE == at_client_state_get())
    {
        //非CMD模式
    }else{
        //CMD模式 调试输出
        for (int i = 0; i < len; i++)
        {
            if (data[i] == '\r')
            {
                printf("\\r");
            }else if (data[i] == '\n')
            {
                printf("\\n\n");
            }else{
                printf("%c",data[i]);
            }
        }
    }

    AT_CLIENT_UART_SEND(data,len);
}

void at_client_port_init(void){
#if QSEM
    rt_sem_init(&qsem, "qsem", 0, RT_IPC_FLAG_FIFO);
#endif /* #if QSEM */
    qhead = 0;
    qend = 0;
    AT_CLIENT_UART_INIT();
    AT_CLIENT_UART_SET_RECEIVE_CALLBACK_FUNCTION(at_client_port_input_loopQueue);
    printf("at client port init.\r\n");
}







#include "gd32f30x.h"

//#define EC200S_REST GET_PIN(B, 1)
static int module_4G_is_init = 0;
static void module_4G_init(void){
    /* enable the led clock */
    rcu_periph_clock_enable(RCU_GPIOB);
    /* configure led GPIO port */ 
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
}
void module_4G_OFF(void){
    if (module_4G_is_init==0)
    {
        module_4G_is_init=1;
        module_4G_init();
    }
    
    gpio_bit_set(GPIOB,GPIO_PIN_1);
}

void module_4G_ON(void){
    if (module_4G_is_init==0)
    {
        module_4G_is_init=1;
        module_4G_init();
    }
    gpio_bit_reset(GPIOB,GPIO_PIN_1);
}

void at_client_OFF(void){
    module_4G_OFF();
    printf("at_client_OFF, tick=%d.\n",AT_CLIENT_TICK_GET);
}

void at_client_ON(void){
    module_4G_ON();
    printf("at_client_ON, tick=%d.\n",AT_CLIENT_TICK_GET);
}