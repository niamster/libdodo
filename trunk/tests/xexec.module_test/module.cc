#include <libdodo/xexec.h>
#include <libdodo/ioSTD.h>

#include <string.h>
#include <iostream>

using namespace dodo;

extern "C"
{

	void
	hook(void *base, 
		xexecObjTypeEnum type,
		void *yep)
	{
		if (type == XEXEC_OBJECT_IOSTD)
		{
			std::cout << "ioSTD module\t";
			
			__xexexIoCollectedData *st = (__xexexIoCollectedData *)base;
			if (st->operType == IOSTD_OPERATION_WRITE)
			{
				char q[100];
				int *a;
				
				a = (int *)(st->buffer.c_str());
				sprintf(q,"%d\n",*a*3);
				st->buffer.assign(q);
			}
		}
		std::cout << "activation\n";
	}

	void
	empty(void *base, 
		xexecObjTypeEnum type,
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
