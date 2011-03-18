/***************************************************************************
 *            toolsLibrary.cc
 *
 *  Thu Mar 13 2008
 *  Copyright  2008  Dmytro Milinevskyy
 *  milinevskyy@gmail.com
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modifyto
 *  it under the terms of the GNU Lesser General Public License version 2.1 as published by
 *  the Free Software Foundation;
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/directives.h>

#ifdef DL_EXT
#include <dlfcn.h>
#include <stdlib.h>

#include <libdodo/toolsLibrary.h>
#include <libdodo/toolsLibraryEx.h>
#include <libdodo/types.h>
#include <libdodo/toolsMisc.h>

using namespace dodo::tools;

library::library() : handle(NULL)
{
}

//-------------------------------------------------------------------

library::library(const dodo::string &path)
{
#ifdef DL_FAST
    handle = dlopen(path.data(), RTLD_LAZY | RTLD_NODELETE);
#else
    handle = dlopen(path.data(), RTLD_LAZY);
#endif

    if (handle == NULL)
        dodo_throw exception::basic(exception::MODULE_TOOLSLIBRARY, LIBRARYEX_LIBRARY, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

library::~library()
{
#ifndef DL_FAST
    if (handle != NULL)
        dlclose(handle);

#endif
}

//-------------------------------------------------------------------

void
library::open(const dodo::string &path)
{
#ifndef DL_FAST
    if (handle != NULL)
        if (dlclose(handle) != 0)
            dodo_throw exception::basic(exception::MODULE_TOOLSLIBRARY, LIBRARYEX_OPEN, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

#endif

#ifdef DL_FAST
    handle = dlopen(path.data(), RTLD_LAZY | RTLD_NODELETE);
#else
    handle = dlopen(path.data(), RTLD_LAZY);
#endif

    if (handle == NULL)
        dodo_throw exception::basic(exception::MODULE_TOOLSLIBRARY, LIBRARYEX_OPEN, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
library::close()
{
#ifndef DL_FAST
    if (handle != NULL)
        if (dlclose(handle) != 0)
            dodo_throw exception::basic(exception::MODULE_TOOLSLIBRARY, LIBRARYEX_CLOSE, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);
#endif
}

//-------------------------------------------------------------------

void *
library::function(const dodo::string &name)
{
    if (handle == NULL)
        dodo_throw exception::basic(exception::MODULE_TOOLSLIBRARY, LIBRARYEX_FUNCTION, exception::ERRNO_LIBDODO, LIBRARYEX_LIBRARYNOTOPENED, TOOLSLIBRARYEX_NOTOPENED_STR, __LINE__, __FILE__);

    void *func = dlsym(handle, name.data());
    if (func == NULL)
        dodo_throw exception::basic(exception::MODULE_TOOLSLIBRARY, LIBRARYEX_FUNCTION, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

    return func;
}

//-------------------------------------------------------------------

void *
library::operator[](const dodo::string &name)
{
    if (handle == NULL)
        dodo_throw exception::basic(exception::MODULE_TOOLSLIBRARY, LIBRARYEX_BROPERATORSTRING, exception::ERRNO_LIBDODO, LIBRARYEX_LIBRARYNOTOPENED, TOOLSLIBRARYEX_NOTOPENED_STR, __LINE__, __FILE__);

    void *func = dlsym(handle, name.data());
    if (func == NULL)
        dodo_throw exception::basic(exception::MODULE_TOOLSLIBRARY, LIBRARYEX_BROPERATORSTRING, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

    return func;
}

#endif

//-------------------------------------------------------------------
