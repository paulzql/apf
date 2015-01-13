/***************************************************************************
 *
 *  Project
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
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#ifndef WIN32
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#else

#endif
#include "log.h"

#define DAY_OF_SECONDS 86400 // 60*60*24
#define MAX_LOG_LEN     1024

#ifdef WIN32
#define getpid GetCurrentProcessId
#define snprintf _snprintf
#endif

// Get Time String [YYYY-MM-DD HH:MI:SS]
static int GetTimeStr(char *out_timestr, int size) {
    time_t  now_t = time(NULL);
    struct  tm  *pTm = localtime(&now_t);

    // YYYY-MM-DD HH:MI:SS
    return snprintf(out_timestr, size-1, "%4d-%02d-%02d %02d:%02d:%02d",
            pTm->tm_year + 1900, pTm->tm_mon + 1, pTm->tm_mday,
            pTm->tm_hour, pTm->tm_min, pTm->tm_sec);
}

Log::Log() {
   file_ = NULL;
   log_socket_ = INVALID_SOCKET;
   max_file_size_ = DEFAULT_FILE_SIZE;
   current_log_file_day_ = 0;
   target_addr_.sin_family = AF_INET;
   target_addr_.sin_addr.s_addr = inet_addr("127.0.1");
   target_addr_.sin_port = ntohs(DEFAULT_PORT);

   init_mutex(&mutex_);

   this->SetTargets(DEFAULT_TARGETS);
}

Log::~Log() {
    SetTargets(0);
    uninit_mutex(&mutex_);
}

void Log::LogCheck() {
    	if (file_) {
         if ((LOG_BACKUP_ONE_FILE & log_backup_strategy_)) {
            if (ftell(file_) > max_file_size_) {
                std::string file_name = path_ + "/";
                std::string backup_file_name = path_ + "/";
                file_name += name_ + suffix_;
                backup_file_name += name_ + "_bk";
                backup_file_name += suffix_;

                fclose(file_);

                unlink(backup_file_name.c_str());

                rename(file_name.c_str(), backup_file_name.c_str());

                file_ = fopen(file_name.c_str(), "a+");
                if (NULL == file_) {
                    fprintf(stderr, "open log file %s failed!\n", file_name.c_str());

                }
            }
        } else if ((time(NULL)%DAY_OF_SECONDS) > current_log_file_day_){
            char buf[32];
            time_t timer = time(NULL);
            strftime(buf, sizeof(buf), "%Y-%m-%d", localtime(&timer));

            std::string new_file_name = path_ + "/";
            new_file_name += name_;
            new_file_name += "-";
            new_file_name += buf;
            new_file_name += suffix_;
            current_log_file_day_ = timer%DAY_OF_SECONDS;

            fclose(file_);

            file_ = fopen(new_file_name.c_str(), "a+");
            if (NULL == file_) {
                fprintf(stderr, "open log file %s failed!\n", new_file_name.c_str());
            }
        }
    }

}

void Log::WriteLog(char* data, int len) {
    lock_mutex(&mutex_);
    LogCheck();

    if ((LOG_TARGET_FILE & targets_) && file_) {
        fwrite(data, 1, len+1, file_);
    }
    if ((LOG_TARGET_NET & targets_) && (INVALID_SOCKET != log_socket_)) {
        sendto(log_socket_, data, len+1, 0, (sockaddr*)&target_addr_, sizeof(target_addr_));
    }
    if (LOG_TARGET_CONSOLE & targets_) {
        printf(data);
    }
    unlock_mutex(&mutex_);
}

void Log::Start() {
    SetTargets(targets_);

    char str_time[64];
    char buf[MAX_LOG_LEN];
    int  len = 0;
    GetTimeStr(str_time, sizeof(str_time));

    len = sprintf(buf, "%s START: ****************************************************\n", str_time);
    WriteLog(buf, len);
    len = sprintf(buf, "%s START: *** Compile Time:  %s %s\n", str_time, __DATE__, __TIME__);
    WriteLog(buf, len);
    len = sprintf(buf, "%s START: *** PID:  %d\n", str_time, getpid());
    WriteLog(buf, len);
    len = sprintf(buf, "%s START: ****************************************************\n", str_time);
    WriteLog(buf, len);
}

void Log::End() {

    char str_time[64];
    char buf[MAX_LOG_LEN];
    int  len = 0;
    GetTimeStr(str_time, sizeof(str_time));

    len = sprintf(buf, "%s END: ******************************************************\n", str_time);
    WriteLog(buf, len);
    len = sprintf(buf, "%s END: ***** PID:  %d\n", str_time, getpid());
    WriteLog(buf, len);
    len = sprintf(buf, "%s END: ******************************************************\n", str_time);
    WriteLog(buf, len);

    SetTargets(0);
}

void Log::SetTargets(int targets) {
    if ((LOG_TARGET_FILE & targets) && !file_) {
        SetFile(path_.c_str(), name_.c_str(), log_backup_strategy_, max_file_size_);
    } else if (!(LOG_TARGET_FILE & targets) && file_) {
        lock_mutex(&mutex_);
        fclose(file_);
        file_ = NULL;
        unlock_mutex(&mutex_);
    }
    if ((LOG_TARGET_NET & targets) && (INVALID_SOCKET == log_socket_)) {
        log_socket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    } else if (!(LOG_TARGET_NET & targets) && (INVALID_SOCKET != log_socket_)) {
        closesocket(log_socket_);
        log_socket_ = INVALID_SOCKET;
    }

    targets_ = targets;
}

void Log::SetFile(const char* path, const char* name, LogBackupStrategy strategy, long max_size) {
    path_ = path;
    name_ = name;
    log_backup_strategy_ = strategy;
    max_file_size_ = max_size;

    std::string file_name = path_ + "/";
    file_name += name_;
    if (LOG_BACKUP_ONE_FILE & strategy) {
        file_name += suffix_;
    } else {
        char buf[32];
        time_t timer = time(NULL);
        strftime(buf, sizeof(buf), "%Y-%m-%d", localtime(&timer));

        file_name += "-";
        file_name += buf;
        file_name += suffix_;
    }

    if (LOG_TARGET_FILE & targets_) {
        lock_mutex(&mutex_);
        if (file_) {
            fclose(file_);
        }
        file_ = fopen(file_name.c_str(), "a+");
        if (NULL == file_) {
            fprintf(stderr, "open log file %s failed!\n", file_name.c_str());
        }
        unlock_mutex(&mutex_);
    }
}

void Log::SetAddr(const char* host, unsigned short port) {
    struct hostent* host_info = gethostbyname(host);
    if (host_info && *host_info->h_addr_list) {
        memcpy(&target_addr_.sin_addr, *host_info->h_addr_list, sizeof(target_addr_.sin_addr));
    } else {
        target_addr_.sin_addr.s_addr = inet_addr(host);
    }
    target_addr_.sin_port = ntohs(port);
}

void Log::Info(const char* tag, const char* format, ...) {
    char buf[MAX_LOG_LEN+1];
    int len = GetTimeStr(buf, MAX_LOG_LEN);
    len += snprintf(buf + len, MAX_LOG_LEN - len, " Info: [%s] ", tag);

    va_list ap;
    va_start(ap, format);
    len += vsnprintf(buf + len, MAX_LOG_LEN - len, format, ap);
    va_end(ap);

    buf[len++] = '\n';
    buf[len] = '\0';

    WriteLog(buf, len);
}

void Log::Warn(const char* tag, const char* format, ...) {
    char buf[MAX_LOG_LEN+1];
    int len = GetTimeStr(buf, MAX_LOG_LEN);
    len += snprintf(buf + len, MAX_LOG_LEN - len, " Warn: [%s] ", tag);

    va_list ap;
    va_start(ap, format);
    len += vsnprintf(buf + len, MAX_LOG_LEN - len, format, ap);
    va_end(ap);

    buf[len++] = '\n';
    buf[len] = '\0';

    WriteLog(buf, len);
}

void Log::Error(const char* tag, const char* format, ...) {
    char buf[MAX_LOG_LEN+1];
    int len = GetTimeStr(buf, MAX_LOG_LEN);
    len += snprintf(buf + len, MAX_LOG_LEN - len, " Error: [%s] ", tag);

    va_list ap;
    va_start(ap, format);
    len += vsnprintf(buf + len, MAX_LOG_LEN - len, format, ap);
    va_end(ap);

    buf[len++] = '\n';
    buf[len] = '\0';

    WriteLog(buf, len);
}

void Log::Debug(const char* tag, const char* format, ...) {
    char buf[MAX_LOG_LEN+1];
    int len = GetTimeStr(buf, MAX_LOG_LEN);
    len += snprintf(buf + len, MAX_LOG_LEN - len, " Debug: [%s] ", tag);

    va_list ap;
    va_start(ap, format);
    len += vsnprintf(buf + len, MAX_LOG_LEN - len, format, ap);
    va_end(ap);

    buf[len++] = '\n';
    buf[len] = '\0';

    WriteLog(buf, len);
}
