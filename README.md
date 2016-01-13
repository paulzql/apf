
# APF

 **APF(Application Plugin Framework) It is a lightweight plugin framework, written in C++, support windows, linux and other platforms.**
 
* The main idea of this framework is to use the interface modular programming, using dependency injection to dynamically create objects.
    
* Applications may be based APF plug-in framework for rapid development, the APF plugin framework plug can be used alone, but can be flexibly combined use.

---------------------------------------
### Features：

#### Simple and flexible interface definition
 *    Using common C++ interface, no special base classes or declare UUID, at the same time you can use a variety of variable types in C++. 

example:
```cpp
class IExample {
    APF_DECLARE_INTERFACE(IExample)
    virtual void SayHello() = 0;  
};
```

#### Separation of interface and implementation
 *     Provide external interface file, the plugin for internal use classes to implement one or more interfaces, without the external exposure derived class or implementation details. 

### Multi-interface conversion, the reference count management 
 *     Smart pointer class management interface to reference count and lifetime of the dynamic conversion from one interface to another interface.

#### Transparent Deployment Module
 *     A module only need to use other modules interface, do not care that the interface is implemented in which the plug. You can implement as needed for each class merger or split, to be distributed to different plugins, and user interfaces are not affected.

#### Module Alternatively and extensible 
 *  You can replace old plugin by supporting more interfaces.

#### Cross-version 
 *     Allows different versions of the  plugins developed call each other's interface.

----------------------------------------------
###example:
```cpp
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "../include/interface.h"
#include "../include/module.h"
#include "../include/plugin_manager.h"
#include "../include/class.h"
#ifdef WIN32
#pragma comment(lib, "../lib/apf-d.lib")
#endif
using namespace std;
// define a interface
class IExample {
public:
    APF_DECLARE_INTERFACE(IExample)
    virtual void SayHello() = 0;
};
// implement interface
class Example : public IExample{
APF_BEGIN_CLASS()
APF_INTERFACE_ENTRY(IExample)
APF_END_CLASS()
public:
    void SayHello(){printf("hello\n");}
};
// define class id
APF_DECLARE_CLASSID(CLSID_Example, "Example")
// Associate class
APF_BEGIN_INTERNAL_MODULE()
APF_CLASSMAP_ENTRY(CLSID_Example, Example)
APF_END_INTERNAL_MODULE()
int main() {
    apf::PluginManager::instance()->SetVersion(1, 0);
    // load internal modules
    APFLoadInternalModules();
    // create a IExample object
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

