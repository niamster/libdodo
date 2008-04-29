#include <libdodo/xexec.h>
#include <libdodo/ioStdio.h>
#include <libdodo/directives.h>

#include <string.h>
#include <iostream>

using namespace dodo;

#ifdef DL_EXT

extern "C"
{

	void
	hook(void *odata, 
		xexecObjectTypeEnum type,
		void *udata)
	{
		if (type == XEXEC_OBJECT_IOSTDIO)
		{
			std::cout << "ioStdio module\t";
			
			__xexexIoCollectedData *st = (__xexexIoCollectedData *)odata;
			if (st->operType == IOSTDIO_OPERATION_WRITE)
			{
				int *a = (int *)(st->buffer.c_str());
				st->buffer.assign(toolsString::iToString(*a*3) + "\n");
			}
		}
		std::cout << "activation\n";
	}

	void
	empty(void *odata, 
		xexecObjectTypeEnum type,
		void *udata)
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
