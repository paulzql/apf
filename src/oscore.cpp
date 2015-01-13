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
#include "oscore.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif
#ifndef WIN32
//#include <sys/wait.h>
#endif


UEndianTest g_endianTest={(long)1};

#if defined(WIN32) || defined(WINCE)	//windows
int init_mutex(pthread_mutex_t *mutex) {
    InitializeCriticalSection(mutex);
    return 0;
}

int uninit_mutex(pthread_mutex_t *mutex) {
    DeleteCriticalSection(mutex);
    return 0;
}

int lock_mutex(pthread_mutex_t *mutex) {
    EnterCriticalSection(mutex);
    return 0;
}

int unlock_mutex(pthread_mutex_t *mutex) {
    LeaveCriticalSection(mutex);
    return 0;
}

int init_semaphore(sem_t *psem, unsigned int initcount) {
    *psem = CreateSemaphore(NULL, initcount, 0x0FFFFFFF, NULL);
    if(NULL == *psem)
        return -1;
    return 0;
}

int uninit_semaphore(sem_t *psem) {
    return !CloseHandle(*psem);
}

int wait_semaphore(sem_t	*psem, unsigned long waittime) {
    return WaitForSingleObject(*psem, waittime);
}

int post_semaphore(sem_t  *psem) {
    return !ReleaseSemaphore(*psem, 1, NULL);
}

int begin_thread(pthread_t *pthread, THREAD_FUNC func, void *arg) {
    int err;

    *pthread = CreateThread(0, 0,  (WIN_THREAD_FUNC)func, arg, 0, NULL);
    if (NULL == *pthread)     {
        err = GetLastError();
        return err;
    }

    return 0;
}

int wait_thread(pthread_t *pthread) {
    WaitForSingleObject(*pthread, INFINITE);
    CloseHandle(*pthread);
    return 0;
}

unsigned long clock_tick() {
    return GetTickCount();
}

void yield() {
    // zero sleep is bad if we have high priority threads, they
    //  won't relinquish the timeslice for lower priority ones
    ::Sleep(1);
}

#else   /////////////////////////////////////////////////
int init_mutex(pthread_mutex_t *pmutex) {
    pthread_mutexattr_t attr;

    memset(pmutex, 0, sizeof(pthread_mutex_t));
    memset(&attr, 0, sizeof(pthread_mutexattr_t));

    pthread_mutexattr_init(&attr);
    // ÉèÖÃ recursive ÊôÐÔ
    pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);

    return pthread_mutex_init(pmutex, &attr);
}

int uninit_mutex(pthread_mutex_t *pmutex) {
    return pthread_mutex_destroy(pmutex);
}

int lock_mutex(pthread_mutex_t *pmutex) {
    return pthread_mutex_lock(pmutex);
}

int unlock_mutex(pthread_mutex_t *pmutex) {
    return pthread_mutex_unlock(pmutex);
}

int begin_thread(pthread_t *pthread, THREAD_FUNC func, void *arg) {
    return pthread_create((pthread_t*)pthread, NULL, func, arg);
}

int wait_thread(pthread_t *pthread) {
    void *retval;
    return pthread_join(*pthread, &retval);
}

unsigned long clock_tick() {
    struct timespec ts;

    #ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
        clock_serv_t cclock;
        mach_timespec_t mts;
        host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
        clock_get_time(cclock, &mts);
        mach_port_deallocate(mach_task_self(), cclock);
        ts.tv_sec = mts.tv_sec;
        ts.tv_nsec = mts.tv_nsec;
    #else
    // note: need load library -lrt
        clock_gettime(CLOCK_REALTIME, &ts);
    #endif

    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

int init_semaphore(sem_t *psem, unsigned int initcount) {
    memset(psem, 0, sizeof(sem_t));
#ifdef __APPLE__
	char sem_name[100];
	static int seq = 1;
	sprintf(sem_name,"/tmp/%ld-%d.sem",(long)getpid(),seq++);
	sem_t* sem_new = sem_open(sem_name, O_CREAT|O_EXCL,S_IWUSR | S_IRUSR,initcount);
	if(sem_new != SEM_FAILED) *psem = (sem_t)sem_new;
	return sem_new == SEM_FAILED ? 0 : -1;
#else
    return sem_init(psem, 0, initcount);
#endif
}

int uninit_semaphore(sem_t *psem) {
#ifdef __APPLE__
	sem_close((sem_t*)(*psem));
#else
    sem_destroy(psem);
#endif
    return 0;
}

int wait_semaphore(sem_t	*psem, unsigned long waittime) {
    struct timespec ts;

    #ifdef __APPLE__
        /*
        // OS X does not have clock_gettime, use clock_get_time
        clock_serv_t cclock;
        mach_timespec_t mts;
        host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
        clock_get_time(cclock, &mts);
        mach_port_deallocate(mach_task_self(), cclock);
        ts.tv_sec = mts.tv_sec;
        ts.tv_nsec = mts.tv_nsec;
        */
        int ret = sem_trywait(psem);
        while ((waittime > 0) && ((0 != ret) && (EINVAL != errno))) {
            unsigned long sleep_time = (waittime < 1000 ? waittime : 1000);
            msleep(sleep_time);
            waittime -= sleep_time;
            ret = sem_trywait(psem);
        }
        return ret;
    #else
        clock_gettime(CLOCK_REALTIME, &ts);
    #endif

    ts.tv_sec = time(NULL) + waittime /1000;
    ts.tv_nsec += (waittime % 1000 ) * 1000;

#ifdef __APPLE__
    /*
	int sem_timedwait (sem_t *sem,  const struct timespec *abs_timeout);
	return sem_timedwait((sem_t*)(*psem), &ts);
    */
#else
	return sem_timedwait(psem, &ts);
#endif
}

int post_semaphore(sem_t  *psem) {
#ifdef __APPLE__
	return sem_post((sem_t*)(*psem));
#else
    return sem_post(psem);
#endif
}

void yield() {
    usleep(0);
}

#endif

int random_int() {
   // return 32 bits of random stuff
   int init=0;
   if ( !init )
   {
      init = 1;

      uint64_t tick;

    #if defined(WIN32)
          volatile unsigned int lowtick=0,hightick=0;
          __asm
             {
                rdtsc
                   mov lowtick, eax
                   mov hightick, edx
             }
          tick = hightick;
          tick <<= 32;
          tick |= lowtick;
    #elif defined(__GNUC__) && ( defined(__i686__) || defined(__i386__) )
          asm("rdtsc" : "=A" (tick));
    #elif defined (__SUNPRO_CC) || defined( __sparc__ )
          tick = gethrtime();
    #elif defined(__MACH__)  || defined(__linux)
          int fd=open("/dev/random",O_RDONLY);
          read(fd,&tick,sizeof(tick));
          closesocket(fd);
    #else
    #     error Need some way to seed the random number generator
    #endif
          int seed = int(tick);
    #ifdef WIN32
          srand(seed);
    #else
          srandom(seed);
    #endif
       }

    #ifdef WIN32
       int r1 = rand();
       int r2 = rand();

       int ret = (r1<<16) + r2;

       return ret;
    #else
       return random();
    #endif
}

