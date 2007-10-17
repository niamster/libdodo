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
#include <errno.h>

namespace dodo
{
	enum BOOL
	{
		FALSE = 0,
		TRUE = 1
	};	

	#ifdef UNICODE
		static short size_of_char = sizeof(char)*2;
	#else
		static short size_of_char = sizeof(char);
	#endif
	
	static short size_of_pointer = sizeof(void *);
	
	static short size_of_int = sizeof(int);
	
	static short size_of_long = sizeof(long);
	
	typedef char* pchar;
	
	static const std::string __string__ = "";
	
	typedef std::vector<std::string> stringArr;
	static stringArr __stringarray__;
	
	typedef std::map<std::string, std::string> assocArr;
	static assocArr __assocarray__;

	typedef std::string (*escape)(const std::string &);
	
	#define SIZEOFNUM 32
	
	#ifndef WIN
		#define FILE_DELIM "/"
	#else
		#define FILE_DELIM "\\"	
	#endif
	
	struct __statements 
	{
		char *str;
	};
}
#endif /* _TYPES_H */
