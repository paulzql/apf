/*!**************************************************************************
 * @file
 * @brief 模块(插件)相关的宏定义文件
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
#ifndef MODULE_H
#define MODULE_H

#include "classentry.h"
#include "object.h"
#include "singleobject.h"
#include "interface.h"

namespace apf {

#ifdef WIN32
#define APF_API extern "C" __declspec(dllexport)
#else
#define APF_API extern "C"
#endif

/** 模块导出函数名称 */
#define APF_GET_MODULE_INFO_NAME "APFGetModuleInfo"
#define APF_SET_OBJECT_CREATOR_NAME "APFSetObjectCreator"
typedef unsigned long (*APF_GET_MODULE_INFO)(unsigned long, apf::ClassEntry**);
typedef void (*APF_SET_OBJECT_CREATOR)(apf::APFCreateObjectFunc);

// Module macros defines ////////////////////////////////////////////////////
// Define macros of class factory registry, such as APF_BEGIN_DEFINE_MODULE
/**
 * 转换模块版本号
 * @param[in] major 主版本号
 * @param[in] sub   次版本号
 * @return 模块版本号
 */
// module version macro
#define APF_VERSION(major,sub) (((unsigned long)major<<16) | sub)

/**
 * 最大模块版本号
 */
// max module version
#define APF_MAX_VERSION 0x0FFFFFFF

/**
 * 开始模块的定义
 * @see APF_END_MODULE()
 * @see APF_CLASSMAP_ENTRY(clsid, cls)
 * @see APF_CLASSMAP_ENTRY_SINGLETEN(clsid, cls)
 * @param[in] module_version 模块的版本
 * @param[in] min_support_version   最低支持的程序版本
 * @param[in] max_support_version   最高支持的程序版本
 *
 * @example 模块的定义示例
 * @code
  // 开始定义模块(版本：1.0, 支持1.0以上的程序)
 APF_BEGIN_MODULE(APF_VERSION(1,0), APF_VERSION(1,0), APF_MAX_VERSION)
     // 映射Example类
     APF_CLASSMAP_ENTRY("class id", Example)
     // 映射Example类为单例模式
     APF_CLASSMAP_ENTRY_SINGLETEN("single id", SingleExample)
 APF_END_MODULE()
 * @endcode
 */
// Begin group of module.
#define APF_BEGIN_MODULE(module_version, min_support_version, max_support_version)  \
APF_API unsigned long APFGetModuleInfo(unsigned long app_version, apf::ClassEntry** module_classes) {\
    const unsigned long version = module_version;\
    const unsigned long min_support = min_support_version;\
    const unsigned long max_support = max_support_version;\
    static apf::ClassEntry classes[] = {\

/**
 * 结束模块的定义
 * @see APF_BEGIN_MODULE(module_version, min_support_version, max_support_version)
 */
// End group of module.
#define APF_END_MODULE() \
        apf::ClassEntry() \
    };\
    *module_classes = classes;\
    return (app_version>=min_support && app_version <= max_support) ? version : 0;\
}\
APF_API void APFSetObjectCreator(apf::APFCreateObjectFunc objcreator) {\
    apf::Module::Instance()->object_creator = objcreator;\
}\

/**
 * 加载内部所有模块
 * @see APF_BEGIN_INTERNAL_MODULE()
 */
int APFLoadInternalModules();

/**
 * 开始内部模块的定义
 * @see APF_END_INTERNAL_MODULE()
 * @see APF_CLASSMAP_ENTRY(clsid, cls)
 * @see APF_CLASSMAP_ENTRY_SINGLETEN(clsid, cls)
 *
 * @example internal_module.cpp 内部模块的使用示例
 *
 * @example 内部模块的定义示例
 * @code
 // 开始定义内部模块
 APF_BEGIN_INTERNAL_MODULE()
     // 映射Example类
     APF_CLASSMAP_ENTRY("class id", Example)
     // 映射Example类为单例模式
     APF_CLASSMAP_ENTRY_SINGLETEN("single id", SingleExample)
 APF_END_INTERNAL_MODULE()
 * @endcode
 */
// Begin group of internal module.
#define APF_BEGIN_INTERNAL_MODULE()  \
int APFLoadInternalModules() {\
    static apf::ClassEntry classes[] = {\

/**
 * 结束内部模块的定义
 * @see APF_BEGIN_INTERNAL_MODULE()
 */
// End group of internal module.
#define APF_END_INTERNAL_MODULE() \
        apf::ClassEntry() \
    };\
    int register_size = 0;\
    int class_len = sizeof(classes) / sizeof(apf::ClassEntry) - 1 ;\
    for (int i = 0; i < class_len; i++) { \
        if (!apf::Class::RegisterClass(classes[i])) { \
            APF_DEBUG("Register internal class failed [classid:%s,name:%s]\n", classes[i].clsid.c_str(), classes[i].class_name);\
            continue;\
        }\
        register_size++;\
    }\
    return register_size;\
}\

// Load internal modules function
#define APFLoadInternalModules APFLoadInternalModules

/**
 * 定义一个类与ID的映射
 * @param[in] clsid 类ID
 * @param[in] cls   类名
 */
// Register a regular class.
#define APF_CLASSMAP_ENTRY(clsid, cls)      \
    apf::ClassEntry("Object<" #cls ">", clsid,  \
        reinterpret_cast<ObjectCreatorFunc>(&apf::Object<cls>::CreateObject), \
        reinterpret_cast<ObjectDestroyerFunc>(&apf::Object<cls>::DestroyObject),  \
        reinterpret_cast<QueryInterfaceFunc>(&apf::Object<cls>::QueryInterface)),

/**
 * 定义一个类与ID的映射（单例类)
 * @param[in] clsid 类ID
 * @param[in] cls   类名
 */
// Register a single instance class.
#define APF_CLASSMAP_ENTRY_SINGLETEN(clsid, cls)    \
    apf::ClassEntry("SingleObject<" #cls ">", clsid,  \
        reinterpret_cast<ObjectCreatorFunc>(&apf::SingleObject<cls>::CreateObject),    \
        reinterpret_cast<ObjectDestroyerFunc>(&apf::SingleObject<cls>::DestroyObject), \
        reinterpret_cast<QueryInterfaceFunc>(&apf::SingleObject<cls>::QueryInterface)),

/**
 * @brief 模块类
 */
class Module {
public:
    Module() : object_creator(0){}
    /**
     * 获取单实例
     * @return 单实例对象
     */
    static Module* Instance() {
        static Module* instance = new Module();
        return instance;
    }

public:
    /** 对象创建函数 */
    apf::APFCreateObjectFunc object_creator;
};

// End Module macros defines ///////////////////////////////////////////////

} // end namespace

#endif // MODULE_H
