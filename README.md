
# APF框架介绍

 **APF(Application Plugin Framework) 是一套轻量级的插件框架, 采用C++编写，支持windows、linux等平台。**
 * 本框架的主要思想是使用接口模块化编程，使用依赖注入方式动态创建对象。<BR/>
 * 应用程序可以基于APF插件框架进行快速开发，APF插件框架中的插件既可以单独使用，又可以灵活组合使用。

---------------------------------------
###APF插件框架的特点：

#### 接口定义简单灵活
 *    采用普通的C++接口，即由纯虚函数组成的结构体，不需要特殊的基类，不需要宏和UUID申明；同时可以使用C++的各种变量类型，不受COM接口那样的约束。例如下面的接口定义：
```cpp
class IExample {
    APF_DECLARE_INTERFACE(IExample)
    virtual void SayHello() = 0;  
};
```

#### 接口与实现分离
 *     对外提供接口文件，在插件内部用类来实现一个或多个接口，不需要对外导出该类或暴露实现细节。这样还有一个好处是只有约定了接口就可以让多个模块并行开发，模块相互之间不存在编译依赖（不需要其他插件的LIB等文件），这可用于测试驱动开发模式。
### 多接口转换、引用计数管理 
 *     采用智能指针类来管理接口的引用计数及生命期，可从一个接口动态转换为另一个接口（内部采用自定义类动态识别机制），可以区分插件内部的接口引用和插件外部的接口引用。

#### 模块透明部署 
 *     一个模块只需要使用其他模块的接口，不需要关心该接口是在哪个插件中实现的。可以根据需要将各个实现类进行合并或拆分，使其分布到不同插件中，而接口使用者不受影响。另外，插件部署于哪个目录也不影响插件接口的使用。

#### 模块可替换、可扩展 
 *     可根据需要替换某个插件，只有该插件实现了相同的接口，即使内部功能不相同，这样就实现了插件可替换、按需组合。通过在新的插件中支持更多的接口，可扩展更多的功能。可以在新插件中局部替换原有插件的某些接口或部分函数，实现重用和扩展。

#### 跨版本 
 *     允许不同版本的C++开发的插件相互调用对方的接口，虽然实际中一般不需要这样做。由于没有采用C++特殊的编译指令，因此容易移植到其他开发平台下。

----------------------------------------------
###示例
```cpp
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
```
--------------------------------------------

 > @version 1.0
 
 > @author Paul Zhou qianlong.zhou@gmail.com
 
