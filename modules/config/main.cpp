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

#include "config_wrapper.h"
#include "module.h"

APF_BEGIN_MODULE(APF_VERSION(1,0), 0, APF_MAX_VERSION)
APF_CLASSMAP_ENTRY(CLSID_Config, ConfigWrapper)
APF_END_MODULE()

