/***************************************************************************
 *            systemProcessSharedDataGuard.cc
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


#include <libdodo/systemProcessSharedDataGuard.h>

using namespace dodo;

systemProcessSharedDataGuard::systemProcessSharedDataGuard(systemProcessSharedDataGuard &sts)
{
}

//-------------------------------------------------------------------

systemProcessSharedDataGuard::systemProcessSharedDataGuard(unsigned int value,
														   const char   *a_key) : data(NULL)
{
	if (a_key == NULL)
	{
		key = new char[32];
		misc::random(key, 31);
		key[31] = '\0';
	}
	else
	{
		key = new char[strlen(a_key) + 1];
		strcpy(key, a_key);
	}

	semaphore = sem_open(key, O_CREAT, 0660, value);
}

//-------------------------------------------------------------------

systemProcessSharedDataGuard::~systemProcessSharedDataGuard()
{
	sem_close(semaphore);

	sem_unlink(key);
}

//-------------------------------------------------------------------

void
systemProcessSharedDataGuard::set(  void *a_data)
{
	if (sem_wait(semaphore) != 0)
		throw baseEx(ERRMODULE_SYSTEMPROCESSSHAREDDATAGUARD, SYSTEMPROCESSSHAREDDATAGUARDEX_SET, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	data = a_data;

	if (sem_post(semaphore) != 0)
		throw baseEx(ERRMODULE_SYSTEMPROCESSSHAREDDATAGUARD, SYSTEMPROCESSSHAREDDATAGUARDEX_SET, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
systemProcessSharedDataGuard::del()
{
	if (sem_wait(semaphore) != 0)
		throw baseEx(ERRMODULE_SYSTEMPROCESSSHAREDDATAGUARD, SYSTEMPROCESSSHAREDDATAGUARDEX_DEL, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	data = NULL;

	if (sem_post(semaphore) != 0)
		throw baseEx(ERRMODULE_SYSTEMPROCESSSHAREDDATAGUARD, SYSTEMPROCESSSHAREDDATAGUARDEX_DEL, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void *
systemProcessSharedDataGuard::lock(unsigned long microseconds)
{
	if (microseconds == 0)
	{
		if (sem_wait(semaphore) != 0)
			throw baseEx(ERRMODULE_SYSTEMPROCESSSHAREDDATAGUARD, SYSTEMPROCESSSHAREDDATAGUARDEX_LOCK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
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
					throw baseEx(ERRMODULE_SYSTEMPROCESSSHAREDDATAGUARD, SYSTEMPROCESSSHAREDDATAGUARDEX_LOCK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

				if (nanosleep(&timeout, NULL) == -1)
					throw baseEx(ERRMODULE_SYSTEMPROCESSSHAREDDATAGUARD, SYSTEMPROCESSSHAREDDATAGUARDEX_LOCK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

				slept += 1;

				if (slept > microseconds)
					throw baseEx(ERRMODULE_SYSTEMPROCESSSHAREDDATAGUARD, SYSTEMPROCESSSHAREDDATAGUARDEX_LOCK, ERR_ERRNO, SYSTEMPROCESSSHAREDDATAGUARDEX_CANNOTLOCK, SYSTEMPROCESSSHAREDDATAGUARDEX_CANNOTLOCK_STR, __LINE__, __FILE__);
			}
			else
				locked = false;
		}
	}

	return data;
}

//-------------------------------------------------------------------

void
systemProcessSharedDataGuard::unlock()
{
	if (sem_post(semaphore) != 0)
		throw baseEx(ERRMODULE_SYSTEMPROCESSSHAREDDATAGUARD, SYSTEMPROCESSSHAREDDATAGUARDEX_UNLOCK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

