/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/exceptionBasic.h>

#include <iostream>
#include <string.h>

using namespace dodo;

#ifdef DL_EXT
extern "C"
{
	void
	baseHandler(int module,
				dodo::exception::basic *ex,
				void *data)
	{
		std::cout << "baseModuleHandler: " << ex->baseErrstr << std::endl;
		std::cout << ex->message << std::endl;
	}

	dodo::exception::__basicMod
	initBaseExModule(void *data)
	{
		dodo::exception::__basicMod module;

		strcpy(module.name, "test");
		strcpy(module.discription, "test module");
		strcpy(module.hook, "baseHandler");
		module.module = exception::ERRMODULE_TOOLSFILESYSTEM;

		return module;
	}

	void
	deinitBaseExModule()
	{
		std::cout << "deactivation.";
	}
};
#endif
