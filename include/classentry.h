/*!**************************************************************************
 * @file
 * @brief 类信息实体类头文件
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
#ifndef CLASSENTRY_H
#define CLASSENTRY_H

#include <string>

/** 类ID类型 */
// class unique id
#define APFClassID std::string

/** 接口ID类型 */
// interface id
#define APFInterfaceID long

// class factory function.
typedef void* (*ObjectCreatorFunc)();

// object destroyer function.
typedef void (*ObjectDestroyerFunc)(void*);

// query interface
typedef bool (*QueryInterfaceFunc)(APFInterfaceID);

namespace apf {

#if defined(WIN32) || defined(WINCE)
#pragma pack(push, 8)
#endif

/**
 * @brief 类信息实体
 * 类名、类ID、创建对象函数、销毁对象函数、查询接口函数等
 */
// class factory registry
class ClassEntry {
public:
    /**
     * 构造函数
     * @param[in] class_name 类名称
     * @param[in] classid 类ID
     * @param[in] create_object 创建对象函数
     * @param[in] destroy_object 销毁对象函数
     * @param[in] query_interface 查询接口函数
     */
    // Used by APF_CLASSMAP_ENTRY, APF_CLASSMAP_ENTRY_SINGLETEN
    ClassEntry(
        const char*             class_name,
        const APFClassID&       clsid,
        ObjectCreatorFunc       create_object,
        ObjectDestroyerFunc     destroy_object,
        QueryInterfaceFunc      query_interface)

        : class_name(class_name), clsid(clsid)
        , create_object(create_object)
        , destroy_object(destroy_object)
        , query_interface(query_interface) {
    }

    /**
     * 拷贝构造函数
     * @param[in] entry 拷贝对象
     */
    // copy constructor
    ClassEntry(const ClassEntry& entry) {
        this->class_name = entry.class_name;
        this->clsid = entry.clsid;
        this->create_object = entry.create_object;
        this->destroy_object = entry.destroy_object;
        this->query_interface = entry.query_interface;
    }

    /**
     * 默认构造函数(空对象）
     */
    // Used by APF_BEGIN_MODULE
    ClassEntry()
        : class_name(""), clsid("")
        , create_object(NULL)
        , destroy_object(NULL)
        , query_interface(NULL) {
    }

    /**
     * 判断对象是否相等
     */
    // operator ==
    bool equals(const ClassEntry& entry) const{
        return (this->clsid == entry.clsid) &&
        (this->create_object == entry.create_object) &&
        (this->destroy_object == entry.destroy_object) &&
        (this->query_interface == entry.query_interface);
    }

public:
    /** 实现类名 */
    // implement class name
    const char*          class_name;
    /** 类ID */
    // class id
    APFClassID     clsid;
    /** 创建对象函数 */
    // class factory function
    ObjectCreatorFunc    create_object;
    /** 销毁对象函数 */
    // object destroyer function
    ObjectDestroyerFunc  destroy_object;
    /** 查询接口函数 */
    // query interface function
    QueryInterfaceFunc   query_interface;

#if defined(WIN32) || defined(WINCE)
};
#pragma pack(pop)
#else
}__attribute__ ((aligned(8)));
#endif

} // namespace

#endif // CLASSENTRY_H
