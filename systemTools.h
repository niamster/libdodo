/***************************************************************************
 *            system.h
 *
 *  Sat Nov 19 15:19:57 2005
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
 
#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#ifndef WIN
	#include <unistd.h>
	#include <sys/time.h>
	#include <sys/resource.h>	
#endif

#include <errno.h>

#include "directives.h"
#include "systemToolsEx.h"

namespace dodo
{

	#ifndef WIN
	
		/**
		 * contains info about process
		 */
		struct __usage
		{
			long time;///in miliseconds
			long mem;///in bytes
		};
	
		/**
		 * system limits
		 */
		struct __limits
		{
			unsigned long current;///can use current process
			unsigned long max;///max amount
		};
	
		/**
		 * used with __limits in `getSystemLimits`
		 */
		enum systemToolsLimitEnum
		{
			SYSTEM_CPUTIME,
			SYSTEM_MAXFILESIZE,
			SYSTEM_MAXMEMUSAGE,
			SYSTEM_MAXSTACK,
			SYSTEM_MAXPROC,
			SYSTEM_MAXOPENFILES
		}; 
		
	#endif

	
	/**
	 * provides misc system operations, gets diff info about system
	 * most *nix compatible
	 */
	class systemTools
	{
					
		public:			
					
			static std::string getWorkingDir();
			
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif			
							setWorkingDir(const std::string &path);

			#ifndef WIN
				
				#ifndef NO_EX
					static void 
				#else
					static bool 
				#endif			
								getUsageinfo(__usage &info);
				
				#ifndef NO_EX
					static void 
				#else
					static bool 
				#endif			
								getLimit(systemToolsLimitEnum type,  __limits &lim);
								
				#ifndef NO_EX
					static void 
				#else
					static bool 
				#endif			
								setLimit(systemToolsLimitEnum type, const __limits &lim);
								
			#endif

		
	};

};

#endif
