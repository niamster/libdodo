/***************************************************************************
 *            flushEx.h
 *
 *  Mon Feb 21 03:03:47 2005
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
 
#ifndef _FLUSHEX_H_
#define _FLUSHEX_H_

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
		enum flushDiskExR
		{
			FLUSHDISK_NOT_ENOUGH_FREE_SPACE = 2,
			FLUSHDISK_ACCESS_DENIED = 4,
			FLUSHDISK_FILE_TOO_BIG = 8,
			FLUSHDISK_INTERRUPTED = 16,
			FLUSHDISK_IOERROR = 32,
			FLUSHDISK_TOO_MANY_OPEN_FILE = 64,
			FLUSHDISK_FILENAME_TOO_LONG = 128,
			FLUSHDISK_NO_SUCH_FILE = 256,
			FLUSHDISK_READ_ONLY_FS = 512,
			FLUSHDISK_WRONG_FILE_NAME = 1024,
			FLUSHDISK_MEMORY_OVER = 2046,
		};
		
		class flushDisk;
		class flushDiskEx;
			
		typedef void (*flushDiskExResolver)(flushDiskEx*);
		
		class flushDiskEx :virtual public baseEx
		{
			public:
	
				virtual baseEx *getSelf();
				virtual int getExID();
	
				flushDiskEx(flushDiskExR reason, flushDisk *obj, unsigned long line, std::string file);
				virtual ~flushDiskEx();
				virtual void setResolve(flushDiskExR,flushDiskExResolver);///try to resolve by own function
				virtual void resolve();//try to resolve
			
				flushDiskExR reason;///reason
			
				flushDisk *obj;///where exception appeared	
			
			private:
				
				flushDiskExResolver resolvers[11];///functions - solvers			
		};
	};

#endif	
	
#endif
