/***************************************************************************
 *            flushSocketEx.cc
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
 
#include "flushSocketEx.h"

#ifndef NO_EX
	
	using namespace dodo;
	
	static void 
	dummyFlushDiskResolve(flushSocketEx *a_ex)
	{
		
	}
	
	//-------------------------------------------------------------------
	
	flushSocketEx::flushSocketEx(flushSocketExR a_reason, 
						flushSocket *a_obj,
						unsigned long a_line, 
						std::string a_file) : reason (a_reason),
						baseEx(a_line,a_file)
	{
		obj = a_obj;
		switch (reason)
		{
			case FLUSHSOCKET_ACCESS_DENIED:
			case FLUSHSOCKET_UNKNOWN_PROTOCOL:
			case FLUSHSOCKET_MEMORY_OVER:
			case FLUSHSOCKET_TOO_MANY_OPEN_FILE:
				state = CRITICAL;
				break;			
		}
		resolvers[0] = &dummyFlushDiskResolve;
		resolvers[1] = &dummyFlushDiskResolve;
		resolvers[2] = &dummyFlushDiskResolve;
		resolvers[3] = &dummyFlushDiskResolve;
		resolvers[4] = &dummyFlushDiskResolve;
		resolvers[5] = &dummyFlushDiskResolve;
	}
	
	//-------------------------------------------------------------------
	
	flushSocketEx::~flushSocketEx()
	{
	}
	//-------------------------------------------------------------------
	
	baseEx *
	flushSocketEx::getSelf()
	{
		return dynamic_cast<baseEx *>(this);
	}
	
	//-------------------------------------------------------------------
	
	int 
	flushSocketEx::getExID()
	{
		return FLUSHSOCKET_EX;
	}	
	
	//-------------------------------------------------------------------
	
	/**
	 * specify type of exception and function to resolve it. u can combine reasons with '|'
	 * tries to resolve problem with own function
	 */
	void 
	flushSocketEx::setResolve(flushSocketExR a_reason, 
							flushSocketExResolver a_resF)
	{
		if (a_resF == NULL)
			a_resF = &dummyFlushDiskResolve;
		if ((FLUSHSOCKET_NOT_SUPPORTED_ADDR_FAMILY & a_reason) == FLUSHSOCKET_NOT_SUPPORTED_ADDR_FAMILY)
			resolvers[0] = a_resF;
		if ((FLUSHSOCKET_ACCESS_DENIED & a_reason) == FLUSHSOCKET_ACCESS_DENIED)
			resolvers[1] = a_resF;
		if ((FLUSHSOCKET_UNKNOWN_PROTOCOL & a_reason) == FLUSHSOCKET_UNKNOWN_PROTOCOL)
			resolvers[2] = a_resF;
		if ((FLUSHSOCKET_TOO_MANY_OPEN_FILE & a_reason) == FLUSHSOCKET_TOO_MANY_OPEN_FILE)
			resolvers[3] = a_resF;
		if ((FLUSHSOCKET_PROTO_NOT_SUPPORTED_WITHIN_DOMAIN & a_reason) == FLUSHSOCKET_PROTO_NOT_SUPPORTED_WITHIN_DOMAIN)
			resolvers[4] = a_resF;
		if ((FLUSHSOCKET_MEMORY_OVER & a_reason) == FLUSHSOCKET_MEMORY_OVER)
			resolvers[5] = a_resF;
	}
	
	//-------------------------------------------------------------------
	
	void 
	flushSocketEx::resolve()
	{
		switch (reason)
		{
			case FLUSHSOCKET_NOT_SUPPORTED_ADDR_FAMILY:
				resolvers[0](this);
				break;
			case FLUSHSOCKET_ACCESS_DENIED:
				resolvers[1](this);
				break;			
			case FLUSHSOCKET_UNKNOWN_PROTOCOL:
				resolvers[2](this);
				break;	
			case FLUSHSOCKET_TOO_MANY_OPEN_FILE:
				resolvers[3](this);
				break;
			case FLUSHSOCKET_PROTO_NOT_SUPPORTED_WITHIN_DOMAIN:
				resolvers[4](this);
				break;			
			case FLUSHSOCKET_MEMORY_OVER:
				resolvers[5](this);
				break;
		}
	}
	
	//-------------------------------------------------------------------	
#endif
