/*
 * @Author: Ricardo
 * @Date: 2024-05-09 11:58:58
 * @Last Modified by: ICEY
 * @Last Modified time: 2024-05-09 12:35:44
 */

#include "CServer.h"
#include "AsioIOServicePool.h"
#include <iostream>
#include <mutex>
#include <utility>

namespace ICEY {

CServer::CServer(boost::asio::io_context &io_context, int16_t port)
    : m_io_context(io_context), m_port(port),
      m_acceptor(io_context, boost::asio::ip::tcp::endpoint(
                                 boost::asio::ip::tcp::v4(), port)) {
  startAccept();
}
CServer::~CServer() {}

void CServer::clearSession(const std::string &uid) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_sessions.erase(uid);
}
void CServer::handleAccept(std::shared_ptr<CSession> new_session,
                           const boost::system::error_code &error) {
  if (!error) {
    new_session->Start();
    std::lock_guard<std::mutex> lock(m_mutex);
    m_sessions.insert(std::make_pair(new_session->getUid(), new_session));
  } else {
    std::cout << "Session accept failed, error is " << error.what()
              << std::endl;
  }
  startAccept();
}
void CServer::startAccept() {
  auto &io_context = AsioIOServicePool::GetInstance().GetIOService();
  auto new_session = std::make_shared<CSession>(io_context, this);
  m_acceptor.async_accept(
      new_session->getSocket(), [this, new_session](auto &&PH1) {
        handleAccept(new_session, std::forward<decltype(PH1)>(PH1));
      });
}

} // namespace ICEY
