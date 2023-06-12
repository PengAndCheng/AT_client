
#ifndef _AT_CLIENT_FN_H_
#define _AT_CLIENT_FN_H_

#define USE_PPP 0

#if USE_PPP
void at_ppp_enter(void);
void at_ppp_quit(void);

void at_ppp_recv(char c);
int at_ppp_send(char* data, int len);
int at_ppp_is_normal(void);
#endif


void open_touchuan(void);
void recv_touchuan(char ch);
void send_touchuan(char* data, int len);

void enter_touchuan(void);
void quit_touchuan(void);
int isTouChan(void);

#endif /* _AT_CLIENT_FN_H_ */
