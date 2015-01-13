/*!**************************************************************************
 * @file
 * @brief 单例类基本操作封装
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
#ifndef APFSINGLEOBJECT_H
#define APFSINGLEOBJECT_H

namespace apf {

/**
 * @brief 单例类基本操作封装
 * @note 用于APF_CLASSMAP_ENTRY_SINGLETEN(clsid, cls)
 */
// Single instance implement template class used by APF_CLASSMAP_ENTRY_SINGLETEN.
// The template parameter is a class which implement a interface.
template <class ClassType>
class SingleObject : public ClassType {
public:
    /**
     * 查询接口
     * @param[in] iid 接口ID
     * @return 是否有iid的接口实现
     */
    // query interface
    static bool QueryInterface(APFInterfaceID iid) {
        return ClassType::DoQueryInterface(iid);
    }

    /**
     * 创建对象
     * @return 对象地址
     */
    // create object
    static void* CreateObject() {
        return Instance();
    }

    /**
     * 销毁对象
     * @param[in] object 对象地址
     */
    // destroy object
    static void DestroyObject(void* object) {
        ClassType *p = static_cast<ClassType*>(object);
        // do nothing
    }

    /**
     * 获取对象实例
     * @return 对象实例
     */
    // singleten instance
    static ClassType* Instance() {
        static ClassType* instance = NULL;
        if (NULL == instance) {
            instance = new ClassType();
        }

        return instance;
    }
};

} // namespace

#endif // APFSINGLEOBJECT_H
