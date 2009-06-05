/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/dodo.h>
#include <string.h>
#include <iostream>

using namespace dodo;
using namespace io;

#ifdef DL_EXT

extern "C"
{

void
hook(__xexecCollectedData__ *odata,
     short int type,
	 void *udata)
{
#ifndef IO_WO_XEXEC

	if (type == XEXEC_OBJECT_IOSTDIO)
	{
		std::cout << "stdio module\t";

		__xexecIoChannelCollectedData__ *st = (__xexecIoChannelCollectedData__ *)odata;
		if (st->operType == IO_OPERATION_WRITE)
		{
			int *a = (int *)(st->buffer.c_str());
			st->buffer.assign(tools::string::iToString(*a * 3) + "\n");
		}
	}
	std::cout << "activation\n";

#endif
}

void
empty(__xexecCollectedData__ *odata,
      short int type,
	  void *udata)
{

	std::cout << "empty activation\n";
}

__xexecMod__
initXexecModule(void *data)
{
	__xexecMod__ module;

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
