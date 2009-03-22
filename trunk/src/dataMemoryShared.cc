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

#include <libdodo/directives.h>

#include <sys/types.h>
#include <sys/mman.h>

#ifdef XSI_IPC
#include <sys/ipc.h>
#include <sys/shm.h>
#else /*POSIX_SEMAPHORES*/
#include <fcntl.h>
#endif

#include <libdodo/dataMemoryShared.h>
#include <libdodo/toolsMisc.h>
#include <libdodo/types.h>
#include <libdodo/toolsCode.h>
#include <libdodo/dataMemorySharedEx.h>

namespace dodo
{
	namespace data
	{
		namespace memory
		{
			/**
			 * @struct __keyHandle
			 * @brief defines shared memory key
			 */
			struct __keyHandle
			{
#ifdef XSI_IPC
				int key;            ///< key for the shared data
#else
				dodoString key;     ///< key for the shared data
#endif
			};
		};
	};
};

using namespace dodo::data::memory;

shared::shared(shared &sts)
{
}

//-------------------------------------------------------------------

shared::shared() : mshared(NULL),
				   autogenerated(false),
				   key(new __keyHandle),
#ifndef XSI_IPC
				   size(0),
#endif
				   shm(-1)
{
}

//-------------------------------------------------------------------

shared::shared(int a_key) : mshared(NULL),
							autogenerated(false),
							key(new __keyHandle),
#ifndef XSI_IPC
							size(0),
#endif
							shm(-1)
{
#ifdef XSI_IPC
	if (a_key == 0)
	{
		key->key = tools::misc::iRandom();

		autogenerated = true;
	}
	else
	{
		key->key = a_key;
	}
#else

#ifdef __FreeBSD__
	key->key = "/tmp/";
#else
	key->key = '/';
#endif

	if (a_key == 0)
	{
		char _key[SH_KEY_SIZE / 2 - 1];
		tools::misc::random(_key, SH_KEY_SIZE / 2 - 1);

		key->key.append(tools::code::binToHex(dodoString(_key, SH_KEY_SIZE / 2 - 1)));

		autogenerated = true;
	}
	else
	{
		char _key[4];
		memcpy(_key, &a_key, 4);

		key->key.append(tools::code::binToHex(dodoString(_key, 4)));
	}

	shm = shm_open(key->key.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	if (shm == -1)
	{
		throw exception::basic(exception::ERRMODULE_DATAMEMORYSHARED, SHAREDEX_SHARED, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}
#endif
}

//-------------------------------------------------------------------

shared::~shared()
{
#ifdef XSI_IPC
	if (shm != -1 && autogenerated)
	{
		shmctl(shm, IPC_RMID, NULL);
	}
#else
	if (shm != -1)
	{
		if (mshared != NULL)
		{
			munmap(mshared, size);
		}

		if (autogenerated)
		{
			shm_unlink(key->key.c_str());
		}
	}
#endif

	delete key;
}

//-------------------------------------------------------------------

void
shared::open(int a_key)
{
	close();

#ifdef XSI_IPC
	if (a_key == 0)
	{
		key->key = tools::misc::iRandom();

		autogenerated = true;
	}
	else
	{
		key = a_key;
	}
#else

#ifdef __FreeBSD__
	key->key = "/tmp/";
#else
	key->key = '/';
#endif

	if (a_key == 0)
	{
		char _key[SH_KEY_SIZE / 2 - 1];
		tools::misc::random(_key, SH_KEY_SIZE / 2 - 1);

		key->key.append(tools::code::binToHex(dodoString(_key, SH_KEY_SIZE / 2 - 1)));

		autogenerated = true;
	}
	else
	{
		char _key[4];
		memcpy(_key, &a_key, 4);

		key->key.append(tools::code::binToHex(dodoString(_key, 4)));
	}

	shm = shm_open(key->key.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	if (shm == -1)
	{
		throw exception::basic(exception::ERRMODULE_DATAMEMORYSHARED, SHAREDEX_OPEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}
#endif
}

//-------------------------------------------------------------------

void
shared::close()
{
#ifdef XSI_IPC
	if (shm != -1 && autogenerated)
	{
		if (shmctl(shm, IPC_RMID, NULL) == -1)
		{
			throw exception::basic(exception::ERRMODULE_DATAMEMORYSHARED, SHAREDEX_CLOSE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}
	}
#else
	if (shm != -1)
	{
		if (mshared != NULL)
		{
			if (munmap(mshared, size) == -1)
			{
				throw exception::basic(exception::ERRMODULE_DATAMEMORYSHARED, SHAREDEX_CLOSE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			}
		}

		if (autogenerated)
		{
			if (shm_unlink(key->key.c_str()) == -1)
			{
				throw exception::basic(exception::ERRMODULE_DATAMEMORYSHARED, SHAREDEX_CLOSE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			}
		}
	}
#endif
}

//-------------------------------------------------------------------

void
shared::remove(int a_key)
{
#ifdef XSI_IPC
	int shm = shmget(a_key, 0, O_RDWR);
	if (shm == -1)
	{
		throw exception::basic(exception::ERRMODULE_DATAMEMORYSHARED, SHAREDEX_REMOVE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	if (shmctl(shm, IPC_RMID, NULL) == -1)
	{
		throw exception::basic(exception::ERRMODULE_DATAMEMORYSHARED, SHAREDEX_REMOVE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}
#else

#ifdef __FreeBSD__
	dodoString key = "/tmp/";
#else
	dodoString key = "/";
#endif

	char _key[4];
	memcpy(_key, &a_key, 4);

	key.append(tools::code::binToHex(dodoString(_key, 4)));

	if (shm_unlink(key.c_str()) == -1)
	{
		throw exception::basic(exception::ERRMODULE_DATAMEMORYSHARED, SHAREDEX_REMOVE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}
#endif
}

//-------------------------------------------------------------------

void *
shared::map(unsigned long a_size)
{
	unmap();

#ifdef XSI_IPC
	if (a_size == 0)
	{
		shm = shmget(key->key, 0, O_RDWR);
		if (shm == -1)
		{
			throw exception::basic(exception::ERRMODULE_DATAMEMORYSHARED, SHAREDEX_MAP, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}
	}
	else
	{
		shm = shmget(key->key, a_size, IPC_CREAT | O_RDWR);
		if (shm == -1)
		{
			throw exception::basic(exception::ERRMODULE_DATAMEMORYSHARED, SHAREDEX_MAP, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}
	}

	mshared = shmat(shm, NULL, 0);
	if (mshared == NULL)
	{
		throw exception::basic(exception::ERRMODULE_DATAMEMORYSHARED, SHAREDEX_MAP, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}
#else
	if (a_size == 0)
	{
		struct stat st;

		if (fstat(shm, &st) == -1)
		{
			throw exception::basic(exception::ERRMODULE_DATAMEMORYSHARED, SHAREDEX_MAP, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}

		size = st.st_size;
	}
	else
	{
		size = a_size;
	}

	if (ftruncate(shm, size) != 0)
	{
		throw exception::basic(exception::ERRMODULE_DATAMEMORYSHARED, SHAREDEX_MAP, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	mshared = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);
	if (mshared == MAP_FAILED)
	{
		throw exception::basic(exception::ERRMODULE_DATAMEMORYSHARED, SHAREDEX_MAP, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}
#endif

	return mshared;
}

//-------------------------------------------------------------------

void
shared::unmap()
{
	if (mshared != NULL)
	{
#ifdef XSI_IPC
		if (shmdt(mshared) == -1)
#else
		if (munmap(mshared, size) == -1)
#endif
		{
			throw exception::basic(exception::ERRMODULE_DATAMEMORYSHARED, SHAREDEX_UNMAP, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}

		mshared = NULL;
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
#ifdef XSI_IPC
	if (shm == -1)
	{
		shm = shmget(key->key, 0, O_RDWR);
		if (shm == -1)
		{
			throw exception::basic(exception::ERRMODULE_DATAMEMORYSHARED, SHAREDEX_GETSIZE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}
	}

	struct shmid_ds ds;

	if (shmctl(shm, IPC_STAT, &ds) == -1)
	{
		throw exception::basic(exception::ERRMODULE_DATAMEMORYSHARED, SHAREDEX_GETSIZE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	return ds.shm_segsz;
#else
	struct stat st;

	if (fstat(shm, &st) == -1)
	{
		throw exception::basic(exception::ERRMODULE_DATAMEMORYSHARED, SHAREDEX_GETSIZE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	return st.st_size;
#endif
}

//-------------------------------------------------------------------

