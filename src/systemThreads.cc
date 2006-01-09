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
				break;
			
			case THREAD_WAIT:
			default:
				if (pthread_join(i->thread,NULL)!=0)
					#ifndef NO_EX
						throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_DESTRUCTOR,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
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
						bool force)
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
				
				k->isRunning = false;
			}
		}
			
		k->data = data;
		k->func = func;
		
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
systemThreads::_isRunning(std::vector<__threadInfo>::iterator &position)
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
