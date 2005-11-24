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

#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <pwd.h>
#include <grp.h>


#include <stdlib.h>
#include <stdio.h>

#include <errno.h>

#include "directives.h"
#include "systemToolsEx.h"
#include "types.h"

namespace dodo
{

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

	/**
	 * real - what ID started process, effective - what ID's permissions process has
	 */
	enum uidTypeEnum
	{
		SYSTEM_UID,
		SYSTEM_EUID
	};
	
	/**
	 * user info
	 */
	struct	__userInfo
	{
		std::string name;
		std::string pass;
		int uid;
		int gid;
		std::string realName;
		std::string home;
		std::string shell;
	};
	
	/**
	 * group info
	 */
	struct	__groupInfo
	{
		std::string name;
		int gid;
		stringArr members;
	};

	/**
	 * provides misc system operations, gets diff info about system
	 * most *nix compatible
	 */
	class systemTools
	{
					
		public:			
		
			/**
			 * gets current working directory
			 */
			static std::string getWorkingDir();
			
			/**
			 * sets current working directory (cd path) 
			 */
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif			
							setWorkingDir(const std::string &path);
				
			/**
			 * displays system usage with current process
			 */
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif			
							getUsageInfo(__usage &info);
			
			/**
			 * get limits from systemToolsLimitEnum
			 */
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif			
							getLimit(systemToolsLimitEnum type,  __limits &lim);
			/**
			 * sets limits from systemToolsLimitEnum
			 */				
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif			
							setLimit(systemToolsLimitEnum type, const __limits &lim);
			
			/**
			 * gets priority of current process for uidTypeEnum
			 */		
			static int getPriority(uidTypeEnum type);///if NO_EX set -> -1 will return!

			/**
			 * sets priority of current process (nice) for uidTypeEnum
			 */				
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif			
							setPriority(uidTypeEnum type, int prio);
							
			/**
			 * gets effective/real user id;
			 */		
			static int getUID(uidTypeEnum type);///returns -1 on error, if NO_EX was set
			
			/**
			 * sets effective/real user id;
			 */				
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif			
							setUID(uidTypeEnum type, int uid);	
							
			/**
			 * gets effective/real group id;
			 */		
			static int getGID(uidTypeEnum type);///returns -1 on error, if NO_EX was set
			
			/**
			 * sets effective/real group id;
			 */				
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif			
							setGID(uidTypeEnum type, int gid);		
			
			/**
			 * gets user info
			 */				
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif			
							getUserInfo(__userInfo &info, int uid);	
										
			/**
			 * gets user info
			 */				
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif			
							getUserInfo(__userInfo &info, const std::string &uid);			
							
			/**
			 * gets users of the system
			 */				
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif			
							getUsers(std::vector<__userInfo> &info);
			
			/**
			 * gets user info
			 */				
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif			
							getGroupInfo(__groupInfo &info, int gid);	
										
			/**
			 * gets user info
			 */				
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif			
							getGroupInfo(__groupInfo &info, const std::string &gid);			
							
			/**
			 * gets users of the system
			 */				
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif			
							getGroups(std::vector<__groupInfo> &info);								
			
		protected:
			
			/**
			 * fills __userInfo with passws
			 */
			static __userInfo &fillUserInfo(__userInfo &info, passwd *pw);
			
			/**
			 * fills __groupInfo with groups
			 */
			static __groupInfo &fillGroupInfo(__groupInfo &info, group *pw);
																						
	};

};

#endif
