 /*!
  *  @file
  *  @brief 内部模块的使用示例
  */
#include <iostream>
#include <stdio.h>
#include<stdlib.h>

#include "../include/interface.h"
#include "../include/signal.h"
#include "../include/module.h"
#include "../include/plugin_manager.h"
#include "../include/class.h"

#ifdef WIN32
#pragma comment(lib, "../lib/apf-d.lib")
#endif

using namespace std;
// 定义接口
class IExample {
public:
    APF_DECLARE_INTERFACE(IExample)
    virtual void SayHello() = 0;
};

// 实现类
class Example : public IExample{
APF_BEGIN_CLASS()
APF_INTERFACE_ENTRY(IExample)
APF_END_CLASS()
public:
    void SayHello(){printf("hello\n");}
};

// 定义ID
APF_DECLARE_CLASSID(CLSID_Example, "Example")

// 定义实现类与ID的映射
APF_BEGIN_INTERNAL_MODULE()
APF_CLASSMAP_ENTRY(CLSID_Example, Example)
APF_END_INTERNAL_MODULE()


int main() {
    apf::PluginManager::instance()->SetVersion(1, 0);
    // 加载内部模块
    APFLoadInternalModules();
    
    Interface<IExample> example(CLSID_Example);
    if (example) {
        example->SayHello();
    } else {
        printf("instance IExample failed\n");
    }

    return 0;
}

