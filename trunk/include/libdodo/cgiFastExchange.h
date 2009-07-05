/***************************************************************************
 *            cgiFastExchange.h
 *
 *  Sat Aug  5 03:31:19 2006
 *  Copyright  2006  Ni@m
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

#ifndef _CGIFASTEXCHANGE_H_
#define _CGIFASTEXCHANGE_H_ 1

#include <libdodo/directives.h>

#ifdef FASTCGI_EXT
#include <libdodo/types.h>
#include <libdodo/cgiExchange.h>

namespace dodo
{
	namespace cgi
	{
		namespace fast
		{
			struct __request__;

			/**
			 * @class exchange
			 * @brief provides interface to fast CGI I/O functionality
			 */
			class exchange : virtual public cgi::exchange
			{
			  public:

				/**
				 * constructor
				 * @param request defines CGI request descriptor
				 * @param protection defines type of IO protection[see channelProtectionTypeEnum]
				 */
				exchange(const __request__ &req,
						 short			 protection = io::CHANNEL_PROTECTION_PROCESS);

				/**
				 * destructor
				 */
				virtual ~exchange();

				/**
				 * flush output
				 */
				virtual void flush();

				/**
				 * @return environment variable
				 * @param data defines name of environment variable
				 */
				virtual char *getenv(const char *data);

			  private:

				/**
				 * copy constructor
				 * @note to prevent copying
				 */
				exchange(exchange &);

			  protected:

				/**
				 * @return descriptor of input stream
				 */
				virtual int getInDescriptor() const;

				/**
				 * @return descriptor of output stream
				 */
				virtual int getOutDescriptor() const;

				/**
				 * read
				 * @param data is filled with read data
				 * if inSize bigger than buffer size - reads with few iterations
				 */
				virtual void _read(char * const data) const;

				/**
				 * read from stream - '\0' or '\n' - terminated string
				 * @param data defines buffer that will be filled
				 * @note not more then inSize(including '\0')
				 */
				virtual unsigned long _readStream(char * const data) const;

				/**
				 * write
				 * @param data is data that will be written
				 * if outSize bigger than buffer size - writes with few iterations
				 */
				virtual void _write(const char * const data);

				/**
				 * write to stream - '\0' - terminated string
				 * @param data defines data that will be written
				 */
				virtual void _writeStream(const char * const data);

			  private:

				__request__ *request; ///< fast CGI descriptor
			};
		};
	};
};
#endif

#endif
