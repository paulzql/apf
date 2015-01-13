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
#ifndef PLUGIN_H
#define PLUGIN_H

#include <string>
#include <map>

#ifdef WIN32
#include <windows.h>
#else
#define HMODULE void*
#endif

#define APF_DEBUG printf

namespace apf {

class ClassEntry;

/**
 * @brief 插件管理类(单例)
 * 插件（模块）的加载、卸载等操作
 */
// plugin manager
class PluginManager
{
public:
    /**
     * @brief 模块基本信息类
     */
    // plugin item data
    struct ModuleItem {
        /**
         * 模块句柄
         */
        // module handle
        HMODULE hmodule;
        /**
         * 模块版本号
         */
        // module version
        unsigned long version;
        /**
         * 模块提供的类信息（以ClassEntry（）结尾的数组)
         */
        // module class entries
        const ClassEntry* classes;
    };
public:
    /**
     * @brief 设置程序版本号
     * 用于加载模块时确定其版本兼容性
     * @param[in] major 主版本号
     * @param[in] sub   次版本号
     * @note 在加载模块之前需先设置版本号
     */
    // set plugin manager version
    void SetVersion(unsigned short major, unsigned short sub);

    /**
     * @brief 从指定模块文件中加载模块
     * @param[in] path 文件路径
     * @return 加载是否成功
     */
    // load plugin
    bool Load(const char* path);

    /**
     * @brief 卸载模块
     * @param[in] path 模块文件路径
     * @return 卸载是否成功
     */
    // unload plugin
    // note: please release the plugin's object before unload.
    bool UnLoad(const char* path);

    /**
     * @brief 卸载所有模块
     */
    // unload all plugins
    void UnLoadAll();

    /**
     * @brief 获取加载的所有模块信息
     * @return 所有模块信息
     */
    // get modules
    const std::map<std::string, ModuleItem> modules() const;

    /**
     * @brief 获取插件管理类单实例
     * @return 插件管理类单实例
     */
    // single instance
    static PluginManager* instance();

protected:
    /**
     * 默认构造函数
     */
    // do not create directly, use PluginManager::instance()
    PluginManager();

    /**
     * 默认析构函数
     */
    virtual ~PluginManager();

    /**
     * 注册类信息
     * @param[in] classes 类信息数组
     */
    // register classes
    // classes is a array end with ClassEntry()
    void RegisterClasses(const ClassEntry* classes);

    /**
     * 取消注册类信息
     * @param[in] classes 类信息数组
     */
    // unregister classes
    // classes is a array end with ClassEntry()
    void UnRegisterClasses(const ClassEntry* classes);
private:
    // loaded plugin <path, ModuleItem>
    std::map<std::string, ModuleItem> modules_;
    // plugin manager version
    unsigned short major_version_;
    unsigned short sub_version_;
    // single instance
    static PluginManager* instance_;
};

} // namespace

#endif // PLUGIN_H
