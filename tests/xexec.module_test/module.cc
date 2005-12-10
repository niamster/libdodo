#include <xexec.h>
#include <flushSTD.h>
#include <string.h>

using namespace dodo;

extern "C"
{

	void
	hook(void *base, 
		xexecObjTypeEnum type,
		void *yep)
	{
		std::cout << type;
		
		flushSTD *st = (flushSTD *)base;
		if (st->operType == FLUSHSTD_OPER_WRITE)
		{
			char q[100];
			int *a;
			a = (int *)(st->buffer.c_str());
			sprintf(q,"!!%d!!\n",*a);
			st->buffer.assign(q);
		}
	}
	
	xexecExMod 
	initXexecModule()
	{
		xexecExMod module;
		
		strcpy(module.name,"test");
		strcpy(module.discription,"test module");
		strcpy(module.hook,"hook");
		module.preExec = true;///preffered
		
		return module;
	}

};
