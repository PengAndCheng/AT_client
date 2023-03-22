
#ifndef _AT_CLIENT_H_
#define _AT_CLIENT_H_


typedef enum
{
    STATE_NON = 0, //0必须占用为默认没状态
#define AT_CMD(_state, _simpleCMD, _exec, _exec2, _first_exec_interval_tick, _exec_sum, _exec_interval_tick, _exec_timeout, _exec_timeout2, _allow_timeout_sum, _exec_non_allow_timeout, _exec_non_allow_timeout2) _state,
#include "at_client_std.h"
    STATE_MAX,
    URC, //不是状态 只是写在这里 为了不和前面的状态号冲突
}At_client_state_m;

//16*4=64 一条CMD占用64字节 有些变量可以抽出，但是意义不大 一套操作变量不需要多少
typedef struct At_cmd
{
    At_client_state_m state;        //( 常量 ) 当前进行时状态
    unsigned int enter_state_tick;  //( 变量 ) 进入状态的tick

    void (*exec)(void); //( 常量 ) 当前状态下需要执行的函数
    void (*exec2)(void); //( 常量 ) 当前状态下需要执行的函数
    unsigned int first_exec_interval_tick;  //( 常量 ) 状态内开始第一次执行功能函数的间隔
    unsigned int exec_sum;  //( 常量 ) 可以循环执行次数 至少为1
    unsigned int exec_interval_tick; //( 常量 ) 后续执行的间隔 exec_interval_tick * exec_sum = 状态的生命周期
    unsigned int exec_this_count; //( 变量 ) 本次执行次数统计 退出状态后会被复位为0
    unsigned int exec_all_count;  //( 变量 ) 总共执行次数
    unsigned int exec_tick; //( 变量 ) 

    void (*exec_timeout)(void); //( 常量 ) 当前状态下需要执行的函数
    void (*exec_timeout2)(void); //( 常量 ) 当前状态下需要执行的函数
    unsigned int exec_timeout_tick; //( 变量 ) 
    unsigned int exec_timeout_count; //( 变量 ) 所有超时统计
    unsigned int exec_timeout_all_count; //( 变量 ) 所有超时统计

    unsigned int allow_timeout_sum; //( 常量 ) 允许超时次数 为0时此功能失效
    void (*exec_non_allow_timeout)(void); //( 常量 ) 超过超时次数后执行一次 同时复位exec_timeout_count为0
    void (*exec_non_allow_timeout2)(void); //( 常量 ) 超过超时次数后执行一次 同时复位exec_timeout_count为0
    unsigned int exec_non_allow_timeout_tick;//( 变量 ) 
}At_cmd;



typedef enum
{
#define AT_FEATURES(_enum_name, _state, _features1, _features2, _cb, _cb2) _enum_name,
#include "at_client_std.h"
    FEATURES_MAX,
}At_features_index_m;

typedef struct At_features
{
    At_features_index_m index;
    At_client_state_m state; //( 常量 ) 在这个状态下需要检查特征
    const char* features1; //前一行特征
    const char* features2; //本行特征
    void (*cb)(void);
    void (*cb2)(void);
}At_features;

void at_csq_get(void);

void at_client_run(void);
At_client_state_m at_client_state_get(void);
int at_client_is_relay_state(void);


#endif /* _AT_CLIENT_H_ */
