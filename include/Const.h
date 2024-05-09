/*
 * @Author: Ricardo
 * @Date: 2024-05-09 11:09:48
 * @Last Modified by: ICEY
 * @Last Modified time: 2024-05-09 11:11:56
 * @Title: 常量头文件
 */
#pragma once
#define MAX_LENGTH 1024 * 2
#define HEAD_TOTAL_LEN 4
#define HEAD_ID_LEN 2
#define HEAD_DATA_LEN 2
#define MAX_RECVQUE 10000
#define MAX_SENDQUE 1000

enum MSG_IDS { MSG_HELLO_WORD = 1001 };
