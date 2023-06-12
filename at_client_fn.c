
#include <string.h>
#include "at_client_fn.h"
#include "at_client.h"
#include "at_client_port.h"

#include "stdio.h"
#include "mqtt_client_address_username_password_private.h"

//转发状态下的可用buf指针
extern char * recv_relay_buf;
#define buf recv_relay_buf
//妆发状态下的可用buf长度
extern int recv_relay_buf_size;
#define buf_size recv_relay_buf_size

extern At_client_state_m at_client_state;


#if USE_PPP
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

#endif /* #if USE_PPP */


void open_touchuan(void){
    char cmdtemp[32]={0};

    char cmd[]="AT+QIOPEN=1,";
    at_client_port_output((uint8_t*)cmd,sizeof(cmd)-1);

    static int socketNb = 0;
    socketNb = socketNb % 8;
    snprintf(cmdtemp,sizeof(cmdtemp),"%d",socketNb);
    socketNb++;
    at_client_port_output((uint8_t*)cmdtemp,1);

    extern uint8_t targetIP[4];
    extern uint16_t targetPort;
    snprintf(cmdtemp, sizeof(cmdtemp), ",\"TCP\",\"%s\",%d,0,2\r\n", mqtt_client_address, mqtt_client_port);
    at_client_port_output((uint8_t*)cmdtemp,strlen(cmdtemp));
}

void recv_touchuan(char ch){
    //收到socket中断通知
    static const char nc[] = "NO CARRIER";
    static const int size = 10;
    static int len = 0;
    if (nc[len] == ch)
    {
        len++;
    }else{
        len=0;
    }
    //debug
    //printf("%c",ch);
    if (len == size)
    {
        len = 0;
        //到这里通知上层应用断开连接
        printf("recv_touchuan %s .\n",nc);

        //状态转移到AT
        extern void state_enter_AT(void);
        state_enter_AT();
        return;
    }
    
    
    //到这里把数据传给上层应用
    extern void mqtt_input(char ch);
    mqtt_input(ch);
}


static int touchuan_state = 0;

void send_touchuan(char* data, int len){
    if (touchuan_state == 1)
    {
        at_client_port_output((uint8_t*)data, len);
        //printf("send_touchuan len=%d.\n",len);
    }
}


void enter_touchuan(void){
    touchuan_state = 1;
}

void quit_touchuan(void){
    touchuan_state = 0;
}

int isTouChan(void){
    return touchuan_state;
}

