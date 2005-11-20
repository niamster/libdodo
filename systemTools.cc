/***************************************************************************
 *            systemTools.cc
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
 
#include "systemTools.h"

using namespace dodo;

std::string 
systemTools::getWorkingDir()
{
	char wd[MAXPATHLEN];
	
	if (getcwd(wd,MAXPATHLEN) == NULL)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_GETWORKINGDIR,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return __string__;
		#endif
	
	return wd;
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool 
#endif 
systemTools::setWorkingDir(const std::string &path)
{
	if (chdir(path.c_str()) == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_SETWORKINGDIR,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif
	
	#ifdef NO_EX
		return true;
	#endif
}

//-------------------------------------------------------------------

#ifndef WIN
	
	#ifndef NO_EX
		void 
	#else
		bool 
	#endif			
	systemTools::getUsageinfo(__usage &info)
	{
		rusage use;
		if (getrusage(RUSAGE_SELF,&use) == -1)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_GETUSAGEINFO,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;
			#endif
			
		info.time = use.ru_utime.tv_sec*100 + use.ru_utime.tv_usec;
		info.mem = use.ru_maxrss*1024;

		#ifdef NO_EX
			return true;
		#endif
	}

	//-------------------------------------------------------------------

	#ifndef NO_EX
		void 
	#else
		bool 
	#endif			
	systemTools::getLimit(systemToolsLimitEnum type, 
						__limits &lim)
	{
		rlimit limit;
		
		int realRes(0);
		
		switch (type)
		{
			case SYSTEM_CPUTIME:
				realRes = RLIMIT_CPU;
				break;
			case SYSTEM_MAXFILESIZE:
				realRes = RLIMIT_FSIZE;
				break;
			case SYSTEM_MAXMEMUSAGE:
				realRes = RLIMIT_DATA;
				break;
			case SYSTEM_MAXSTACK:
				realRes = RLIMIT_STACK;
				break;
			case SYSTEM_MAXPROC:
				realRes = RLIMIT_NPROC;
				break;
			case SYSTEM_MAXOPENFILES:
				realRes = RLIMIT_OFILE;
				break;
			default:
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_GETLIMIT,ERR_LIBDODO,SYSTEMTOOLS_WRONG_PARAMETHER,SYSTEMTOOLS_WRONG_PARAMETHER_STR,__LINE__,__FILE__);
				#else
					return false;
				#endif				
		}
		
		if (getrlimit(realRes,&limit) == -1)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_GETLIMIT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;
			#endif

		lim.current = limit.rlim_cur;
		lim.max = limit.rlim_max;			

		#ifdef NO_EX
			return true;
		#endif
	}
	
	//-------------------------------------------------------------------
	
	#ifndef NO_EX
		void 
	#else
		bool 
	#endif			
	systemTools::setLimit(systemToolsLimitEnum type, 
						const __limits &lim)
	{
		rlimit limit;
		
		int realRes(0);
		
		switch (type)
		{
			case SYSTEM_CPUTIME:
				realRes = RLIMIT_CPU;
				break;
			case SYSTEM_MAXFILESIZE:
				realRes = RLIMIT_FSIZE;
				break;
			case SYSTEM_MAXMEMUSAGE:
				realRes = RLIMIT_DATA;
				break;
			case SYSTEM_MAXSTACK:
				realRes = RLIMIT_STACK;
				break;
			case SYSTEM_MAXPROC:
				realRes = RLIMIT_NPROC;
				break;
			case SYSTEM_MAXOPENFILES:
				realRes = RLIMIT_OFILE;
				break;
			default:
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_SETLIMIT,ERR_LIBDODO,SYSTEMTOOLS_WRONG_PARAMETHER,SYSTEMTOOLS_WRONG_PARAMETHER_STR,__LINE__,__FILE__);
				#else
					return false;
				#endif				
		}
		

		limit.rlim_cur = lim.current;
		limit.rlim_max = lim.max;		
		
		if (setrlimit(realRes,&limit) == -1)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTOOLS,SYSTEMTOOLS_SETLIMIT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;
			#endif			

		#ifdef NO_EX
			return true;
		#endif
	}

	//-------------------------------------------------------------------
	
#endif