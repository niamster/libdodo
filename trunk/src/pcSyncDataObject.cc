/***************************************************************************
 *            pcSyncDataObject.cc
 *
 *  Sun Jul 22 2007
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

#include <libdodo/directives.h>

#include <libdodo/pcSyncDataObject.h>
#include <libdodo/pcSyncProtector.h>
#include <libdodo/pcSyncStack.h>
#include <libdodo/types.h>

using namespace dodo::pc::sync::data;

object::object(protector &lock) : data(NULL),
								  lock(lock)
{
}

//-------------------------------------------------------------------

object::~object()
{
}

//-------------------------------------------------------------------

void
object::set(void *a_data)
{
	sync::stack g(&lock);

	data = a_data;
}
//-------------------------------------------------------------------

const void *
object::get()
{
	sync::stack g(&lock);

	return data;
}

//-------------------------------------------------------------------

void *
object::acquire(unsigned long timeout)
{
	lock.acquire(timeout);
	return data;
}

//-------------------------------------------------------------------

void
object::release()
{
	lock.release();
}

//-------------------------------------------------------------------
