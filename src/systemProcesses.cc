/***************************************************************************
 *            systemProcesses.cc
 *
 *  Tue Feb 27 08:47:16 2007
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


#include <systemProcesses.h>
	
using namespace dodo;

__processInfo::__processInfo() : isRunning(false),
				executed(0),
				executeLimit(0)
{
}

//-------------------------------------------------------------------

systemProcesses::systemProcesses(systemProcesses &sp)
{
}

//-------------------------------------------------------------------

systemProcesses::systemProcesses() : processNum(0)
{
}

//-------------------------------------------------------------------

systemProcesses::~systemProcesses()
{
	std::list<__processInfo>::iterator i(processes.begin()), j(processes.end());
	
	#ifdef DL_EXT
		deinitSystemProcessesModule deinit;
	#endif
	
	for (;i!=j;++i)
	{
		if (!_isRunning(i))
			continue;
		
		switch (i->action)
		{
			case PROCESS_KEEP_ALIVE:
			
				waitpid(i->pid,NULL,WNOHANG);
			
				break;
				
			case PROCESS_STOP:
			
				kill(i->pid,2);
				
				#ifdef DL_EXT
				
					if (i->handle != NULL)
					{
						deinit = (deinitSystemProcessesModule)dlsym(i->handle, "deinitSystemProcessesModule");
						if (deinit != NULL)
							deinit();
						
						dlclose(i->handle);						
					}
					
				#endif		
				
				break;
			
			case PROCESS_WAIT:
			default:
			
				waitpid(i->pid,NULL,0);
				
				#ifdef DL_EXT
				
					if (i->handle != NULL)
					{
						deinit = (deinitSystemProcessesModule)dlsym(i->handle, "deinitSystemProcessesModule");
						if (deinit != NULL)
							deinit();
						
						dlclose(i->handle);						
					}
					
				#endif				
		}
	}	
}
	
//-------------------------------------------------------------------
	
unsigned long 
systemProcesses::add(processFunc func,
						void *data,
						short action)
{
	__processInfo process;
	
	process.data = data;
	process.func = func;
	process.position = ++processNum;
	process.action = action;
	process.executeLimit = 0;
	
	#ifdef DL_EXT
		process.handle = NULL;
	#endif
	
	processes.push_back(process);
	
	return process.position;
}
		
//-------------------------------------------------------------------

unsigned long 
systemProcesses::add(jobFunc func,
						void *data)
{
	return add(func,data,PROCESS_WAIT);
}
	
//-------------------------------------------------------------------

unsigned long 
systemProcesses::addNRun(jobFunc func,
						void *data)
{
	return addNRun(func,data,1,PROCESS_WAIT);
}

//-------------------------------------------------------------------

unsigned long 
systemProcesses::addNRun(processFunc func, 
						void *data, 
						unsigned long limit, 
						short action)
{
	__processInfo process;
	
	process.data = data;
	process.func = func;
	process.position = ++processNum;
	process.action = action;
	process.executeLimit = 0;
	
	#ifdef DL_EXT
		process.handle = NULL;
	#endif
	
	register pid_t pid = fork();
		
	if (pid == 0)
	{
		func(data);
		
		_exit(0);
	}
	else
	{
		if (pid == -1)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMPROCESSES,SYSTEMPROCESSES_ADDNRUN,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;
			#endif			
		else	
			process.pid = pid;
	}
	
	process.isRunning = true;
	++(process.executed);
	
	processes.push_back(process);
	
	return process.position;
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
systemProcesses::del(unsigned long position, 
					bool force)
{
	if (getProcess(position))
	{
		if (_isRunning(k))
		{
			if (!force)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMPROCESSES,SYSTEMPROCESSES_DEL,ERR_LIBDODO,SYSTEMPROCESSES_ISALREADYRUNNING,SYSTEMPROCESSES_ISALREADYRUNNING_STR,__LINE__,__FILE__);
				#else
					return false;
				#endif
			else
			{
				if (kill(k->pid,2) == -1)
					#ifndef NO_EX
						throw baseEx(ERRMODULE_SYSTEMPROCESSES,SYSTEMPROCESSES_DEL,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
					#else
						return false;
					#endif
			}
		}

		#ifdef DL_EXT
		
			if (k->handle != NULL)
			{
				deinitSystemProcessesModule deinit;	
	
				deinit = (deinitSystemProcessesModule)dlsym(k->handle, "deinitSystemProcessesModule");
				if (deinit != NULL)
					deinit();
				
				if (dlclose(k->handle)!=0)
					#ifndef NO_EX
						throw baseEx(ERRMODULE_SYSTEMPROCESSES,SYSTEMPROCESSES_DEL,ERR_DYNLOAD,0,dlerror(),__LINE__,__FILE__);
					#endif
					
				k->handle = NULL;	
			}
				
		#endif
			
		processes.erase(k);
		
		#ifdef NO_EX
			return true;
		#endif
	}
	else
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMPROCESSES,SYSTEMPROCESSES_DEL,ERR_LIBDODO,SYSTEMPROCESSES_NOTFOUND,SYSTEMPROCESSES_NOTFOUND_STR,__LINE__,__FILE__);
		#else
			return false;
		#endif
}

//-------------------------------------------------------------------
	
bool 
systemProcesses::getProcess(unsigned long position) const
{
	std::list<__processInfo>::const_iterator i(processes.begin()), j(processes.end());
	for (;i!=j;++i)
		if (i->position == position)
		{
			k = *((std::list<__processInfo>::iterator *)&i);
			return true;
		}
	
	return false;	
}

//-------------------------------------------------------------------

bool 
systemProcesses::_isRunning(std::list<__processInfo>::iterator &position) const
{
	if (!position->isRunning)
		return false;
		
	register int res = kill(position->pid,0);	
	if (res != 0)	
	{
		if (errno == ESRCH)
		{
			position->isRunning = false;
			
			return false;
		}

		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMPROCESSES,SYSTEMPROCESSES__ISRUNNING,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif			
	}
	
	return true;	
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif 
systemProcesses::replace(unsigned long position, 
						processFunc func, 
						void *data,
						bool force,
						short action)
{
	if (getProcess(position))
	{
		if (_isRunning(k))
		{
			if (!force)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMPROCESSES,SYSTEMPROCESSES_REPLACE,ERR_LIBDODO,SYSTEMPROCESSES_ISALREADYRUNNING,SYSTEMPROCESSES_ISALREADYRUNNING_STR,__LINE__,__FILE__);
				#else
					return false;
				#endif
			else
			{
				if (kill(k->pid,2) == -1)
					#ifndef NO_EX
						throw baseEx(ERRMODULE_SYSTEMPROCESSES,SYSTEMPROCESSES_REPLACE,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
					#else
						return false;
					#endif
			}
		}


		#ifdef DL_EXT
		
			if (k->handle!=NULL)
			{
				deinitSystemProcessesModule deinit;	
	
				deinit = (deinitSystemProcessesModule)dlsym(k->handle, "deinitSystemProcessesModule");
				if (deinit != NULL)
					deinit();
				
				if (dlclose(k->handle)!=0)
					#ifndef NO_EX
						throw baseEx(ERRMODULE_SYSTEMPROCESSES,SYSTEMPROCESSES_REPLACE,ERR_DYNLOAD,0,dlerror(),__LINE__,__FILE__);
					#endif
					
				k->handle = NULL;	
			}
				
		#endif
			
		k->data = data;
		k->func = func;
		k->isRunning = false;
		k->action = action;
			
		#ifdef NO_EX
			return true;
		#endif
	}
	else
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMPROCESSES,SYSTEMPROCESSES_REPLACE,ERR_LIBDODO,SYSTEMPROCESSES_NOTFOUND,SYSTEMPROCESSES_NOTFOUND_STR,__LINE__,__FILE__);
		#else
			return false;
		#endif
	
}
	
//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
systemProcesses::run(unsigned long position, 
						bool force)
{
	if (getProcess(position))
	{
		if (k->executeLimit >0 && (k->executeLimit <= k->executed))
		{
			processes.erase(k);
			
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMPROCESSES,SYSTEMPROCESSES_RUN,ERR_LIBDODO,SYSTEMPROCESSES_SWEPT,SYSTEMPROCESSES_SWEPT_STR,__LINE__,__FILE__);
			#else
				return false;
			#endif				
		}
		
		if (_isRunning(k) && !force)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMPROCESSES,SYSTEMPROCESSES_RUN,ERR_LIBDODO,SYSTEMPROCESSES_ISALREADYRUNNING,SYSTEMPROCESSES_ISALREADYRUNNING_STR,__LINE__,__FILE__);
			#else
				return false;
			#endif
		
		register pid_t pid = fork();
			
		if (pid == 0)
		{
			k->func(k->data);
			
			_exit(0);
		}
		else
		{
			if (pid == -1)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMPROCESSES,SYSTEMPROCESSES_RUN,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else
					return false;
				#endif			
			else
				k->pid = pid;
		}
		
		k->isRunning = true;
		++(k->executed);
				
		#ifdef NO_EX
			return true;
		#endif
	}
	else
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMPROCESSES,SYSTEMPROCESSES_RUN,ERR_LIBDODO,SYSTEMPROCESSES_NOTFOUND,SYSTEMPROCESSES_NOTFOUND_STR,__LINE__,__FILE__);
		#else
			return false;
		#endif	
}
	
//-------------------------------------------------------------------
	
#ifndef NO_EX
	void 
#else
	bool
#endif
systemProcesses::stop(unsigned long position)
{
	if (getProcess(position))
	{
		if (!_isRunning(k))
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMPROCESSES,SYSTEMPROCESSES_STOP,ERR_LIBDODO,SYSTEMPROCESSES_ISNOTRUNNING,SYSTEMPROCESSES_ISNOTRUNNING_STR,__LINE__,__FILE__);
			#else
				return false;
			#endif
		
		if (kill(k->pid,9) == -1)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMPROCESSES,SYSTEMPROCESSES_STOP,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;
			#endif
		
		k->isRunning = false;
				
		#ifdef NO_EX
			return true;
		#endif
	}
	else
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMPROCESSES,SYSTEMPROCESSES_STOP,ERR_LIBDODO,SYSTEMPROCESSES_NOTFOUND,SYSTEMPROCESSES_NOTFOUND_STR,__LINE__,__FILE__);
		#else
			return false;
		#endif	
}
	
//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
systemProcesses::stop()
{
	std::list<__processInfo>::iterator i(processes.begin()), j(processes.end());
	for (;i!=j;++i)
	{
		if (!_isRunning(i))
			continue;
		
		
		if (kill(i->pid,9) == -1)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMPROCESSES,SYSTEMPROCESSES_STOP,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;
			#endif
		
		i->isRunning = false;
	}
	
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
systemProcesses::wait(unsigned long position)
{
	if (getProcess(position))
	{
		if (!_isRunning(k))
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMPROCESSES,SYSTEMPROCESSES_WAIT,ERR_LIBDODO,SYSTEMPROCESSES_ISNOTRUNNING,SYSTEMPROCESSES_ISNOTRUNNING_STR,__LINE__,__FILE__);
			#else
				return false;
			#endif
			
		if (waitpid(k->pid,NULL,0) == -1)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMPROCESSES,SYSTEMPROCESSES_WAIT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;
			#endif
		
		k->isRunning = false;			
	
		#ifdef NO_EX
			return true;
		#endif
	}
	else
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMPROCESSES,SYSTEMPROCESSES_WAIT,ERR_LIBDODO,SYSTEMPROCESSES_NOTFOUND,SYSTEMPROCESSES_NOTFOUND_STR,__LINE__,__FILE__);
		#else
			return false;
		#endif	
}
		
//-------------------------------------------------------------------
