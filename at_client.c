#include "at_client.h"
#include "at_client_port.h"
#include "at_client_fn.h"
#include "at_client_cfg.h"

#include <stdio.h>
#include <string.h>


//全局
#define FN_NULL 0 //空函数
#define FN_BLOCK 0xFFFFFFFF
static At_client_state_m at_client_state = 0;
#define at_state_his_size AT_STATE_HIS_SIZE
static At_client_state_m at_state_his[at_state_his_size]={0}; //状态历史记录 循环列队的记录
static int at_state_his_head=0;//假如增加第一个数据头不动
static int at_state_his_end=0; //假如增加第一个数据时尾部增加 

static At_cmd cmd[STATE_MAX]={0};
static const char* state_name[STATE_MAX];

At_client_state_m at_client_state_get(void){
    return at_client_state;
}

int at_client_is_relay_state(void){
    if (at_client_state == RECV_RELAY_STATE)
    {
        return 1;
    }
    return 0;
}

void at_client_state_enter(At_client_state_m state){
    At_client_state_m excessive_state = at_client_state;//缓存一下当前状态
    At_client_state_m new_state = state; //缓存一下将要改变的心状态 可能会出现拦截逻辑导致最终改的状态不是想要的状态


    //最终改变状态
    at_client_state = new_state;
    cmd[at_client_state].enter_state_tick = AT_CLIENT_TICK_GET;//这个参数很重要决定延时执行
    cmd[at_client_state].exec_this_count = 0;//这个参数很重要决定是否重新进入第一次执行
    //记录历史状态
    at_state_his[at_state_his_end] = excessive_state; //最新记录的是上一个状态 方便读取上一次状态 也省掉一个记录当前状态的内存
    at_state_his_end = (at_state_his_end+1)%at_state_his_size;
    if (at_state_his_end == at_state_his_head)
    {
        at_state_his_head = (at_state_his_head+1)%at_state_his_size;
    }
    printf("at client state change: %s -- > %s.\r\n",state_name[excessive_state],state_name[new_state]);
}





//声明状态名字符串常量
#define AT_CMD(_state, _simpleCMD, _exec, _exec2, _first_exec_interval_tick, _exec_sum, _exec_interval_tick, _exec_timeout, _exec_timeout2, _allow_timeout_sum, _exec_non_allow_timeout, _exec_non_allow_timeout2)  \
const char state_name_##_state##[] = #_state;
#include "at_client_std.h"





//声明简单的发送CMD函数 可能会生成不用的多余函数 但是不用也不会进入编译系统就让它多余
#define AT_CMD(_state, _simpleCMD, _exec, _exec2, _first_exec_interval_tick, _exec_sum, _exec_interval_tick, _exec_timeout, _exec_timeout2, _allow_timeout_sum, _exec_non_allow_timeout, _exec_non_allow_timeout2)  \
void send_simple_cmd_##_state##(void){\
    char* cmd = _simpleCMD;\
    at_client_port_output((uint8_t*)cmd,strlen(cmd));\
    SEND_LINE_MARK;\
}
#include "at_client_std.h"

//生成状态转换函数
#define AT_CMD(_state, _simpleCMD, _exec, _exec2, _first_exec_interval_tick, _exec_sum, _exec_interval_tick, _exec_timeout, _exec_timeout2, _allow_timeout_sum, _exec_non_allow_timeout, _exec_non_allow_timeout2) \
void state_enter_##_state##(void){\
    at_client_state_enter(_state);\
}
#include "at_client_std.h"
//补充一个默认状态
void state_enter_STATE_NON(void){
    at_client_state_enter(STATE_NON);
}


void at_cmd_init(void){
    //补充默认状态名字
    state_name[STATE_NON] = "STATE_NON";

#define AT_CMD(_state, _simpleCMD, _exec, _exec2, _first_exec_interval_tick, _exec_sum, _exec_interval_tick, _exec_timeout, _exec_timeout2, _allow_timeout_sum, _exec_non_allow_timeout, _exec_non_allow_timeout2)  \
    cmd[_state].state = _state;\
    cmd[_state].exec = _exec;\
    cmd[_state].exec2 = _exec2;\
    cmd[_state].first_exec_interval_tick = _first_exec_interval_tick;\
    cmd[_state].exec_sum = _exec_sum;\
    cmd[_state].exec_interval_tick = _exec_interval_tick;\
    cmd[_state].exec_timeout = _exec_timeout;\
    cmd[_state].exec_timeout2 = _exec_timeout2;\
    cmd[_state].allow_timeout_sum = _allow_timeout_sum;\
    cmd[_state].exec_non_allow_timeout = _exec_non_allow_timeout;\
    cmd[_state].exec_non_allow_timeout2 = _exec_non_allow_timeout2;\
    state_name[_state] = state_name_##_state;\
    printf("%s \t\t%s \t\t%d \t\t%d \t\t%d \t\t%d \t\t%d \t\t%d \t\t%d \t\t%d \t\t%d \t\t%d\r\n",state_name[_state],_simpleCMD,_exec,_exec2,_first_exec_interval_tick,_exec_sum,_exec_interval_tick,_exec_timeout,_exec_timeout2,_allow_timeout_sum,_exec_non_allow_timeout,_exec_non_allow_timeout2);
#include "at_client_std.h"
    //输出状态列表
    for (int i = 0; i < STATE_MAX; i++)
    {
        printf("at_client_state[%d]: %s.\r\n",i,state_name[i]);
    }
    
}




void at_client_cmd_exec(void){
    if (at_client_state == STATE_NON || at_client_state == NOT_EXEC_CMD_STATE)
    {
        //默认不执行 需要动态激活 也可以撤销
        return;
    }

    //判断是否第一次进入命令 是的话等待一个间隔
    if (cmd[at_client_state].exec_this_count == 0)
    {
        if (at_client_tick_timeout(cmd[at_client_state].enter_state_tick,cmd[at_client_state].first_exec_interval_tick))
        {
            //第一次执行
            printf("at client first exec %s.\r\n",state_name[at_client_state]);
            if (cmd[at_client_state].exec)
            {
                cmd[at_client_state].exec();
            }
            if (cmd[at_client_state].exec2)
            {
                cmd[at_client_state].exec2();
            }
            cmd[at_client_state].exec_this_count++;
            cmd[at_client_state].exec_all_count++;
            cmd[at_client_state].exec_tick = AT_CLIENT_TICK_GET;
        }
        return;
    }

    //后续间隔执行： 已经执行过一遍 && 不超过可执行次数 exec_sum至少为1 写0功能也等同于1
    if (cmd[at_client_state].exec_this_count >= 1 && cmd[at_client_state].exec_this_count < cmd[at_client_state].exec_sum)
    {
        if (at_client_tick_timeout(cmd[at_client_state].exec_tick,cmd[at_client_state].exec_interval_tick))
        {
            //间隔执行
            if (cmd[at_client_state].exec)
            {
                cmd[at_client_state].exec();
            }
            if (cmd[at_client_state].exec2)
            {
                cmd[at_client_state].exec2();
            }
            cmd[at_client_state].exec_this_count++;
            cmd[at_client_state].exec_all_count++;
            cmd[at_client_state].exec_tick = AT_CLIENT_TICK_GET;
        }
        return;
    }

    //超时检测
    if (cmd[at_client_state].exec_this_count >= cmd[at_client_state].exec_sum){
        if (at_client_tick_timeout(cmd[at_client_state].exec_tick,cmd[at_client_state].exec_interval_tick)){
            cmd[at_client_state].exec_timeout_count++;
            cmd[at_client_state].exec_timeout_all_count++;

            //允许超时次数为先 判断取>=号 当允许超时1次时 这里可以拦截1
            if (cmd[at_client_state].exec_timeout_count >= cmd[at_client_state].allow_timeout_sum && cmd[at_client_state].allow_timeout_sum != 0)
            {
                if (cmd[at_client_state].exec_non_allow_timeout)
                {
                    cmd[at_client_state].exec_non_allow_timeout();
                }
                if (cmd[at_client_state].exec_non_allow_timeout2)
                {
                    cmd[at_client_state].exec_non_allow_timeout2();
                }
                //复位
                cmd[at_client_state].exec_timeout_count = 0;
                cmd[at_client_state].exec_non_allow_timeout_tick = AT_CLIENT_TICK_GET;
            }else{
                if (cmd[at_client_state].exec_timeout)
                {
                    cmd[at_client_state].exec_timeout();
                }
                if (cmd[at_client_state].exec_timeout2)
                {
                    cmd[at_client_state].exec_timeout2();
                }
                cmd[at_client_state].exec_timeout_tick = AT_CLIENT_TICK_GET;
            }
        }
        return;
    }
}






static At_features features[FEATURES_MAX]={0};

//生成所有特征的字符串常量
#define AT_FEATURES(_enum_name, _state, _features1, _features2, _cb, _cb2) \
const char _enum_name##_##features1[] = _features1;\
const char _enum_name##_##features2[] = _features2;
#include "at_client_std.h"


static char recv_exec_buf[AT_CLIENT_RECV_EXEC_BUF_SIZE]={0};
static char* prv_line;//前一行指针
static char* new_line;//本行指针
static int prv_line_size = 0;
static int new_line_size = 0;
char * recv_relay_buf = 0;
int recv_relay_buf_size = 0;

void at_client_recv_exec_init(void){
    //计算处所有需要查找的特征长度
#define AT_FEATURES(_enum_name, _state, _features1, _features2, _cb, _cb2) \
    features[_enum_name].index = _enum_name;\
    features[_enum_name].state = _state;\
    features[_enum_name].features1 = _enum_name##_##features1;\
    features[_enum_name].features2 = _enum_name##_##features2;\
    features[_enum_name].cb = _cb;\
    features[_enum_name].cb2 = _cb2;\
    if (prv_line_size < strlen(_enum_name##_##features1)){prv_line_size = strlen(_enum_name##_##features1);}\
    if (prv_line_size < strlen(_enum_name##_##features2)){prv_line_size = strlen(_enum_name##_##features2);}
#include "at_client_std.h"

    //特征不能为1个‘0’字符 0字符会将特征改为空
    for (int i = 0; i < FEATURES_MAX; i++)
    {
        if (strlen(features[i].features1) == 1 && features[i].features1[0] == '0')
        {
            features[i].features1 = 0;
        }
        if (strlen(features[i].features2) == 1 && features[i].features2[0] == '0')
        {
            features[i].features2 = 0;
        }
    }

    

    //这里很重要 特征只是一部分 整行长度有变化
    prv_line_size = prv_line_size + PRV_NEW_LINE_ADDITIONAL;//包含尾行标志 有的尾行多一个\r 包含一个0 加个额外
    prv_line = recv_exec_buf;
    new_line = recv_exec_buf + prv_line_size + 4;//充分隔离
    new_line_size = prv_line_size;//两行的特征此村相同
    recv_relay_buf = new_line + new_line_size + 4;//充分隔离
    recv_relay_buf_size = AT_CLIENT_RECV_EXEC_BUF_SIZE - prv_line_size - 4 - new_line_size - 4;
    printf("prv_line_size = %d. new_line_size=%d. recv_relay_buf_size=%d.\r\n",prv_line_size,new_line_size,recv_relay_buf_size);
    printf("recv_relay_buf=%d, chaeck=%d.\r\n",recv_relay_buf, recv_exec_buf + AT_CLIENT_RECV_EXEC_BUF_SIZE - recv_relay_buf_size);
}

static char prvRchar = 0;
static char newRchar = 0;
static int newRchar_count = 0;
void at_client_recv_exec(void){

    char recvChar = 0;
    if (!at_client_port_take_byte((uint8_t*)&recvChar))
    {
        return;
    }

    prvRchar=newRchar;
    newRchar = recvChar;

    if (RECV_USE_RELAY && RECV_RELAY_STATE == at_client_state)
    {
        //非CMD模式
    }else{
        //CMD模式 调试输出
        if (newRchar == '\r')
        {
            printf("\\r");
        }else if (newRchar == '\n')
        {
            printf("\\n\n");
        }else{
            printf("%c",newRchar);
        }
    }
    
    
    if (newRchar_count < new_line_size)
    {
        new_line[newRchar_count]=newRchar;
        newRchar_count++;
    }else{
        for (int i = 0; i < new_line_size - 1; i++)
        {
            new_line[i]=new_line[i+1];
        }
        new_line[new_line_size-1]=newRchar;
        newRchar_count = new_line_size;
    }

    //百分百避免字符串越界
    new_line[newRchar_count]=0;
    new_line[new_line_size]=0;//由上可知new_line_size-1是会被放置字符串的，初始化时充分隔离给予了4字节
    new_line[prv_line_size]=0;//由上可知new_line_size-1是会被放置字符串的，初始化时充分隔离给予了4字节

    if (prvRchar=='\r' && newRchar == '\n'){

        //遍历所有特征
        for (int i = 0; i < FEATURES_MAX; i++)
        {
            //在扫描特征下或者所有
            if (features[i].state == at_client_state || features[i].state == URC)
            {
                if (features[i].features1 != 0 && features[i].features2 != 0)
                {
                    //printf("--prv_line=%s++new_line=%s**\n",prv_line,new_line);
                    //需要检查特征1和特征2
                    if (strstr(prv_line,features[i].features1) > 0 && strstr(new_line,features[i].features2) > 0)
                    {
                        //printf("features1_len=%d, features2_len=%d.\n",features1_len,features2_len);
                        //printf("features1_len=%d, features2_len=%d.\n",strncmp(features[i].features1,prv_line,features1_len),strncmp(features[i].features2,new_line,features2_len));
                        printf("Snap to Feature 1 1.\n");
                        if (features[i].cb)
                        {
                            features[i].cb();
                        }
                        if (features[i].cb2)
                        {
                            features[i].cb2();
                        }
                        goto __check_line_end;//跳出for循环 不跳出会碰到改变到下一个状态时刚好存在特征
                    }
                }else if (features[i].features1 != 0 && features[i].features2 == 0)
                {
                    //只有特征1 只比较新行
                    if (strstr(prv_line,features[i].features1) > 0)
                    {
                        printf("Snap to Feature 1 0.\n");
                        if (features[i].cb)
                        {
                            features[i].cb();
                        }
                        if (features[i].cb2)
                        {
                            features[i].cb2();
                        }
                        goto __check_line_end;
                    }
                }else if (features[i].features1 == 0 && features[i].features2 != 0)
                {
                    //只有特征2 只比较新行
                    if (strstr(new_line,features[i].features2) > 0)
                    {
                        printf("Snap to Feature 0 1.\n");
                        if (features[i].cb)
                        {
                            features[i].cb();
                        }
                        if (features[i].cb2)
                        {
                            features[i].cb2();
                        }
                        goto __check_line_end;
                    }
                }
            }
            
        }

        //处理完新一行
        __check_line_end:
        strcpy(prv_line,new_line);
        newRchar_count=0;
        new_line[newRchar_count]=0;
    }

    __check_feature_end:
    //等于某个状态时将数据newRchar转发 需要自己实现如将接收转给PPP
    if (RECV_USE_RELAY && NOT_EXEC_CMD_STATE == at_client_state)
    {
        RECV_RELAY_FN(newRchar);
    }
    

}


void at_client_init(void){
    at_client_port_init();
    at_cmd_init();
    at_client_recv_exec_init();
}

static int at_client_is_init = 0;
void at_client_run(void){
    if (at_client_is_init == 0)
    {
        at_client_is_init = 1;
        at_client_init();
        INITIALIZATION_COMPLETE_STATE_ENTER;
    }
    
    at_client_cmd_exec();
    at_client_recv_exec();
}








