/*
 * @Author: Ricardo
 * @Date: 2024-05-09 11:43:24
 * @Last Modified by: ICEY
 * @Last Modified time: 2024-05-09 12:05:35
 */
#include "AsioIOServicePool.h"
#include <iostream>
#include <memory>

namespace ICEY {
AsioIOServicePool::AsioIOServicePool(std::size_t size)
    : m_ioServices(size), m_works(size), m_next_ioService_idx(0) {
  for (std::size_t i = 0; i < size; ++i) {
    m_works[i] = std::make_unique<Work>(m_ioServices[i]);
  }
  // 遍历多个ioservice，创建多个线程，每个线程内部启动ioservice
  for (auto &ioService : m_ioServices) {
    m_threads.emplace_back([&ioService] { ioService.run(); });
  }
}

AsioIOServicePool::~AsioIOServicePool() {
  std::cout << "AsioIOService Pool destruct " << std::endl;
}

boost::asio::io_context &AsioIOServicePool::GetIOService() {
  // 轮询
  auto &service = m_ioServices[m_next_ioService_idx++];
  if (m_next_ioService_idx == m_ioServices.size()) {
    m_next_ioService_idx = 0;
  }

  return service;
}
void AsioIOServicePool::Stop() {
  for (auto &work : m_works) {
    work.reset();
  }
  for (auto &thd : m_threads) {
    thd.join();
  }
}

AsioIOServicePool &AsioIOServicePool::GetInstance() {
  static AsioIOServicePool instance(1);
  return instance;
}

} // namespace ICEY
