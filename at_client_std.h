

#ifdef AT_CMD
//AT_CMD(进行状态, 简单的发送CMD, 执行函数, 执行函数2, 第一次执行间隔（延时）, 可以定时执行次数（至少执行了frist），定时执行间隔， 超时函数, 超时函数2, 允许超时次数（为0失效）, 不允许超时次数执行一次, 不允许超时次数执行一次2)
//AT_CMD(_state, _simpleCMD, _exec, _exec2, _first_exec_interval_tick, _exec_sum, _exec_interval_tick, _exec_timeout, _exec_timeout2, _allow_timeout_sum, _exec_non_allow_timeout, _exec_non_allow_timeout2)
//_exec_sum可以定时执行次数（至少执行了frist） exec_sum值为0和1的时候效果是一样的
//在 exec_timeout1 2为NULL时 相当于exec_timeout_count会迅速增长 快速进入_exec_non_allow_timeout1 2 效果相当于_allow_timeout_sum=1
//当_allow_timeout_sum=0时永远不会进入快速进入_exec_non_allow_timeout1 2
//ATI命令用于测试回调2

AT_CMD(REBOOT, "0", at_client_OFF, FN_NULL, DEFAULT_FIRST_EXEC_INTERVAL, 1, 2000, at_client_ON, FN_NULL, 2, state_enter_AT, FN_NULL) //allow_timeout_sum必须为2； 1是执行了at_client_ON统计+1； exec_non_allow_timeout不能拦截1只能拦截2
AT_CMD(AT, "AT", send_simple_cmd_AT, FN_NULL, DEFAULT_FIRST_EXEC_INTERVAL, 10, 3000, state_enter_REBOOT, FN_NULL, 0, FN_NULL, FN_NULL)
AT_CMD(ATI, "ATI", FN_NULL, send_simple_cmd_ATI, DEFAULT_FIRST_EXEC_INTERVAL, 5, 1000, FN_NULL, state_enter_REBOOT, 0, FN_NULL ,FN_NULL)
AT_CMD(ATCPIN, "AT+CPIN?", send_simple_cmd_ATCPIN, FN_NULL, DEFAULT_FIRST_EXEC_INTERVAL, 20, 1000, state_enter_REBOOT, FN_NULL, 0, FN_NULL ,FN_NULL)
AT_CMD(ATCGSN, "AT+CGSN", send_simple_cmd_ATCGSN, FN_NULL, DEFAULT_FIRST_EXEC_INTERVAL, 5, 1000, state_enter_REBOOT, FN_NULL, 0, FN_NULL ,FN_NULL)
AT_CMD(ATQCCID, "AT+QCCID", send_simple_cmd_ATQCCID, FN_NULL, DEFAULT_FIRST_EXEC_INTERVAL, 5, 1000, state_enter_REBOOT, FN_NULL, 0, FN_NULL ,FN_NULL)
AT_CMD(ATCREG, "AT+CREG?", send_simple_cmd_ATCREG, FN_NULL, DEFAULT_FIRST_EXEC_INTERVAL, 90, 1000, state_enter_REBOOT, FN_NULL, 0, FN_NULL ,FN_NULL)
/* 进入PPP模式的命令
AT_CMD(ATCGREG, "AT+CGREG?", send_simple_cmd_ATCGREG, FN_NULL, DEFAULT_FIRST_EXEC_INTERVAL, 60, 1000, state_enter_ATCGDCONT, FN_NULL, 0, FN_NULL ,FN_NULL)
AT_CMD(ATCGDCONT, "AT+CGDCONT=1,\"IP\",\"CMNET\"", send_simple_cmd_ATCGDCONT, FN_NULL, DEFAULT_FIRST_EXEC_INTERVAL, 5, 1000, state_enter_REBOOT, FN_NULL, 0, FN_NULL ,FN_NULL)
AT_CMD(ATD99, "ATD*99#", send_simple_cmd_ATD99, FN_NULL, DEFAULT_FIRST_EXEC_INTERVAL, 5, 1000, state_enter_REBOOT, FN_NULL, 0, FN_NULL ,FN_NULL)
AT_CMD(PPPPRV, "0", FN_NULL, state_enter_PPP, 1000, 0, 1000, FN_NULL, FN_NULL, 0, FN_NULL ,FN_NULL)
AT_CMD(PPP, "0", FN_NULL, FN_NULL, FN_BLOCK, FN_BLOCK, FN_BLOCK, FN_NULL, FN_NULL, 0, FN_NULL ,FN_NULL)
*/
/* 进入透传模式 */
AT_CMD(ATCGREG, "AT+CGREG?", send_simple_cmd_ATCGREG, FN_NULL, DEFAULT_FIRST_EXEC_INTERVAL, 60, 1000, state_enter_REBOOT, FN_NULL, 0, FN_NULL ,FN_NULL)
AT_CMD(ATQIOPEN, "0", open_touchuan, FN_NULL, DEFAULT_FIRST_EXEC_INTERVAL, 3, 5000, state_enter_REBOOT, FN_NULL, 0, FN_NULL ,FN_NULL)

AT_CMD(TC, "0", FN_NULL, FN_NULL, FN_BLOCK, FN_BLOCK, FN_BLOCK, FN_NULL, FN_NULL, 0, FN_NULL ,FN_NULL)



//查询信号质量
//AT_CMD(状态, at简单指令, 发送at简单指令1函数, 发送at指令2函数, 第一次执行离上次执行间隔, 可以执行次数最小1, 每次执行间隔, 超时函数1, 超时函数2, 允许超时次数, 超时超限次数函数1 ,超时超限次数函数2)
AT_CMD(ATCSQ, "AT+CSQ", send_simple_cmd_ATCSQ, FN_NULL, DEFAULT_FIRST_EXEC_INTERVAL, 2, 1000, FN_NULL, FN_NULL, 2, at_csq_return ,FN_NULL)

#endif /* #ifdef AT_CMD */
#undef AT_CMD

//state_enter_STATE_NON 回归默认

#ifdef AT_FEATURES
//特征不能为1个‘0’字符 0字符会将特征改为空，需要保证所有特征唯一 不唯一会命中第一条特征
//AT_FEATURES(特征枚举名字(唯一)，该状态下检查特征, 特征1, 特征2,回调函数)
//AT_FEATURES(_enum_name, _state, _features1, _features2, _cb, _cb2)
AT_FEATURES(AT_OK, AT, "AT", "OK", state_enter_ATI, FN_NULL)
AT_FEATURES(ATI_OK, ATI, "0", "OK", FN_NULL, state_enter_ATCPIN)
AT_FEATURES(ATCPIN_OK, ATCPIN, "0", "+CPIN: READY", state_enter_ATCGSN, FN_NULL)
AT_FEATURES(ATCGSN_OK, ATCGSN, "AT+CGSN", "0", FN_NULL, state_enter_ATQCCID)
AT_FEATURES(ATQCCID_OK, ATQCCID, "0", "+QCCID:", FN_NULL, state_enter_ATCREG)
AT_FEATURES(ATCREG_OK, ATCREG, "+CREG: 0,1", "0", FN_NULL, state_enter_ATCGREG)
AT_FEATURES(ATCREG2_OK, ATCREG, "+CREG: 0,5", "0", FN_NULL, state_enter_ATCGREG)
/* 进入PPP模式的命令
AT_FEATURES(ATCGREG_OK, ATCGREG, "+CGREG: 0,1", "0", FN_NULL, state_enter_ATCGDCONT)
AT_FEATURES(ATCGREG2_OK, ATCGREG, "+CGREG: 0,5", "0", FN_NULL, state_enter_ATCGDCONT)
AT_FEATURES(ATCGDCONT_OK, ATCGDCONT, "AT+CGDCONT=1", "0", FN_NULL, state_enter_ATD99)
AT_FEATURES(ATD99_OK, ATD99, "0", "CONNECT", FN_NULL, state_enter_PPPPRV)
*/
/* 进入透传模式 */
AT_FEATURES(ATCGREG_OK, ATCGREG, "+CGREG: 0,1", "0", FN_NULL, state_enter_ATQIOPEN)
AT_FEATURES(ATCGREG2_OK, ATCGREG, "+CGREG: 0,5", "0", FN_NULL, state_enter_ATQIOPEN)
AT_FEATURES(ATQIOPEN_OK, ATQIOPEN, "0", "CONNECT", FN_NULL, state_enter_TC)

//得到信号质量 知道信号稳定才会跳出这一步
AT_FEATURES(ATCSQ_OK, ATCSQ, "0", "+CSQ: ", at_csq_get, FN_NULL)

#endif
#undef AT_FEATURES


