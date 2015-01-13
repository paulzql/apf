/*!**************************************************************************
 * @file
 * @brief 信号槽相关
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
#ifndef APFSIGNAL_H
#define APFSIGNAL_H

#include <vector>

////////////////////////////////////////////////////
// mutex functions, defined in oscore.cpp
#if defined(WIN32)
#ifndef pthread_mutex_t
#define pthread_mutex_t     CRITICAL_SECTION
#endif
#else
#include <pthread.h>
#endif
int init_mutex(pthread_mutex_t *mutex);
int uninit_mutex(pthread_mutex_t *mutex);
int lock_mutex(pthread_mutex_t *mutex);
int unlock_mutex(pthread_mutex_t *mutex);
/////////////////////////////////////////////////////

namespace apf {

// simple use apf signal and slot ///////////////////////////////////////
/**
 * @example signal.cpp 信号槽的使用示例
 */
/////////////////////////////////////////////////////////////////////////

#define slots public
#define signals public

/**
 * 连接信号与槽
 * @param[in] sender 信号发送者(指针)
 * @param[in] signal 信号(指针)
 * @param[in] receiver 信号接收者(指针)
 * @param[in] method 槽函数(指针)
 * @see APFDisconnect( sender, signal, receiver, method)
 * @warning 当receiver 销毁之前一定要先断开与之相关的信号连接
 */
// connect between signal and slot
#define APFConnect( sender, signal, receiver, method) ( (sender)->signal.Bind(receiver, method) )

/**
 * 断开信号与槽的连接
 * @param[in] sender 信号发送者(指针)
 * @param[in] signal 信号(指针)
 * @param[in] receiver 信号接收者(指针)
 * @param[in] method 槽函数(指针)
 * @see APFConnect( sender, signal, receiver, method)
 */
// disconnect between signal and slot
#define APFDisconnect( sender, signal, receiver, method) ( (sender)->signal.UnBind(receiver, method) )

/**
 * @brief 槽的基类
 */
// base slot class with parameter
template<class T1>
class SlotBase {
public:
    // invoke slot function
    /**
     * 执行槽函数
     * @param[in] param1 参数
     */
    virtual void Exec(T1 param1) = 0;

    /**
     * 默认析构函数
     */
    virtual ~SlotBase(){}
};

/**
 * @brief 槽的实现类
 */
// slot implement with parameter
template<class T, class T1>
class SlotImpl : public SlotBase<T1> {
public:
    /**
     * 构造函数
     * @param[in] obj 槽所属对象
     * @param[in] func槽函数
     */
    // constructor
    SlotImpl(T* obj, void (T::*func)(T1) ) {
        obj_ = obj;
        func_ = func;
    }

    // invoke slot function
    void Exec( T1 param1) {
        (obj_->*func_)(param1);
    }

private:
    // slot object
    T* obj_;
    // slot function
    void (T::*func_)(T1);
};

/**
 * @brief 槽封装
 */
// slot class with parameter
template<class T1>
class Slot {
public:
    /**
     * 构造函数
     * @param[in] obj 槽所属对象
     * @param[in] func槽函数
     */
    template<class T>
    Slot(T* obj, void (T::*func)(T1)) {
        slot_base_ = new SlotImpl<T, T1>(obj, func);
        obj_ = obj;
        func_ = reinterpret_cast<void*>(&func);
    }

    /**
     * 拷贝构造函数
     */
    // copy constructor
    // note: don't use the src after this function
    Slot(Slot<T1>& src) {
        slot_base_ = src.slot_base_;
        src.slot_base_ = NULL;
    }

    /**
     * 析构函数
     */
    ~Slot() {
        if (NULL != slot_base_) {
            delete slot_base_;
        }
    }

    /**
     * 判断槽对象是否与提供信息相等
     * @param[in] obj 槽所属对象
     * @param[in] func 槽函数
     * @return 是否相等
     */
    bool equals(void* obj, void* func) {
        return (obj_ == obj) && (func_ == func);
    }

    /**
     * 获取槽所属对象
     * @return 槽所属对象
     */
    void* obj() {
        return obj_;
    }

    void Exec(T1 param1) {
        slot_base_->Exec(param1);
    }

private:
    SlotBase<T1>* slot_base_;
    void* obj_;
    void* func_;
};

/**
 * @brief 信号类
 */
// signal class with parameter
template<class T1>
class Signal {
public:
    Signal() {
        init_mutex(&mutex_);
    }

    ~Signal() {
        lock_mutex(&mutex_);
        for (unsigned int i = 0; i < slot_set_.size(); i++) {
            delete slot_set_[i];
        }
        unlock_mutex(&mutex_);
        uninit_mutex(&mutex_);
    }

    /**
     * 绑定信号与槽
     * @param[in] obj 槽所属对象
     * @param[in] func 槽函数
     * @return 绑定是否成功
     */
    // bind between signal and slot
    template<class T>
    bool Bind(T* obj, void (T::*func)(T1)) {
        bool ret = true;
        lock_mutex(&mutex_);
        for(unsigned int i = 0; i < slot_set_.size(); i++) {
            if (slot_set_[i]->equals(obj, reinterpret_cast<void*>(&func))) {
                ret = false;
                break;
            }
        }
        if (ret) {
            slot_set_.push_back(new Slot<T1>(obj, func));
        }
        unlock_mutex(&mutex_);

        return ret;
    }

    /**
     * 取消信号与槽的绑定
     * @param[in] obj 槽所属对象
     * @param[in] func 槽函数
     * @return 取消绑定是否成功
     */
    // unbind between signal and slot
    template<class T>
    void UnBind(T* obj, void (T::*func)(T1)) {
        lock_mutex(&mutex_);
        for(unsigned int i = 0; i < slot_set_.size(); i++) {
            if (slot_set_[i]->equals(obj, reinterpret_cast<void*>(&func))) {
                delete slot_set_[i];
                // move front
                for (i++; i < slot_set_.size(); i++) {
                    slot_set_[i-1] = slot_set_[i];
                }
                slot_set_.resize(slot_set_.size()-1);
                break;
            }
        }
        unlock_mutex(&mutex_);
    }

    /**
     * 取消与对象相关的所有槽绑定
     * @param[in] obj 槽所属对象
     * @return 取消绑定是否成功
     */
    // unbind between signal and slot
    template<class T>
    void UnBind(T* obj) {
        lock_mutex(&mutex_);
        for(unsigned int i = 0; i < slot_set_.size(); i++) {
            if (slot_set_[i]->obj() == (void*)obj) {
                delete slot_set_[i];
                // remove from slot_set_
                for (i++; i < slot_set_.size(); i++) {
                    slot_set_[i-1] = slot_set_[i];
                }
                slot_set_.resize(slot_set_.size()-1);
                break;
            }
        }
        unlock_mutex(&mutex_);
    }

    /**
     * 重载括号操作符(执行绑定槽函数)
     * @param[in] param1 参数
     */
    void operator()(T1 param1) {
        lock_mutex(&mutex_);
        for(int i=0; i<(int)slot_set_.size(); i++) {
            slot_set_[i]->Exec(param1);
        }
        unlock_mutex(&mutex_);
    }

private:
    // connected slots
    std::vector< Slot<T1>* > slot_set_;
    // thread safe mutex
    pthread_mutex_t mutex_;
};

} // namespace

#endif // APFSIGNAL_H
