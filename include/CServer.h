/*
 * @Author: Ricardo
 * @Date: 2024-05-09 11:59:03
 * @Last Modified by: ICEY
 * @Last Modified time: 2024-05-09 12:48:20
 */
#pragma once
#include "CSession.h"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/system/detail/error_code.hpp>
#include <map>
#include <memory.h>
#include <mutex>

namespace ICEY {
class CSession;

class CServer {
public:
  CServer(boost::asio::io_context &io_context, int16_t port);
  ~CServer();

  void clearSession(const std::string &uid);

private:
  void handleAccept(std::shared_ptr<CSession>,
                    const boost::system::error_code &error);
  void startAccept();

  boost::asio::io_context &m_io_context;
  int16_t m_port;
  boost::asio::ip::tcp::acceptor m_acceptor;
  std::map<std::string, std::shared_ptr<CSession>> m_sessions;
  std::mutex m_mutex;
};
} // namespace ICEY
