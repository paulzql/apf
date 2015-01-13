/*!**************************************************************************
 * @file
 * @brief 跨平台基本函数
 *
 * Copyright (C) 2013 - 2013, Paul Zhou, <qianlong.zhou@gmail.com>.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/
#ifndef OSCORE_H
#define OSCORE_H

#include <time.h>

//#define BIT64		//64Î»

#if defined(WIN32) || defined(WINCE)	//windows / wince
#include <windows.h>
    #ifdef _MSC_VER
        #ifdef WINCE
//            #pragma comment(lib, "WS2.LIB")
        #else
//            #pragma comment(lib, "WS2_32.LIB")
        #endif
        #if (_MSC_VER < 1300)
           typedef unsigned int      uint32_t;
        #else
           typedef unsigned __int32  uint32_t;
        #endif
        typedef unsigned __int64     uint64_t;
        typedef unsigned short       uint16_t;
    #else
        #include <inttypes.h>
    #endif
//    #include <WinSock2.h>

    #define WIN_THREAD_FUNC LPTHREAD_START_ROUTINE
    #define pthread_mutex_t     CRITICAL_SECTION
    #define sem_t				HANDLE
    #define pthread_t           HANDLE
    #define socklen_t			int
    #define msleep	Sleep
#else //linux / mac os / android / ios
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
    #include <errno.h>
    #include <sys/stat.h>
    #include <sys/time.h>
    #include <sys/types.h>
    #include <pthread.h>
    #include <semaphore.h>
    #include <inttypes.h>
    #include <fcntl.h>
    typedef unsigned int SOCKET;
    #define INVALID_SOCKET	-1
    #define closesocket ::close
    #define msleep(t)	usleep(t*1000)
    #define INFINITE    0x0FFFFFFF
#endif

/**
 * 大端/小端CPU测试类型
 */
typedef union _UEndianTest{
    char little_endian;
    long value;
}UEndianTest;

/**
 * 大端/小端CPU测试对象
 */
extern UEndianTest g_endianTest;

#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN	g_endianTest.little_endian
#endif

/**
 * 将32位数字转换为网络字节序
 */
#define NET32(n) g_endianTest.little_endian ? ((n>>24)|((n>>8)&0xFF00)|((n<<8)&0xFF0000)|(n<<24)):n
/**
 * 将16位数字转换为网络字节序
 */
#define NET16(n) g_endianTest.little_endian ? (((n>>8)&0x00FF)|((n<<8)&0xFF00)):n
////////

/**
 * 线程函数类型
 */
typedef void* (*THREAD_FUNC)(void*);

////functions//////
/**
 * 初始化互斥锁
 * @param[in,out] mutex 互斥锁指针
 * @return 初始化状态(0: 成功， 其它：失败)
 */
int init_mutex(pthread_mutex_t *mutex);

/**
 * 释放互斥锁
 * @param[in] mutex 互斥锁指针
 * @return 状态(0: 成功， 其它：失败)
 */
int uninit_mutex(pthread_mutex_t *mutex);

/**
 * 互斥锁加锁
 * @param[in] mutex 互斥锁指针
 * @return 状态(0: 成功， 其它：失败)
 */
int lock_mutex(pthread_mutex_t *mutex);

/**
 * 互斥锁解锁
 * @param[in] mutex 互斥锁指针
 * @return 状态(0: 成功， 其它：失败)
 */
int unlock_mutex(pthread_mutex_t *mutex);

/**
 * 初始化信号量
 * @param[in,out] sem 信号量指针
 * @param[in] initcount 初始信号数量
 * @return 初始化状态(0: 成功， 其它：失败)
 */
int init_semaphore(sem_t *sem, unsigned int initcount);

/**
 * 释放信号量
 * @param[in] sem 信号量指针
 * @return 状态(0: 成功， 其它：失败)
 */
int uninit_semaphore(sem_t *sem);

/**
 * 等待信号
 * @param[in] sem 信号量指针
 * @param[in] waittime 等待时间（毫秒)
 * @return 状态(0: 成功， 其它：失败)
 */
int wait_semaphore(sem_t	*sem, unsigned long waittime);

/**
 * 增加信号
 * @param[in] sem 信号量指针
 * @return 状态(0: 成功， 其它：失败)
 */
int post_semaphore(sem_t  *sem);

/**
 * 创建线程
 * @param[out] pthread 线程指针
 * @param[in] func 线程执行函数
 * @param[in] arg 线程参数
 * @return 状态(0: 成功， 其它：失败)
 */
int begin_thread(pthread_t *pthread, THREAD_FUNC func, void *arg);

/**
 * 等待线程结束
 * @param[out] pthread 线程指针
 * @return 状态(0: 成功， 其它：失败)
 */
int wait_thread(pthread_t *pthread);

/**
 * 随机生成一个数字
 * @return 随机数
 */
int random_int();

/**
 * 获取CPU时间(毫秒)
 * @return CPU时间
 */
unsigned long clock_tick();

/**
 * 释放CPU使用权
 */
void yield();

#endif // OSCORE_H
