/***************************************************************************
 *            rpcEx.h
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

#ifndef _RPCEX_H_
#define _RPCEX_H_

#include <libdodo/directives.h>

#include <libdodo/baseEx.h>

namespace dodo
{
	/**
	 * libdodo defined errors
	 */
	enum rpcExR
	{
		RPCVALUEEX_WRONGTYPEREQUESTED,
		RPCVALUEEX_ARRAYOUTOFRANGE,
	};

	/**
	 * explanations for libdodo defined errors
	 */
#define RPCVALUEEX_WRONGTYPEREQUESTED_STR "Wrong type was requested. Use proper get* method."
#define RPCVALUEEX_ARRAYOUTOFRANGE_STR "Array key is out of range."

	/**
	 * IDs of functions where exception might be thrown
	 */
	enum rpcFunctionsID
	{
		RPCVALUEEX_BROPERATORSTRING,
		RPCVALUEEX_BROPERATORNUMERIC,
		RPCVALUEEX_GETSTRING,
		RPCVALUEEX_GETBOOLEAN,
		RPCVALUEEX_GETINTEGER,
		RPCVALUEEX_GETDOUBLE,
		RPCVALUEEX_GETARRAY,
		RPCVALUEEX_GETSTRUCT,
	};

};

#endif
