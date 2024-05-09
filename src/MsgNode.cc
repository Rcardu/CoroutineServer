/*
 * @Author: Ricardo
 * @Date: 2024-05-09 11:14:09
 * @Last Modified by: ICEY
 * @Last Modified time: 2024-05-09 12:50:14
 */
#include "MsgNode.h"
#include "boost/asio/detail/impl/socket_ops.ipp"
#include <cstring>

namespace ICEY {
RecvNode::RecvNode(int16_t max_len, int16_t msg_id)
    : MsgNode(max_len), m_msg_id(msg_id) {}

SendNode::SendNode(const char *msg, int16_t max_len, int16_t msg_id)
    : MsgNode(max_len + HEAD_TOTAL_LEN), m_msg_id(msg_id) {
  // 先发送id，转为网络字节序
  int16_t msg_id_net =
      boost::asio::detail::socket_ops::host_to_network_short(msg_id);
  memcpy(m_data, &msg_id_net, HEAD_DATA_LEN);
  // 再发送数据，转为网络字节序
  int16_t max_len_net =
      boost::asio::detail::socket_ops::host_to_network_short(max_len);
  memcpy(m_data + HEAD_ID_LEN, &max_len_net, HEAD_DATA_LEN);
  memcpy(m_data + HEAD_TOTAL_LEN, msg, max_len);
}

}; // namespace ICEY
