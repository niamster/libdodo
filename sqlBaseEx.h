/***************************************************************************
 *            sqlBaseEx.h
 *
 *  Mon Jul 18 20:25:14 2005
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
 
#ifndef _SQLBASEEX_H_
#define _SQLBASEEX_H_

#include "directives.h"
#include "baseEx.h"
#include <vector>
#include <string>

#ifndef NO_EX

	namespace dodo
	{
		
		/**
		 * class to catch and resolve exceptions in string
		 * by default it does nothing
		 * but u can define functions for resolving
		 * for example to resolve problem with MEMORY_OVER:
		 *		void someF(stringEx *strEx)
		 *		{
		 *			///do smth
		 *		}
		 *		setResolve(MEMORY_OVER, someF);
		 *	u also can combine reasons with '|'
		 *   if u want to reset to default use NULL;
		 */
		
		///reasons of exception
		enum sqlBaseExR
		{
			SQLBASE_MEMORY_OVER = 2,///not enough memmory
			SQLBASE_EMPTY_REQUEST
		};
		
		class sqlBase;
		class sqlBaseEx;
		
		typedef void (*sqlBaseExResolver)(sqlBaseEx*);
		
		class sqlBaseEx : virtual public baseEx
		{
			public:
			
				virtual baseEx *getSelf();
				virtual int getExID();
				
				sqlBaseEx(sqlBaseExR reason, sqlBase *obj, unsigned long line, std::string file);
				virtual ~sqlBaseEx();
				virtual void setResolve(sqlBaseExR, sqlBaseExResolver);///try to resolve by own function
				virtual void resolve();//try to resolve
			
				sqlBaseExR reason;///reason
			
				sqlBase *obj;///string where exception appeared	
			
			private:
				
				sqlBaseExResolver resolvers[2];///functions - solvers	
		};
	};

#endif	

#endif /* _SQLBASEEX_H */
