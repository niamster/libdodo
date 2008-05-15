/***************************************************************************
 *            cgiProcessorEx.h
 *
 * Tue Jan 24 19:05:57 2006
 *  Copyright  2005  Ni@m
 *  niam.niam@gmail.com
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License version 2.1 as published by
 *  the Free Software Foundation;
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef _CGIPROCESSOREX_H_
#define _CGIPROCESSOREX_H_

#include <libdodo/directives.h>

#include <libdodo/baseEx.h>

namespace dodo
{
	namespace cgi
	{
		/**
		 * libdodo defined errors
		 */
		enum processorExR
		{
			PROCESSOREX_WRONGIFSTATEMENT,
			PROCESSOREX_WRONGFORSTATEMENT,
			PROCESSOREX_WRONGVARSTATEMENT,
			PROCESSOREX_DODOISRESERVEDVARNAME,
			PROCESSOREX_WRONGASSIGNSTATEMENT,
			PROCESSOREX_WRONGBLOCK
		};
	
		/**
		 * explanations for libdodo defined errors
		 */
#define CGIPROCESSOREX_WRONGIFSTATEMENT_STR "Wrong `if` statement."
#define CGIPROCESSOREX_WRONGFORSTATEMENT_STR "Wrong `for` statement."
#define CGIPROCESSOREX_WRONGVARSTATEMENT_STR "In variable statement `}` wasn't closed."
#define CGIPROCESSOREX_DODOISRESERVEDVARNAME_STR "dodo.* is reserved variable name."
#define CGIPROCESSOREX_WRONGASSIGNSTATEMENT_STR "Wrong `assing` statement."
#define CGIPROCESSOREX_WRONGBLOCK_STR "Wrong block construction."
	
		/**
		 * IDs of functions where exception might be thrown
		 */
		enum processorFunctionsID
		{
			PROCESSOREX__IF,
			PROCESSOREX_BLOCKEND,
			PROCESSOREX__FOR,
			PROCESSOREX_GETVAR,
			PROCESSOREX_ASSIGN,
			PROCESSOREX__ASSIGN
		};
	};
};

#endif
