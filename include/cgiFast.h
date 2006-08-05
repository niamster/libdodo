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

	#include <fcgiapp.h>
	
	#include <types.h>
	
	namespace dodo
	{
		class cgiFast;
		
		/**
		 * @typedef that describes function that will be called on new cgi request
		 */
		typedef void (*cgiProc)(cgiFast *);
		
		/**
		 * @class cgiFast
		 * provides cpp wrapper for fast-cgi technology
		 */
		class cgiFast
		{
			friend class cgiTools;
			
			private:
				
				/**
				 * copy constructor
				 * to prevent copying
				 */
				cgiFast(cgiFast &cf);
			
			public:
				
				/**
				 * constructor
				 * @param threading indicates whether to call new thread on cgi request or proceed a queue
				 */
				cgiFast(bool threading = true);
				virtual ~cgiFast(); 
				
				/**
				 * sets function that would be called on CGI request
				 * @param func indicates what function to set
				 */
				virtual void setCGIFunction(cgiProc func);
				
				/**
				 * sends buf to specific output[fast-cgi safe]
				 * @param buf dscribes whet to send to user
				 */
				virtual void print(const std::string &buf);
				
				/**
				 * listen for incoming requests
				 */
				virtual void listen();
				
			private:
			
				bool threading;///< threading or not
				
				cgiProc cgiF;///< function to be called on new request
				
				FCGX_Request request;///< CGI request
		};
	};

#endif

#endif /*CGIFAST_H_*/
