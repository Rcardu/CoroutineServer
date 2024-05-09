/*
 * @Author: Ricardo
 * @Date: 2024-05-09 14:23:24
 * @Last Modified by: ICEY
 * @Last Modified time: 2024-05-09 14:53:41
 */
#pragma once

#include "CSession.h"
#include "Const.h"

#include <condition_variable>
#include <functional>
#include <map>
#include <queue>
#include <thread>
#include <type_traits>

namespace ICEY {
using FunCallBack =
    std::function<void(std::shared_ptr<CSession>, const int16_t &msg_id,
                       const std::string &msg_data)>;
class LogicSystem {
public:
  LogicSystem(const LogicSystem &) = delete;
  LogicSystem &operator=(const LogicSystem &) = delete;

  ~LogicSystem();

  void postMsgToQue(std::shared_ptr<LogicNode> msg);
  static LogicSystem &GetInstance();

private:
  LogicSystem();

  void dealMsg();
  void registerCallBacks();
  void helloWroldCallBack(std::shared_ptr<CSession> session,
                          const int16_t &msg_id, const std::string &msg_data);

  std::thread m_work_thread;
  std::queue<std::shared_ptr<LogicNode>> m_msg_que;
  std::mutex m_mutex;
  std::condition_variable m_consume;
  bool is_stop{0};
  std::map<int16_t, FunCallBack> m_fun_callbacks;
};
} // namespace ICEY
