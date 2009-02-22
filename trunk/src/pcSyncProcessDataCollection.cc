/***************************************************************************
 *            pcSyncProcessDataCollection.cc
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/pcSyncProcessDataCollection.h>

using namespace dodo::pc::sync::process::data;

collection::collection(collection &sts)
{
}

//-------------------------------------------------------------------

collection::collection() : shareNum(0)
{
}

//-------------------------------------------------------------------

collection::~collection()
{
}

//-------------------------------------------------------------------

unsigned long collection::add(void *data)
{
	protector tg(this);

	pc::sync::data::__info share;

	share.position = ++shareNum;
	share.data = data;

	shares.push_back(share);

	return share.position;
}

//-------------------------------------------------------------------

void collection::del(unsigned long position)
{
	protector tg(this);

	if (getShare(position))
	{
		shares.erase(current);
	}
	else
	{
		throw exception::basic(exception::ERRMODULE_PCSYNCPROCESSDATACOLLECTION, COLLECTIONEX_DEL, exception::ERRNO_LIBDODO, COLLECTIONEX_NOTFOUND, PCSYNCPROCESSDATACOLLECTIONEX_NOTFOUND_STR, __LINE__, __FILE__);
	}
}

//-------------------------------------------------------------------

void collection::set(unsigned long position,
					 void          *data)
{
	protector tg(this);

	if (getShare(position))
	{
		current->data = data;
	}
	else
	{
		throw exception::basic(exception::ERRMODULE_PCSYNCPROCESSDATACOLLECTION, COLLECTIONEX_SET, exception::ERRNO_LIBDODO, COLLECTIONEX_NOTFOUND, PCSYNCPROCESSDATACOLLECTIONEX_NOTFOUND_STR, __LINE__, __FILE__);
	}
}

//-------------------------------------------------------------------

const void *collection::get(unsigned long position)
{
	protector tg(this);

	if (getShare(position))
	{
		return current->data;
	}
	else
	{
		throw exception::basic(exception::ERRMODULE_PCSYNCPROCESSDATACOLLECTION, COLLECTIONEX_SET, exception::ERRNO_LIBDODO, COLLECTIONEX_NOTFOUND, PCSYNCPROCESSDATACOLLECTIONEX_NOTFOUND_STR, __LINE__, __FILE__);
	}
}

//-------------------------------------------------------------------

bool collection::getShare(unsigned long position)
{
	dodoList<pc::sync::data::__info>::iterator i(shares.begin()), j(shares.end());
	for (; i != j; ++i)
	{
		if (i->position == position)
		{
			current = i;

			return true;
		}
	}

	return false;
}

//-------------------------------------------------------------------

dodoList<unsigned long>collection::getIds()
{
	dodoList<unsigned long> ids;

	dodoList<pc::sync::data::__info>::iterator i(shares.begin()), j(shares.end());
	for (; i != j; ++i)
	{
		ids.push_back(i->position);
	}

	return ids;
}

//-------------------------------------------------------------------

