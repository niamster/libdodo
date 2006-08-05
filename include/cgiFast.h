/***************************************************************************
 *            cgiFast.h
 *
 *  Sat Aug  5 03:31:19 2006
 *  Copyright  2006  Ni@m
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

#ifndef CGIFAST_H_
#define CGIFAST_H_

#include <directives.h>

#ifdef FCGI_EXT

	#include <types.h>
	
	namespace dodo
	{
		/**
		 * @typedef that describes function that will be called on new cgi request
		 */
		typedef void (*cgiProc)();
		
		/**
		 * @class cgiFast
		 * provides cpp wrapper for fast-cgi technology
		 */
		class cgiFast
		{
			public:
				
				/**
				 * constructor
				 * @param threading indicates whether to call new thread on cgi request or proceed a queue
				 */
				cgiFast(bool threading = true);
				virtual ~cgiFast(); 
				
			private:
			
				bool threading;///< threading or not
				
				cgiProc cgiF;///< function to be called on new request
		};
	};

#endif

#endif /*CGIFAST_H_*/
