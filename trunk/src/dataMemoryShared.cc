/***************************************************************************
 *            dataMemoryShared.cc
 *
 *  Wed Nov 21 01:25:14 2007
 *  Copyright  2007  Ni@m
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/dataMemoryShared.h>

using namespace dodo::data::memory;

shared::shared(shared &sts)
{
}

//-------------------------------------------------------------------

shared::shared(const dodoString &a_key) : mshared(NULL),
									  size(0),
									  shm(-1)
{
	key = '/';

	if (a_key.empty())
	{
		char _key[32];
		tools::misc::random(_key, 31);
		_key[31] = '\0';

		key.append(tools::code::MD5Hex(_key));
	}
	else
		key.append(a_key);

	shm = shm_open(key.c_str(), O_CREAT | O_RDWR, 0600);
}

//-------------------------------------------------------------------

shared::~shared()
{
	if (shm != -1)
	{
		if (mshared != NULL)
			munmap(mshared, size);

		shm_unlink(key.c_str());
	}
}

//-------------------------------------------------------------------

void *
shared::map(unsigned long size)
{
	unmap();

	if (shm == -1)
		throw exception::basic(exception::ERRMODULE_DATAMEMORYSHARED, SHAREDEX_MAP, exception::ERRNO_LIBDODO, SHAREDEX_SHAREDOBJECTWASNOTOPENED, DATAMEMORYSHAREDEX_SHAREDOBJECTWASNOTOPENED_STR, __LINE__, __FILE__);

	if (ftruncate(shm, sizeof(size)) != 0)
		throw exception::basic(exception::ERRMODULE_DATAMEMORYSHARED, SHAREDEX_MAP, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	mshared = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);

	if (mshared == MAP_FAILED)
		throw exception::basic(exception::ERRMODULE_DATAMEMORYSHARED, SHAREDEX_MAP, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	return mshared;
}

//-------------------------------------------------------------------

void
shared::unmap()
{
	if (mshared != NULL)
	{
		if (munmap(mshared, size) == -1)
			throw exception::basic(exception::ERRMODULE_DATAMEMORYSHARED, SHAREDEX_UNMAP, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		mshared = NULL;
		size = 0;
	}
}

//-------------------------------------------------------------------

void *
shared::get()
{
	return mshared;
}

//-------------------------------------------------------------------

unsigned long
shared::getSize()
{
	return size;
}

//-------------------------------------------------------------------

