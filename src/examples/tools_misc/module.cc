/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/dodo.h>
#include <iostream>
#include <string.h>

using namespace dodo;

#ifdef DL_EXT
extern "C"
{
void
handler(int                    module,
        dodo::exception::basic *ex,
        void                   *data)
{
    std::cout << "module:handler: " << ex->errStr << std::endl;
    std::cout << ex->message << std::endl;
}

dodo::exception::basic::__module__
initExceptionBasicModule(void *data)
{
    dodo::exception::basic::__module__ module = {
        "test",
        "test module",
        "handler",
        "cookie",
        { false, }
    };

    module.modules[exception::MODULE_TOOLSFILESYSTEM] = true;

    return module;
}

void
deinitExceptionBasicModule(char cookie[32])
{
    std::cout << "deactivation(cookie = " << cookie << ")\n";
}
};
#endif
