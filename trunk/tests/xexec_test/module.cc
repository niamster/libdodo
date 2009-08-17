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
		std::cout << "stdio module ";

		__xexecIoChannelCollectedData__ *st = (__xexecIoChannelCollectedData__ *)odata;
		if (st->operType == IO_OPERATION_WRITE)
			st->buffer.assign("<" + dodoString(1, st->buffer[0]) + ">\n");
	}
	std::cout << "hook\n";
#endif
}

void
empty(__xexecCollectedData__ *odata,
      short int type,
	  void *udata)
{

	std::cout << "empty hook\n";
}

__xexecModule__
xexecModuleInit(void *data)
{
	__xexecModule__ module;

	strcpy(module.name, "test");
	strcpy(module.discription, "test module");
	module.type = XEXEC_ACTION_PREEXEC;

	if (data == NULL)
		strcpy(module.hook, "empty");
	else
		strcpy(module.hook, "hook");

	return module;
}

void
xexecModuleDeinit()
{
	std::cout << "deactivation\n";
}

};
#endif
