/***************************************************************************
 *            types.h
 *
 *  Thu Jul  7 00:06:57 2005
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
	
	#ifdef USE_DEQUE
		#define dodoArray std::deque
	#else
		#define dodoArray std::vector
	#endif

	typedef dodoArray<std::string> dodoStringArr;///< array of strings

	typedef std::map<std::string, std::string> dodoAssocArr;///< array where key=>string; value=>string; toy may access like arr["key"] = "value";

	typedef __dodoMap<std::string> dodoStringMap;///< hash of strings
	
	typedef dodoArray<dodoStringMap> dodoStringMapArr;///< array of hashes of string
	
	struct __statements 
	{
		char *str;
	};	

	/**
	 * diferent predifined constants
	 */

	static const std::string __string__ = "";

	static dodoStringArr __stringarray__;

	static dodoAssocArr __assocarray__;

	static dodoArray<dodoStringArr> __stringarrayvector__;

	static dodoStringMapArr __dodostringmap__;

};

#endif
