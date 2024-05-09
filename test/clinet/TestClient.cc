/*
 * @Author: Ricardo
 * @Date: 2024-05-09 15:14:24
 * @Last Modified by: ICEY
 * @Last Modified time: 2024-05-09 15:39:07
 */
#include "Const.h"
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <iostream>
#include <json/json.h>
#include <json/reader.h>
#include <json/value.h>

using namespace std;
using namespace boost::asio::ip;

std::vector<std::thread> vec_threads;
int main() {

  auto start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < 100; i++) {
    vec_threads.emplace_back([]() {
      try {
        boost::asio::io_context ioc;
        tcp::endpoint ep(address::from_string("127.0.0.1"), 10086);
        tcp::socket sock(ioc);
        boost::system::error_code error = boost::asio::error::host_not_found;
        sock.connect(ep, error);
        if (error) {
          cout << "connect failed, code is " << error.value()
               << " error msg is " << error.message();
          return 0;
        }
        int i = 0;
        while (i < 500) {
          Json::Value root;
          root["id"] = 1001;
          root["data"] = "hello world";
          std::string request = root.toStyledString();
          size_t request_length = request.length();
          char send_data[MAX_LENGTH] = {0};
          int16_t msgid = 1001;
          int msgid_host =
              boost::asio::detail::socket_ops::host_to_network_short(msgid);
          memcpy(send_data, &msgid_host, HEAD_ID_LEN);
          ;
          // 转为网络字节序
          int request_host_length =
              boost::asio::detail::socket_ops::host_to_network_short(
                  request_length);
          memcpy(send_data + HEAD_ID_LEN, &request_host_length, HEAD_DATA_LEN);
          memcpy(send_data + HEAD_TOTAL_LEN, request.c_str(), request_length);
          boost::asio::write(
              sock,
              boost::asio::buffer(send_data, request_length + HEAD_TOTAL_LEN));
          std::cout << "begin to receive..." << std::endl;
          char reply_head[HEAD_TOTAL_LEN];
          size_t reply_length = boost::asio::read(
              sock, boost::asio::buffer(reply_head, HEAD_TOTAL_LEN));
          msgid = 0;
          memcpy(&msgid, reply_head, HEAD_ID_LEN);
          int16_t msglen = 0;
          memcpy(&msglen, reply_head + HEAD_ID_LEN, HEAD_DATA_LEN);
          // 转为本地字节序
          msgid = boost::asio::detail::socket_ops::network_to_host_short(msgid);
          msglen =
              boost::asio::detail::socket_ops::network_to_host_short(msglen);
          char msg[MAX_LENGTH] = {0};
          size_t msg_length =
              boost::asio::read(sock, boost::asio::buffer(msg, msglen));
          Json::Reader reader;
          reader.parse(std::string(msg, msg_length), root);
          std::cout << "msg id is " << root["id"] << " msg is " << root["data"]
                    << std::endl;
          i++;
        }

      } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
      }
      return 0;
    });
    std::this_thread::sleep_for(std::chrono::microseconds(10));
  }
  for (auto &thd : vec_threads) {
    thd.join();
  }
  auto end = std::chrono::high_resolution_clock::now();

  auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);

  std::cout << "Time spent " << duration.count() << "seconds\n";
  getchar();

  return 0;
}
