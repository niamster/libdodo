/***************************************************************************
 *            jsonProcessorEx.h
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

#ifndef _JSONPROCESSOREX_H_
#define _JSONPROCESSOREX_H_

#include <libdodo/directives.h>

#include <libdodo/baseEx.h>

namespace dodo
{
	namespace json
	{
		/**
		 * libdodo defined errors
		 */
		enum processorExR
		{
			PROCESSOREX_MALFORMEDJSONNUMERIC,
			PROCESSOREX_MALFORMEDJSONNULL,
			PROCESSOREX_MALFORMEDJSONBOOLEAN,
		};
	
		/**
		 * explanations for libdodo defined errors
		 */
	#define PROCESSOREX_MALFORMEDJSONNUMERIC_STR "Malformed json object: numeric."
	#define PROCESSOREX_MALFORMEDJSONNULL_STR "Malformed json object: null."
	#define PROCESSOREX_MALFORMEDJSONBOOLEAN_STR "Malformed json object: boolean."
	
		/**
		 * IDs of functions where exception might be thrown
		 */
		enum processorFunctionsID
		{
			PROCESSOREX_PROCESSBOOLEAN,
			PROCESSOREX_PROCESSNULL,
			PROCESSOREX_PROCESSNUMERIC,
		};
	};
};

#endif