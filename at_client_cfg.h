
#ifndef _AT_CLIENT_CFG_H_
#define _AT_CLIENT_CFG_H_

//断言调试宏
#define AT_CLIENT_PLATFORM_ASSERT(x) do {printf("Assertion \"%s\" failed at line %d in %s\n", \
                                     x, __LINE__, __FILE__);} while(0)
#define AT_CLIENT_ASSERT(message, assertion) do { if (!(assertion)) { \
  AT_CLIENT_PLATFORM_ASSERT(message); }} while(0)


//历史记录保存记录数
#define AT_STATE_HIS_SIZE 16

//每条CMD前执行的共有间隔时间
#define DEFAULT_FIRST_EXEC_INTERVAL 100


//结尾标识符
#define LINE_MARK "\r\n"
//结尾标识符长度
#define LINE_MARK_LEN 2 //有的是"\r\r\n" 初始化at_client_recv_exec_init那里核对很重要
//发送结尾表示符调用
#define SEND_LINE_MARK at_client_port_output((uint8_t*)LINE_MARK,LINE_MARK_LEN)
//前行和后行除了最大的特征长度额外预留充足长度
#define PRV_NEW_LINE_ADDITIONAL  (LINE_MARK_LEN + 1 + 1 + 50) //包含尾行标志 有的尾行多一个\r 包含一个0 加个额外

//串口中断输入循环列队数据长度，使用DMA时，一帧长度尽量和DMA长度保持一致
#define AT_CLIENT_PORT_INPUT_LOOPQUEUE_SIZE 1500
//接收处理缓冲区大小 用于上一行和本行，富余用于转发，转发的时候都是一个字符一个字符的转发，此长度只影响分析据子的缓存区
#define AT_CLIENT_RECV_EXEC_BUF_SIZE   512



//初始化完成第一次进入的状态 如上电就进入注网过程
#define INITIALIZATION_COMPLETE_STATE_ENTER state_enter_AT()
//#define INITIALIZATION_COMPLETE_STATE_ENTER state_enter_REBOOT()

//该状态下不在进行CMD执行 效果同STATE_NON 如果没有就填STATE_NON 在CMD测也可以使用阻塞时间 增加这个功能就是减少不必要的延时检测
#define NOT_EXEC_CMD_STATE TC
//启用转发功能
#define USE_RELAY_STATE 1
//注网成功后会进入可转发数据状态 进入状态延时在CMD中自行增加yu
#define RELAY_STATE TC
//转发状态下的转发函数
#define RELAY_RECV_FN(CHAR) recv_touchuan(CHAR)
//进入转发状态执行一次
#define RELAY_STATE_ENTER() enter_touchuan();
//退出转发状态执行一次
#define RELAY_STATE_QUIT() quit_touchuan();

//使用RTOS
#define USE_RTOS 0
#if USE_RTOS
#include <rtthread.h>
//实现延时函数
#define AT_CLIENT_MDELAY_M(MS) rt_thread_mdelay(MS);
#endif

#endif /* _AT_CLIENT_CFG_H_ */
