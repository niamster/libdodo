/***************************************************************************
 *            sqlBaseEx.cc
 *
 *  Mon Jul 18 20:32:42 2005
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
 
#include "sqlBaseEx.h"
 
#ifndef NO_EX

	using namespace dodo;

	static void 
	dummySqlBaseExResolve(sqlBaseEx *a_ex)
	{
		
	}
	
	//-------------------------------------------------------------------
		
	sqlBaseEx::sqlBaseEx(sqlBaseExR a_reason, 
						sqlBase *a_obj,
						unsigned long a_line, 
						std::string a_file) : reason (a_reason),
						baseEx(a_line,a_file)
	{
		obj = a_obj;
		resolvers[0] = &dummySqlBaseExResolve;
		resolvers[1] = &dummySqlBaseExResolve;
	}
	
	//-------------------------------------------------------------------
	
	sqlBaseEx::~sqlBaseEx()
	{
	}
	
	//-------------------------------------------------------------------
		
	baseEx *
	sqlBaseEx::getSelf()
	{
		return dynamic_cast<baseEx *>(this);
	}
	
	//-------------------------------------------------------------------
	
	int 
	sqlBaseEx::getExID()
	{
		return SQLBASE_EX;
	}
	
	//-------------------------------------------------------------------
			
	/**
	 * specify type of exception and function to resolve it. u can combine reasons with '|'
	 * tries to resolve problem with own function
	 */
	void 
	sqlBaseEx::setResolve(sqlBaseExR a_reason, 
							sqlBaseExResolver a_resF)
	{
		if (a_resF == NULL)
			a_resF = &dummySqlBaseExResolve;
		if ((SQLBASE_MEMORY_OVER & a_reason) == SQLBASE_MEMORY_OVER)
			resolvers[0] = a_resF;
		if ((SQLBASE_MEMORY_OVER & a_reason) == SQLBASE_MEMORY_OVER)
			resolvers[0] = a_resF;
	}
	
	//-------------------------------------------------------------------
		
	void 
	sqlBaseEx::resolve()
	{
		switch (reason)
		{
			case SQLBASE_MEMORY_OVER:
				resolvers[0](this);
				break;
			case SQLBASE_EMPTY_REQUEST:
				resolvers[1](this);
				break;
		}
	}
	
	//-------------------------------------------------------------------
	
#endif
