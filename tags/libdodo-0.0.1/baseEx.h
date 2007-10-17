/***************************************************************************
 *            baseEx.h
 *
 *  Mon Feb  7 20:02:06 2005
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
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#ifndef _BASEEX_H_
#define _BASEEX_H_

#include "directives.h"
#include <string>

#ifndef NO_EX

	namespace dodo
	{	
		///base class for excaptions. it contais basic stats and have to be derived
		class baseEx
		{
			public:		
			
				enum baseExStat///basic stats in exception
				{
					UNKNOWN = -10,
					SAFE = 0,
					UNSAFE = 10,
					CRITICAL = 100
				};
				
				baseEx(unsigned long line, std::string file);
			
				unsigned long line;///line, where problem detected
				std::string file;
				baseExStat state;///state of problem
		};
	};

#endif	
	
#endif
