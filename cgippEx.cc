/***************************************************************************
 *            cgippEx.cc
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

#include "cgippEx.h"


#ifndef NO_EX

	using namespace dodo;
	
	static void 
	dummyCgippResolve(cgippEx *a_ex)
	{
		
	}
	
	//-------------------------------------------------------------------
	
	cgippEx::cgippEx(cgippExR a_reason, 
						cgipp *a_obj,
						unsigned long a_line, 
						std::string a_file) : reason (a_reason),
						baseEx(a_line,a_file)
	{
		obj = a_obj;
		switch (reason)
		{
			case CGIPP_MEMORY_OVER:
				state = CRITICAL;
				break;			
		}
		resolvers[0] = &dummyCgippResolve;
	}
	
	//-------------------------------------------------------------------
	
	cgippEx::~cgippEx()
	{
	}
	//-------------------------------------------------------------------
	
	baseEx *
	cgippEx::getSelf()
	{
		return dynamic_cast<baseEx *>(this);
	}
	
	//-------------------------------------------------------------------
	
	int 
	cgippEx::getExID()
	{
		return CGIPP_EX;
	}	
	
	//-------------------------------------------------------------------
	
	/**
	 * specify type of exception and function to resolve it. u can combine reasons with '|'
	 * tries to resolve problem with own function
	 */
	void 
	cgippEx::setResolve(cgippExR a_reason, 
							cgippExResolver a_resF)
	{
		if (a_resF == NULL)
			a_resF = &dummyCgippResolve;
		if ((CGIPP_MEMORY_OVER & a_reason) == CGIPP_MEMORY_OVER)
			resolvers[0] = a_resF;
	}
	
	//-------------------------------------------------------------------
	
	void 
	cgippEx::resolve()
	{
		switch (reason)
		{
			case CGIPP_MEMORY_OVER:
				resolvers[0](this);
				break;	
		}
	}
	
	//-------------------------------------------------------------------

#endif
