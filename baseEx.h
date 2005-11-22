/***************************************************************************
 *            baseEx.h
 *
 *  Mon Feb  7 20:02:06 2005
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
 
#ifndef _BASEEX_H_
#define _BASEEX_H_

#include "directives.h"
#include <string>

#ifndef NO_EX

	namespace dodo
	{	
		/**
		 * WHERE number of error was got.
		 */
		enum errnoSourceEnum
		{
			ERR_LIBDODO,
			ERR_ERRNO,
			ERR_MYSQL,
			ERR_H_ERRNO,
			ERR_WINSOCK,
			ERR_DYNLOAD,
			ERR_WINLOAD
		};

		/**
		 * if source is ERR_LIBDODO, u can match errno usin' errorModuleEnum. see *Ex headers for err numbers
		 */
		enum errorModuleEnum
		{
			ERRMODULE_FLUSHSOCKET,
			ERRMODULE_CGIPP,
			ERRMODULE_FLUSHDISK,
			ERRMODULE_MYSQLPP,
			ERRMODULE_REGEXP,
			ERRMODULE_SQLBASE,
			ERRMODULE_FLUSHSTD,
			ERRMODULE_XEXEC,
			ERRMODULE_SYSTEMTOOLS
		};	
			
		class baseEx
		{
			public:		
			
				enum baseExStat///basic stats in exception
				{
					UNKNOWN = -10,
					SAFE = 0,
					UNSAFE = 10,
					CRITICAL = 100
				};

				baseEx(unsigned long errModule, unsigned long functionID, unsigned long errnoSource, unsigned long baseErrno, std::string baseErrstr, unsigned long line, std::string file);
				
				/**
				 * returns string str explanation
				 * 	catch(baseEx ex)
				 *	{
				 *		cout << ex << endl;
				 *	}
				 */
				operator const char *();
			
				unsigned long errModule;
			
				unsigned long funcID;
				unsigned long errnoSource;			
			
				unsigned long baseErrno;
				std::string baseErrstr;
				
				unsigned long line;///line, where problem detected
				std::string file;
				baseExStat state;///state of problem
		};
	};

#endif	
	
#endif
