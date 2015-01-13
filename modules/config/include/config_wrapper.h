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
#ifndef CONFIG_WRAPPER_H_INCLUDED
#define CONFIG_WRAPPER_H_INCLUDED

#include <string>
#include "interface.h"
#include "iconfig.h"

class Config;

class ConfigWrapper : public IConfig {
APF_BEGIN_CLASS()
APF_INTERFACE_ENTRY(IConfig)
APF_END_CLASS()
public:
    ConfigWrapper();
    ~ConfigWrapper();
    // load config from file
    bool Load(const char* filepath);
    // save config to file
    // if filepath is NULL then save into current loaded config file
    bool Save(const char* filepath=NULL);
    // set int value with key
    void Put(const char* key, int value);
    // set long value with key
    void Put(const char* key, long value);
    // set double value with key
    void Put(const char* key, double value);
    // set string value with key
    void Put(const char* key, const char* value);
    // set bool value with key
    void Put(const char* key, bool value);
    // get int value with key
    // if cann't find key just return def
    int  Get(const char* key, int def=0);
    // get long value with key
    // if cann't find key just return def
    long Get(const char* key, long def=0l);
    // get double value with key
    // if cann't find key just return def
    double Get(const char* key, double def=0.0);
    // get string value with key
    // if cann't find key just return def
    const char* Get(const char* key, const char* def=NULL);
    // get bool value with key
    // if cann't find key just return def
    bool Get(const char* key, bool def=false);
    // detect if has the key in config
    bool Has(const char* key);
private:
    std::string filepath_;
    Config* config_;
};


#endif // CONFIG_WRAPPER_H_INCLUDED
