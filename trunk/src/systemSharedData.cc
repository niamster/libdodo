/***************************************************************************
 *            systemSharedData.cc
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


#include <libdodo/systemSharedData.h>

using namespace dodo::system::shared;

data::data(data &sts)
{
}

//-------------------------------------------------------------------

data::data(const char   *a_key) : mdata(NULL),
														  size(0)
{
	if (a_key == NULL)
	{
		key = new char[33];
		key[0] = '/';
		misc::random(key + 1, 31);
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

data::~data()
{
	if (mdata != NULL)
		munmap(mdata, size);

	shm_unlink(key);
}

//-------------------------------------------------------------------

void *
data::map(unsigned long size)
{
	unmap();

	if (shm <= 0)
		throw baseEx(ERRMODULE_SYSTEMSHAREDDATA, DATAEX_MAP, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	ftruncate(shm, sizeof(size));

	mdata = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);

	if (mdata == MAP_FAILED)
		throw baseEx(ERRMODULE_SYSTEMSHAREDDATA, DATAEX_MAP, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	return mdata;
}

//-------------------------------------------------------------------

void
data::unmap()
{
	if (mdata != NULL)
		if (munmap(mdata, size) == -1)
			throw baseEx(ERRMODULE_SYSTEMSHAREDDATA, DATAEX_UNMAP, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	mdata = NULL;
	size = 0;
}

//-------------------------------------------------------------------

void *
data::getMapped()
{
	return mdata;
}

//-------------------------------------------------------------------

unsigned long
data::getSize()
{
	return size;
}

//-------------------------------------------------------------------

