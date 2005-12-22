/***************************************************************************
 *            dodoMap.cc
 *
 *  Mon Dec 20 23:55:19 2005
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
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#include <dodoMap.h>

using namespace dodo;

dodoMap::dodoMap() : icase(false)
{
}

//-------------------------------------------------------------------

std::string
dodoMap::operator [](const std::string &varName)
{
	i = realArr.begin();
	j = realArr.end();
	
	if (icase)
	{
		for (;i!=j;++i)
			if (strcasecmp(varName.c_str(),i->first.c_str()) == 0)
				return i->second;		
	}
	else
	{
		for (;i!=j;++i)
			if (strcmp(varName.c_str(),i->first.c_str()) == 0)
				return i->second;
	}		
	return __UNDEFINED__;
}

//-------------------------------------------------------------------