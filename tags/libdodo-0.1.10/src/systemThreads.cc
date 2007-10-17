/***************************************************************************
 *            systemThreads.cc
 *
 *  Wed Nov 30 22:02:16 2005
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


#include <systemThreads.h>
	
using namespace dodo;

static void
dummySystemThreads(int signal)
{
}

//-------------------------------------------------------------------

__threadInfo::__threadInfo() : thread(0),
								isRunning(false),
								executed(0),
								executeLimit(0)
{
}

//-------------------------------------------------------------------

systemThreads::systemThreads(systemThreads &st)
{
}

//-------------------------------------------------------------------

systemThreads::systemThreads() : threadNum(0)
{
	struct sigaction act;
	act.sa_handler = dummySystemThreads;
	act.sa_flags = 0;
	
	if (sigaction(5,&act,NULL)==-1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_CONSTRUCTOR,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif		
	
	sigset_t signal_mask;
	sigemptyset(&signal_mask);
	sigaddset(&signal_mask,5);
	
	pthread_sigmask(SIG_BLOCK,&signal_mask,NULL);
	
	pthread_attr_init(&attr);
}

//-------------------------------------------------------------------

systemThreads::~systemThreads()
{
	pthread_attr_destroy(&attr);
	
	i = threads.begin();
	j = threads.end();
	
	#ifdef DL_EXT
		deinitSystemThreadsModule deinit;
	#endif
	
	for (;i!=j;++i)
	{
		if (!_isRunning(i) || i->detached)
			continue;
		
		switch (i->action)
		{
			case THREAD_KEEP_ALIVE:
				if (pthread_detach(i->thread)!=0)
					#ifndef NO_EX
						throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_DESTRUCTOR,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
					#endif
				break;
				
			case THREAD_STOP:
				if (pthread_cancel(i->thread)!=0)
					#ifndef NO_EX
						throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_DESTRUCTOR,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
					#endif
				
				#ifdef DL_EXT
				
					if (i->handle != NULL)
					{
						deinit = (deinitSystemThreadsModule)dlsym(i->handle, "deinitSystemThreadsModule");
						if (deinit != NULL)
							deinit();
						
						if (dlclose(i->handle)!=0)
							#ifndef NO_EX
								throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_DESTRUCTOR,ERR_DYNLOAD,0,dlerror(),__LINE__,__FILE__);
							#endif							
					}
					
				#endif
				break;
			
			case THREAD_WAIT:
			default:
				if (pthread_join(i->thread,NULL)!=0)
					#ifndef NO_EX
						throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_DESTRUCTOR,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
					#endif
				
				#ifdef DL_EXT
				
					if (i->handle != NULL)
					{
						deinit = (deinitSystemThreadsModule)dlsym(i->handle, "deinitSystemThreadsModule");
						if (deinit != NULL)
							deinit();
						
						if (dlclose(i->handle)!=0)
							#ifndef NO_EX
								throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_DESTRUCTOR,ERR_DYNLOAD,0,dlerror(),__LINE__,__FILE__);
							#endif							
					}
					
				#endif								
		}
	}	
}

//-------------------------------------------------------------------

unsigned long 
systemThreads::add(threadFunc func,
						void *data,
						bool detached,
						systemThreadOnDestructEnum action,
						int stackSize)
{
	thread.detached = detached;
	thread.data = data;
	thread.func = func;
	thread.position = ++threadNum;
	thread.stackSize = stackSize;
	thread.action = action;
	
	#ifdef DL_EXT
		thread.handle = NULL;
	#endif
	
	threads.push_back(thread);
	
	return thread.position;
}

//-------------------------------------------------------------------

bool 
systemThreads::getThread(unsigned long position)
{
	i = threads.begin();
	j = threads.end();
	
	for (;i!=j;++i)
		if (i->position == position)
		{
			k = i;
			return true;
		}
	
	return false;	
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
systemThreads::del(unsigned long position,
						bool force)
{
	if (getThread(position))
	{
		if (_isRunning(k))
		{
			if (!force)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_DEL,ERR_LIBDODO,SYSTEMTHREADS_ISALREADYRUNNING,SYSTEMTHREADS_ISALREADYRUNNING_STR,__LINE__,__FILE__);
				#else
					return false;
				#endif
			else
			{
				if (pthread_cancel(k->thread)!=0)
					#ifndef NO_EX
						throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_DEL,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
					#else
						return false;
					#endif
			}
		}

		#ifdef DL_EXT
		
			if (k->handle!=NULL)
			{
				deinitSystemThreadsModule deinit;	
	
				deinit = (deinitSystemThreadsModule)dlsym(k->handle, "deinitSystemThreadsModule");
				if (deinit != NULL)
					deinit();
				
				if (dlclose(k->handle)!=0)
					#ifndef NO_EX
						throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_DEL,ERR_DYNLOAD,0,dlerror(),__LINE__,__FILE__);
					#endif
					
				k->handle = NULL;	
			}
				
		#endif
			
		threads.erase(k);
		
		#ifdef NO_EX
			return true;
		#endif
	}
	else
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_DEL,ERR_LIBDODO,SYSTEMTHREADS_NOTFOUND,SYSTEMTHREADS_NOTFOUND_STR,__LINE__,__FILE__);
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
systemThreads::replace(unsigned long position, 
						threadFunc func, 
						void *data,
						bool force,
						bool detached,
						systemThreadOnDestructEnum action,
						int stackSize)
{
	if (getThread(position))
	{
		if (_isRunning(k))
		{
			if (!force)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_REPLACE,ERR_LIBDODO,SYSTEMTHREADS_ISALREADYRUNNING,SYSTEMTHREADS_ISALREADYRUNNING_STR,__LINE__,__FILE__);
				#else
					return false;
				#endif
			else
			{
				if (pthread_cancel(k->thread)!=0)
					#ifndef NO_EX
						throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_REPLACE,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
					#else
						return false;
					#endif
			}
		}


		#ifdef DL_EXT
		
			if (k->handle!=NULL)
			{
				deinitSystemThreadsModule deinit;	
	
				deinit = (deinitSystemThreadsModule)dlsym(k->handle, "deinitSystemThreadsModule");
				if (deinit != NULL)
					deinit();
				
				if (dlclose(k->handle)!=0)
					#ifndef NO_EX
						throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_DEL,ERR_DYNLOAD,0,dlerror(),__LINE__,__FILE__);
					#endif
					
				k->handle = NULL;	
			}
				
		#endif
			
		k->data = data;
		k->func = func;
		k->isRunning = false;
		k->detached = detached;
		k->stackSize = stackSize;
		k->action = action;
			
		#ifdef NO_EX
			return true;
		#endif
	}
	else
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_REPLACE,ERR_LIBDODO,SYSTEMTHREADS_NOTFOUND,SYSTEMTHREADS_NOTFOUND_STR,__LINE__,__FILE__);
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
systemThreads::run(unsigned long position, 
						bool force)
{
	if (getThread(position))
	{
		if (k->executeLimit>0 && (k->executeLimit<=k->executed))
		{
			threads.erase(k);
			
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_RUN,ERR_LIBDODO,SYSTEMTHREADS_SWEPT,SYSTEMTHREADS_SWEPT_STR,__LINE__,__FILE__);
			#else
				return false;
			#endif				
		}
		
		if (_isRunning(k) && !force)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_RUN,ERR_LIBDODO,SYSTEMTHREADS_ISALREADYRUNNING,SYSTEMTHREADS_ISALREADYRUNNING_STR,__LINE__,__FILE__);
			#else
				return false;
			#endif

		if (k->detached)
			pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		else
			pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

		if (pthread_attr_setstacksize(&attr,k->stackSize)!=0)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_RUN,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;
			#endif
			
		if (pthread_create(&(k->thread),&attr,k->func,k->data)!=0)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_RUN,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;
			#endif
		
		k->isRunning = true;
		++(k->executed);
				
		#ifdef NO_EX
			return true;
		#endif
	}
	else
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_RUN,ERR_LIBDODO,SYSTEMTHREADS_NOTFOUND,SYSTEMTHREADS_NOTFOUND_STR,__LINE__,__FILE__);
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
systemThreads::wait(unsigned long position,
						void **data)
{
	if (getThread(position))
	{
		if (!_isRunning(k))
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_WAIT,ERR_LIBDODO,SYSTEMTHREADS_ISNOTRUNNING,SYSTEMTHREADS_ISNOTRUNNING_STR,__LINE__,__FILE__);
			#else
				return false;
			#endif
		
		if (k->detached)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_WAIT,ERR_LIBDODO,SYSTEMTHREADS_ISDETACHED,SYSTEMTHREADS_ISDETACHED_STR,__LINE__,__FILE__);
			#else
				return false;
			#endif
			
		if (pthread_join(k->thread,data)!=0)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_WAIT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
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
			throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_WAIT,ERR_LIBDODO,SYSTEMTHREADS_NOTFOUND,SYSTEMTHREADS_NOTFOUND_STR,__LINE__,__FILE__);
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
systemThreads::wait()
{
	i = threads.begin();
	j = threads.end();
	
	
	for (;i!=j;++i)
	{
		if (!_isRunning(i) || i->detached)
			continue;
		
		if (pthread_join(i->thread,NULL)!=0)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_WAIT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
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

void 
systemThreads::returnFromThread(void *data)
{	
	pthread_exit(data);
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
systemThreads::stop(unsigned long position)
{
	if (getThread(position))
	{
		if (!_isRunning(k))
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_STOP,ERR_LIBDODO,SYSTEMTHREADS_ISNOTRUNNING,SYSTEMTHREADS_ISNOTRUNNING_STR,__LINE__,__FILE__);
			#else
				return false;
			#endif
			
		if (pthread_cancel(k->thread)!=0)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_STOP,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
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
			throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_STOP,ERR_LIBDODO,SYSTEMTHREADS_NOTFOUND,SYSTEMTHREADS_NOTFOUND_STR,__LINE__,__FILE__);
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
systemThreads::stop()
{
	i = threads.begin();
	j = threads.end();
	
	
	for (;i!=j;++i)
	{
		if (!_isRunning(i))
			continue;
		
		if (pthread_cancel(i->thread)!=0)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_STOP,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
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


bool
systemThreads::isRunning(unsigned long position)
{
	if (getThread(position))
		return _isRunning(k);
	else
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_ISRUNNING,ERR_LIBDODO,SYSTEMTHREADS_NOTFOUND,SYSTEMTHREADS_NOTFOUND_STR,__LINE__,__FILE__);
		#else
			return false;
		#endif	
}

//-------------------------------------------------------------------

bool 
systemThreads::_isRunning(std::list<__threadInfo>::iterator &position)
{
	if (!position->isRunning)
		return false;
	
	if (pthread_kill(position->thread,5)!=0)	
	{
		if (errno == ESRCH || errno == EAGAIN)
		{
			position->isRunning = false;
			
			return false;
		}

		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS__ISRUNNING,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif			
	}
	
	return true;	
}

//-------------------------------------------------------------------

void
systemThreads::sweepTrash()
{
	i = threads.begin();
	j = threads.end();
	
	for (;i!=j;++i)
	{
		if (_isRunning(i))
			continue;
				
		if (i->executeLimit>0 && (i->executeLimit<=i->executed))
			threads.erase(i);
	}	
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
systemThreads::setExecutionLimit(unsigned long position, unsigned long limit)
{
	if (getThread(position))
		k->executeLimit = limit;
	else
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_ISRUNNING,ERR_LIBDODO,SYSTEMTHREADS_NOTFOUND,SYSTEMTHREADS_NOTFOUND_STR,__LINE__,__FILE__);
		#else
			return false;
		#endif	
}

//-------------------------------------------------------------------

unsigned long 
systemThreads::running()
{
	i = threads.begin();
	j = threads.end();
	
	unsigned long amount(0);
	
	for (;i!=j;++i)
		if (_isRunning(i))
			++amount;
	
	return amount;		
}

//-------------------------------------------------------------------
	
#ifdef DL_EXT
	
	systemThreadsMod 
	systemThreads::getModuleInfo(const std::string &module, 
								void *toInit)
	{
		void *handle = dlopen(module.c_str(), RTLD_LAZY);
		if (handle == NULL)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_GETMODULEINFO,ERR_DYNLOAD,0,dlerror(),__LINE__,__FILE__);
			#else
				return xexecExMod();
			#endif
			
		initSystemThreadsModule init = (initSystemThreadsModule)dlsym(handle, "initSystemThreadsModule");
		if (init == NULL)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_GETMODULEINFO,ERR_DYNLOAD,0,dlerror(),__LINE__,__FILE__);
			#else
				return xexecExMod();
			#endif
			
		systemThreadsMod mod = init(toInit);
		
		if (dlclose(handle)!=0)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_GETMODULEINFO,ERR_DYNLOAD,0,dlerror(),__LINE__,__FILE__);
			#else
				return mod;
			#endif
		
		return mod;	
	}

	//-------------------------------------------------------------------
	
	unsigned long 
	systemThreads::add(const std::string &module,
						void *data,
						void *toInit,
						bool detached,
						systemThreadOnDestructEnum action,
						int stackSize)
	{
		
		
		
		thread.detached = detached;
		thread.data = data;
		thread.position = ++threadNum;
		thread.stackSize = stackSize;
		thread.action = action;
		
		thread.handle = dlopen(module.c_str(), RTLD_LAZY);
		if (thread.handle == NULL)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_ADD,ERR_DYNLOAD,0,dlerror(),__LINE__,__FILE__);
			#else
				return -1;
			#endif
		
		initSystemThreadsModule init = (initSystemThreadsModule)dlsym(thread.handle, "initSystemThreadsModule");
		if (init == NULL)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_ADD,ERR_DYNLOAD,0,dlerror(),__LINE__,__FILE__);
			#else
				return -1;
			#endif	
		
		systemThreadsMod temp = init(toInit);
		
		threadFunc in = (threadFunc)dlsym(thread.handle, temp.hook);
		if (in == NULL)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_ADD,ERR_DYNLOAD,0,dlerror(),__LINE__,__FILE__);
			#else
				return -1;
			#endif
	
		thread.executeLimit = temp.executeLimit;
		thread.func = in;		
		
		threads.push_back(thread);
		
		return thread.position;
	}

	//-------------------------------------------------------------------
	
	unsigned long 
	systemThreads::add(const std::string &module,
							void *data, 
							void *toInit)
	{
		thread.data = data;
		thread.position = ++threadNum;
		
		thread.handle = dlopen(module.c_str(), RTLD_LAZY);
		if (thread.handle == NULL)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_ADD,ERR_DYNLOAD,0,dlerror(),__LINE__,__FILE__);
			#else
				return -1;
			#endif
		
		initSystemThreadsModule init = (initSystemThreadsModule)dlsym(thread.handle, "initSystemThreadsModule");
		if (init == NULL)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_ADD,ERR_DYNLOAD,0,dlerror(),__LINE__,__FILE__);
			#else
				return -1;
			#endif	
		
		systemThreadsMod temp = init(toInit);
		
		threadFunc in = (threadFunc)dlsym(thread.handle, temp.hook);
		if (in == NULL)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_ADD,ERR_DYNLOAD,0,dlerror(),__LINE__,__FILE__);
			#else
				return -1;
			#endif
	
		thread.executeLimit = temp.executeLimit;
		thread.detached = temp.detached;
		thread.stackSize = temp.stackSize;
		thread.action = temp.action;
		thread.func = in;		
		
		threads.push_back(thread);
		
		return thread.position;
	}

#endif

//-------------------------------------------------------------------

