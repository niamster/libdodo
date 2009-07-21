/***************************************************************************
 *            ioNetworkSslExchangeEx.h
 *
 *  Tue Jun 10 16:45:47 2008
 *  Copyright  2008  Ni@m
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

#ifndef _IONETWORKSSLEXCHANGEEX_H_
#define _IONETWORKSSLEXCHANGEEX_H_ 1

#include <libdodo/directives.h>

#include <libdodo/exceptionBasic.h>

namespace dodo {
	namespace io {
		namespace network {
			namespace ssl {
				/**
				 * libdodo defined errors
				 */
				enum exchangeExR {
					EXCHANGEEX_NOCONNECTION,
				};

				/**
				 * explanations for libdodo defined errors
				 */
#define IONETWORKSSLEXCHANGEEX_NOCONNECTION_STR "No connection."

				/**
				 * IDs of functions where exception might be thrown
				 */
				enum exchangeFunctionsID {
					EXCHANGEEX__WRITE,
					EXCHANGEEX__READ,
					EXCHANGEEX__READSTREAM,
					EXCHANGEEX__CLOSE,
				};
			};
		};
	};
};

#endif
