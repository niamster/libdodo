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

#include <vector>
#include <list>
#include <deque>

#include <errno.h>

#include <directives.h>

#include <dodoString.h>
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
	

	typedef dodoArray<dodoString> dodoStringArr;///< array of strings

	typedef std::map<dodoString, dodoString> dodoAssocArr;///< array where key=>string; value=>string; toy may access like arr["key"]="value";

	typedef dodoMap<dodoString> dodoStringMap;///< hash of strings
	
	typedef dodoArray<dodoStringMap> dodoStringMapArr;///< array of hashes of string
	
	struct __statements 
	{
		char *str;
	};	

	/**
	 * diferent predifined constants
	 */

	extern const dodoString __string__;

	extern dodoStringArr __stringarray__;

	extern dodoAssocArr __assocarray__;

	extern dodoArray<dodoStringArr> __stringarrayvector__;

	extern dodoStringMapArr __dodostringmap__;

};

#endif
