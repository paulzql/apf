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
#ifndef ILOG_H_INCLUDED
#define ILOG_H_INCLUDED

// show log on console
#define LOG_TARGET_CONSOLE  0x01
// send log to udp port
#define LOG_TARGET_NET      0x02
// write log to file
#define LOG_TARGET_FILE     0x04

// declare log class id
APF_DECLARE_CLASSID(CLSID_Log, "Log")

// log file type
enum LogBackupStrategy {
    // log on single file, if file size exceed max size, the file will be backup to {name}.log.bk
    LOG_BACKUP_ONE_FILE,
    // write a file with {name}-{date}.log every day.
    LOG_BACKUP_DATE_FILE
};


// log interface
class ILog {
APF_DECLARE_INTERFACE(ILog)
public:
    virtual ~ILog(){}
    // start log
    // note: before start, you should set log first (see: SetTargets/SetFile/SetPort)
    virtual void Start()=0;
    // end log
    virtual void End()=0;
    // set log target
    // eg. log->SetTargets(LOG_TARGET_CONSOLE | LOG_TARGET_FILE);
    virtual void SetTargets(int targets)=0;
    // set log file
    virtual void SetFile(const char* path, const char* name, LogBackupStrategy type, long max_size)=0;
    // set net log addr (UDP)
    virtual void SetAddr(const char* host, unsigned short port)=0;
    // log info
    virtual void Info(const char* tag, const char* format, ...)=0;
    // log warning
    virtual void Warn(const char* tag, const char* format, ...)=0;
    // log error
    virtual void Error(const char* tag, const char* format, ...)=0;
    // log debug
    virtual void Debug(const char* tag, const char* format, ...)=0;
};

#endif // ILOG_H_INCLUDED
