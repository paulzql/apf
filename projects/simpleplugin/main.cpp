// The functions contained in this file are pretty dummy
// and are included only as a placeholder. Nevertheless,
// they *will* get included in the shared library if you
// don't remove them :)
//
// Obviously, you 'll have to write yourself the super-duper
// functions to include in the resulting library...
// Also, it's not necessary to write every function in this file.
// Feel free to add more files in this project. They will be
// included in the resulting library.

#include <stdio.h>
#include "../../include/interface.h"
#include "../../include/module.h"
#include "isimpleplugin.h"
#include "../../modules/log/ilog.h"

class SimplePlugin : public ISimplePlugin {
APF_BEGIN_CLASS()
APF_INTERFACE_ENTRY(ISimplePlugin)
APF_END_CLASS()
    void test(){
        printf("SimplePlugin implement from ISimplePlugin\n");
    }

};

APF_BEGIN_MODULE(APF_VERSION(1,0), 0, APF_MAX_VERSION)
APF_CLASSMAP_ENTRY(CLSID_SimplePlugin, SimplePlugin)
APF_END_MODULE()


