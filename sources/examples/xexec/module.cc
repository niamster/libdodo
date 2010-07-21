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
hook(xexec::__collected_data__ *odata UNUSED,
     short                     type UNUSED,
     short                     operation UNUSED,
     void                      *udata UNUSED)
{
#ifndef IO_WO_XEXEC
    if (type == xexec::OBJECT_IOSTDIO) {
        std::cout << "stdio module ";

        channel::__collected_data__ *st = (channel::__collected_data__ *)odata;
        if (operation == channel::OPERATION_WRITE)
            st->buffer += dodo::string("<" + dodo::string(st->buffer[0]) + ">\n");
    }
    std::cout << "hook\n";
#endif
}

void
empty(xexec::__collected_data__ *odata UNUSED,
      short int                 type UNUSED,
      void                      *udata UNUSED)
{
    std::cout << "empty hook\n";
}

xexec::__module__
initXexecModule(void *data)
{
    xexec::__module__ module = {
        {'\0', },
        {'\0', },
        {'\0', },
        {'\0', },
        0
    };

    std::cout << "activation\n";

    strcpy(module.name, "test");
    strcpy(module.discription, "test module");
    strcpy(module.cookie, "test cookie");
    module.type = xexec::ACTION_PREEXEC;

    if (data == NULL)
        strcpy(module.hook, "empty");
    else
        strcpy(module.hook, "hook");

    return module;
}

void
deinitXexecModule(char cookie[32])
{
    std::cout << "deactivation(cookie = " << cookie << ")\n";
}
};
#endif
