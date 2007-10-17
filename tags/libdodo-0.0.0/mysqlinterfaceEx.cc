/***************************************************************************
 *            mysqlinterfaceEx.cc
 *
 *  Thu Jul  7 00:35:15 2005
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

#include "mysqlinterfaceEx.h"

#ifndef NO_EX

	using namespace dodo;
	
	static void 
	simpleMysqlppResolve(mysqlppEx *a_ex)
	{
		
	}
	
	mysqlppEx::mysqlppEx(mysqlppExR a_reason, 
						mysqlpp *a_obj) : reason (a_reason),
											obj(a_obj)
	{
		switch (reason)
		{
			case MYSQL_CONNECT:
			case MYSQL_STORE:
				state = CRITICAL;
				break;			
		}
		resolvers[0] = &simpleMysqlppResolve;
		resolvers[1] = &simpleMysqlppResolve;
		resolvers[2] = &simpleMysqlppResolve;
		resolvers[3] = &simpleMysqlppResolve;
	}
	
	mysqlppEx::~mysqlppEx()
	{
	}
	
	/**
	* specify type of exception and function to resolve it. u can combine reasons with '|'
	* tries to resolve problem with own function
	*/
	void 
	mysqlppEx::setResolve(mysqlppExR a_reason, 
							mysqlppExResolver a_resF)
	{
		if (a_resF == NULL)
			a_resF = &simpleMysqlppResolve;	
		if ((MYSQL_CONNECT & a_reason) == MYSQL_CONNECT)
			resolvers[0] = a_resF;
		if ((MYSQL_QUERY & a_reason) == MYSQL_QUERY)
			resolvers[1] = a_resF;
		if ((MYSQL_STORE & a_reason) == MYSQL_STORE)
			resolvers[2] = a_resF;
		if ((MYSQLPPEXR_MEMORY_OVER & a_reason) == MYSQLPPEXR_MEMORY_OVER)
			resolvers[3] = a_resF;
	}
	
	mysqlppExR 
	mysqlppEx::getReason() const
	{
		return reason;
	}
	
	baseEx::baseExStat 
	mysqlppEx::getState() const
	{
		return state;
	}
	
	mysqlpp* 
	mysqlppEx::getObj()
	{
		return obj;
	}
	
	void 
	mysqlppEx::resolve()
	{
		switch (reason)
		{
			case MYSQL_CONNECT:
				resolvers[0](this);
				break;
			case MYSQL_QUERY:
				resolvers[1](this);
				break;			
			case MYSQL_STORE:
				resolvers[2](this);
				break;			
			case MYSQLPPEXR_MEMORY_OVER:
				resolvers[3](this);
				break;	
		}
	}

#endif
