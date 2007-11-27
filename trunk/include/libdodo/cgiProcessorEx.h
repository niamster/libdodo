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
	/**
	 * libdodo defined errors
	 */
	enum cgiProcessorExR
	{
		CGIPREPROCESSOR_WRONGIFSTATEMENT,
		CGIPREPROCESSOR_WRONGFORSTATEMENT,
		CGIPREPROCESSOR_WRONGVARSTATEMENT,
		CGIPREPROCESSOR_DODOISRESERVEDVARNAME,
		CGIPREPROCESSOR_WRONGASSIGNSTATEMENT,
		CGIPREPROCESSOR_WRONGBLOCK
	};

	/**
	 * libdodo defined errors' explanation
	 */
		#define CGIPREPROCESSOR_WRONGIFSTATEMENT_STR         "Wrong `if` statement."
		#define CGIPREPROCESSOR_WRONGFORSTATEMENT_STR        "Wrong `for` statement."
		#define CGIPREPROCESSOR_WRONGVARSTATEMENT_STR        "In variable statement `}` wasn't closed."
		#define CGIPREPROCESSOR_DODOISRESERVEDVARNAME_STR    "dodo.* is reserved variable name."
		#define CGIPREPROCESSOR_WRONGASSIGNSTATEMENT_STR     "Wrong `assing` statement."
		#define CGIPREPROCESSOR_WRONGBLOCK_STR               "Wrong block construction."

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
		CGIPROCESSOR__ASSIGN
	};
};

#endif
