/*!**************************************************************************
 * @file
 * @brief APF Class 类头文件
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
#ifndef APFCLASS_H
#define APFCLASS_H

#ifdef WIN32
#include <windows.h>
#endif

#include <map>

#include "interface.h"
#include "classentry.h"

namespace apf {

/**
 * @brief APF 类管理
 * 类的注册、创建、销毁等
 * @note 全局模式
 */
 // apf object manager class.
class Class {
public:
    /**
     * 注册类信息
     * @param[in] class_entry 类信息对象
     * @param[in] replace 是否替换已注册的的类（相同的类ID)
     * @return 注册是否成功
     */
    // register class entry
    // ClassEntry will replace old ClassEntry with same classid when replace is true
    static bool RegisterClass(const ClassEntry& class_entry, bool replace=true);

    /**
     * 注销类信息
     * @param[in] class_id 类ID
     * @return
     * @note 将会注销注册ID为class_id的类
     */
    // unregister class
    static void UnRegisterClass(const APFClassID& class_id);

    /**
     * 注销类信息
     * @param[in] class_entry 类信息对象
     * @return
     * @note 将会注销与class_entry相同类信息的类
     */
    // unregister class
    static void UnRegisterClass(const ClassEntry& class_entry);

    /**
     * 跟据提供的类ID创建对象
     * @param[in] class_id 类ID
     * @param[out] class_info 创建对象的类信息指针
     * @return 对象地址
     * @retval NULL 创建对象失败(通常是没有注册类ID为class_id的类)
     */
    // create an object
    static void* CreateObject(const APFClassID& class_id, ClassEntry* class_info);

    /**
     * 销毁对象
     * @param[in] pobj 对象地址
     * @param[in] class_info 对象的类信息指针
     * @return
     * @note 对象与类信息一定要对应
     */
    // destroy object
    static void DestroyObject(void* pobj, ClassEntry* class_info);

    /**
     * 是否已注册类
     * @param[in] class_id 类ID
     * @return 是否已注册类
     */
    // have registered the class id?
    static bool HasClass(const APFClassID& class_id);
private:
    Class();
    virtual ~Class();
    // class entry map
    static std::map<const APFClassID, ClassEntry> class_map_;
};

} // namespace

#endif // APFCLASS_H
