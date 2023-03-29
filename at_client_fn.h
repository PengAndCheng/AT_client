
#ifndef _AT_CLIENT_FN_H_
#define _AT_CLIENT_FN_H_

void at_ppp_enter(void);
void at_ppp_quit(void);

void at_ppp_recv(char c);
int at_ppp_send(char* data, int len);
int at_ppp_is_normal(void);



#endif /* _AT_CLIENT_FN_H_ */
