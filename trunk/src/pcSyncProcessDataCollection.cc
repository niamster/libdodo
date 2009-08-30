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

#include <libdodo/directives.h>

#include <libdodo/pcSyncProcessDataCollection.h>
#include <libdodo/types.h>
#include <libdodo/pcSyncDataCollection.h>
#include <libdodo/pcSyncProcessDataCollectionEx.h>
#include <libdodo/pcSyncProcessSection.h>
#include <libdodo/pcSyncProtector.h>

using namespace dodo::pc::sync::process::data;

collection::collection(collection &sts)
{
}

//-------------------------------------------------------------------

collection::collection() : shareNum(0),
						   keeper(new section(0))
{
}

//-------------------------------------------------------------------

collection::~collection()
{
	delete keeper;
}

//-------------------------------------------------------------------

unsigned long
collection::add(void *data)
{
	pc::sync::protector pg(keeper);

	pc::sync::data::__info__ share;

	share.id = ++shareNum;
	share.data = data;

	shares.push_back(share);

	return share.id;
}

//-------------------------------------------------------------------

void
collection::remove(unsigned long id)
{
	pc::sync::protector pg(keeper);

	if (getShare(id))
		shares.erase(current);
	else
		throw exception::basic(exception::MODULE_PCSYNCPROCESSDATACOLLECTION, COLLECTIONEX_REMOVE, exception::ERRNO_LIBDODO, COLLECTIONEX_NOTFOUND, PCSYNCPROCESSDATACOLLECTIONEX_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
collection::set(unsigned long id,
				void          *data)
{
	pc::sync::protector pg(keeper);

	if (getShare(id))
		current->data = data;
	else
		throw exception::basic(exception::MODULE_PCSYNCPROCESSDATACOLLECTION, COLLECTIONEX_SET, exception::ERRNO_LIBDODO, COLLECTIONEX_NOTFOUND, PCSYNCPROCESSDATACOLLECTIONEX_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

const void *
collection::get(unsigned long id)
{
	pc::sync::protector pg(keeper);

	if (getShare(id))
		return current->data;
	else
		throw exception::basic(exception::MODULE_PCSYNCPROCESSDATACOLLECTION, COLLECTIONEX_SET, exception::ERRNO_LIBDODO, COLLECTIONEX_NOTFOUND, PCSYNCPROCESSDATACOLLECTIONEX_NOTFOUND_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

bool
collection::getShare(unsigned long id)
{
	dodoList<pc::sync::data::__info__>::iterator i(shares.begin()), j(shares.end());
	for (; i != j; ++i) {
		if (i->id == id) {
			current = i;

			return true;
		}
	}

	return false;
}

//-------------------------------------------------------------------

dodoList<unsigned long>
collection::identificators()
{
	dodoList<unsigned long> ids;

	dodoList<pc::sync::data::__info__>::iterator i(shares.begin()), j(shares.end());
	for (; i != j; ++i)
		ids.push_back(i->id);

	return ids;
}

//-------------------------------------------------------------------

