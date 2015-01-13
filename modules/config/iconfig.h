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
#ifndef ICONFIG_H_INCLUDED
#define ICONFIG_H_INCLUDED

// declare config class id
APF_DECLARE_CLASSID(CLSID_Config, "Config")

class IConfig {
APF_DECLARE_INTERFACE(IConfig)
public:
    virtual ~IConfig(){}
    // load config from file
    virtual bool Load(const char* filepath)=0;
    // save config to file
    // if filepath is NULL then save into current loaded config file
    virtual bool Save(const char* filepath=NULL)=0;
    // set int value with key
    virtual void Put(const char* key, int value)=0;
    // set long value with key
    virtual void Put(const char* key, long value)=0;
    // set double value with key
    virtual void Put(const char* key, double value)=0;
    // set string value with key
    virtual void Put(const char* key, const char* value)=0;
    // get int value with key
    // if cann't find key just return def
    virtual int  Get(const char* key, int def=0)=0;
    // get long value with key
    // if cann't find key just return def
    virtual long Get(const char* key, long def=0l)=0;
    // get double value with key
    // if cann't find key just return def
    virtual double Get(const char* key, double def=0.0)=0;
    // get string value with key
    // if cann't find key just return def
    virtual const char* Get(const char* key, const char* def=NULL)=0;
    // detect if has the key in config
    virtual bool Has(const char* key)=0;
};



#endif // ICONFIG_H_INCLUDED
