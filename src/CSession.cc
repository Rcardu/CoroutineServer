/*
 * @Author: Ricardo
 * @Date: 2024-05-09 12:13:27
 * @Last Modified by: ICEY
 * @Last Modified time: 2024-05-09 15:37:51
 */
#include "CSession.h"
#include "Const.h"
#include "LogicSystem.h"
#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/detail/socket_ops.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/write.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <iostream>
#include <memory>
#include <utility>

namespace ICEY {

CSession::CSession(boost::asio::io_context &io_context, CServer *server)
    : m_io_context(io_context), m_server(server), m_socket(io_context),
      m_uid(boost::uuids::to_string(boost::uuids::random_generator()())),
      m_recv_head_node(std::make_shared<MsgNode>(HEAD_TOTAL_LEN)) {}

CSession::~CSession() {
  try {
    std::cout << "~CSession destruct " << std::endl;
    Close();
  } catch (std::exception &e) {
    std::cout << "exception is " << e.what() << std::endl;
  }
}

boost::asio::ip::tcp::socket &CSession::getSocket() { return m_socket; }

void CSession::Start() {
  auto shared_this = shared_from_this();
  // 开启协程接收
  boost::asio::co_spawn(
      m_io_context,
      [=, this]() -> boost::asio::awaitable<void> {
        try {
          for (; !m_is_close;) {
            m_recv_head_node->Clear();
            std::size_t ret = co_await boost::asio::async_read(
                m_socket,
                boost::asio::buffer(m_recv_head_node->getData(),
                                    HEAD_TOTAL_LEN),
                boost::asio::use_awaitable);
            if (ret == 0) {
              std::cout << "receive peer closed" << std::endl;
              Close();
              m_server->clearSession(m_uid);
              co_return;
            }
            // 获取头部MSGID数据
            int16_t msg_id = 0;
            memcpy(&msg_id, m_recv_head_node->getData(), HEAD_ID_LEN);
            // 网络字节序转本地字节序
            msg_id =
                boost::asio::detail::socket_ops::network_to_host_short(msg_id);
            std::cout << "msg_id is " << msg_id << std::endl;
            if (msg_id > MAX_LENGTH) {
              std::cout << "invalid msg id is " << msg_id << std::endl;
              Close();
              m_server->clearSession(m_uid);

              co_return;
            }

            int16_t msg_len{0};
            memcpy(&msg_len, m_recv_head_node->getData() + HEAD_ID_LEN,
                   HEAD_DATA_LEN);
            msg_len =
                boost::asio::detail::socket_ops::network_to_host_short(msg_len);
            std::cout << "msg len is " << msg_len << std::endl;
            if (msg_len > MAX_LENGTH) {
              std::cout << "invalid msg length is " << msg_len << std::endl;
              Close();
              m_server->clearSession(m_uid);

              co_return;
            }

            m_recv_msg_node = std::make_shared<RecvNode>(msg_len, msg_id);

            // 读出包体
            ret = co_await boost::asio::async_read(
                m_socket,
                boost::asio::buffer(m_recv_msg_node->getData(),
                                    m_recv_msg_node->getTotal()),
                boost::asio::use_awaitable);
            if (ret == 0) {
              std::cout << "recvive peer closed " << std::endl;
              Close();
              m_server->clearSession(m_uid);

              co_return;
            }
            m_recv_head_node->setChar();
            std::cout << "receive data is " << m_recv_msg_node->getData()
                      << std::endl;

            // 投递逻辑线程处理
            LogicSystem::GetInstance().postMsgToQue(std::make_shared<LogicNode>(
                shared_from_this(), m_recv_msg_node));
          }
        } catch (std::exception &e) {
          std::cerr << "Exception: " << e.what() << std::endl;
          Close();
          m_server->clearSession(m_uid);
        }
      },
      boost::asio::detached);
}
void CSession::Close() {
  m_socket.close();
  m_is_close = true;
}
void CSession::Send(const char *msg, int16_t max_length, int16_t msgid) {
  std::unique_lock<std::mutex> lock(m_send_lock);
  int send_que_size = m_send_que.size();
  if (send_que_size > MAX_SENDQUE) {
    std::cout << "session: " << m_uid << " send que fulled, size is "
              << MAX_SENDQUE << std::endl;
    return;
  }
  m_send_que.push(std::make_shared<SendNode>(msg, max_length, msgid));
  if (send_que_size > 0) {
    return;
  }
  auto msgnode = m_send_que.front();
  lock.unlock();
  boost::asio::async_write(
      m_socket, boost::asio::buffer(msgnode->getData(), msgnode->getTotal()),
      [this](auto &&PH1, auto &&) {
        handleWrite(std::forward<decltype(PH1)>(PH1), shared_from_this());
      });
}
void CSession::Send(const std::string &msg, int16_t msgid) {
  Send(msg.c_str(), msg.length(), msgid);
}
void CSession::handleWrite(const boost::system::error_code &error,
                           std::shared_ptr<CSession> shared_self) {
  try {
    if (!error) {
      std::unique_lock<std::mutex> lock(m_send_lock);
      m_send_que.pop();
      if (!m_send_que.empty()) {
        auto msgnode = m_send_que.front();
        lock.unlock();
        boost::asio::async_write(
            m_socket,
            boost::asio::buffer(msgnode->getData(), msgnode->getTotal()),
            [this](auto &&PH1, auto &&) {
              handleWrite(std::forward<decltype(PH1)>(PH1), shared_from_this());
            });
      }
    } else {
      std::cout << "handle write failed, error is " << error.what()
                << std::endl;
      Close();
      m_server->clearSession(m_uid);
    }

  } catch (std::exception &e) {
    std::cout << "Exception: " << e.what() << std::endl;
    Close();
    m_server->clearSession(m_uid);
  }
}
LogicNode::LogicNode(std::shared_ptr<CSession> session,
                     std::shared_ptr<RecvNode> recvnode)
    : m_session(session), m_recvnode(recvnode) {}

} // namespace ICEY
