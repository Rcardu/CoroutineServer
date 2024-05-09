/*
 * @Author: Ricardo
 * @Date: 2024-05-09 12:04:08
 * @Last Modified by: ICEY
 * @Last Modified time: 2024-05-09 14:27:26
 */
#pragma once
#include "CServer.h"
#include "MsgNode.h"
#include <Const.h>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/system.hpp>
#include <memory>
#include <queue>

namespace ICEY {
class CServer;

class CSession : public std::enable_shared_from_this<CSession> {
public:
  CSession(boost::asio::io_context &io_context, CServer *server);
  ~CSession();

  boost::asio::ip::tcp::socket &getSocket();
  const std::string &getUid() const { return m_uid; }
  void Start();
  void Close();
  void Send(const char *msg, int16_t max_length, int16_t msgid);
  void Send(const std::string &msg, int16_t msgid);

private:
  void handleWrite(const boost::system::error_code &error,
                   std::shared_ptr<CSession> shared_self);

  boost::asio::io_context &m_io_context;
  CServer *m_server;
  boost::asio::ip::tcp::socket m_socket;
  std::string m_uid;
  bool m_is_close{false};
  std::mutex m_send_lock;
  std::queue<std::shared_ptr<SendNode>> m_send_que;
  std::shared_ptr<RecvNode> m_recv_msg_node;
  std::shared_ptr<MsgNode> m_recv_head_node;
};

class LogicNode {
public:
  LogicNode(std::shared_ptr<CSession> session,
            std::shared_ptr<RecvNode> recvnode);
  std::shared_ptr<CSession> m_session;
  std::shared_ptr<RecvNode> m_recvnode;
};
} // namespace ICEY
