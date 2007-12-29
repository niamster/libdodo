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
	 * @enum ioSocketOperationTypeEnum describes type of operation for hook
	 */
	enum ioSocketExchangeOperationTypeEnum
	{
		IOSOCKETEXCHANGE_OPERATION_RECEIVE,
		IOSOCKETEXCHANGE_OPERATION_SEND,
		IOSOCKETEXCHANGE_OPERATION_RECEIVESTREAM,
		IOSOCKETEXCHANGE_OPERATION_SENDSTREAM,
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
			 * @note if you want to copy it, the object, from what was copy is not be able to init new session => you have to reinit it with accept method!
			 */
			__initialAccept(__initialAccept &init);

		private:

			int socket;             ///< id of socket

			short family;           ///< socket family[see socketProtoFamilyEnum]
			short type;             ///< socket type[see socketTransferTypeEnum]

			bool blocked;           ///< if blocked
			bool blockInherited;    ///< if block inherited
	};

	/**
	 * @class ioSocketExchange used for communication[send/receive data]
	 * you may use it's functions only after passing it to connect(accept)
	 * otherwise you'll receive exeptions about socket(or false) from all of this' class' methods
	 * if you'll init this class again with another connection=previous will be closed
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
			 * @note object that inited new object of this class you can use for future connections;
			 * you can safely pass it to the functions;
			 */
			ioSocketExchange(ioSocketExchange &fse);

			/**
			 * constructor
			 * @param init is initial data[got from accept method]
			 * @note object that inited new object of this class you can use for future connections;
			 */
			ioSocketExchange(__initialAccept &init);

			/**
			 * destructor
			 */
			virtual ~ioSocketExchange();

			/**
			 * @return copy of object
			 */
			virtual ioSocketExchange *createCopy();

			/**
			 * deletes copy of object
			 * @param copy is copy of object to delete
			 */
			static void deleteCopy(ioSocketExchange *copy);

			#ifndef IOSOCKETEXCHANGE_WO_XEXEC

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
			 * set function from module that will be executed before/after the main action call
			 * the type of hook[pre/post] is defined in module
			 * @return number in list where function is set
			 * @param func is a pointer to function
			 * @param data is pointer to data toy want to pass to hook
			 * @param toInit indicates data that will path to initialize function
			 */
			virtual __xexecCounts addExec(const dodoString &module, void *data, void *toInit=NULL);

			/**
			 * adds hook after the operation by callback
			 * @return number in list where function is set
			 * @param module is a path to module, whrere hook exists
			 * @param data is pointer to data toy want to pass to hook
			 * @param toInit indicates data that will path to initialize function
			 */
			virtual int addPostExec(const dodoString &module, void *data, void *toInit=NULL);

			/**
			 * adds hook after the operation by callback
			 * @return number in list where function is set
			 * @param module is a path to module, whrere hook exists
			 * @param data is pointer to data toy want to pass to hook
			 * @param toInit indicates data that will path to initialize function
			 */
			virtual int addPreExec(const dodoString &module, void *data, void *toInit=NULL);

				#endif

			#endif

			/**
			 * init oject with given data
			 * @param init is initial data[got from accept method]
			 */
			virtual void init(__initialAccept &init);

			/**
			 * @return true if connection is alive
			 */
			virtual bool alive();

			/**
			 * write
			 * @param data is data that would be sent
			 * @param urgent -> send out-of-band data
			 * @note sends no longer than outSize
			 * if outSize bigger than socket buffer size - sends with few iterations
			 * signal safe
			 */
			virtual void
			write(const char * const data);

			/**
			 * write
			 * @param data is string that would be sent
			 * @note sends no longer than outSize
			 * if outSize bigger than socket buffer size - sends with few iterations
			 * signal safe
			 */
			virtual void
			writeString(const dodoString &data);

			/**
			 * receive
			 * @param data is data that would be received
			 * @note receives no longer than inSize
			 * if inSize bigger than socket buffer size - receives with few iterations
			 * signal safe
			 */
			virtual void
			read(char * const data);

			/**
			 * read
			 * @param data is string that would be received
			 * @note receives no longer than inSize
			 * if inSize bigger than socket buffer size - receives with few iterations
			 * signal safe
			 */
			virtual void
			readString(dodoString &data);

			/**
			 * write - null-terminated string
			 * @param data is data that would be sent
			 * @note sends no longer than outSize
			 * if outSize bigger than socket buffer size - sends with few iterations
			 * max data size is outSocketBuffer
			 * @note - appends '\n'
			 * signal safe
			 */
			virtual void
			writeStream(const char * const data);

			/**
			 * write - null-terminated string
			 * @param data is string that would be sent
			 * @note sends no longer than outSize
			 * if outSize bigger than socket buffer size - sends with few iterations
			 * max data size is outSocketBuffer
			 * @note - appends '\n'
			 * signal safe
			 */
			virtual void
			writeStreamString(const dodoString &data);

			/**
			 * read - null-terminated string
			 * @param data is data that would be received
			 * @note receives no longer than inSize
			 * if inSize bigger than socket buffer size - receives with few iterations
			 * max data size is inSocketBuffer
			 * signal safe
			 */
			virtual void
			readStream(char * const data);

			/**
			 * read - null-terminated string
			 * @param data is string that would be received
			 * @note receives no longer than inSize
			 * if inSize bigger than socket buffer size - receives with few iterations
			 * max data size is inSocketBuffer
			 * signal safe
			 */
			virtual void
			readStreamString(dodoString &data);

			/**
			 * closes this socket
			 */
			virtual void
			close();

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
			 * inits this class' data
			 * @param socket is id of socket
			 * @param blockInherited indicates whether to inherit block of parent
			 */
			virtual void init(int socket, bool blockInherited);

			/**
			 * receive
			 * @param data is data that would be received
			 * @note receives no longer than inSize
			 * if inSize bigger than socket buffer size - receives with few iterations
			 * signal safe
			 */
			virtual void
			_read(char * const data);

			/**
			 * read - null-terminated string
			 * @param data is data that would be received
			 * @note receives no longer than inSize
			 * if inSize bigger than socket buffer size - receives with few iterations
			 * max data size is inSocketBuffer
			 * signal safe
			 */
			virtual void
			_readStream(char * const data);
	};

};

#endif
