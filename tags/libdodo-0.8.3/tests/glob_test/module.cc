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
	
	__exMod 
	initExModule(void *data)
	{
		__exMod module;
		
		strcpy(module.name,"test");
		strcpy(module.discription,"test module");
		strcpy(module.hook,"baseHandler");
		module.module = ERRMODULE_IODISK;
		
		return module;
	}

	void
	deinitExModule()
	{
		std::cout << "deactivation.";
	}

};
#endif
