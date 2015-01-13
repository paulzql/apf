#include <iostream>
#include <stdio.h>
#include<stdlib.h>

#include "../../include/interface.h"
#include "../../include/signal.h"
#include "../../include/module.h"
#include "../../include/plugin_manager.h"
#include "../../include/class.h"

#ifdef WIN32
#pragma comment(lib, "../../lib/apf-d.lib")
#endif

using namespace std;

class A {
public:
    APF_DECLARE_INTERFACE(A)
    virtual void f() = 0;
};

class C {
APF_DECLARE_INTERFACE(C)
virtual void f() = 0;
};

class B : public A,C{
APF_BEGIN_CLASS()
APF_INTERFACE_ENTRY(A)
APF_INTERFACE_ENTRY(C)
APF_END_CLASS()
public:
signals:
    apf::Signal<int> signal;
slots:
    void f(){printf("hello\n");}
    void f(int){printf("hello\n");}
};

APF_BEGIN_INTERNAL_MODULE()
APF_CLASSMAP_ENTRY("B", B)
APF_END_INTERNAL_MODULE()

void TestSignal() {
    B *b = new B();

    APFConnect(b, signal, b, &B::f);

    b->signal(0);

    APFDisconnect(b, signal, b, &B::f);
    b->signal(0);
}

void TestInternalModule() {
    apf::Interface<A> a("B");
    a->f();
    apf::Interface<C> c(a);
    c->f();
}
#include "../simpleplugin/isimpleplugin.h"
void TestSimplePlugin() {
#ifdef WIN32
	apf::PluginManager::instance()->Load("../simpleplugin/bin/Debug/simpleplugin.dll");
#else
    apf::PluginManager::instance()->Load("../simpleplugin/bin/Debug/libsimpleplugin.so");
#endif
    apf::Interface<ISimplePlugin> simple_plugin(CLSID_SimplePlugin);
    simple_plugin->test();
	simple_plugin.Release();
}

#include "../../modules/log/ilog.h"
void TestLog() {
    apf::PluginManager::instance()->Load("../../modules/log/bin/Debug/liblog.so");

    apf::Interface<ILog> log(CLSID_Log);

    log->SetTargets(LOG_TARGET_CONSOLE | LOG_TARGET_NET);
    log->Start();
    for (int i = 0; i < 10; i++) {
        log->Info("test", "log test1");
    }
    log->Warn("test", "log test1");
    log->Warn("test", "log test2");
    log->Debug("test", "log test3");
    log->Error("test", "log test4");
    log->Debug("test", "log test5");
    log->Warn("test", "log test6");
    log->Warn("test", "log test7");
    log->Debug("test", "log test8");
    log->Error("test", "log test9");
    log->Debug("test", "log test10");
    log->Warn("test", "log test11");
    log->Warn("test", "log test12");
    log->Debug("test", "log test13");
    log->Error("test", "log test14");
    log->Debug("test", "log test15");

    log->End();

}


#include "../../modules/config/iconfig.h"
void TestConfig() {
    apf::PluginManager::instance()->Load("../../modules/config/bin/Debug/libconfig.so");
    apf::Interface<IConfig> config(CLSID_Config);
    config->Load("./config.conf");
    config->Put("key", "value");
    printf("%s\n", config->Get("key", "default"));
    config->Save("./config1.conf");
}

int main() {
	apf::PluginManager::instance()->SetVersion(1, 0);

    APFLoadInternalModules();
    //TestInternalModule();

    //TestLog();

    TestSimplePlugin();


    //TestConfig();

	int d;
	scanf("%d", &d);

    return 0;
}
