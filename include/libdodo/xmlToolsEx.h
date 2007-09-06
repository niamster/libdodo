/***************************************************************************
 *            cgiToolsEx.h
 *
 *  Wed Oct 5 16:25:14 2005
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

#ifndef _XMLTOOLSEX_H_
#define _XMLTOOLSEX_H_

#include <directives.h>

#include <baseEx.h>

#ifndef NO_EX

namespace dodo
{
	/**
	 * libdodo defined errors
	 */
	enum xmlToolsExR
	{
		XMLTOOLS_NOT_PARCED_BEFORE = 1,
	};

	/**
	 * libdodo defined errors' explanation
	 */
        #define XMLTOOLS_NOT_PARCED_BEFORE_STR    "The document haven't been parsed before."

	/**
	 * ID of function where exception was thrown
	 */
	enum xmlToolsFunctionsID
	{
		XMLTOOLS_PARCEFILE,
		XMLTOOLS_PARCE,
		XMLTOOLS_ERRHANDLER,
		XMLTOOLS_GETXMLBUFFERINFO,
		XMLTOOLS_PARCEBUFFER,
		XMLTOOLS_GETXMLFILEINFO,
		XMLTOOLS_REPARCE,
		XMLTOOLS_PARCEBUFFERINT,
		XMLTOOLS_PARCEFILEINT,

	};

};

#endif
#endif

