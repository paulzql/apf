/*!**************************************************************************
 * @file
 * @brief 接口定义文件
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
#ifndef INTERFACE_H
#define INTERFACE_H

#include <string>
#include "classentry.h"
#include "class.h"

/** 声明一个类ID常量
 * @param[in] clsid 常量名称
 * @param[in] str  类ID值
 */
// the macro to define classid
#define APF_DECLARE_CLASSID(clsid, str)  static const APFClassID clsid(str);


/**
 * 声明接口（在类中声明）
 * @param[in] _Interface 要声明为接口的类
 * @code
  class IExample {
  public:
      //declare interface
      APF_INTERFACE(IExample)
      virtual void foo() = 0;
  };
 * @endcode
 */
// Declare GetInterfaceID() in a interface.
#define APF_DECLARE_INTERFACE(_Interface) \
    public:\
    static APFInterfaceID GetInterfaceID() { static APFInterfaceID id = apf::APFHashKey(#_Interface); return id; }\

/**
 * 开始声明类中实现的接口
 * @see APF_INTERFACE_ENTRY(_Interface)
 * @see APF_END_CLASS()
 * @code
  class ExampleImpl : public IExample {
      APF_BEGIN_CLASS()
          APF_INTERFACE_ENTRY(Ix_Example)
      APF_END_CLASS()

      virtual void foo() {printf("foo");}
  };
 * @endcode
 */
// Begin definition group of how many interfaces supported by the class.
#define APF_BEGIN_CLASS()  \
public: \
    static bool DoQueryInterface(APFInterfaceID iid) {\

/**
 * 声明类中实现的接口
 * @param[in] _Interface 接口类型
 * @see APF_BEGIN_CLASS()
 * @see APF_END_CLASS()
 */
// Indicate a interface is supported by the class.
#define APF_INTERFACE_ENTRY(_Interface)    \
        if (iid == _Interface::GetInterfaceID()) {      \
            return true;    \
        }

/**
 * 声明父类实现的接口
* @param[in] _BaseClass 父类型
 * @see APF_BEGIN_CLASS()
 * @see APF_END_CLASS()
 * @note 若本类的父类实现了某些接口，那么使用此宏代替APF_INTERFACE_ENTRY(_Interface)来声明父类已声明的接口
 */
// Indicate this class is derived from a implement class.
// All interfaces of the base class are supported by this class.
#define APF_USE_INTERFACE_ENTRY(_BaseClass)       \
        if (_BaseClass::DoQueryInterface(iid)) { \
            return true;    \
        }

/**
 * 结束声明类中实现的接口
 * @see APF_BEGIN_CLASS()
 */
// End group of class definition.
#define APF_END_CLASS() \
        return false; \
    }

namespace apf {
/**
 * 对象创建函数类型
 */
typedef void* (*APFCreateObjectFunc)(const APFClassID&, ClassEntry&);
/** @name 内部函数 @{ */
// internal functions //////////////////////////////////////////
/** 将字符串hash为数字 */
// hash string to long
inline long APFHashKey(const char* str) {
	long value = 0;
    while (*str) {value = (value<<5) + value + *str++;}
	return value;
}
/**
 * 通过类ID创建对象
 * @param[in] clsid 类ID
 * @param[out] class_entry 类信息
 * @return 对象地址
 */
// Create an object with the class id.
void* APFCreateObject(const APFClassID& clsid, ClassEntry& class_entry);
//////////////////////////////////////////////////////////////
/** @} */

/**
 * @brief 智能指针接口
 * @note Interface是一个模板智能指针，内部使用引用计数
 *       初始化完一个Interface后需要检查是否有效，确定有效后才能正常使用
 * @code
 Interface<IExample> example("example_class_id");
 if (example) {
    example->SayHello();
 }
 example.Release();
 * @endcode
 */
// An autoptr class with a specified interface.
template <class InterfaceType>
class Interface {
public:
    /**
     * 拷贝构造函数(同类型)
     * @param[in] inf 拷贝对象
     */
    // Copy constructor.
    explicit Interface(Interface<InterfaceType>& inf) {
        Reset();
        if (inf.interface_) {
            interface_ = inf.interface_;
            reference_count_ = inf.reference_count_;
            class_info_      = inf.class_info();
            (*reference_count_)++;
        }
    }

    /**
     * 拷贝构造函数(不同类型)
     * @param[in] inf 拷贝对象
     */
    // Copy constructor.
    template <class InterfaceType2>
    explicit Interface(const Interface<InterfaceType2>& inf) {
        Reset();
        if (inf) {
            if (inf.class_info().query_interface(InterfaceType::GetInterfaceID())) {
                interface_ = reinterpret_cast<InterfaceType*>(inf.P());
                class_info_ = inf.class_info();
                reference_count_ = (long*)(inf.reference_count());
                (*reference_count_)++;
            }
        }
    }

    /**
     * 构造函数
     * @param[in] clsid 类ID
     */
    // constructor. will create an object with the clsid
    explicit Interface(const APFClassID& clsid) {
        Create(clsid);
    }

    /**
     * 构造默认函数
     * @param[in] clsid 类ID
     */
    // default constructor.
    Interface() {
        // checking interface, interface must have GetInterfaceID() function
        // see APF_INTERFACE
        InterfaceType::GetInterfaceID();

        Reset();
    }

    /**
     * 析构函数
     */
    // Destructor
    virtual ~Interface() {
        Release();
    }

    /**
     * 重载赋值操作符
     * @param[in] inf 赋值对象
     * @return 当前对象
     */
    Interface<InterfaceType>& operator=(Interface<InterfaceType>& inf) {
        Release();
        if (inf.interface_) {
            interface_ = inf.interface_;
            class_info_ = inf.class_info();
            reference_count_ = inf.reference_count_;
            (*reference_count_)++;
        }
        return *this;
    }

    /**
     * 重载等于操作符(主要用于与NULL比较）
     * @return 是否等于NULL
     */
    bool operator==(const int) const {
        //ASSERT(0 == nul);
        return NULL == interface_;
    }

    /**
     * 重载不等于操作符(主要用于与NULL比较）
     * @return 是否不等于NULL
     */
    bool operator!=(const int) const {
        //ASSERT(0 == nul);
        return NULL == interface_;
    }

    /**
     * 重载等于操作符
     * @return 对象是否相等
     */
    bool operator==(const Interface<InterfaceType>& src) const {
        return interface_ == src.interface_;
    }

    /**
     * 重载不等于操作符
     * @return 对象是否不相等
     */
    bool operator!=(const Interface<InterfaceType>& src) const {
        return interface_ != src.interface_;
    }

    /**
     * 重载真操作符
     * @return 对象是否有效
     */
    inline operator bool() const {
        return NULL != interface_;
    }

    /**
     * 重载非真操作符
     * @return 对象是否无效
     */
    inline bool operator!() const {
        return NULL == interface_;
    }

    /**
     * 获取内部对象指针
     * @return 内部对象指针
     * @warning 尽量不要直接使用此指针，因为当内部对象销毁后指针将无效
     */
    inline InterfaceType* P() const {
        return interface_;
    }

    /**
     * 获取引用计数指针
     * @return 引用计数指针
     * @warning 不要在外面改变引用计数的值
     */
    inline const long* reference_count() const {
        return reference_count_;
    }

    /**
     * 获取类信息实体
     * @return 类信息实体
     */
    inline const ClassEntry class_info() const {
        return class_info_;
    }

    /**
     * 重载指针操作符
     * @return 内部对象的指针
     */
    inline InterfaceType* operator->() const {
        return interface_;
    }

    /**
     * 释放所有资源
     */
    // Release Interface reference
    // Note: don't use the Interface after Release(), until init again.
    void Release() {
        if (interface_) {
            (*reference_count_)--;
            if (0 == *reference_count_) {
                class_info_.destroy_object(interface_);
                interface_ = NULL;
                delete reference_count_;
                reference_count_ = NULL;
            }
        }

        Reset();
    }

protected:
    /**
     * 通过类ID创建对象(内部使用)
     */
    // Create Interface instance with class id
    void Create(const APFClassID& clsid) {
        // checking interface, interface must have GetInterfaceID() function
        // see APF_INTERFACE
        InterfaceType::GetInterfaceID();

        Reset();
        void* obj = APFCreateObject(clsid, class_info_);
        if (NULL != obj && class_info_.query_interface(InterfaceType::GetInterfaceID())) {
            interface_ = static_cast<InterfaceType*>(obj);
        }
        if (interface_) {
            reference_count_ = new long;
            *reference_count_ = 1;
        }
    }

    /**
     * 重新初始化内部变量
     */
    // reset vars
    void Reset() {
        interface_ = NULL;
        reference_count_ = NULL;
        class_info_ = ClassEntry();
    }

private:
    // pointer reference count
    long *reference_count_;
    // object pointer
    InterfaceType *interface_;
    // object info
    ClassEntry class_info_;
};

} // namespace

#endif // INTERFACE_H
