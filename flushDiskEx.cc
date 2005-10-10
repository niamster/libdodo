/***************************************************************************
 *            flushEx.cc
 *
 *  Mon Feb 21 03:11:23 2005
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
 
#include "flushDiskEx.h"

#ifndef NO_EX
	
	using namespace dodo;
	
	static void 
	dummyFlushDiskResolve(flushDiskEx *a_ex)
	{
		
	}
	
	//-------------------------------------------------------------------
	
	flushDiskEx::flushDiskEx(flushDiskExR a_reason, 
						flushDisk *a_obj,
						unsigned long a_line, 
						std::string a_file) : reason (a_reason),
						baseEx(a_line,a_file)
	{
		obj = a_obj;
		switch (reason)
		{
			case FLUSHDISK_IOERROR:
			case FLUSHDISK_NOT_ENOUGH_FREE_SPACE:
			case FLUSHDISK_MEMORY_OVER:
				state = CRITICAL;
				break;			
		}
		resolvers[0] = &dummyFlushDiskResolve;
		resolvers[1] = &dummyFlushDiskResolve;
		resolvers[2] = &dummyFlushDiskResolve;
		resolvers[3] = &dummyFlushDiskResolve;
		resolvers[4] = &dummyFlushDiskResolve;
		resolvers[5] = &dummyFlushDiskResolve;
		resolvers[6] = &dummyFlushDiskResolve;
		resolvers[7] = &dummyFlushDiskResolve;
		resolvers[8] = &dummyFlushDiskResolve;
		resolvers[9] = &dummyFlushDiskResolve;
		resolvers[10] = &dummyFlushDiskResolve;
	}
	
	//-------------------------------------------------------------------
	
	flushDiskEx::~flushDiskEx()
	{
	}
	//-------------------------------------------------------------------
	
	baseEx *
	flushDiskEx::getSelf()
	{
		return dynamic_cast<baseEx *>(this);
	}
	
	//-------------------------------------------------------------------
	
	int 
	flushDiskEx::getExID()
	{
		return FLUSHDISK_EX;
	}	
	
	//-------------------------------------------------------------------
	
	/**
	 * specify type of exception and function to resolve it. u can combine reasons with '|'
	 * tries to resolve problem with own function
	 */
	void 
	flushDiskEx::setResolve(flushDiskExR a_reason, 
							flushDiskExResolver a_resF)
	{
		if (a_resF == NULL)
			a_resF = &dummyFlushDiskResolve;
		if ((FLUSHDISK_NOT_ENOUGH_FREE_SPACE & a_reason) == FLUSHDISK_NOT_ENOUGH_FREE_SPACE)
			resolvers[0] = a_resF;
		if ((FLUSHDISK_ACCESS_DENIED & a_reason) == FLUSHDISK_ACCESS_DENIED)
			resolvers[1] = a_resF;
		if ((FLUSHDISK_FILE_TOO_BIG & a_reason) == FLUSHDISK_FILE_TOO_BIG)
			resolvers[2] = a_resF;
		if ((FLUSHDISK_INTERRUPTED & a_reason) == FLUSHDISK_INTERRUPTED)
			resolvers[3] = a_resF;
		if ((FLUSHDISK_IOERROR & a_reason) == FLUSHDISK_IOERROR)
			resolvers[4] = a_resF;
		if ((FLUSHDISK_TOO_MANY_OPEN_FILE & a_reason) == FLUSHDISK_TOO_MANY_OPEN_FILE)
			resolvers[5] = a_resF;
		if ((FLUSHDISK_FILENAME_TOO_LONG & a_reason) == FLUSHDISK_FILENAME_TOO_LONG)
			resolvers[6] = a_resF;
		if ((FLUSHDISK_NO_SUCH_FILE & a_reason) == FLUSHDISK_NO_SUCH_FILE)
			resolvers[7] = a_resF;
		if ((FLUSHDISK_READ_ONLY_FS & a_reason) == FLUSHDISK_READ_ONLY_FS)
			resolvers[8] = a_resF;
		if ((FLUSHDISK_WRONG_FILE_NAME & a_reason) == FLUSHDISK_WRONG_FILE_NAME)
			resolvers[9] = a_resF;
		if ((FLUSHDISK_MEMORY_OVER & a_reason) == FLUSHDISK_MEMORY_OVER)
			resolvers[10] = a_resF;
	}
	
	//-------------------------------------------------------------------
	
	void 
	flushDiskEx::resolve()
	{
		switch (reason)
		{
			case FLUSHDISK_NOT_ENOUGH_FREE_SPACE:
				resolvers[0](this);
				break;
			case FLUSHDISK_ACCESS_DENIED:
				resolvers[1](this);
				break;			
			case FLUSHDISK_FILE_TOO_BIG:
				resolvers[2](this);
				break;	
			case FLUSHDISK_INTERRUPTED:
				resolvers[3](this);
				break;
			case FLUSHDISK_IOERROR:
				resolvers[4](this);
				break;			
			case FLUSHDISK_TOO_MANY_OPEN_FILE:
				resolvers[5](this);
				break;	
			case FLUSHDISK_FILENAME_TOO_LONG:
				resolvers[6](this);
				break;
			case FLUSHDISK_NO_SUCH_FILE:
				resolvers[7](this);
				break;			
			case FLUSHDISK_READ_ONLY_FS:
				resolvers[8](this);
				break;	
			case FLUSHDISK_WRONG_FILE_NAME:
				resolvers[9](this);
				break;
			case FLUSHDISK_MEMORY_OVER:
				resolvers[10](this);
				break;		
		}
	}
	
	//-------------------------------------------------------------------	
#endif
