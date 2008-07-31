/***************************************************************************
 *            ioStdio.h
 *
 *  Tue Nov 15 21:19:57 2005
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

#ifndef _IOSTDIO_H_
#define _IOSTDIO_H_

#include <fcntl.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/ioFifo.h>
#include <libdodo/pcThreadGuard.h>

namespace dodo
{
	namespace io
	{
		/**
		 * @class stdio provides interface for stdin/stdout/stderr I/O operations
		 */
		class stdio : virtual public fifo
		{
			private:

				/**
				 * copy constructor
				 * @note to prevent copying
				 */
				stdio(stdio &fd);

			public:

				/**
				 * constructor
				 */
				stdio();

				/**
				 * destructor
				 */
				virtual ~stdio();

				/**
				 * redirect output stream to stderr or stdout
				 * @param toStderr defines to redirect stream to stderr if true
				 */
				virtual void redirectToStderr(bool toStderr);

				/**
				 * @return true if output stream is redirected to stderr
				 */
				virtual bool isRedirectedToStderr();

			private:
				
				/**
				 * do nothing
				 * @note stdin/stdout/stderr are already available for I/O
				 */
				virtual void open();

				/**
				 * do nothing
				 * @note stdin/stdout/stderr shouldn't be closed 
				 */
				virtual void close();

				bool err;                               ///< true if output stream is redirected to stderr
		};
	};
};

#endif
