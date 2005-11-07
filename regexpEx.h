/***************************************************************************
 *            regexpEx.h
 *
 *  Wed Oct 5 16:25:14 2005
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

#ifndef _REGEXPEX_H_
#define _REGEXPEX_H_

#include "directives.h"
#include "baseEx.h"

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
		enum regexpExR
		{
			REGEXP_MEMORY_OVER = 1
		};
		
		class regexp;
		class regexpEx;
			
		typedef void (*regexpExResolver)(regexpEx*);
		
		class regexpEx : virtual public baseEx
		{
			public:
	
				virtual baseEx *getSelf();
				virtual int getExID();
	
				regexpEx(regexpExR reason, regexp *obj, unsigned long line, std::string file);
				virtual ~regexpEx();
				virtual void setResolve(regexpExR,regexpExResolver);///try to resolve by own function
				virtual void resolve();//try to resolve
			
				regexpExR reason;///reason
			
				regexp *obj;///where exception appeared	
			
			private:
				
				regexpExResolver resolvers[1];///functions - solvers
				
		};
	};

#endif
#endif /*REGEXPEX_H_*/
