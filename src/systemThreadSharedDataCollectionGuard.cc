/***************************************************************************
 *            systemThreadSharedDataCollectionGuard.cc
 *
 *  Wed Nov 30 22:02:16 2005
 *  Copyright  2005  Ni@m
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


#include <systemThreadSharedDataCollectionGuard.h>

using namespace dodo;

systemThreadSharedDataCollectionGuard::systemThreadSharedDataCollectionGuard(systemThreadSharedDataCollectionGuard &sts)
{
}

//-------------------------------------------------------------------

systemThreadSharedDataCollectionGuard::systemThreadSharedDataCollectionGuard() : shareNum(0)
{
}

//-------------------------------------------------------------------

systemThreadSharedDataCollectionGuard::~systemThreadSharedDataCollectionGuard()
{
}

//-------------------------------------------------------------------

unsigned long
systemThreadSharedDataCollectionGuard::add(void *data)
{
	threadGuard tg(this);

	__shareInfo share;

	share.position = ++shareNum;
	share.data = data;

	shares.push_back(share);

	return share.position;
}

//-------------------------------------------------------------------

#ifndef NO_EX
void
#else
bool
#endif
systemThreadSharedDataCollectionGuard::del(unsigned long position)
{
	threadGuard tg(this);

	if (getShare(position))
		shares.erase(current);
	else
        #ifndef NO_EX
		throw baseEx(ERRMODULE_SYSTEMTHREADHAREDATACOLLECTIONGUARD, SYSTEMTHREADSHAREDDATACOLLECTIONGUARD_DEL, ERR_LIBDODO, SYSTEMTHREADSHAREDDATACOLLECTIONGUARD_NOTFOUND, SYSTEMTHREADSHAREDDATACOLLECTIONGUARD_NOTFOUND_STR, __LINE__, __FILE__);
        #else
		return false;
        #endif

    #ifdef NO_EX
	return true;
    #endif
}

//-------------------------------------------------------------------

#ifndef NO_EX
void
#else
bool
#endif
systemThreadSharedDataCollectionGuard::set(unsigned long position,
										   void          *data)
{
	threadGuard tg(this);

	if (getShare(position))
		current->data = data;
	else
        #ifndef NO_EX
		throw baseEx(ERRMODULE_SYSTEMTHREADHAREDATACOLLECTIONGUARD, SYSTEMTHREADSHAREDDATACOLLECTIONGUARD_SET, ERR_LIBDODO, SYSTEMTHREADSHAREDDATACOLLECTIONGUARD_NOTFOUND, SYSTEMTHREADSHAREDDATACOLLECTIONGUARD_NOTFOUND_STR, __LINE__, __FILE__);
        #else
		return false;
        #endif

    #ifdef NO_EX
	return true;
    #endif
}

//-------------------------------------------------------------------

const void *
systemThreadSharedDataCollectionGuard::get(unsigned long position)
{
	threadGuard tg(this);

	if (getShare(position))
		return current->data;
	else
        #ifndef NO_EX
		throw baseEx(ERRMODULE_SYSTEMTHREADHAREDATACOLLECTIONGUARD, SYSTEMTHREADSHAREDDATACOLLECTIONGUARD_SET, ERR_LIBDODO, SYSTEMTHREADSHAREDDATACOLLECTIONGUARD_NOTFOUND, SYSTEMTHREADSHAREDDATACOLLECTIONGUARD_NOTFOUND_STR, __LINE__, __FILE__);
        #else
		return NULL;
        #endif
}

//-------------------------------------------------------------------

bool
systemThreadSharedDataCollectionGuard::getShare(unsigned long position)
{
	std::list<__shareInfo>::const_iterator i(shares.begin()), j(shares.end());
	for (; i != j; ++i)
		if (i->position == position)
		{
			current = *((std::list<__shareInfo>::iterator *) & i);
			return true;
		}

	return false;
}

//-------------------------------------------------------------------
