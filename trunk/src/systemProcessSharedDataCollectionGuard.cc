/***************************************************************************
 *            systemProcessSharedDataCollectionGuard.cc
 *
 *  Sat Oct 20 12:20:55 2007
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


#include <libdodo/systemProcessSharedDataCollectionGuard.h>

using namespace dodo;

systemProcessSharedDataCollectionGuard::systemProcessSharedDataCollectionGuard(systemProcessSharedDataCollectionGuard &sts)
{
}

//-------------------------------------------------------------------

systemProcessSharedDataCollectionGuard::systemProcessSharedDataCollectionGuard() : shareNum(0)
{
}

//-------------------------------------------------------------------

systemProcessSharedDataCollectionGuard::~systemProcessSharedDataCollectionGuard()
{
}

//-------------------------------------------------------------------

unsigned long
systemProcessSharedDataCollectionGuard::add(void *data)
{
	guard tg(this);

	__shareInfo share;

	share.position = ++shareNum;
	share.data = data;

	shares.push_back(share);

	return share.position;
}

//-------------------------------------------------------------------

void
systemProcessSharedDataCollectionGuard::del(unsigned long position)
{
	guard tg(this);

	if (getShare(position))
		shares.erase(current);
	else
		throw baseEx(ERRMODULE_SYSTEMPROCESSSHAREDDATACOLLECTIONGUARD, SYSTEMPROCESSSHAREDDATACOLLECTIONGUARD_DEL, ERR_LIBDODO, SYSTEMTHREADSHAREDDATACOLLECTIONGUARD_NOTFOUND, SYSTEMTHREADSHAREDDATACOLLECTIONGUARD_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
systemProcessSharedDataCollectionGuard::set(unsigned long position,
										   void          *data)
{
	guard tg(this);

	if (getShare(position))
		current->data = data;
	else
		throw baseEx(ERRMODULE_SYSTEMPROCESSSHAREDDATACOLLECTIONGUARD, SYSTEMPROCESSSHAREDDATACOLLECTIONGUARD_SET, ERR_LIBDODO, SYSTEMTHREADSHAREDDATACOLLECTIONGUARD_NOTFOUND, SYSTEMTHREADSHAREDDATACOLLECTIONGUARD_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

const void *
systemProcessSharedDataCollectionGuard::get(unsigned long position)
{
	guard tg(this);

	if (getShare(position))
		return current->data;
	else
		throw baseEx(ERRMODULE_SYSTEMPROCESSSHAREDDATACOLLECTIONGUARD, SYSTEMPROCESSSHAREDDATACOLLECTIONGUARD_SET, ERR_LIBDODO, SYSTEMTHREADSHAREDDATACOLLECTIONGUARD_NOTFOUND, SYSTEMTHREADSHAREDDATACOLLECTIONGUARD_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

bool
systemProcessSharedDataCollectionGuard::getShare(unsigned long position)
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
