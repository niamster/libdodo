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
					#ifdef DL_EXT
					
						if (i->handle != NULL)
						{
							deinit = (deinitSystemThreadsModule)dlsym(i->handle, "deinitSystemThreadsModule");
							if (deinit != NULL)
								deinit();
							
							dlclose(i->handle);						
						}
						
					#endif
					
					break;
				
				case THREAD_WAIT:
				default:
				
					pthread_join(i->thread,NULL);
					
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
				k = *((std::list<__threadInfo>::iterator *)&i);
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
					errno = pthread_cancel(k->thread);
					if (errno != 0)
						#ifndef NO_EX
							throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_DEL,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
						#else
							return false;
						#endif
				}
			}
	
			#ifdef DL_EXT
			
				if (k->handle != NULL)
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
							short action,
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
					errno = pthread_cancel(k->thread);
					if (errno != 0)
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
			if (k->executeLimit > 0 && (k->executeLimit <= k->executed))
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
	
			errno = pthread_attr_setstacksize(&attr,k->stackSize);
			if (errno != 0)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_RUN,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else
					return false;
				#endif
				
			errno = pthread_create(&(k->thread),&attr,k->func,k->data);	
			if (errno != 0)
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
	systemThreads::wait(unsigned long position)
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
			
			errno = pthread_join(k->thread,NULL);	
			if (errno != 0)
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
			if (!_isRunning(k))
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_STOP,ERR_LIBDODO,SYSTEMTHREADS_ISNOTRUNNING,SYSTEMTHREADS_ISNOTRUNNING_STR,__LINE__,__FILE__);
				#else
					return false;
				#endif
			
			errno = pthread_cancel(k->thread);	
			if (errno != 0)
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
		int o = 0, p = threads.size();
		for (;o<p;++p)
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
				return false;
			#endif
			
		errno = pthread_create(&(thread.thread),&attr,func,data);	
		if (errno != 0)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_ADDNRUN,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;
			#endif
		
		thread.isRunning = true;
		++(thread.executed);
		
		threads.push_back(thread);
		
		return thread.position;
	}
	
	//-------------------------------------------------------------------

#endif
