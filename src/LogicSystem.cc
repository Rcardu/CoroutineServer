/*
 * @Author: Ricardo
 * @Date: 2024-05-09 14:38:28
 * @Last Modified by: ICEY
 * @Last Modified time: 2024-05-09 15:04:56
 */
#include "LogicSystem.h"
#include "Const.h"
#include <json/json.h>
#include <json/reader.h>
#include <json/value.h>

namespace ICEY {
LogicSystem::LogicSystem() {
  registerCallBacks();
  m_work_thread = std::thread(&LogicSystem::dealMsg, this);
}
LogicSystem::~LogicSystem() {
  is_stop = true;
  m_consume.notify_one();
  m_work_thread.join();
}
LogicSystem &LogicSystem::GetInstance() {
  static LogicSystem instance;
  return instance;
}

void LogicSystem::postMsgToQue(std::shared_ptr<LogicNode> msg) {
  std::unique_lock<std::mutex> unique_lk(m_mutex);
  m_msg_que.push(msg);
  if (m_msg_que.size() == 1) {
    m_consume.notify_one();
  }
}
void LogicSystem::dealMsg() {
  for (;;) {
    std::unique_lock<std::mutex> unique_lk(m_mutex);
    while (m_msg_que.empty() && !is_stop) {
      m_consume.wait(unique_lk);
    }
    if (is_stop) {
      while (!m_msg_que.empty()) {
        auto msg_node = m_msg_que.front();
        std::cout << "recv msg id is " << msg_node->m_recvnode->getId()
                  << std::endl;
        auto call_back_iter =
            m_fun_callbacks.find(msg_node->m_recvnode->getId());
        if (call_back_iter == m_fun_callbacks.end()) {
          m_msg_que.pop();
          continue;
        }
        call_back_iter->second(msg_node->m_session,
                               msg_node->m_recvnode->getId(),
                               std::string(msg_node->m_recvnode->getData(),
                                           msg_node->m_recvnode->getTotal()));
        m_msg_que.pop();
      }
      break;
    }
    // 没有停服，说明队列中有数据
    auto msg_node = m_msg_que.front();
    std::cout << "recv_msg id is " << msg_node->m_recvnode->getId()
              << std::endl;
    auto call_back_iter = m_fun_callbacks.find(msg_node->m_recvnode->getId());
    if (call_back_iter == m_fun_callbacks.end()) {
      m_msg_que.pop();
      continue;
    }
    call_back_iter->second(msg_node->m_session, msg_node->m_recvnode->getId(),
                           std::string(msg_node->m_recvnode->getData(),
                                       msg_node->m_recvnode->getTotal()));
    m_msg_que.pop();
  }
}
void LogicSystem::registerCallBacks() {
  m_fun_callbacks[MSG_HELLO_WORD] = [this](auto &&PH1, auto &&PH2, auto &&PH3) {
    helloWroldCallBack(std::forward<decltype(PH1)>(PH1),
                       std::forward<decltype(PH2)>(PH2),
                       std::forward<decltype(PH3)>(PH3));
  };
}
void LogicSystem::helloWroldCallBack(std::shared_ptr<CSession> session,
                                     const int16_t &msg_id,
                                     const std::string &msg_data) {
  Json::Reader reader;
  Json::Value root;
  reader.parse(msg_data, root);
  std::cout << "receive msg id is " << root["id"].asInt() << "msg data is "
            << root["data"].asString() << std::endl;
  root["data"] =
      "server has received msg, msg data is " + root["data"].asString();

  std::string return_str = root.toStyledString();
  session->Send(return_str, root["id"].asInt());
}

} // namespace ICEY
