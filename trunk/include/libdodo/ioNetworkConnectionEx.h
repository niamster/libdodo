/***************************************************************************
 *            ioNetworkConnectionEx.h
 *
 *  Mon Feb 21 03:03:47 2005
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

#ifndef _IONETWORKCONNECTIONEX_H_
#define _IONETWORKCONNECTIONEX_H_

#include <libdodo/directives.h>

#include <libdodo/exceptionBasic.h>

namespace dodo
{
	namespace io
	{
		namespace network
		{
			/**
			 * libdodo defined errors
			 */
			enum connectionExR
			{
				CONNECTIONEX_NOSOCKETCREATED,
				CONNECTIONEX_WRONGPARAMETER,
			};

			/**
			 * explanations for libdodo defined errors
			 */
#define IONETWORKCONNECTIONEX_NOSOCKETCREATED_STR "No socket was created =(."
#define IONETWORKCONNECTIONEX_WRONGPARAMETER_STR  "Wrong paramather passed to function."

			/**
			 * IDs of functions where exception might be thrown
			 */
			enum connectionFunctionsID
			{
				CONNECTIONEX_SETINBUFFERSIZE,
				CONNECTIONEX_SETOUTBUFFERSIZE,
				CONNECTIONEX_SETINTIMEOUT,
				CONNECTIONEX_SETOUTTIMEOUT,
				CONNECTIONEX_SETSOCKOPT,
				CONNECTIONEX_SETLINGERSOCKOPT,
				CONNECTIONEX_BLOCK,
				CONNECTIONEX__CLOSE,
			};
		};
	};
};

#endif
