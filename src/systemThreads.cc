/***************************************************************************
 *            systemThreads.cc
 *
 *  Wed Nov 30 22:02:16 2005
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


#include <systemThreads.h>

#ifdef PTHREAD_EXT
	
	using namespace dodo;
	
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
		pthread_attr_init(&attr);
	}
	
	//-------------------------------------------------------------------
	
	systemThreads::~systemThreads()
	{
		pthread_attr_destroy(&attr);
		
		std::list<__threadInfo>::iterator i(threads.begin()), j(threads.end());
		
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
				
					pthread_detach(i->thread);
					
					break;
					
				case THREAD_STOP:
				
					pthread_cancel(i->thread);
					
					break;
				
				case THREAD_WAIT:
				default:
				
					pthread_join(i->thread,NULL);						
			}
			
			#ifdef DL_EXT
			
				if (i->handle != NULL)
				{
					deinit = (deinitSystemThreadsModule)dlsym(i->handle, "deinitSystemThreadsModule");
					if (deinit != NULL)
						deinit();
					
					dlclose(i->handle);						
				}
				
			#endif
		}	
	}
		
	//-------------------------------------------------------------------
	
	unsigned long 
	systemThreads::add(jobFunc func,
							void *data)
	{
		return add(func,data,false,THREAD_WAIT,2097152);
	}
	
	//-------------------------------------------------------------------
	
	unsigned long 
	systemThreads::add(threadFunc func,
							void *data,
							bool detached,
							short action,
							int stackSize)
	{
		__threadInfo thread;
		
		thread.detached = detached;
		thread.data = data;
		thread.func = func;
		thread.position = ++threadNum;
		thread.stackSize = stackSize;
		thread.action = action;
		thread.executeLimit = 0;
		
		#ifdef DL_EXT
			thread.handle = NULL;
		#endif
		
		threads.push_back(thread);
		
		return thread.position;
	}
	
	//-------------------------------------------------------------------
	
	bool 
	systemThreads::getThread(unsigned long position) const
	{
		std::list<__threadInfo>::const_iterator i(threads.begin()), j(threads.end());
		for (;i!=j;++i)
			if (i->position == position)
			{
				current = *((std::list<__threadInfo>::iterator *)&i);
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
			if (_isRunning(current))
			{
				if (!force)
					#ifndef NO_EX
						throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_DEL,ERR_LIBDODO,SYSTEMTHREADS_ISALREADYRUNNING,SYSTEMTHREADS_ISALREADYRUNNING_STR,__LINE__,__FILE__);
					#else
						return false;
					#endif
				else
				{
					errno = pthread_cancel(current->thread);
					if (errno != 0)
						#ifndef NO_EX
							throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_DEL,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
						#else
							return false;
						#endif
				}
			}
	
			#ifdef DL_EXT
			
				if (current->handle != NULL)
				{
					deinitSystemThreadsModule deinit;	
		
					deinit = (deinitSystemThreadsModule)dlsym(current->handle, "deinitSystemThreadsModule");
					if (deinit != NULL)
						deinit();
					
					if (dlclose(current->handle)!=0)
						#ifndef NO_EX
							throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_DEL,ERR_DYNLOAD,0,dlerror(),__LINE__,__FILE__);
						#endif
						
					current->handle = NULL;	
				}
					
			#endif
				
			threads.erase(current);
			
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
							short action,
							int stackSize)
	{
		if (getThread(position))
		{
			if (_isRunning(current))
			{
				if (!force)
					#ifndef NO_EX
						throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_REPLACE,ERR_LIBDODO,SYSTEMTHREADS_ISALREADYRUNNING,SYSTEMTHREADS_ISALREADYRUNNING_STR,__LINE__,__FILE__);
					#else
						return false;
					#endif
				else
				{
					errno = pthread_cancel(current->thread);
					if (errno != 0)
						#ifndef NO_EX
							throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_REPLACE,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
						#else
							return false;
						#endif
				}
			}
	
	
			#ifdef DL_EXT
			
				if (current->handle!=NULL)
				{
					deinitSystemThreadsModule deinit;	
		
					deinit = (deinitSystemThreadsModule)dlsym(current->handle, "deinitSystemThreadsModule");
					if (deinit != NULL)
						deinit();
					
					if (dlclose(current->handle)!=0)
						#ifndef NO_EX
							throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_DEL,ERR_DYNLOAD,0,dlerror(),__LINE__,__FILE__);
						#endif
						
					current->handle = NULL;	
				}
					
			#endif
				
			current->data = data;
			current->func = func;
			current->isRunning = false;
			current->detached = detached;
			current->stackSize = stackSize;
			current->action = action;
				
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
			if (current->executeLimit > 0 && (current->executeLimit <= current->executed))
			{
				threads.erase(current);
				
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_RUN,ERR_LIBDODO,SYSTEMTHREADS_SWEPT,SYSTEMTHREADS_SWEPT_STR,__LINE__,__FILE__);
				#else
					return false;
				#endif				
			}
			
			if (_isRunning(current) && !force)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_RUN,ERR_LIBDODO,SYSTEMTHREADS_ISALREADYRUNNING,SYSTEMTHREADS_ISALREADYRUNNING_STR,__LINE__,__FILE__);
				#else
					return false;
				#endif
			
			if (current->detached)
				pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
			else
				pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	
			errno = pthread_attr_setstacksize(&attr,current->stackSize);
			if (errno != 0)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_RUN,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else
					return false;
				#endif
				
			errno = pthread_create(&(current->thread),&attr,current->func,current->data);	
			if (errno != 0)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_RUN,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else
					return false;
				#endif
			
			current->isRunning = true;
			++(current->executed);
					
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
	systemThreads::wait(unsigned long position)
	{
		if (getThread(position))
		{
			if (!_isRunning(current))
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_WAIT,ERR_LIBDODO,SYSTEMTHREADS_ISNOTRUNNING,SYSTEMTHREADS_ISNOTRUNNING_STR,__LINE__,__FILE__);
				#else
					return false;
				#endif
			
			if (current->detached)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_WAIT,ERR_LIBDODO,SYSTEMTHREADS_ISDETACHED,SYSTEMTHREADS_ISDETACHED_STR,__LINE__,__FILE__);
				#else
					return false;
				#endif
			
			errno = pthread_join(current->thread,NULL);	
			if (errno != 0)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_WAIT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else
					return false;
				#endif
			
			current->isRunning = false;			
		
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
		std::list<__threadInfo>::iterator i(threads.begin()), j(threads.end());
		for (;i!=j;++i)
		{
			if (!_isRunning(i) || i->detached)
				continue;
			
			errno = pthread_join(i->thread,NULL);
			if (errno != 0)
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
	
	#ifndef NO_EX
		void 
	#else
		bool
	#endif
	systemThreads::stop(unsigned long position)
	{
		if (getThread(position))
		{
			if (!_isRunning(current))
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_STOP,ERR_LIBDODO,SYSTEMTHREADS_ISNOTRUNNING,SYSTEMTHREADS_ISNOTRUNNING_STR,__LINE__,__FILE__);
				#else
					return false;
				#endif
			
			errno = pthread_cancel(current->thread);	
			if (errno != 0)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_STOP,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else
					return false;
				#endif
			
			current->isRunning = false;
					
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
		std::list<__threadInfo>::iterator i(threads.begin()), j(threads.end());
		for (;i!=j;++i)
		{
			if (!_isRunning(i))
				continue;
			
			errno = pthread_cancel(i->thread);
			if (errno != 0)
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
	systemThreads::isRunning(unsigned long position) const
	{
		if (getThread(position))
			return _isRunning(current);
		else
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_ISRUNNING,ERR_LIBDODO,SYSTEMTHREADS_NOTFOUND,SYSTEMTHREADS_NOTFOUND_STR,__LINE__,__FILE__);
			#else
				return false;
			#endif	
	}
	
	//-------------------------------------------------------------------
	
	bool 
	systemThreads::_isRunning(std::list<__threadInfo>::iterator &position) const
	{
		if (!position->isRunning)
			return false;
			
		errno = pthread_kill(position->thread,0);	
		if (errno != 0)	
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
		std::list<__threadInfo>::iterator i(threads.begin()), j(threads.end());
		while(i!=j)
		{
			if (_isRunning(i))
			{
				++i;

				continue;
			}
					
			if (i->executeLimit > 0 && (i->executeLimit <= i->executed))
			{
				i = threads.erase(i);

				continue;
			}

			++i;
		}	
	}
	
	//-------------------------------------------------------------------
	
	#ifndef NO_EX
		void 
	#else
		bool
	#endif
	systemThreads::setExecutionLimit(unsigned long position, 
									unsigned long limit)
	{
		if (getThread(position))
			current->executeLimit = limit;
		else
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_SETEXECUTIONLIMIT,ERR_LIBDODO,SYSTEMTHREADS_NOTFOUND,SYSTEMTHREADS_NOTFOUND_STR,__LINE__,__FILE__);
			#else
				return false;
			#endif	
			
		#ifdef NO_EX		
			return true;
		#endif
	}
	
	//-------------------------------------------------------------------
	
	unsigned long 
	systemThreads::running() const
	{
		unsigned long amount(0);

		std::list<__threadInfo>::const_iterator i(threads.begin()), j(threads.end());		
		for (;i!=j;++i)
			if (_isRunning(*((std::list<__threadInfo>::iterator *)&i)))
				++amount;
		
		return amount;		
	}
	
	//-------------------------------------------------------------------
		
	#ifdef DL_EXT
		
		systemThreadsMod 
		systemThreads::getModuleInfo(const dodoString &module, 
									void *toInit)
		{
			void *handle = dlopen(module.c_str(), RTLD_LAZY);
			if (handle == NULL)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_GETMODULEINFO,ERR_DYNLOAD,0,dlerror(),__LINE__,__FILE__);
				#else
					return systemThreadsMod();
				#endif
				
			initSystemThreadsModule init = (initSystemThreadsModule)dlsym(handle, "initSystemThreadsModule");
			if (init == NULL)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_GETMODULEINFO,ERR_DYNLOAD,0,dlerror(),__LINE__,__FILE__);
				#else
					return systemThreadsMod();
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
		systemThreads::add(const dodoString &module,
							void *data,
							void *toInit,
							bool detached,
							short action,
							int stackSize)
		{
			__threadInfo thread;
			
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
					return 0;
				#endif
			
			initSystemThreadsModule init = (initSystemThreadsModule)dlsym(thread.handle, "initSystemThreadsModule");
			if (init == NULL)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_ADD,ERR_DYNLOAD,0,dlerror(),__LINE__,__FILE__);
				#else
					return 0;
				#endif	
			
			systemThreadsMod temp = init(toInit);
			
			threadFunc in = (threadFunc)dlsym(thread.handle, temp.hook);
			if (in == NULL)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_ADD,ERR_DYNLOAD,0,dlerror(),__LINE__,__FILE__);
				#else
					return 0;
				#endif
		
			thread.executeLimit = temp.executeLimit;
			thread.func = in;		
			
			threads.push_back(thread);
			
			return thread.position;
		}
	
		//-------------------------------------------------------------------
		
		unsigned long 
		systemThreads::add(const dodoString &module,
								void *data, 
								void *toInit)
		{
			__threadInfo thread;
			
			thread.data = data;
			thread.position = ++threadNum;
			
			thread.handle = dlopen(module.c_str(), RTLD_LAZY);
			if (thread.handle == NULL)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_ADD,ERR_DYNLOAD,0,dlerror(),__LINE__,__FILE__);
				#else
					return 0;
				#endif
			
			initSystemThreadsModule init = (initSystemThreadsModule)dlsym(thread.handle, "initSystemThreadsModule");
			if (init == NULL)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_ADD,ERR_DYNLOAD,0,dlerror(),__LINE__,__FILE__);
				#else
					return 0;
				#endif	
			
			systemThreadsMod temp = init(toInit);
			
			threadFunc in = (threadFunc)dlsym(thread.handle, temp.hook);
			if (in == NULL)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_ADD,ERR_DYNLOAD,0,dlerror(),__LINE__,__FILE__);
				#else
					return 0;
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
	
	void 
	systemThreads::blockSignal(int signals, 
								bool block)
	{	
		sigset_t signal_mask;
		sigemptyset(&signal_mask);
		
		systemTools::sigMask(&signal_mask, signals);
		
		if (block)
			pthread_sigmask(SIG_BLOCK,&signal_mask,NULL);	
		else
			pthread_sigmask(SIG_UNBLOCK,&signal_mask,NULL);
	}
	
	//-------------------------------------------------------------------
	
	unsigned long 
	systemThreads::addNRun(jobFunc func,
							void *data)
	{
		return addNRun(func,data,1,false,THREAD_WAIT,2097152);
	}
	
	//-------------------------------------------------------------------
	
	unsigned long 
	systemThreads::addNRun(threadFunc func,
							void *data, 
							unsigned long limit,
							bool detached,
							short action,
							int stackSize)
	{
		__threadInfo thread;
			
		thread.detached = detached;
		thread.data = data;
		thread.func = func;
		thread.position = ++threadNum;
		thread.stackSize = stackSize;
		thread.action = action;
		thread.executeLimit = limit;
		
		#ifdef DL_EXT
			thread.handle = NULL;
		#endif
			
		if (detached)
			pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		else
			pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	
		errno = pthread_attr_setstacksize(&attr,stackSize);
		if (errno != 0)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_ADDNRUN,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return 0;
			#endif
			
		errno = pthread_create(&(thread.thread),&attr,func,data);	
		if (errno != 0)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_ADDNRUN,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return 0;
			#endif
		
		thread.isRunning = true;
		++(thread.executed);
		
		threads.push_back(thread);
		
		return thread.position;
	}
	
	//-------------------------------------------------------------------

#endif
