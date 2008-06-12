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

#ifndef _CGIFASTEXCHANGE_H_
#define _CGIFASTEXCHANGE_H_

#include <libdodo/directives.h>

#ifdef FASTCGI_EXT

#include <fcgiapp.h>

#include <libdodo/types.h>
#include <libdodo/cgiFastExchangeEx.h>
#include <libdodo/ioChannel.h>
#include <libdodo/ipcThreadGuard.h>

namespace dodo
{
	namespace cgi
	{
		namespace fast
		{
			/**
			 * @enum exchangeOperationTypeEnum describes type of operation for hook
			 */
			enum exchangeOperationTypeEnum
			{
				EXCHANGE_OPERATION_READ,
				EXCHANGE_OPERATION_READSTRING,
				EXCHANGE_OPERATION_READSTREAM,
				EXCHANGE_OPERATION_READSTREAMSTRING,
				EXCHANGE_OPERATION_WRITE,
				EXCHANGE_OPERATION_WRITESTRING,
				EXCHANGE_OPERATION_WRITESTREAM,
				EXCHANGE_OPERATION_WRITESTREAMSTRING,
			};

#ifndef CGIFASTEXCHANGE_WO_XEXEC

			/**
			 * @struct __xexexCgiFastExchangeCollectedData defines data that could be retrieved from class(to modificate)[contains references]
			 */
			struct __xexexCgiFastExchangeCollectedData
			{
				/**
				 * constructor
				 * @param operType defines xexec operation
				 * @param executor defines class that executed hook
				 */
				__xexexCgiFastExchangeCollectedData(int &operType, void *executor);

				dodoString buffer;              ///< data buffer

				int &operType;                  ///< xexec operation

				void *executor;                 ///< class that executed hook
			};

#endif

			/**
			 * @class exchange provides interface to fast CGI I/O functionality
			 */
			class exchange : public io::channel,
					 virtual public ipc::thread::guardHolder
			{
				private:

					/**
					 * copy constructor
					 * to prevent copying
					 */
					exchange(exchange &cf);

				public:

					/**
					 * constructor
					 * @param request defines CGI request descriptor
					 */
					exchange(FCGX_Request *request);

					/**
					 * destructor
					 */
					virtual ~exchange();

					/**
					 * @param data defines buffer that will be filled
					 * @note not more then inSize(including '\0')
					 */
					virtual void readString(dodoString &data);

					/**
					 * @param data defines buffer that will be filled
					 * @note not more then inSize(including '\0')
					 */
					virtual void read(char * const data);

					/**
					 * @param data defines data that will be written
					 */
					virtual void writeString(const dodoString &data);

					/**
					 * @param data defines data that will be written
					 */
					virtual void write(const char * const data);

					/**
					 * read from stream - '\0' or '\n' - terminated string
					 * @param data defines buffer that will be filled
					 * @note not more then inSize(including '\0')
					 */
					virtual void readStreamString(dodoString &data);

					/**
					 * read from stream - '\0' or '\n' - terminated string
					 * @param data defines buffer that will be filled
					 * @note not more then inSize(including '\0')
					 */
					virtual void readStream(char * const data);

					/**
					 * write to stream - '\0' - terminated string
					 * @param data defines data that will be written
					 */
					virtual void writeStreamString(const dodoString &data);

					/**
					 * write to stream - '\0' - terminated string
					 * @param data defines data that will be written
					 */
					virtual void writeStream(const char * const data);

					/**
					 * flush output
					 */
					virtual void flush();

					/**
					 * @return environment variable
					 * @param data defines name of environment variable
					 */
					virtual char *getenv(const char *data);

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
					virtual void _read(char * const data);

					/**
					 * write
					 * @param data is data that will be written
					 * if outSize bigger than buffer size - writes with few iterations
					 */
					virtual void _write(const char * const data);

				private:

					FCGX_Request *request;    ///< fast CGI descriptor

#ifndef CGIFASTEXCHANGE_WO_XEXEC

					__xexexCgiFastExchangeCollectedData collectedData;      ///< data collected for xexec

#endif
			};
		};
	};
};

#endif

#endif
