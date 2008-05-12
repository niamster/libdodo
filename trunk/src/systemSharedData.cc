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

using namespace dodo;

systemSharedData::systemSharedData(systemSharedData &sts)
{
}

//-------------------------------------------------------------------

systemSharedData::systemSharedData(const char   *a_key) : data(NULL),
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

systemSharedData::~systemSharedData()
{
	if (data != NULL)
		munmap(data, size);

	shm_unlink(key);
}

//-------------------------------------------------------------------

void *
systemSharedData::map(unsigned long size)
{
	unmap();

	if (shm <= 0)
		throw baseEx(ERRMODULE_SYSTEMSHAREDDATA, SYSTEMSHAREDDATAEX_MAP, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	ftruncate(shm, sizeof(size));

	data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);

	if (data == MAP_FAILED)
		throw baseEx(ERRMODULE_SYSTEMSHAREDDATA, SYSTEMSHAREDDATAEX_MAP, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	return data;
}

//-------------------------------------------------------------------

void
systemSharedData::unmap()
{
	if (data != NULL)
		if (munmap(data, size) == -1)
			throw baseEx(ERRMODULE_SYSTEMSHAREDDATA, SYSTEMSHAREDDATAEX_UNMAP, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	data = NULL;
	size = 0;
}

//-------------------------------------------------------------------

void *
systemSharedData::getMapped()
{
	return data;
}

//-------------------------------------------------------------------

unsigned long
systemSharedData::getSize()
{
	return size;
}

//-------------------------------------------------------------------

