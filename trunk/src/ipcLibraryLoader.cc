/***************************************************************************
 *            ipcLibraryLoader.cc
 *
 *  Thu Mar 13 20:50:55 2008
 *  Copyright  2008  Ni@m
 *  niam.niam@gmail.com
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
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

#include <libdodo/ipcLibraryLoader.h>

#ifdef DL_EXT

using namespace dodo::ipc;

libraryLoader::libraryLoader() : handle(NULL)
{
}

//-------------------------------------------------------------------

libraryLoader::libraryLoader(const dodoString &path) : handle(NULL)
{
#ifdef DL_FAST
	handle = dlopen(path.c_str(), RTLD_LAZY|RTLD_NODELETE);
#else
	handle = dlopen(path.c_str(), RTLD_LAZY);
#endif
}

//-------------------------------------------------------------------

libraryLoader::~libraryLoader()
{
#ifdef DL_FAST	
	if (handle != NULL)
		dlclose(handle);
#endif
}

//-------------------------------------------------------------------

void
libraryLoader::open(const dodoString &path)
{
#ifdef DL_FAST
	handle = dlopen(path.c_str(), RTLD_LAZY|RTLD_NODELETE);
#else
	handle = dlopen(path.c_str(), RTLD_LAZY);
#endif
	if (handle == NULL)
		throw baseEx(ERRMODULE_IPCLIBRARYLOADER, LIBRARYLOADEREX_OPEN, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
libraryLoader::close()
{
#ifndef DL_FAST
	if (dlclose(handle) != 0)
		throw baseEx(ERRMODULE_IPCLIBRARYLOADER, LIBRARYLOADEREX_CLOSE, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);
#endif
}

//-------------------------------------------------------------------

void *
libraryLoader::get(const dodoString &name)
{
	if (handle == NULL)
		throw baseEx(ERRMODULE_IPCLIBRARYLOADER, LIBRARYLOADEREX_GET, ERR_LIBDODO, LIBRARYLOADEREX_LIBRARYNOTOPENED, LIBRARYLOADEREX_LIBRARYNOTOPENED_STR, __LINE__, __FILE__);
	
	void *func = dlsym(handle, name.c_str());
	if (func == NULL)
		throw baseEx(ERRMODULE_IPCLIBRARYLOADER, LIBRARYLOADEREX_GET, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void *
libraryLoader::operator[](const dodoString &name)
{
	if (handle == NULL)
		throw baseEx(ERRMODULE_IPCLIBRARYLOADER, LIBRARYLOADEREX_BROPERATORSTRING, ERR_LIBDODO, LIBRARYLOADEREX_LIBRARYNOTOPENED, LIBRARYLOADEREX_LIBRARYNOTOPENED_STR, __LINE__, __FILE__);
	
	void *func = dlsym(handle, name.c_str());
	if (func == NULL)
		throw baseEx(ERRMODULE_IPCLIBRARYLOADER, LIBRARYLOADEREX_BROPERATORSTRING, ERR_DYNLOAD, 0, dlerror(), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

#ifdef BFD_EXT

dodo::dodoStringArray 
libraryLoader::getSymbols(const dodoString &path)
{
	bfd_init();
	
	bfd *lib = bfd_openr(path.c_str(), NULL);
	if (lib == NULL)
	{
		bfd_error_type err = bfd_get_error();
		throw baseEx(ERRMODULE_IPCLIBRARYLOADER, LIBRARYLOADEREX_GETSYMBOLS, ERR_BFD, err, bfd_errmsg(err), __LINE__, __FILE__);
	}
    
	if (bfd_check_format(lib, bfd_object) == FALSE)
	{
		bfd_error_type err = bfd_get_error();
		throw baseEx(ERRMODULE_IPCLIBRARYLOADER, LIBRARYLOADEREX_GETSYMBOLS, ERR_BFD, err, bfd_errmsg(err), __LINE__, __FILE__);
	}

	long storageSize = bfd_get_symtab_upper_bound(lib);
	
	if (storageSize < 0)
	{
		bfd_error_type err = bfd_get_error();
		throw baseEx(ERRMODULE_IPCLIBRARYLOADER, LIBRARYLOADEREX_GETSYMBOLS, ERR_BFD, err, bfd_errmsg(err), __LINE__, __FILE__);
	}
	
	if (storageSize == 0)
	   return dodoStringArray();

	asymbol **symbolTable = (asymbol **)malloc(storageSize);

	long numberOfSymbols = bfd_canonicalize_symtab(lib, symbolTable);
	
	if (numberOfSymbols < 0)
	{
		bfd_error_type err = bfd_get_error();
		throw baseEx(ERRMODULE_IPCLIBRARYLOADER, LIBRARYLOADEREX_GETSYMBOLS, ERR_BFD, err, bfd_errmsg(err), __LINE__, __FILE__);
	}
	
	dodoStringArray arr;
	for (long i=0;i<numberOfSymbols;++i)
		if (isSetFlag(symbolTable[i]->flags, BSF_FUNCTION) && isSetFlag(symbolTable[i]->flags, BSF_GLOBAL))
		arr.push_back(symbolTable[i]->name);
    
	if (bfd_close(lib) == FALSE)
	{
		bfd_error_type err = bfd_get_error();
		throw baseEx(ERRMODULE_IPCLIBRARYLOADER, LIBRARYLOADEREX_GETSYMBOLS, ERR_BFD, err, bfd_errmsg(err), __LINE__, __FILE__);
	}
	
	return arr;
}

#endif

#endif

//-------------------------------------------------------------------
