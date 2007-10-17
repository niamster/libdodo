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

#include "directives.h"
#include "flushSTDEx.h"
#include "types.h"
#include "tools.h"
#include "flush.h"

namespace dodo
{
	
	enum flushSTDOperationTypeEnum///for xExec
	{
		FLUSHSTD_OPER_READ,
		FLUSHSTD_OPER_WRITE,
		FLUSHSTD_OPER_OPEN,
		FLUSHSTD_OPER_CLOSE
	};
	
	/**
	 * 
	 */
	
	class flushSTD : public flush
	{
		private:
		
			flushSTD(flushSTD &fd);///to prevent from copyin'
		
		public:
					
			/**
			 * return self, casted to base class - dodoBase; usefull to cast from child to parent;
			 */		
			virtual dodoBase * const getSelf();
							
			///constructors and destructors
			flushSTD();
			virtual ~flushSTD();

			/**
			 * for xExec
			 */			
			virtual int addPostExec(inExec func, void *data) const;
			virtual int addPreExec(inExec func, void *data) const;	
			
			/**
			 * read functions. first argument - buffer, second - position
			 * returns false if nothing was read
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							readString(std::string &data) const;///reads to string; return false if eof
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							read(char * const data) const;///reads to void*; return false if eof		
			
			/**
			 * write functions
			 * first argument - buffer, second - position(if u want to add to end of the file set 'append' to true)
			 * returns false if exists, copy to buffer content of the node
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							writeString(const std::string &data);///writes string
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							write(const char * const data);///writes void*

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
			 * sometimes, when you ouput/input from some pther programs, you have bounds in input/output buffer
			 * this parameters will help you by default, they are too large, so you don't have to change it
			 */				 
			int inSTDBuffer;
			int outSTDBuffer;
	};

};

#endif
