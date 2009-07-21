/***************************************************************************
 *            ioNetworkSslServerEx.h
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

#ifndef _IONETWORKSSLSERVEREX_H_
#define _IONETWORKSSLSERVEREX_H_ 1

#include <libdodo/directives.h>

#include <libdodo/exceptionBasic.h>

namespace dodo {
	namespace io {
		namespace network {
			namespace ssl {
				/**
				 * libdodo defined errors
				 */
				enum serverExR {
					SERVEREX_WRONGPARAMETER,
					SERVEREX_WRONGFILENAME,
					SERVEREX_UNABLETOINITCONTEXT,
					SERVEREX_UNABLETOINITSSL,
					SERVEREX_UNKNOWNKEYTYPE,
					SERVEREX_LONGPATH,
				};

				/**
				 * explanations for libdodo defined errors
				 */
#define IONETWORKSSLSERVEREX_WRONGPARAMETER_STR "Wrong paramather passed to function."
#define IONETWORKSSLSERVEREX_WRONGFILENAME_STR  "Probably wrong filename, type of file is wrong or `path` is empty!"
#define IONETWORKSSLSERVEREX_UNABLETOINITCONTEXT_STR "Unable to initialize context for SSL connection."
#define IONETWORKSSLSERVEREX_UNABLETOINITSSL_STR "Unable to initialize SSL object."
#define IONETWORKSSLSERVEREX_UNKNOWNKEYTYPE_STR "Unknown key type."
#define IONETWORKSSLSERVEREX_LONGPATH_STR "Path to socket is too long."
				/**
				 * IDs of functions where exception might be thrown
				 */
				enum serverFunctionsID {
					SERVEREX_INITSSL,
					SERVEREX_ACCEPTSSL,
					SERVEREX_BINDNLISTEN,
					SERVEREX_ACCEPT,
					SERVEREX_SETSERTIFICATES,
					SERVEREX_REMOVESERTIFICATES,
				};
			};
		};
	};
};

#endif
