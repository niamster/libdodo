/***************************************************************************
 *            cgiPreprocessorEx.h
 *
 * Sun Jan 22 19:05:57 2006
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

#ifndef _CGIPREPROCESSOREX_H_
#define _CGIPREPROCESSOREX_H_

#include <directives.h>

#include <baseEx.h>

#ifndef NO_EX

namespace dodo
{
	/**
	 * libdodo defined errors
	 */
	enum cgiPreprocessorExR
	{
		CGIPREPROCESSOR_NOTCLOSEDBRACKET,
	};

	/**
	 * libdodo defined errors' explanation
	 */
        #define CGIPREPROCESSOR_NOTCLOSEDBRACKET_STR    "Bracket was opened(closed) but not closed(opened)."

	/**
	 * ID of function where exception was thrown
	 */
	enum cgiPreprocessorFunctionsID
	{
		CGIPREPROCESSOR_PREPROCESS,
		CGIPREPROCESSOR_READ,
	};
};

#endif

#endif
