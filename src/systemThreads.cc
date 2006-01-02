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

systemThreads::systemThreads() : threadNum(0)
{
}

//-------------------------------------------------------------------

systemThreads::~systemThreads()
{
}

//-------------------------------------------------------------------

int 
systemThreads::addThread(threadFunc func,
						void *data)
{
	thread.data = data;
	thread.func = func;
	thread.isRunning = false;
	thread.position = ++threadNum;
	thread.thread = 0;
	
	threads.push_back(thread);
	
	return thread.position;
}

//-------------------------------------------------------------------

bool 
systemThreads::getThread(int position, 
						std::vector<__threadInfo>::iterator &iter)
{
	i = threads.begin();
	j = threads.end();
	
	for (;i!=j;++i)
		if (i->position == position)
		{
			iter = i;
			return true;
		}
	
	return false;	
}

//-------------------------------------------------------------------

bool 
systemThreads::delThread(int position,
						bool force)
{
	if (getThread(position,k))
	{
		if (k->isRunning)
		{
			if (!force)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_DELTHREAD,ERR_LIBDODO,SYSTEMTHREADS_ISRUNNING,SYSTEMTHREADS_ISRUNNING_STR,__LINE__,__FILE__);
				#else
					return false;
				#endif
			else
			{
				if (pthread_cancel(k->thread)!=0)
					#ifndef NO_EX
						throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_DELTHREAD,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
					#else
						return false;
					#endif
			}
		}
			
		threads.erase(k);
		
		return true;
	}
	else
		return false;
}

//-------------------------------------------------------------------

bool 
systemThreads::replaceThread(int position, 
						threadFunc func, 
						void *data,
						bool force)
{
	if (getThread(position,k))
	{
		if (k->isRunning)
		{
			if (!force)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_REPLACETHREAD,ERR_LIBDODO,SYSTEMTHREADS_ISRUNNING,SYSTEMTHREADS_ISRUNNING_STR,__LINE__,__FILE__);
				#else
					return false;
				#endif
			else
			{
				if (pthread_cancel(k->thread)!=0)
					#ifndef NO_EX
						throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_REPLACETHREAD,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
					#else
						return false;
					#endif
			}
		}
			
		k->data = data;
		k->func = func;
		
		return true;
	}
	else
		return false;
	
}

//-------------------------------------------------------------------

bool 
systemThreads::runThread(int position, 
						bool force)
{
	if (getThread(position,k))
	{
		if (k->isRunning && !force)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_RUNTHREAD,ERR_LIBDODO,SYSTEMTHREADS_ISRUNNING,SYSTEMTHREADS_ISRUNNING_STR,__LINE__,__FILE__);
			#else
				return false;
			#endif
			
		if (pthread_create(&(k->thread),NULL,k->func,k->data)!=0)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_RUNTHREAD,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;
			#endif
		
		k->isRunning = true;
				
		return true;
	}
	else
		return false;	
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
systemThreads::waitThread(int position,
						void **data)
{
	if (getThread(position,k))
	{
		if (!k->isRunning)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_WAITTHREAD,ERR_LIBDODO,SYSTEMTHREADS_ISNOTRUNNING,SYSTEMTHREADS_ISNOTRUNNING_STR,__LINE__,__FILE__);
			#else
				return false;
			#endif
			
		if (pthread_join(k->thread,data)!=0)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_WAITTHREAD,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;
			#endif
		
		k->isRunning = false;
				
	
		#ifdef NO_EX
			return true;
		#endif
	}
	
	#ifdef NO_EX
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
				throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_WAITTHREAD,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
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
