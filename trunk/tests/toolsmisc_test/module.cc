#include <libdodo/baseEx.h>

#include <iostream>
#include <string.h>

using namespace dodo;

#ifdef DL_EXT
extern "C"
{
void
baseHandler(errorModuleEnum module,
			baseEx *ex,
			void *data)
{
	std::cout << "baseModuleHandler: " << ex->baseErrstr << std::endl;
}

__baseExMod
initBaseExModule(void *data)
{
	__baseExMod module;

	strcpy(module.name, "test");
	strcpy(module.discription, "test module");
	strcpy(module.hook, "baseHandler");
	module.module = ERRMODULE_IOFILE;

	return module;
}

void
deinitBaseExModule()
{
	std::cout << "deactivation.";
}

};
#endif
