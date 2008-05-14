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


#include <libdodo/systemThreadSharedDataCollectionGuard.h>

using namespace dodo::system::thread::shared;

dataCollectionGuard::dataCollectionGuard(dataCollectionGuard &sts)
{
}

//-------------------------------------------------------------------

dataCollectionGuard::dataCollectionGuard() : shareNum(0)
{
}

//-------------------------------------------------------------------

dataCollectionGuard::~dataCollectionGuard()
{
}

//-------------------------------------------------------------------

unsigned long
dataCollectionGuard::add(void *data)
{
	raceHazardGuard tg(this);

	system::shared::__shareInfo share;

	share.position = ++shareNum;
	share.data = data;

	shares.push_back(share);

	return share.position;
}

//-------------------------------------------------------------------

void
dataCollectionGuard::del(unsigned long position)
{
	raceHazardGuard tg(this);

	if (getShare(position))
		shares.erase(current);
	else
		throw baseEx(ERRMODULE_SYSTEMTHREADSHAREDDATACOLLECTIONGUARD, DATACOLLECTIONGUARDEX_DEL, ERR_LIBDODO, DATACOLLECTIONGUARDEX_NOTFOUND, DATACOLLECTIONGUARDEX_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
dataCollectionGuard::set(unsigned long position,
										   void          *data)
{
	raceHazardGuard tg(this);

	if (getShare(position))
		current->data = data;
	else
		throw baseEx(ERRMODULE_SYSTEMTHREADSHAREDDATACOLLECTIONGUARD, DATACOLLECTIONGUARDEX_SET, ERR_LIBDODO, DATACOLLECTIONGUARDEX_NOTFOUND, DATACOLLECTIONGUARDEX_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

const void *
dataCollectionGuard::get(unsigned long position)
{
	raceHazardGuard tg(this);

	if (getShare(position))
		return current->data;
	else
		throw baseEx(ERRMODULE_SYSTEMTHREADSHAREDDATACOLLECTIONGUARD, DATACOLLECTIONGUARDEX_SET, ERR_LIBDODO, DATACOLLECTIONGUARDEX_NOTFOUND, DATACOLLECTIONGUARDEX_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

bool
dataCollectionGuard::getShare(unsigned long position)
{
	dodoList<system::shared::__shareInfo>::const_iterator i(shares.begin()), j(shares.end());
	for (; i != j; ++i)
		if (i->position == position)
		{
			current = *((dodoList<system::shared::__shareInfo>::iterator *) & i);

			return true;
		}

	return false;
}


//-------------------------------------------------------------------

dodoList<unsigned long>
dataCollectionGuard::getSharedDataIds()
{
	dodoList<unsigned long> ids;

	dodoList<system::shared::__shareInfo>::const_iterator i(shares.begin()), j(shares.end());
	for (; i != j; ++i)
		ids.push_back(i->position);

	return ids;
}

//-------------------------------------------------------------------

