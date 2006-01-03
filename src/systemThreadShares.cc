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


#include <systemThreadShares.h>
	
using namespace dodo;

systemThreadShares::systemThreadShares() : sharedNum(0)
{
}

//-------------------------------------------------------------------

systemThreadShares::~systemThreadShares()
{
}

bool 
systemThreadShares::getShared(int position)
{
	l = shareds.begin();
	m = shareds.end();
	
	for (;l!=m;++l)
		if (l->position == position)
		{
			n = l;
			return true;
		}
	
	return false;	
}

//-------------------------------------------------------------------

int 
systemThreadShares::addShared(void *data)
{
	shared.data = data;
	shared.position = ++sharedNum;
	
	shareds.push_back(shared);
	
	return shared.position;
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
systemThreadShares::delShared(int position,
						bool force)
{
/*	if (getThread(position))
	{
		if (k->isRunning)
		{
			if (!force)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_DELTHREAD,ERR_LIBDODO,SYSTEMTHREADS_ISALREADYRUNNING,SYSTEMTHREADS_ISALREADYRUNNING_STR,__LINE__,__FILE__);
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
			throw baseEx(ERRMODULE_SYSTEMTHREADS,SYSTEMTHREADS_DELTHREAD,ERR_LIBDODO,SYSTEMTHREADS_NOTFOUND,SYSTEMTHREADS_NOTFOUND_STR,__LINE__,__FILE__);
		#else
			return false;
		#endif*/
}

//-------------------------------------------------------------------

