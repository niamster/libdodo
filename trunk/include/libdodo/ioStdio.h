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
#define _IOSTDIO_H_ 1

#include <libdodo/directives.h>

#include <libdodo/ioChannel.h>
#include <libdodo/ioPipe.h>

namespace dodo
{
	namespace io
	{
		/**
		 * @class stdio
		 * @brief provides interface for stdin/stdout/stderr I/O operations
		 */
		class stdio : virtual public pipe
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
			 * @param protection defines type of IO protection[see channelProtectionTypeEnum]
			 */
			stdio(short protection = CHANNEL_PROTECTION_PROCESS);

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

		  protected:

			/**
			 * write to stream - '\0' - terminated string
			 * @param data defines data that will be written
			 */
			virtual void _writeStream(const char * const data);

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

			bool err; ///< true if output stream is redirected to stderr
		};
	};
};

#endif
