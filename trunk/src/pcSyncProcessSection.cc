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
                 const dodoString &a_key) : keeper(NULL)
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

	keeper = sem_open(key.c_str(), O_CREAT, 0600, value);
}

//-------------------------------------------------------------------

section::~section()
{
	if (keeper != NULL)
	{
		sem_close(keeper);

		sem_unlink(key.c_str());
	}
}

//-------------------------------------------------------------------

void
section::acquire()
{
	if (keeper == NULL)
		throw exception::basic(exception::ERRMODULE_PCSYNCPROCESSSECTION, SECTIONEX_ACQUIRE, exception::ERRNO_LIBDODO, SECTIONEX_SEMAPHOREWASNOTOPENED, PCSYNCPROCESSSECTIONEX_SEMAPHOREWASNOTOPENED_STR, __LINE__, __FILE__);

	if (sem_wait(keeper) != 0)
		throw exception::basic(exception::ERRMODULE_PCSYNCPROCESSSECTION, SECTIONEX_ACQUIRE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
section::release()
{
	if (keeper == NULL)
		throw exception::basic(exception::ERRMODULE_PCSYNCPROCESSSECTION, SECTIONEX_RELEASE, exception::ERRNO_LIBDODO, SECTIONEX_SEMAPHOREWASNOTOPENED, PCSYNCPROCESSSECTIONEX_SEMAPHOREWASNOTOPENED_STR, __LINE__, __FILE__);

	if (sem_post(keeper) != 0)
		throw exception::basic(exception::ERRMODULE_PCSYNCPROCESSSECTION, SECTIONEX_RELEASE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

