/***************************************************************************
 *            regexpEx.cc
 *
 *  Wed Oct 5 16:25:15 2005
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

#include "regexpEx.h"


#ifndef NO_EX

	using namespace dodo;
	
	static void 
	dummyRegexpResolve(regexpEx *a_ex)
	{
		
	}
	
	//-------------------------------------------------------------------
	
	regexpEx::regexpEx(regexpExR a_reason, 
						regexp *a_obj,
						unsigned long a_line, 
						std::string a_file) : reason (a_reason),
						baseEx(a_line,a_file)
	{
		obj = a_obj;
		switch (reason)
		{
			case REGEXP_MEMORY_OVER:
				state = CRITICAL;
				break;			
		}
		resolvers[0] = &dummyRegexpResolve;
	}
	
	//-------------------------------------------------------------------
	
	regexpEx::~regexpEx()
	{
	}
	//-------------------------------------------------------------------
	
	baseEx *
	regexpEx::getSelf()
	{
		return dynamic_cast<baseEx *>(this);
	}
	
	//-------------------------------------------------------------------
	
	int 
	regexpEx::getExID()
	{
		return REGEXP_EX;
	}	
	
	//-------------------------------------------------------------------
	
	/**
	 * specify type of exception and function to resolve it. u can combine reasons with '|'
	 * tries to resolve problem with own function
	 */
	void 
	regexpEx::setResolve(regexpExR a_reason, 
							regexpExResolver a_resF)
	{
		if (a_resF == NULL)
			a_resF = &dummyRegexpResolve;
		if ((REGEXP_MEMORY_OVER & a_reason) == REGEXP_MEMORY_OVER)
			resolvers[0] = a_resF;
	}
	
	//-------------------------------------------------------------------
	
	void 
	regexpEx::resolve()
	{
		switch (reason)
		{
			case REGEXP_MEMORY_OVER:
				resolvers[0](this);
				break;	
		}
	}
	
	//-------------------------------------------------------------------

#endif
