/***************************************************************************
 *            ipcAtomicSemaphore.cc
 *
 *  Sat Oct 20 02:00:55 2007
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

#include <libdodo/ipcAtomicSemaphore.h>

using namespace dodo::ipc::atomic;

semaphore::semaphore(unsigned int value,
					 const char   *a_key)
{
	key = new char[strlen(a_key) + 1];
	strcpy(key, a_key);

	keeper = sem_open(key, O_CREAT, 0660, value);
}

//-------------------------------------------------------------------

semaphore::~semaphore()
{
	sem_close(keeper);

	sem_unlink(key);
}

//-------------------------------------------------------------------

void
semaphore::acquire()
{
	if (sem_wait(keeper) != 0)
		throw baseEx(ERRMODULE_IPCATOMICSEMAPHORE, SEMAPHOREEX_LOCK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
semaphore::release()
{
	if (sem_post(keeper) != 0)
		throw baseEx(ERRMODULE_IPCATOMICSEMAPHORE, SEMAPHOREEX_UNLOCK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

