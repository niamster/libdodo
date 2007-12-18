/***************************************************************************
 *            jsonEx.h
 *
 *  Mon Oct 20 02:15:55 2007
 *  Copyright  2007  Ni@m
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

#ifndef _JSONEX_H_
#define _JSONEX_H_

#include <libdodo/directives.h>

#include <libdodo/baseEx.h>

namespace dodo
{
	/**
	 * next for json
	 */

	/**
	 * libdodo defined errors
	 */
	enum imageFunctionsExR
	{
		JSONNODEEX_WRONGTYPEREQUESTED,
		JSONNODEEX_ARRAYOUTOFRANGE,
		JSONNODEEX_MALFORMEDJSON,
		JSONEX_MALFORMEDJSON,
	};
	
	/**
	 * libdodo defined errors' explanation
	 */
	#define JSONNODEEX_WRONGTYPEREQUESTED_STR "Wrong type was requested. Use proper get* method."
	#define JSONNODEEX_ARRAYOUTOFRANGE_STR "Array key is out of range."
	#define JSONNODEEX_MALFORMEDJSON_STR "Malformed json object."
	#define JSONEX_MALFORMEDJSON_STR "Malformed json object."

	/**
	 * ID of function where exception was thrown
	 */
	enum jsonFunctionsID
	{
		JSONNODEEX_BROPERATORSTRING,
		JSONNODEEX_BROPERATORNUMERIC,
		JSONNODEEX_GETSTRING,
		JSONNODEEX_GETBOOLEAN,
		JSONNODEEX_NUMERIC,
		JSONNODEEX_GETARRAY,
		JSONNODEEX_GETOBJECT,
		JSONEX_PROCESSOBJECT,
		JSONEX_PROCESSARRAY,
		JSONEX_PROCESSBOOLEAN,
		JSONEX_PROCESSNULL,
	};

};

#endif
