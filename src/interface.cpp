/*!**************************************************************************
 * @file
 * @brief 接口(Interface)内部函数实现文件
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
#include "interface.h"
#include "class.h"
#include "module.h"

namespace apf {

// Create an object with the class id.
void* APFCreateObject(const APFClassID& clsid, ClassEntry& class_info) {
    void* pobj = Class::CreateObject(clsid, &class_info);
    if (!pobj && apf::Module::Instance()->object_creator) {
        pobj = apf::Module::Instance()->object_creator(clsid, class_info);
    }
    return pobj;
}

} //namespace

