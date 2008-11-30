/***************************************************************************
 *            pcSyncProcessDataSingle.cc
 *
 *  Sun Jul 22 23:14:14 2007
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


#include <libdodo/pcSyncProcessDataSingle.h>

using namespace dodo::pc::sync::process::data;

single::single(single &sts)
{
}

//-------------------------------------------------------------------

single::single(unsigned int value,
			   const dodoString &a_key) : data(NULL),
										   semaphore(NULL)
{
	key = '/';

	if (a_key.empty())
	{
		char _key[SH_KEY_SIZE / 2 - 1];
		tools::misc::random(_key, SH_KEY_SIZE / 2 - 1 - 1);
		_key[SH_KEY_SIZE / 2 - 1 - 1] = '\0';

		key.append(tools::code::binToHex(_key));
	}
	else
		key.append(a_key);

	semaphore = sem_open(key.c_str(), O_CREAT, 0600, value);
}

//-------------------------------------------------------------------

single::~single()
{
	if (semaphore != NULL)
	{
		sem_close(semaphore);

		sem_unlink(key.c_str());
	}
}

//-------------------------------------------------------------------

void
single::set(void *a_data)
{
	if (semaphore == NULL)
		throw exception::basic(exception::ERRMODULE_PCSYNCPROCESSDATASINGLE, SINGLEEX_SET, exception::ERRNO_LIBDODO, SINGLEEX_SEMAPHOREWASNOTOPENED, PCSYNCPROCESSDATASINGLEEX_SEMAPHOREWASNOTOPENED_STR, __LINE__, __FILE__);

	if (sem_wait(semaphore) != 0)
		throw exception::basic(exception::ERRMODULE_PCSYNCPROCESSDATASINGLE, SINGLEEX_SET, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	data = a_data;

	if (sem_post(semaphore) != 0)
		throw exception::basic(exception::ERRMODULE_PCSYNCPROCESSDATASINGLE, SINGLEEX_SET, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
single::del()
{
	if (semaphore == NULL)
		throw exception::basic(exception::ERRMODULE_PCSYNCPROCESSDATASINGLE, SINGLEEX_DEL, exception::ERRNO_LIBDODO, SINGLEEX_SEMAPHOREWASNOTOPENED, PCSYNCPROCESSDATASINGLEEX_SEMAPHOREWASNOTOPENED_STR, __LINE__, __FILE__);

	if (sem_wait(semaphore) != 0)
		throw exception::basic(exception::ERRMODULE_PCSYNCPROCESSDATASINGLE, SINGLEEX_DEL, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	data = NULL;

	if (sem_post(semaphore) != 0)
		throw exception::basic(exception::ERRMODULE_PCSYNCPROCESSDATASINGLE, SINGLEEX_DEL, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void *
single::acquire(unsigned long microseconds)
{
	if (semaphore == NULL)
		throw exception::basic(exception::ERRMODULE_PCSYNCPROCESSDATASINGLE, SINGLEEX_ACQUIRE, exception::ERRNO_LIBDODO, SINGLEEX_SEMAPHOREWASNOTOPENED, PCSYNCPROCESSDATASINGLEEX_SEMAPHOREWASNOTOPENED_STR, __LINE__, __FILE__);

	if (microseconds == 0)
	{
		if (sem_wait(semaphore) != 0)
			throw exception::basic(exception::ERRMODULE_PCSYNCPROCESSDATASINGLE, SINGLEEX_ACQUIRE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}
	else
	{
		bool locked = true;
		unsigned long slept = 0;

		while (locked)
		{
			if (sem_trywait(semaphore) != 0)
			{
				if (errno != EAGAIN)
					throw exception::basic(exception::ERRMODULE_PCSYNCPROCESSDATASINGLE, SINGLEEX_ACQUIRE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

				if (nanosleep(&timeout, NULL) == -1)
					throw exception::basic(exception::ERRMODULE_PCSYNCPROCESSDATASINGLE, SINGLEEX_ACQUIRE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

				slept += 1;

				if (slept > microseconds)
					throw exception::basic(exception::ERRMODULE_PCSYNCPROCESSDATASINGLE, SINGLEEX_ACQUIRE, exception::ERRNO_LIBDODO, SINGLEEX_CANNOTLOCK, PCSYNCPROCESSDATASINGLEEX_CANNOTLOCK_STR, __LINE__, __FILE__);
			}
			else
				locked = false;
		}
	}

	return data;
}

//-------------------------------------------------------------------

void
single::release()
{
	if (semaphore == NULL)
		throw exception::basic(exception::ERRMODULE_PCSYNCPROCESSDATASINGLE, SINGLEEX_RELEASE, exception::ERRNO_LIBDODO, SINGLEEX_SEMAPHOREWASNOTOPENED, PCSYNCPROCESSDATASINGLEEX_SEMAPHOREWASNOTOPENED_STR, __LINE__, __FILE__);

	if (sem_post(semaphore) != 0)
		throw exception::basic(exception::ERRMODULE_PCSYNCPROCESSDATASINGLE, SINGLEEX_RELEASE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

