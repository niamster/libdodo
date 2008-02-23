/***************************************************************************
 *            ioSocket.h
 *
 *  Thu Oct 04 02:02:24 2005
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

#ifndef _IOSOCKETEXCHANGE_H_
#define _IOSOCKETEXCHANGE_H_

#include <libdodo/directives.h>

#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include <libdodo/ioSocketOptions.h>
#include <libdodo/ioSocketExchangeEx.h>
#include <libdodo/types.h>
#include <libdodo/xexec.h>
#include <libdodo/threadGuard.h>

namespace dodo
{

	/**
	 * @enum ioSocketOperationTypeEnum defines type of operation for hook
	 */
	enum ioSocketExchangeOperationTypeEnum
	{
		IOSOCKETEXCHANGE_OPERATION_READ,
		IOSOCKETEXCHANGE_OPERATION_READSTRING,
		IOSOCKETEXCHANGE_OPERATION_READSTREAM,
		IOSOCKETEXCHANGE_OPERATION_READSTREAMSTRING,
		IOSOCKETEXCHANGE_OPERATION_WRITE,
		IOSOCKETEXCHANGE_OPERATION_WRITESTRING,
		IOSOCKETEXCHANGE_OPERATION_WRITESTREAM,
		IOSOCKETEXCHANGE_OPERATION_WRITESTREAMSTRING,
		IOSOCKETEXCHANGE_OPERATION_CLOSE,
	};

	/**
	 * @class __initialAccept holds info that passes to accept call, and then inits ioSocketExchange;
	 */
	class __initialAccept
	{
			friend class ioSocketExchange;
			friend class ioSocket;

		public:

			/**
			 * constructor
			 */
			__initialAccept();

			/**
			 * copy constructor
			 * @note if you want to copy it, the object, from what has been copied is not more able to init new session: you have to reinit it with ioSocket::accept method
			 */
			__initialAccept(__initialAccept &init);

		private:

			int socket;             ///< socket

			short family;           ///< socket family[see socketProtoFamilyEnum]
			short type;             ///< socket type[see socketTransferTypeEnum]

			bool blocked;           ///< true if blocked
			bool blockInherited;    ///< true if block flag is inherited
	};

	/**
	 * @class ioSocketExchange provides communication interface[send/receive data]
	 */
	class ioSocketExchange : public ioSocketOptions,
							 public io,
							 virtual public threadGuardHolder
	{

			friend class ioSocket;

		public:

			/**
			 * constructor
			 */
			ioSocketExchange();

			/**
			 * copy constructor
			 * @note the object that has inited the object of current instance can be used for another connections
			 */
			ioSocketExchange(ioSocketExchange &fse);

			/**
			 * constructor
			 * @param init is initial data[got from the ioSocket::accept method]
			 * @note the object that has inited the object of current instance can be used for another connections
			 */
			ioSocketExchange(__initialAccept &init);

			/**
			 * destructor
			 */
			virtual ~ioSocketExchange();

			/**
			 * @return copy of the instance
			 * @note current instance can't be used any more
			 */
			virtual ioSocketExchange *createCopy();

			/**
			 * delete a copy of an object
			 * @param copy defines an instance of an object
			 */
			static void deleteCopy(ioSocketExchange *copy);

#ifndef IOSOCKETEXCHANGE_WO_XEXEC

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
			virtual int addPreExec(const dodoString &module, void *data, void *toInit = NULL);

			/**
			 * set hook from the library that will be executed before/after the operation
			 * @return number in list where function is set
			 * @return id of the hook method
			 * @param path defines path to the library[if not in ldconfig db] or library name
			 * @param data defines data that will be passed to hook function
			 * @param toInit defines data that will be passed to the init function
			 * @note type of hook[pre/post] is defined in the library
			 */
			virtual __xexecCounts addExec(const dodoString &module, void *data, void *toInit = NULL);

#endif

#endif

			/**
			 * init object
			 * @param init defines initial data[got from ioSocket::accept method]
			 */
			virtual void init(__initialAccept &init);

			/**
			 * @return true if connection is alive
			 */
			virtual bool alive();

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
			 * close connection
			 */
			virtual void close();

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
			 * init current instance
			 * @param socket defines socket
			 * @param blockInherited defines block flag inheritance
			 */
			virtual void init(int socket, bool blockInherited);

			/**
			 * @param data defines buffer that will be filled
			 * @note not more then inSize(including '\0')
			 */
			virtual void _read(char * const data);

			/**
			 * read from stream - '\0' or '\n' - terminated string
			 * @param data defines buffer that will be filled
			 * @note not more then inSize(including '\0')
			 */
			virtual void _readStream(char * const data);

			/**
			 * @param data defines data that will be written
			 */
			virtual void _write(const char * const data);
	};

};

#endif
