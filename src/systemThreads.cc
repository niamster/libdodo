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

systemThreads::systemThreads(systemThreads &st)
{
}

//-------------------------------------------------------------------

systemThreads::systemThreads() : threadNum(0)
{
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
}

//-------------------------------------------------------------------

systemThreads::~systemThreads()
{
	pthread_attr_destroy(&attr);
	
	i = threads.begin();
	j = threads.end();
	
	for (;i!=j;++i)
	{
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

int 
systemThreads::add(threadFunc func,
						void *data,
						systemThreadOnDestructEnum action,
						int stackSize)
{
	thread.data = data;
	thread.func = func;
	thread.isRunning = false;
	thread.position = ++threadNum;
	thread.thread = 0;
	thread.stackSize = stackSize;
	thread.action = action;
	
	threads.push_back(thread);
	
	return thread.position;
}

//-------------------------------------------------------------------

bool 
systemThreads::getThread(int position)
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
systemThreads::del(int position,
						bool force)
{
	if (getThread(position))
	{
		if (k->isRunning)
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
				
				k->isRunning = false;
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
systemThreads::replace(int position, 
						threadFunc func, 
						void *data,
						bool force)
{
	if (getThread(position))
	{
		if (k->isRunning)
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
systemThreads::run(int position, 
						bool force)
{
	if (getThread(position))
	{
		if (k->isRunning && !force)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_RUN,ERR_LIBDODO,SYSTEMTHREADS_ISALREADYRUNNING,SYSTEMTHREADS_ISALREADYRUNNING_STR,__LINE__,__FILE__);
			#else
				return false;
			#endif

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
systemThreads::wait(int position,
						void **data)
{
	if (getThread(position))
	{
		if (!k->isRunning)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_WAIT,ERR_LIBDODO,SYSTEMTHREADS_ISNOTRUNNING,SYSTEMTHREADS_ISNOTRUNNING_STR,__LINE__,__FILE__);
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
		if (!i->isRunning)
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
systemThreads::stop(int position)
{
	if (getThread(position))
	{
		if (!k->isRunning)
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
		if (!i->isRunning)
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
systemThreads::isRunning(int position)
{
	if (getThread(position))
		return k->isRunning;
	else
		#ifndef NO_EX
			throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_ISRUNNING,ERR_LIBDODO,SYSTEMTHREADS_NOTFOUND,SYSTEMTHREADS_NOTFOUND_STR,__LINE__,__FILE__);
		#else
			return false;
		#endif	
}

//-------------------------------------------------------------------
