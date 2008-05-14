/***************************************************************************
 *            cgiFastServerExchange.h
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

#ifndef _CGIFASTSERVEREXCHANGE_H_
#define _CGIFASTSERVEREXCHANGE_H_

#include <libdodo/directives.h>

#ifdef FASTCGI_EXT

#include <fcgiapp.h>

#include <libdodo/types.h>
#include <libdodo/cgiFastServerExchangeEx.h>
#include <libdodo/ioChannel.h>
#include <libdodo/systemThreadGuard.h>

namespace dodo
{
	namespace cgi
	{
		namespace fast
		{
			/**
			 * @enum fastServerExchangeOperationTypeEnum describes type of operation for hook
			 */
			enum fastServerExchangeOperationTypeEnum
			{
				FASTSERVEREXCHANGE_OPERATION_READ,
				FASTSERVEREXCHANGE_OPERATION_READSTRING,
				FASTSERVEREXCHANGE_OPERATION_READSTREAM,
				FASTSERVEREXCHANGE_OPERATION_READSTREAMSTRING,
				FASTSERVEREXCHANGE_OPERATION_WRITE,
				FASTSERVEREXCHANGE_OPERATION_WRITESTRING,
				FASTSERVEREXCHANGE_OPERATION_WRITESTREAM,
				FASTSERVEREXCHANGE_OPERATION_WRITESTREAMSTRING,
			};
			
#ifndef CGIFASTSERVEREXCHANGE_WO_XEXEC
			
			/**
			 * @struct __xexexCgiFastServerExchangeCollectedData defines data that could be retrieved from class(to modificate)[contains references]
			 */
			struct __xexexCgiFastServerExchangeCollectedData
			{
				/**
				 * constructor
				 */
				__xexexCgiFastServerExchangeCollectedData(dodoString &buffer,
									   int &operType,
									   void *executor);

				dodoString &buffer;         ///< data buffer

				int &operType;              ///< xexec operation

				void *executor;             ///< class that executed hook
			};
			
#endif
			
			/**
			 * @class serverExchange provides interface to fast CGI I/O functionality
			 */
			class serverExchange : public io::channel,
							virtual public system::thread::guardHolder
			{
				private:
		
					/**
					 * copy constructor
					 * to prevent copying
					 */
					serverExchange(serverExchange &cf);
		
				public:
		
					/**
					 * constructor
					 * @param request defines CGI request descriptor
					 */
					serverExchange(FCGX_Request *request);
		
					/**
					 * destructor
					 */
					virtual ~serverExchange();
					
		#ifndef CGIFASTSERVEREXCHANGE_WO_XEXEC
		
					/**
					 * add hook after the operation
					 * @return id of the hook method
					 * @param func defines hook function
					 * @param data defines data that will be passed to hook function
					 */
					virtual int addPostExec(inExec func, void *data);
		
					/**
					 * add hook before the operation
					 * @return id of the hook method
					 * @param func defines hook function
					 * @param data defines data that will be passed to hook function
					 */
					virtual int addPreExec(inExec func, void *data);
		
		#ifdef DL_EXT
		
					/**
					 * add hook after the operation
					 * @return id of the hook method
					 * @param path defines path to the library[if not in ldconfig db] or library name
					 * @param data defines data that will be passed to hook function
					 * @param toInit defines data that will be passed to the init function
					 */
					virtual int addPostExec(const dodoString &path, void *data, void *toInit = NULL);
		
					/**
					 * add hook after the operation
					 * @return id of the hook method
					 * @param path defines path to the library[if not in ldconfig db] or library name
					 * @param data defines data that will be passed to hook function
					 * @param toInit defines data that will be passed to the init function
					 */
					virtual int addPreExec(const dodoString &path, void *data, void *toInit = NULL);
		
					/**
					 * set hook from the library that will be executed before/after the operation
					 * @return number in list where function is set
					 * @return id of the hook method
					 * @param path defines path to the library[if not in ldconfig db] or library name
					 * @param data defines data that will be passed to hook function
					 * @param toInit defines data that will be passed to the init function
					 * @note type of hook[pre/post] is defined in the library
					 */
					virtual __xexecCounts addExec(const dodoString &path, void *data, void *toInit = NULL);
		
		#endif
		
		#endif
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
					
#ifndef CGIFASTSERVEREXCHANGE_WO_XEXEC
					
					dodoString buffer;                      ///< buffer
		
					__xexexCgiFastServerExchangeCollectedData collectedData;   ///< data collected for xexec
					
#endif
			};
		};
	};
};

#endif

#endif
