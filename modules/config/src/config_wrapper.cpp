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
#include <iostream>
#include "config_wrapper.h"
#include "config.h"

ConfigWrapper::ConfigWrapper() {
    config_ = new Config();
}

ConfigWrapper::~ConfigWrapper() {
    if (NULL != config_) {
        delete config_;
    }
}

// load config from file
bool ConfigWrapper::Load(const char* filepath) {
    if (NULL != config_) {
        delete config_;
        config_ = new Config();
    }

    if (NULL != filepath) {
        filepath_ = filepath;
        try {
            config_->ReadFile(filepath);
        } catch(...) {
            return false;
        }
    } else {
        filepath_ = "";
    }

    return true;
}
// save config to file
// if filepath is NULL then save into current loaded config file
bool ConfigWrapper::Save(const char* filepath) {
    const char* filename = filepath;
    if (NULL == filename) {
        filename = filepath_.empty() ? NULL : filepath_.c_str();
    }

    if (NULL != filename) {
        std::ofstream out(filename);
        out << *(this->config_);
    }

    return true;
}

// set int value with key
void ConfigWrapper::Put(const char* key, int value) {
    config_->Add(key, value);
}

// set long value with key
void ConfigWrapper::Put(const char* key, long value) {
    config_->Add(key, value);
}

// set double value with key
void ConfigWrapper::Put(const char* key, double value) {
    config_->Add(key, value);
}

// set string value with key
void ConfigWrapper::Put(const char* key, const char* value) {
    config_->Add(key, value);
}

// set bool value with key
void ConfigWrapper::Put(const char* key, bool value) {
    config_->Add(key, value);
}

// get int value with key
// if cann't find key just return def
int  ConfigWrapper::Get(const char* key, int def) {
    return config_->Read(key, def);
}

// get long value with key
// if cann't find key just return def
long ConfigWrapper::Get(const char* key, long def) {
    return config_->Read(key, def);
}

// get double value with key
// if cann't find key just return def
double ConfigWrapper::Get(const char* key, double def) {
    return config_->Read(key, def);
}

// get string value with key
// if cann't find key just return def
const char* ConfigWrapper::Get(const char* key, const char* def) {
    return config_->Read(key, def);
}

// get bool value with key
// if cann't find key just return def
bool ConfigWrapper::Get(const char* key, bool def) {
    return config_->Read(key, def);
}

// detect if has the key in config
bool ConfigWrapper::Has(const char* key) {
    return config_->KeyExists(key);
}
