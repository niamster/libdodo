/***************************************************************************
 *            baseEx.cc
 *
 *  Sun Jul 17 17:38:02 2005
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
 
#include "baseEx.h"

#ifndef NO_EX

	using namespace dodo;
	
	baseEx::baseEx(unsigned long a_errModule,
					unsigned long functionID, 
					unsigned long errnoSource, 
					unsigned long a_errno, 
					std::string a_errstr,
					unsigned long a_line, 
					std::string a_file) : line(a_line),
									file(a_file),
									baseErrno(a_errno),
									baseErrstr(a_errstr),
									funcID(functionID),
									errnoSource(errnoSource),
									errModule(a_errModule)
	{
		state = UNKNOWN;
	}
	
	//-------------------------------------------------------------------
	
#endif
