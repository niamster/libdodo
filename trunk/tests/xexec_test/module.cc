#include <libdodo/xexec.h>
#include <libdodo/ioStdio.h>
#include <libdodo/directives.h>

#include <string.h>
#include <iostream>

using namespace dodo;
using namespace io;

#ifdef DL_EXT

extern "C"
{

void
hook(void *odata,
	 xexecObjectTypeEnum type,
	 void *udata)
{
#ifndef IOSTDIO_WO_XEXEC

	if (type == XEXEC_OBJECT_IOSTDIO)
	{
		std::cout << "stdio module\t";

		__xexexIoStdioCollectedData *st = (__xexexIoStdioCollectedData *)odata;
		if (st->operType == STDIO_OPERATION_WRITE)
		{
			int *a = (int *)(st->buffer.c_str());
			st->buffer.assign(tools::string::iToString(*a * 3) + "\n");
		}
	}
	std::cout << "activation\n";

#endif
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

	strcpy(module.name, "test");
	strcpy(module.discription, "test module");
	module.execType = XEXEC_MODULEACTIONTYPE_PRE;

	if (data == NULL)
		strcpy(module.hook, "empty");
	else
		strcpy(module.hook, "hook");

	return module;
}

void
deinitXexecModule()
{
	std::cout << "deactivation.\n";
}

};

#endif
