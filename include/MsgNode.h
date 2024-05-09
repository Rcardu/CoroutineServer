/*
 * @Author: Ricardo
 * @Date: 2024-05-09 11:09:15
 * @Last Modified by: ICEY
 * @Last Modified time: 2024-05-09 14:49:39
 */

#pragma once
#include "Const.h"
#include <cstdint>
#include <iostream>
#include <string>

namespace ICEY {
class MsgNode {
public:
  MsgNode(int16_t max_len)
      : m_total_len(max_len), m_data(new char[m_total_len + 1]) {
    m_data[m_total_len] = '\0';
  }
  ~MsgNode() {
    std::cout << "destruct MsgNode" << std::endl;
    delete[] m_data;
  }
  void Clear() {
    ::memset(m_data, 0, m_total_len);
    m_cur_len = 0;
  }

  void setChar() { m_data[m_total_len] = '\0'; }
  void serChar(int idx, char ch) { m_data[idx] = ch; }

  [[nodiscard]] char *getData() const { return m_data; }
  [[nodiscard]] int16_t getTotal() const { return m_total_len; }
  [[nodiscard]] int16_t getCur() const { return m_cur_len; }

protected:
  int16_t m_cur_len{0};
  int16_t m_total_len;
  char *m_data;
};

class RecvNode : public MsgNode {
public:
  RecvNode(int16_t max_len, int16_t msg_id);

  [[nodiscard]] int16_t getId() const { return m_msg_id; }

private:
  int16_t m_msg_id;
};
class SendNode : public MsgNode {
public:
  SendNode(const char *msg, int16_t max_len, int16_t msg_id);

  [[nodiscard]] int16_t getid() const { return m_msg_id; }

private:
  int16_t m_msg_id;
};

} // namespace ICEY
