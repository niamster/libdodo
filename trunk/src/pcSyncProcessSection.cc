/***************************************************************************
 *            pcSyncProcessSection.cc
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/pcSyncProcessSection.h>

using namespace dodo::pc::sync::process;

section::section(unsigned int value,
					 const char   *a_key)
{
	key = new char[strlen(a_key) + 1];
	strcpy(key, a_key);

	keeper = sem_open(key, O_CREAT, 0660, value);
}

//-------------------------------------------------------------------

section::~section()
{
	sem_close(keeper);

	sem_unlink(key);
}

//-------------------------------------------------------------------

void
section::acquire()
{
	if (sem_wait(keeper) != 0)
		throw exception::basic(exception::ERRMODULE_PCSYNCPROCESSSECTION, SECTIONEX_LOCK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
section::release()
{
	if (sem_post(keeper) != 0)
		throw exception::basic(exception::ERRMODULE_PCSYNCPROCESSSECTION, SECTIONEX_UNLOCK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

