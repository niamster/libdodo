/***************************************************************************
 *            mysqlinterfaceEx.h
 *
 *  Thu Jul  7 00:25:19 2005
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
 
#ifndef _MYSQLINTERFACEEX_H_
#define _MYSQLINTERFACEEX_H_

#include "derectives.h"
#include "baseEx.h"
#include "mysqlinterface.h"

#ifndef NO_EX

	namespace dodo
	{
		/**
		* class to catch and resolve exceptions in flush
		* by default it does nothing
		* but u can define functions for resolving
		* for example to resolve problem with NOT_ENOUGH_FREE_SPACE:
		*		void someF(stringEx *strEx)
		*		{
		*			///do smth
		*		}
		*		setResolve(NOT_ENOUGH_FREE_SPACE, someF);
		*	u also can combine reasons with '|'
		*   if u want to reset to default use NULL;
		*/	
		
		///reasons of exception
		enum mysqlppExR
		{
			MYSQL_CONNECT = 2,
			MYSQL_QUERY = 4,
			MYSQL_STORE = 8,
			MYSQLPPEXR_MEMORY_OVER = 16,
		};
		
		class mysqlpp;
		class mysqlppEx;
			
		typedef void (*mysqlppExResolver)(mysqlppEx*);
		
		class mysqlppEx : virtual public baseEx
		{
			public:
	
				mysqlppEx(mysqlppExR, mysqlpp *);
				virtual ~mysqlppEx();
				virtual void setResolve(mysqlppExR,mysqlppExResolver);///try to resolve by own function
				virtual void resolve();//try to resolve
				
				virtual mysqlppExR getReason() const;///returns reason
				virtual baseExStat getState() const;///returns state
				virtual mysqlpp* getObj();///returns class where exception appeared
			
			private:
				mysqlppExResolver resolvers[4];///functions - solvers
				mysqlppExR reason;///reason
				baseExStat state;///state of problem
				mysqlpp *obj;///flush where exception appeared				
		};
	};

#endif

#endif /* _MYSQLINTERFACEEX_H */
