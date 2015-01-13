/*!**************************************************************************
 * @file
 * @brief 插件管理
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
#ifdef WIN32
//#include <windows.h>
#else
#include <dlfcn.h>
#endif

#include "plugin_manager.h"
#include "interface.h"
#include "classentry.h"
#include "class.h"
#include "module.h"

namespace apf {

#ifndef MAX_PATH
#define MAX_PATH 256
#endif

PluginManager* PluginManager::instance_ = NULL;

PluginManager::PluginManager()
{
    major_version_ = 0;
    sub_version_ = 0;
}

PluginManager::~PluginManager()
{
    //dtor
}

void PluginManager::SetVersion(unsigned short major, unsigned short sub) {
    major_version_ = major;
    sub_version_ = sub;
}

bool PluginManager::Load(const char* path) {
    APF_DEBUG("loading module %s\n", path);
    if (modules_.end() != modules_.find(path)) {
        APF_DEBUG("load module failed (alread loaded)\n");
        return false;
    }

    HMODULE hmodule = NULL;
    ClassEntry* classes = NULL;
    unsigned long  version = 0;
    APF_GET_MODULE_INFO module_func = NULL;
    APF_SET_OBJECT_CREATOR set_object_creator_func = NULL;
    #ifdef WIN32
    hmodule = ::LoadLibraryExA(path, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	if (NULL != hmodule) {
        module_func = (APF_GET_MODULE_INFO)GetProcAddress(hmodule, APF_GET_MODULE_INFO_NAME);
        set_object_creator_func = (APF_SET_OBJECT_CREATOR)GetProcAddress(hmodule, APF_SET_OBJECT_CREATOR_NAME);
    }
    #else
    hmodule = dlopen(path, RTLD_LAZY);
    if (NULL != hmodule) {
        module_func = (APF_GET_MODULE_INFO)dlsym(hmodule, APF_GET_MODULE_INFO_NAME);
        set_object_creator_func = (APF_SET_OBJECT_CREATOR)dlsym(hmodule, APF_SET_OBJECT_CREATOR_NAME);
    }
    #endif
    if (NULL == hmodule) {
        APF_DEBUG("open module file failed\n");
        return false;
    }
    if (NULL == module_func) {
        APF_DEBUG("can't find module interface : %s\n", APF_GET_MODULE_INFO_NAME);
        return false;
    }
    // get module info
    version = module_func(APF_VERSION(major_version_, sub_version_), &classes);
    if (0 == version) {
        APF_DEBUG("incompatible module\n");
        return false;
    }
    if (NULL == classes) {
        APF_DEBUG("no classes found\n");
        return false;
    }

    if (NULL != set_object_creator_func) {
        set_object_creator_func(apf::APFCreateObject);
    }

    RegisterClasses(classes);

    ModuleItem item;
    item.hmodule = hmodule;
    item.classes = classes;
    item.version = version;

    modules_.insert(std::map<std::string, ModuleItem>::value_type(path, item));

    APF_DEBUG("loaded module %s\n", path);

    return true;
}

bool PluginManager::UnLoad(const char* path) {
    std::map<std::string, ModuleItem>::iterator it = modules_.find(path);
    if (modules_.end() == modules_.find(path)) {
        APF_DEBUG("no module found %s\n", path);
        return false;
    }

    UnRegisterClasses(it->second.classes);
    #ifdef WIN32
	FreeLibrary(it->second.hmodule);
    #else
    dlclose(it->second.hmodule);
    #endif
    modules_.erase(it);

    return true;
}

void PluginManager::UnLoadAll() {
    std::map<std::string, ModuleItem>::iterator it = modules_.begin();
    while (modules_.end() != it) {
        UnRegisterClasses(it->second.classes);
        #ifdef WIN32
        FreeLibrary(it->second.hmodule);
        #else
        dlclose(it->second.hmodule);
        #endif
        it++;
    }
    modules_.clear();
}

void PluginManager::RegisterClasses(const ClassEntry* classes) {
    const ClassEntry empty_class;
    while (classes && (!classes->equals(empty_class))) {
        if (!Class::RegisterClass(*classes)) {
            // register class failed
            APF_DEBUG("Register class failed [classid:%s,name:%s]\n", classes->clsid.c_str(), classes->class_name);
        } else {
            APF_DEBUG("Register class ok [classid:%s,name:%s]\n", classes->clsid.c_str(), classes->class_name);
        }
        classes++;
    }
}

void PluginManager::UnRegisterClasses(const ClassEntry* classes) {
    const ClassEntry empty_class;
    while (NULL != classes && !(classes->equals(empty_class))) {
        Class::UnRegisterClass(*classes);
        classes++;
    }
}

const std::map<std::string, PluginManager::ModuleItem> PluginManager::modules() const {
    return modules_;
}

PluginManager* PluginManager::instance() {
    if (NULL == instance_) {
        instance_ = new PluginManager();
    }

    return instance_;
}


} // namespace
