#include <baseEx.h>
#include <iostream>
#include <string.h>

using namespace dodo;

extern "C"
{

	void
	baseHandler(errorModuleEnum module, 
				baseEx *ex, 
				void *data)
	{
		std::cout << "baseModuleHandler: " << ex->baseErrstr << std::endl;
	}
	
	exMod 
	initExModule()
	{
		exMod module;
		
		strcpy(module.name,"test");
		strcpy(module.discription,"test module");
		strcpy(module.hook,"baseHandler");
		module.module = ERRMODULE_FLUSHDISK;
		
		return module;
	}

	void
	deinitExModule()
	{
		std::cout << "deactivation.";
	}

};
