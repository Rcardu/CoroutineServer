/*
 * @Author: Ricardo
 * @Date: 2024-05-09 12:40:10
 * @Last Modified by: ICEY
 * @Last Modified time: 2024-05-09 12:46:48
 */
#include "AsioIOServicePool.h"
#include "CServer.h"
#include "boost/asio/signal_set.hpp"
#include <boost/asio/io_context.hpp>
#include <csignal>
#include <iostream>
#include <thread>

int main() {
  try {
    auto &pool = ICEY::AsioIOServicePool::GetInstance();
    boost::asio::io_context ioc;
    boost::asio::signal_set signal(ioc, SIGINT, SIGTERM);
    signal.async_wait([&ioc, &pool](auto, auto) {
      ioc.stop();
      pool.Stop();
    });
    ICEY::CServer s(ioc, 10086);
    ioc.run();

  } catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << std::endl;
  }
}
