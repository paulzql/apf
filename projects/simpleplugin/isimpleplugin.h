#ifndef ISIMPLEPLUGIN_INCLUDED
#define ISIMPLEPLUGIN_INCLUDED

class ISimplePlugin {
public:
    APF_DECLARE_INTERFACE(ISimplePlugin)
    virtual void test()=0;
};

APF_DECLARE_CLASSID(CLSID_SimplePlugin, "SimplePlugin")

#endif // ISIMPLEPLUGIN_INCLUDED
