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

shared::shared(const char   *a_key) : mshared(NULL),
									  size(0)
{
	if (a_key == NULL)
	{
		key = new char[33];
		key[0] = '/';
		tools::misc::random(key + 1, 31);
		key[31] = '\0';
	}
	else
	{
		key = new char[strlen(a_key) + 2];
		key[0] = '/';
		strcpy(key + 1, a_key);
	}

	shm = shm_open(key, O_CREAT | O_RDWR, 0660);
}

//-------------------------------------------------------------------

shared::~shared()
{
	if (mshared != NULL)
		munmap(mshared, size);

	shm_unlink(key);
}

//-------------------------------------------------------------------

void *
shared::map(unsigned long size)
{
	unmap();

	if (shm <= 0)
		throw exception::basic(exception::ERRMODULE_DATAMEMORYSHARED, SHAREDEX_MAP, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

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
		if (munmap(mshared, size) == -1)
			throw exception::basic(exception::ERRMODULE_DATAMEMORYSHARED, SHAREDEX_UNMAP, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	mshared = NULL;
	size = 0;
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

