/***************************************************************************
 *            dodoString.cc
 *
 *  Tue Aug 28 21:27:19 2007
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


#include <dodoString.h>

using namespace dodo;

dodoString::dodoString()
{	
}

//-------------------------------------------------------------------

dodoString::dodoString(const std::string &str) : std::string(str)
{
}

//-------------------------------------------------------------------

dodoString::dodoString(const char *str) : std::string(str)
{
}

//-------------------------------------------------------------------

dodoString::dodoString(const dodoString &str) : std::string(str)
{
}

//-------------------------------------------------------------------

dodoString::dodoString(const char *str, 
						size_type length) : std::string(str, length)
{
}

//-------------------------------------------------------------------

dodoString::dodoString(const std::string &str, 
						size_type index, 
						size_type length) : std::string(str, index, length)
{
}

//-------------------------------------------------------------------

dodoString::dodoString(size_type length, 
						const char &ch) : std::string(length, ch)
{
}

//-------------------------------------------------------------------

template<typename inputIterator>
dodoString::dodoString(inputIterator start, 
						inputIterator end) : std::string(start, end)
{
	
}

//-------------------------------------------------------------------

bool 
dodoString::equal(const dodoString &first, 
					const dodoString &second)
{
	return strcmp(first.c_str(), second.c_str()) == 0?true:false;
}

//-------------------------------------------------------------------

bool 
dodoString::iequal(const dodoString &first, 
					const dodoString &second)
{
	return strcasecmp(first.c_str(), second.c_str()) == 0?true:false;
}

//-------------------------------------------------------------------
