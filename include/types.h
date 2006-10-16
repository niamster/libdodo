/***************************************************************************
 *            types.h
 *
 *  Thu Jul  7 00:06:57 2005
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
 
#ifndef _TYPES_H_
#define _TYPES_H_

#include <string>
#include <vector>
#include <map>
#include <list>
#include <iostream>
#include <deque>

#include <errno.h>

#include <directives.h>

#include <dodoMap.h>

namespace dodo
{

	/**
	 * diferent predifined types
	 */
	 
	#define USE_DEQUE 
	
	#ifdef USE_DEQUE
		#define dodoArray std::deque
	#else
		#define dodoArray std::vector
	#endif
	
	typedef char* pchar;

	typedef dodoArray<std::string> stringArr;///< array of strings

	typedef std::map<std::string, std::string> assocArr;///< array where key=>string; value=>string; toy may access like arr["key"] = "value";

	typedef __dodoMap<std::string> dodoStringMap;
	
	typedef dodoArray<dodoStringMap> dodoStringMapArr;
	
	struct __statements 
	{
		char *str;
	};	

	/**
	 * diferent predifined constants
	 */

	static const std::string __string__ = "";

	static stringArr __stringarray__;

	static assocArr __assocarray__;

	static dodoArray<stringArr> __stringarrayvector__;

	static dodoStringMapArr __dodostringmap__;

};

#endif
