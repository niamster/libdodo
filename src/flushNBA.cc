/***************************************************************************
 *            flushNBA.cc
 *
 *  Thu Sep 09 03:21:24 2006
 *  Copyright  2006  Ni@m
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

#include <flushNBA.h>

using namespace dodo;

flushNBA::flushNBA(flushNBA &rt)
{
}

//-------------------------------------------------------------------

flushNBA::flushNBA() : descs(0)
{
}

//-------------------------------------------------------------------

flushNBA::~flushNBA()
{
}

//-------------------------------------------------------------------
 
int 
flushNBA::addFlush(const flush &fl)
{
	temp.position = ++descs;
	temp.in = fl.getInDescriptor();
	temp.out = fl.getOutDescriptor();
	
	desc.push_back(temp);
	
	return temp.position; 
}

//-------------------------------------------------------------------

bool 
flushNBA::isReadable(int pos,
					int timeout)
{
	i = desc.begin();
	j = desc.end();
	for (;i!=j;++i)
		if (i->position == pos)
		{
			fd.fd = i->in;
			fd.events = POLLIN | POLLPRI;
			
			int res = poll(&fd,1,timeout);
			
			if (res > 0)
			{
				if ((POLLIN&fd.revents) == POLLIN || (POLLPRI&fd.revents) == POLLPRI)
					return true;
				else
					return false;
			}
			else
			{
				if (res == 0)
					return false;
				else	
					#ifndef NO_EX
						throw baseEx(ERRMODULE_FLUSHNBA,FLUSHNBA_ISREADABLE,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
					#else			
						return false;
					#endif
			}
		}
	
	return false;
}

//-------------------------------------------------------------------

void 
flushNBA::delFlush(int pos)
{
	i = desc.begin();
	j = desc.end();
	for (;i!=j;++i)
		if (i->position == pos)
		{
			desc.erase(i);
			break;
		}
}

//-------------------------------------------------------------------

bool 
flushNBA::isWritable(int pos,
					int timeout)
{
	i = desc.begin();
	j = desc.end();
	for (;i!=j;++i)
		if (i->position == pos)
		{
			fd.fd = i->in;
			fd.events = POLLIN | POLLPRI;
			
			int res = poll(&fd,1,timeout);
			
			if (res > 0)
			{
				if ((POLLIN&fd.revents) == POLLIN || (POLLPRI&fd.revents) == POLLPRI)
					return true;
				else
					return false;
			}
			else
			{
				if (res == 0)
					return false;
				else	
					#ifndef NO_EX
						throw baseEx(ERRMODULE_FLUSHNBA,FLUSHNBA_ISWRITABLE,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
					#else			
						return false;
					#endif
			}			
		}
	
	return false;
}

//-------------------------------------------------------------------
