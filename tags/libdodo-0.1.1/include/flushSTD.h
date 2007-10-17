/***************************************************************************
 *            flushSTD.h
 *
 *  Tue Nov 15 21:19:57 2005
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
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#ifndef _FLUSHSTD_H_
#define _FLUSHSTD_H_

#include <directives.h>

#include <flushSTDEx.h>
#include <types.h>
#include <tools.h>
#include <flush.h>
#include <flushSocket.h>

namespace dodo
{
	/**
	 * @enum flushSTDOperationTypeEnum describes type of operation for hook
	 */
	enum flushSTDOperationTypeEnum
	{
		FLUSHSTD_OPER_READ,
		FLUSHSTD_OPER_WRITE,
		FLUSHSTD_OPER_OPEN,
		FLUSHSTD_OPER_CLOSE
	};
	
	/**
	 * @class flushSTD performs actions with stdin/out.
	 * @note it's usefull when you are using in/out operations through some proxy -> for example inetd!
	 */
	
	class flushSTD : public flush	
	
	#ifndef FLUSH_STD_WO_XEXEC
								, public xexec
	#endif
	
	
	{
		private:
			
			/**
			 * constructor
			 * to prevent from copying
			 */
			flushSTD(flushSTD &fd);
		
		public:
							
			/**
			 * constructor
			 */
			flushSTD();
			
			/**
			 * destructor
			 */
			virtual ~flushSTD();

			/**
			 * @return info about source of inputting
			 * @note it can be used to get info foreign `inputter` if you ar usin'g inetd
			 */
			static __connInfo inputterInfo();

			#ifndef FLUSH_STD_WO_XEXEC

				/**
				 * adds hook after the operation by callback
				 * @return number in list where function is set
				 * @param func is a pointer to function
				 * @param data is pointer to data toy want to pass to hook
				 */			
				virtual int addPostExec(inExec func, void *data) const;
				
				/**
				 * adds hook before the operation by callback
				 * @return number in list where function is set
				 * @param func is a pointer to function
				 * @param data is pointer to data toy want to pass to hook
				 */
				virtual int addPreExec(inExec func, void *data) const;
		
				#ifdef DL_EXT
				
					/**
					 * adds hook after the operation by callback
					 * @return number in list where function is set
					 * @param module is a path to module, whrere hook exists
					 * @param data is pointer to data toy want to pass to hook
					 */
					virtual int addPostExec(const std::string &module, void *data) const;
					
					/**
					 * adds hook after the operation by callback
					 * @return number in list where function is set
					 * @param module is a path to module, whrere hook exists
					 * @param data is pointer to data toy want to pass to hook
					 */
					virtual int addPreExec(const std::string &module, void *data) const;
					
					/**
					 * set function from module that will be executed before/after the main action call
					 * the type of hook[pre/post] is defined in module
					 * @return number in list where function is set
					 * @param func is a pointer to function
					 * @param data is pointer to data toy want to pass to hook
					 */			
					virtual xexecCounts addExec(const std::string &module, void *data) const;
				
				#endif
			
			#endif
							
			/**
			 * read
			 * @param data is filled with read string
			 * if inSize bigger than buffer size - reads with few iterations
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							readString(std::string &data) const;
			/**
			 * read
			 * @param data is filled with read data
			 * if inSize bigger than buffer size - reads with few iterations
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							read(char * const data) const;
			
			/**
			 * write
			 * @param data is string that will be written
			 * if outSize bigger than buffer size - writes with few iterations
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							writeString(const std::string &data);
							
			/**
			 * write
			 * @param data is data that will be written
			 * if outSize bigger than buffer size - writes with few iterations
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							write(const char * const data);

			/**
			 * flushes to output
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							flush();		
			
			/**
			 * sometimes, when you ouput/input from some other programs, you have bounds in input/output buffer
			 * this parameters will help you;
			 * by default, they are too large, so you don't have to change it
			 */				 
			int inSTDBuffer;///< input buffer
			int outSTDBuffer;///< output buffer
			
			bool err;///< redirect output to stderr; false by default
		
		protected: 
		
			FILE *desc;///< descriptor that is needed for redirection
		
		private:
			
			/**
			 * @note share vars
			 */
			mutable long iter;///< amount of iterations to do to operate with data
			mutable long rest;///< amount of data that is needed to operate at last
		
	};

};

#endif
