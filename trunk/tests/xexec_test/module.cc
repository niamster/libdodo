#include <libdodo/xexec.h>
#include <libdodo/ioSTD.h>
#include <libdodo/directives.h>

#include <string.h>
#include <iostream>

using namespace dodo;

#ifdef DL_EXT

extern "C"
{

	void
	hook(void *base, 
		xexecObjectTypeEnum type,
		void *yep)
	{
		if (type == XEXEC_OBJECT_IOSTD)
		{
			std::cout << "ioSTD module\t";
			
			__xexexIoCollectedData *st = (__xexexIoCollectedData *)base;
			if (st->operType == IOSTD_OPERATION_WRITE)
			{
				int *a = (int *)(st->buffer.c_str());
				st->buffer.assign(toolsString::iToString(*a*3) + "\n");
			}
		}
		std::cout << "activation\n";
	}

	void
	empty(void *base, 
		xexecObjectTypeEnum type,
		void *yep)
	{
	
		std::cout << "empty activation\n";
	}
	
	__xexecMod 
	initXexecModule(void *data)
	{	
		__xexecMod module;
		
		strcpy(module.name,"test");
		strcpy(module.discription,"test module");
		module.execType = XEXEC_MODULEACTIONTYPE_PRE;
		
		if (data == NULL)
			strcpy(module.hook,"empty");
		else
			strcpy(module.hook,"hook");
		
		return module;
	}

	void
	deinitXexecModule()
	{
		std::cout << "deactivation.\n";
	}

};

#endif
