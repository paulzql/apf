/*!**************************************************************************
 * @file
 * @brief 类基本操作封装
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
#ifndef APFOBJECT_H
#define APFOBJECT_H

namespace apf {

/**
 * @brief 类基本操作封装
 * @note 用于APF_CLASSMAP_ENTRY(clsid, cls)
 */
// The regular implement template class used by APF_CLASSMAP_ENTRY.
// The template parameter is a class which implement a interface.
template <class ClassType>
class Object : public ClassType {
public:
    /**
     * 查询接口
     * @param[in] iid 接口ID
     * @return 是否有iid的接口实现
     */
    // query interface
    // note: you must define APF_BEGIN_CLASS in the ClassType first
    static bool QueryInterface(APFInterfaceID iid) {
        return ClassType::DoQueryInterface(iid);
    }

    /**
     * 创建对象
     * @return 对象地址
     */
    // create object
    static void* CreateObject() {
        return new ClassType();
    }

    /**
     * 销毁对象
     * @param[in] object 对象地址
     */
    // destroy object
    static void DestroyObject(void* object) {
        ClassType *p = static_cast<ClassType*>(object);
        delete p;
    }
};

} // namespace

#endif // APFOBJECT_H
