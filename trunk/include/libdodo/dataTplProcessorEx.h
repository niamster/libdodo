/***************************************************************************
 *            dataTplProcessorEx.h
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#ifndef _DATATPLPROCESSOREX_H_
#define _DATATPLPROCESSOREX_H_ 1

#include <libdodo/directives.h>

#include <libdodo/exceptionBasic.h>

namespace dodo
{
	namespace data
	{
		namespace tpl
		{
			/**
			 * libdodo defined errors
			 */
			enum processorExR
			{
				PROCESSOREX_WRONGFORSTATEMENT,
				PROCESSOREX_WRONGVARSTATEMENT,
				PROCESSOREX_DODOISRESERVEDVARNAME,
				PROCESSOREX_WRONGASSIGNSTATEMENT,
				PROCESSOREX_WRONGBLOCK,
				PROCESSOREX_NOTCLOSEDBRACKET,
			};

			/**
			 * explanations for libdodo defined errors
			 */
#define DATATPLPROCESSOREX_WRONGFORSTATEMENT_STR     "Wrong `for` statement."
#define DATATPLPROCESSOREX_WRONGVARSTATEMENT_STR     "In variable statement `}` wasn't closed."
#define DATATPLPROCESSOREX_DODOISRESERVEDVARNAME_STR "dodo.* is reserved variable name."
#define DATATPLPROCESSOREX_WRONGASSIGNSTATEMENT_STR  "Wrong `assing` statement."
#define DATATPLPROCESSOREX_WRONGBLOCK_STR            "Wrong block construction."
#define DATATPLPROCESSOREX_NOTCLOSEDBRACKET_STR "Bracket was opened(closed) but not closed(opened)."

			/**
			 * IDs of functions where exception might be thrown
			 */
			enum processorFunctionsID
			{
				PROCESSOREX__PREPROCESSSTRING__,
				PROCESSOREX_BLOCKEND,
				PROCESSOREX__FOR__,
				PROCESSOREX_GETVAR,
				PROCESSOREX_GETVARNAME,
				PROCESSOREX_ASSIGN,
				PROCESSOREX__ASSIGN__,
			};
		};
	};
};

#endif
