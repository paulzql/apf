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
#ifndef LOG_H
#define LOG_H

#include <string>
#include "oscore.h"
#include "interface.h"
#include "ilog.h"

#define DEFAULT_SUFFIX ".log"
#define DEFAULT_FILE_SIZE 10*1024*1024 //10MB
#define DEFAULT_PORT   4096
#define DEFAULT_TARGETS LOG_TARGET_FILE
#define DEFAULT_BACKUP_STRATEGY LOG_BACKUP_ONE_FILE

class Log : public ILog {
APF_BEGIN_CLASS()
APF_INTERFACE_ENTRY(ILog)
APF_END_CLASS()
public:
    Log();
    virtual ~Log();

    // start log
    // note: before start, you should set log first (see: SetTargets/SetFile/SetPort)
    virtual void Start();
    // end log
    virtual void End();
    // set log target
    // eg. log->SetTargets(LOG_TARGET_CONSOLE | LOG_TARGET_FILE);
    virtual void SetTargets(int targets);
    // set log file
    virtual void SetFile(const char* path, const char* name, LogBackupStrategy type, long max_size);
    // set net log addr (UDP)
    virtual void SetAddr(const char* host, unsigned short port);
    // log info
    virtual void Info(const char* tag, const char* format, ...);
    // log warning
    virtual void Warn(const char* tag, const char* format, ...);
    // log error
    virtual void Error(const char* tag, const char* format, ...);
    // log debug
    virtual void Debug(const char* tag, const char* format, ...);
protected:
    void LogCheck();
    void WriteLog(char* data, int len);
private:
    // log file's path
    std::string path_;
    // log file name (exclude path and suffix)
    std::string name_;
    // log file suffix
    std::string suffix_;
    // max file size
    long max_file_size_;
    // log file backup strategy
    LogBackupStrategy log_backup_strategy_;
    // mutex
    pthread_mutex_t mutex_;
    // log targets
    int targets_;
    // file
    FILE*  file_;
    // UDP socket use to send log to net
    SOCKET log_socket_;
    // target sockaddr
    sockaddr_in target_addr_;
    // current log file day
    unsigned long current_log_file_day_;
};

#endif // LOG_H
