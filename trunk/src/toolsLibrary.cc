/***************************************************************************
 *            toolsLibrary.cc
 *
 *  Thu Mar 13 20:50:55 2008
 *  Copyright  2008  Ni@m
 *  niam.niam@gmail.com
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
#ifdef BFD_EXT
#include <bfd.h>
#endif
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

library::library(const dodoString &path)
{
#ifdef DL_FAST
	handle = dlopen(path.c_str(), RTLD_LAZY | RTLD_NODELETE);
#else
	handle = dlopen(path.c_str(), RTLD_LAZY);
#endif

	if (handle == NULL)
		throw exception::basic(exception::ERRMODULE_TOOLSLIBRARY, LIBRARYEX_LIBRARY, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);
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
library::open(const dodoString &path)
{
#ifndef DL_FAST
	if (handle != NULL)
		if (dlclose(handle) != 0)
			throw exception::basic(exception::ERRMODULE_TOOLSLIBRARY, LIBRARYEX_OPEN, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

#endif

#ifdef DL_FAST
	handle = dlopen(path.c_str(), RTLD_LAZY | RTLD_NODELETE);
#else
	handle = dlopen(path.c_str(), RTLD_LAZY);
#endif

	if (handle == NULL)
		throw exception::basic(exception::ERRMODULE_TOOLSLIBRARY, LIBRARYEX_OPEN, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
library::close()
{
#ifndef DL_FAST
	if (handle != NULL)
		if (dlclose(handle) != 0)
			throw exception::basic(exception::ERRMODULE_TOOLSLIBRARY, LIBRARYEX_CLOSE, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);
#endif
}

//-------------------------------------------------------------------

void *
library::get(const dodoString &name)
{
	if (handle == NULL)
		throw exception::basic(exception::ERRMODULE_TOOLSLIBRARY, LIBRARYEX_GET, exception::ERRNO_LIBDODO, LIBRARYEX_LIBRARYNOTOPENED, TOOLSLIBRARYEX_NOTOPENED_STR, __LINE__, __FILE__);

	void *func = dlsym(handle, name.c_str());
	if (func == NULL)
		throw exception::basic(exception::ERRMODULE_TOOLSLIBRARY, LIBRARYEX_GET, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	return func;
}

//-------------------------------------------------------------------

void *
library::operator[](const dodoString &name)
{
	if (handle == NULL)
		throw exception::basic(exception::ERRMODULE_TOOLSLIBRARY, LIBRARYEX_BROPERATORSTRING, exception::ERRNO_LIBDODO, LIBRARYEX_LIBRARYNOTOPENED, TOOLSLIBRARYEX_NOTOPENED_STR, __LINE__, __FILE__);

	void *func = dlsym(handle, name.c_str());
	if (func == NULL)
		throw exception::basic(exception::ERRMODULE_TOOLSLIBRARY, LIBRARYEX_BROPERATORSTRING, exception::ERRNO_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);

	return func;
}

//-------------------------------------------------------------------

#ifdef BFD_EXT
dodo::dodoStringArray
library::getSymbols(const dodoString &path)
{
	bfd_init();

	bfd *lib = bfd_openr(path.c_str(), NULL);
	if (lib == NULL) {
		bfd_error_type err = bfd_get_error();
		throw exception::basic(exception::ERRMODULE_TOOLSLIBRARY, LIBRARYEX_GETSYMBOLS, exception::ERRNO_BFD, err, bfd_errmsg(err), __LINE__, __FILE__);
	}

	if (bfd_check_format(lib, bfd_object) == FALSE) {
		bfd_error_type err = bfd_get_error();
		throw exception::basic(exception::ERRMODULE_TOOLSLIBRARY, LIBRARYEX_GETSYMBOLS, exception::ERRNO_BFD, err, bfd_errmsg(err), __LINE__, __FILE__);
	}

	long storageSize = bfd_get_symtab_upper_bound(lib);

	if (storageSize < 0) {
		bfd_error_type err = bfd_get_error();
		throw exception::basic(exception::ERRMODULE_TOOLSLIBRARY, LIBRARYEX_GETSYMBOLS, exception::ERRNO_BFD, err, bfd_errmsg(err), __LINE__, __FILE__);
	}

	if (storageSize == 0)
		return dodoStringArray();

	asymbol **symbolTable = (asymbol **)malloc(storageSize);

	long numberOfSymbols = bfd_canonicalize_symtab(lib, symbolTable);

	if (numberOfSymbols < 0) {
		bfd_error_type err = bfd_get_error();
		throw exception::basic(exception::ERRMODULE_TOOLSLIBRARY, LIBRARYEX_GETSYMBOLS, exception::ERRNO_BFD, err, bfd_errmsg(err), __LINE__, __FILE__);
	}

	dodoStringArray arr;
	for (long i = 0; i < numberOfSymbols; ++i)
		if (isSetFlag(symbolTable[i]->flags, BSF_FUNCTION) && isSetFlag(symbolTable[i]->flags, BSF_GLOBAL))
			arr.push_back(symbolTable[i]->name);

	if (bfd_close(lib) == FALSE) {
		bfd_error_type err = bfd_get_error();
		throw exception::basic(exception::ERRMODULE_TOOLSLIBRARY, LIBRARYEX_GETSYMBOLS, exception::ERRNO_BFD, err, bfd_errmsg(err), __LINE__, __FILE__);
	}

	return arr;
}
#endif
#endif

//-------------------------------------------------------------------
