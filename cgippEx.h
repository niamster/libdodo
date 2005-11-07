/***************************************************************************
 *            cgippEx.h
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

#ifndef _CGIPPEX_H_
#define _CGIPPEX_H_

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
		enum cgippExR
		{
			CGIPP_MEMORY_OVER = 1
		};
		
		class cgipp;
		class cgippEx;
			
		typedef void (*cgippExResolver)(cgippEx*);
		
		class cgippEx : virtual public baseEx
		{
			public:
	
				virtual baseEx *getSelf();
				virtual int getExID();
	
				cgippEx(cgippExR reason, cgipp *obj, unsigned long line, std::string file);
				virtual ~cgippEx();
				virtual void setResolve(cgippExR,cgippExResolver);///try to resolve by own function
				virtual void resolve();//try to resolve
			
				cgippExR reason;///reason
			
				cgipp *obj;///where exception appeared	
			
			private:
				
				cgippExResolver resolvers[1];///functions - solvers
				
		};
	};

#endif
#endif /*CGIPPEX_H_*/
