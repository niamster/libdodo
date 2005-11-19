#include <xexec.h>
#include <flushSTD.h>
#include <string.h>

using namespace dodo;

extern "C"
{

	void
	hook(dodoBase *base, 
		void *yep)
	{
		flushSTD *st = dynamic_cast<flushSTD *>(base->getSelf());
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
		module.name = (char *)malloc(5);
		strcpy(module.name,"test");
		
		module.discription = (char *)malloc(12);
		strcpy(module.discription,"test module");
		
		module.hook = (char *)malloc(5);
		strcpy(module.hook,"hook");
		return module;
	}

};
