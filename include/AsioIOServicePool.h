/*
 * @Author: Ricardo
 * @Date: 2024-05-09 11:37:12
 * @Last Modified by: ICEY
 * @Last Modified time: 2024-05-09 12:04:51
 */

#pragma once
#include "boost/asio/io_context.hpp"
#include "boost/asio/io_service.hpp"
#include <vector>

namespace ICEY {
class AsioIOServicePool {
  using IOService = boost::asio::io_context;
  using Work = boost::asio::io_context::work;
  using WorkPtr = std::unique_ptr<Work>;

public:
  ~AsioIOServicePool();
  AsioIOServicePool(const AsioIOServicePool &) = delete;
  AsioIOServicePool &operator=(const AsioIOServicePool &) = delete;

  boost::asio::io_context &GetIOService();
  void Stop();
  static AsioIOServicePool &GetInstance();

private:
  explicit AsioIOServicePool(
      std::size_t size = std::thread::hardware_concurrency());

  std::vector<IOService> m_ioServices;
  std::vector<WorkPtr> m_works;
  std::vector<std::thread> m_threads;
  std::size_t m_next_ioService_idx;
};
} // namespace ICEY
