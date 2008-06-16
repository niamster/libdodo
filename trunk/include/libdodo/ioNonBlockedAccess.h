/***************************************************************************
 *            ioNonBlockedAccess.h
 *
 *  Thu Sep 09 03:21:24 2006
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


#ifndef _IONONBLOCKEDACCESS_H_
#define _IONONBLOCKEDACCESS_H_

#include <libdodo/directives.h>

#include <poll.h>

#include <libdodo/ioNonBlockedAccessEx.h>
#include <libdodo/types.h>
#include <libdodo/toolsMisc.h>
#include <libdodo/ipcThreadGuard.h>

namespace dodo
{
	namespace io
	{
		/**
		 * @struct __inOutDescriptors defines input and output descriptors of the io* classes
		 */
		struct __inOutDescriptors
		{
			int position;               ///< identificator of the pair

			int in;                     ///< input stream
			int out;                    ///< output stream
		};

		/**
		 * @class nonBlockedAccess provides information if in/output stream is readable/writable
		 */
		class nonBlockedAccess : virtual public ipc::thread::guardHolder
		{

			private:

				/**
				 * copy constructor
				 * to prevent copying
				 */
				nonBlockedAccess(nonBlockedAccess &rt);

			public:

				/**
				 * constructor
				 */
				nonBlockedAccess();

				/**
				 * destructor
				 */
				virtual ~nonBlockedAccess();

				/**
				 * @return identificator of stream pair
				 * @param fl defines a stream[file, stdio, exchange, ...]
				 */
				virtual int addFlush(const nonBlockedAccessInfo &fl);

				/**
				 * delete stream pair from watchers
				 * @param pos defines identificator of stream pair
				 */
				virtual void delFlush(int pos);

				/**
				 * @return true if io* is readable
				 * @param pos defines identificator of stream pair
				 * @param timeout defines amount of time to wait for result[in milliseconds]
				 * @note if timeout is negative - infinit timeout
				 */
				virtual bool isReadable(int pos, int timeout = 100) const;

				/**
				 * @return collection of boolean values[value is true if io* is readable]
				 * @param pos defines collection of identificators of stream pairs
				 * @param timeout defines amount of time to wait for result[in milliseconds]
				 * @note if timeout is negative - infinit timeout
				 */
				virtual dodoArray<bool> isReadable(const dodoArray<int> &pos, int timeout = 100) const;

				/**
				 * @return true if io* is writable
				 * @param pos defines identificator of stream pair
				 * @param timeout defines amount of time to wait for result[in milliseconds]
				 * @note if timeout is negative - infinit timeout
				 */
				virtual bool isWritable(int pos, int timeout = 100) const;

				/**
				 * @return collection of boolean values[value is true if io* is writable]
				 * @param pos defines collection of identificators of stream pairs
				 * @param timeout defines amount of time to wait for result[in milliseconds]
				 * @note if timeout is negative - infinit timeout
				 */
				virtual dodoArray<bool> isWritable(const dodoArray<int> &pos, int timeout = 100) const;

			protected:

				dodoArray<__inOutDescriptors> desc;                     ///< stream pairs

				int descs;                                              ///< descriptors counter
		};
	};
};

#endif
