/***************************************************************************
 *            ioSTD.h
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

#ifndef _IOSTD_H_
#define _IOSTD_H_

#include <fcntl.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <libdodo/directives.h>

#include <libdodo/tools.h>
#include <libdodo/ioSTDEx.h>
#include <libdodo/types.h>
#include <libdodo/io.h>
#include <libdodo/ioSocket.h>
#include <libdodo/threadGuard.h>

namespace dodo
{
	/**
	 * @enum ioSTDOperationTypeEnum describes type of operation for hook
	 */
	enum ioSTDOperationTypeEnum
	{
		IOSTD_OPERATION_READ,
		IOSTD_OPERATION_WRITE,
		IOSTD_OPERATION_READSTREAM,
		IOSTD_OPERATION_WRITESTREAM,
		IOSTD_OPERATION_OPEN,
		IOSTD_OPERATION_CLOSE
	};

	/**
	 * @class ioSTD performs actions with stdin/out.
	 * @note it's usefull when you are using in/out operations through some proxy -> for example inetd!
	 */

	class ioSTD : public io,
				  virtual public threadGuardHolder
	{
		private:

			/**
			 * constructor
			 * to prevent from copying
			 */
			ioSTD(ioSTD &fd);

		public:

			/**
			 * constructor
			 */
			ioSTD();

			/**
			 * destructor
			 */
			virtual ~ioSTD();

			/**
			 * @return info about source of inputting
			 * @note it can be used to get info foreign `inputter` if you ar usin'g inetd
			 */
			__connInfo inputterInfo();

			#ifndef IOSTD_WO_XEXEC

			/**
			 * adds hook after the operation by callback
			 * @return number in list where function is set
			 * @param func is a pointer to function
			 * @param data is pointer to data toy want to pass to hook
			 */
			virtual int addPostExec(inExec func, void *data);

			/**
			 * adds hook before the operation by callback
			 * @return number in list where function is set
			 * @param func is a pointer to function
			 * @param data is pointer to data toy want to pass to hook
			 */
			virtual int addPreExec(inExec func, void *data);

				#ifdef DL_EXT

			/**
			 * adds hook after the operation by callback
			 * @return number in list where function is set
			 * @param module is a path to module, whrere hook exists
			 * @param data is pointer to data toy want to pass to hook
			 * @param toInit indicates data that will path to initialize function
			 */
			virtual int addPostExec(const dodoString &module, void *data, void *toInit = NULL);

			/**
			 * adds hook after the operation by callback
			 * @return number in list where function is set
			 * @param module is a path to module, whrere hook exists
			 * @param data is pointer to data toy want to pass to hook
			 * @param toInit indicates data that will path to initialize function
			 */
			virtual int addPreExec(const dodoString &module, void *data, void *toInit = NULL);

			/**
			 * set function from module that will be executed before/after the main action call
			 * the type of hook[pre/post] is defined in module
			 * @return number in list where function is set
			 * @param func is a pointer to function
			 * @param data is pointer to data toy want to pass to hook
			 * @param toInit indicates data that will path to initialize function
			 */
			virtual __xexecCounts addExec(const dodoString &module, void *data, void *toInit = NULL);

				#endif

			#endif

			/**
			 * read
			 * @param data is filled with read string
			 * if inSize bigger than buffer size - reads with few iterations
			 */
			virtual void readString(dodoString &data);

			/**
			 * read
			 * @param data is filled with read data
			 * if inSize bigger than buffer size - reads with few iterations
			 */
			virtual void read(char * const data);

			/**
			 * write
			 * @param data is string that will be written
			 * if outSize bigger than buffer size - writes with few iterations
			 */
			virtual void writeString(const dodoString &data);

			/**
			 * write
			 * @param data is data that will be written
			 * if outSize bigger than buffer size - writes with few iterations
			 */
			virtual void write(const char * const data);

			/**
			 * read from stream - null[or \n]-terminated string
			 * @param data is filled with read string
			 * max size of data is inSTDBuffer
			 */
			virtual void readStreamString(dodoString &data);

			/**
			 * read from stream - null[or \n]-terminated string
			 * @param data is filled with read data
			 * max size of data is inSTDBuffer
			 */
			virtual void readStream(char * const data);

			/**
			 * write to stream - null-terminated string
			 * @param data is string that will be written
			 * @note max data size is outSTDBuffer
			 * @note - appends '\n'
			 */
			virtual void writeStreamString(const dodoString &data);

			/**
			 * write to stream - null-terminated string
			 * @param data is data that will be written
			 * @note max data size is outSTDBuffer
			 * @note - appends '\n'
			 */
			virtual void writeStream(const char * const data);

			/**
			 * flushes to output
			 */
			virtual void flush();

			/**
			 * sometimes, when you ouput/input from some other programs, you have bounds in input/output buffer
			 * this parameters will help you;
			 * by default, they are too large, so you don't have to change it
			 */
			int inSTDBuffer;    ///< input buffer
			int outSTDBuffer;   ///< output buffer

			bool err;           ///< redirect output to stderr; false by default

			/**
			 * @return true if stream is blocked
			 */
			virtual bool isBlocked();

			/**
			 * blocks/unblocks stream
			 * @param flag indicates whether to block or unblock stream
			 */
			virtual void block(bool flag);

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
			 * read from stream - null[or \n]-terminated string
			 * @param data is filled with read data
			 * max size of data is inSTDBuffer
			 */
			virtual void _readStream(char * const data);

		private:

			FILE *desc;     ///< descriptor that is needed for redirection

			bool blocked;   ///< indicates, whether blocked or not;
	};

};

#endif
