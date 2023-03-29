
#include "at_client_fn.h"
#include "at_client.h"
#include "at_client_port.h"

#include "stdio.h"

//转发状态下的可用buf指针
extern char * recv_relay_buf;
#define buf recv_relay_buf
//妆发状态下的可用buf长度
extern int recv_relay_buf_size;
#define buf_size recv_relay_buf_size

extern At_client_state_m at_client_state;


#define at_ppp_recv_debug 0
static int recv_nb = 0;
void at_ppp_recv(char c){
    buf[recv_nb] = c;
    recv_nb++;
    if (recv_nb == buf_size || ( (recv_nb>1)&&(c==0x7E) ))//缓解协议栈压力使用这句
	//if ( recv_nb == buf_size || recv_nb >= 1 )
    {
        //可以将数据输入PPP协议栈
#if at_ppp_recv_debug
        printf("at_ppp_recv:");
        for (int i = 0; i < recv_nb; i++)
        {
            printf(" %02X",buf[i]);
        }
        printf(" .\r\n");
#endif /* #if at_ppp_recv_debug */

        extern void at_ppp_input(uint8_t* buffer, int buffer_len);at_ppp_input(buf,recv_nb);
        recv_nb = 0;//很重要
    }
}

void at_ppp_enter(void){
	extern void PPPOS_connect(void);
	PPPOS_connect();
}

void at_ppp_quit(void){
	extern void PPPOS_disconnect(void);
	PPPOS_disconnect();
}

#define at_ppp_send_debug 0
int at_ppp_send(char* data, int len){
#if at_ppp_send_debug
    printf("at_ppp_send:");
    for (int i = 0; i < len; i++)
    {
        printf(" %02X",data[i]);
    }
    printf(" .\r\n");
#endif /* #if at_ppp_send_debug */

    if (at_client_state_get() == PPP)
    {
        at_client_port_output((uint8_t*)data,len);
        return len;
    }else{
        return 0;
    }
}

int at_ppp_is_normal(void){
    if (at_client_state_get() == PPP)
    {
        return 1;
    }
    return 0;
}




