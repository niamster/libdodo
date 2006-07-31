/***************************************************************************
 *            cgiProcessorEx.h
 *
 * Tue Jan 24 19:05:57 2006
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
 
#ifndef _CGIPROCESSOREX_H_
#define _CGIPROCESSOREX_H_

#include <directives.h>

#include <baseEx.h>

#ifndef NO_EX

	namespace dodo
	{	
		/**
		 * libdodo defined errors
		 */
		enum cgiProcessorExR
		{
			CGIPREPROCESSOR_WRONGIFSTATEMENT,
			CGIPREPROCESSOR_WRONGFORSTATEMENT,
			CGIPREPROCESSOR_WRONGVARSTATEMENT,
			CGIPREPROCESSOR_DODOISRESERVEDVARNAME
		};
		
		/**
		 * libdodo defined errors' explanation
		 */				
		#define CGIPREPROCESSOR_WRONGIFSTATEMENT_STR "Wrong `if` statemenet."
		#define CGIPREPROCESSOR_WRONGFORSTATEMENT_STR "Wrong `for` statemenet."		
		#define CGIPREPROCESSOR_WRONGVARSTATEMENT_STR "In variable statement `}` wasn't closed."
		#define CGIPREPROCESSOR_DODOISRESERVEDVARNAME_STR "dodo.* is reserved variable name." 
		
		/**
		 * ID of function where exception was thrown
		 */
		enum cgiProcessorFunctionsID
		{
			CGIPROCESSOR__IF,
			CGIPROCESSOR_BLOCKEND,
			CGIPROCESSOR__FOR,
			CGIPROCESSOR_GETVAR,
			CGIPROCESSOR_ASSIGN,
		};
	};

#endif	
	
#endif
