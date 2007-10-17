/***************************************************************************
 *            dbInterface.cc
 *
 *  Sun Jan  15 19:45:19 2005
 *  Copyright  2005  Ni@m
 *  niam.niam@gmail.com
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  aint with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <dbInterface.h>

using namespace dodo;

dbInterface::dbInterface() : connected(false)
{
}

//-------------------------------------------------------------------

dbInterface::dbInterface(dbInterface &a_pp)
{
}

//-------------------------------------------------------------------

dbInterface::~dbInterface()
{
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
dbInterface::connect() const
{
	if (connected)
		disconnect();
	
	connected = true;
	
	#ifdef NO_EX
		return true;
	#endif
}

//-------------------------------------------------------------------

void 
dbInterface::disconnect() const
{
	if (connected)
	{
	 	connected = false;
	}
}

//-------------------------------------------------------------------

std::vector<stringArr>
dbInterface::fetchRow() const
{
	return rows;
}

//-------------------------------------------------------------------

stringArr
dbInterface::fetchField() const
{					
	return fields;
}

//-------------------------------------------------------------------

__dbStorage 
dbInterface::fetch() const
{
	return __dbStorage(rows, fields);
}

//-------------------------------------------------------------------

unsigned int 
dbInterface::rowsCount() const
{
	return 0;
}

//-------------------------------------------------------------------

unsigned int 
dbInterface::fieldsCount() const
{
	return 0;
}

//-------------------------------------------------------------------

unsigned int
dbInterface::affectedRowsCount()
{
	return 0;
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
dbInterface::exec(const std::string &query, 
				bool result) const
{
	#ifdef NO_EX
		return true;
	#endif
}

//-------------------------------------------------------------------

dodoStringMapArr 
dbInterface::fetchAssoc() const
{
	return rowsFields;
}

//-------------------------------------------------------------------
	
