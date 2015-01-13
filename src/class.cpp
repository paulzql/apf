/*!**************************************************************************
 * @file
 * @brief APF Class 类实现
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
#include <assert.h>
#include "class.h"
#include "oscore.h"

namespace apf {
static pthread_mutex_t global_mutex;
static int init_global_mutex_ret = init_mutex(&global_mutex);
// class entry map
std::map<const APFClassID, ClassEntry> Class::class_map_;

// register class entry
bool Class::RegisterClass(const ClassEntry& class_entry, bool replace) {
    assert(0 == init_global_mutex_ret);

    bool ret = false;

    lock_mutex(&global_mutex);
    std::map<const APFClassID, ClassEntry>::iterator it = Class::class_map_.find(class_entry.clsid);
    if (class_map_.end() == it) {
        class_map_.insert(std::map<const APFClassID, ClassEntry>::value_type(class_entry.clsid, class_entry));
        ret = true;
    } else if (replace) {
        class_map_[class_entry.clsid] = class_entry;
        ret = true;
    }
    unlock_mutex(&global_mutex);

    return ret;
}

// unregister class
void Class::UnRegisterClass(const ClassEntry& class_entry) {
    lock_mutex(&global_mutex);
    std::map<const APFClassID, ClassEntry>::iterator it = class_map_.find(class_entry.clsid);
    if ((class_map_.end() != it) && (it->second.equals(class_entry))) {
        class_map_.erase(class_entry.clsid);
    }
    unlock_mutex(&global_mutex);
}

// unregister class
void Class::UnRegisterClass(const APFClassID& class_id) {
    lock_mutex(&global_mutex);
    if (class_map_.end() != class_map_.find(class_id)) {
        class_map_.erase(class_id);
    }
    unlock_mutex(&global_mutex);
}


// create an object with class id and interface id
void* Class::CreateObject(const APFClassID& class_id, ClassEntry* class_info) {
    void* p_interface = NULL;
    lock_mutex(&global_mutex);
    std::map<const APFClassID, ClassEntry>::iterator it = class_map_.find(class_id);
    if (class_map_.end() != it) {
        p_interface = it->second.create_object();
        if (p_interface) {
            *class_info = it->second;
        }
    }
    unlock_mutex(&global_mutex);

    return p_interface;
}

// destroy object
void Class::DestroyObject(void* object, ClassEntry* class_info) {
    if (NULL != class_info->destroy_object) {
        class_info->destroy_object(object);
    }
}

// have registered the class id?
bool Class::HasClass(const APFClassID& class_id) {
    bool ret = false;

    lock_mutex(&global_mutex);
    if (class_map_.end() != class_map_.find(class_id)) {
        ret = true;
    }
    unlock_mutex(&global_mutex);

    return ret;
}

} // namespace
