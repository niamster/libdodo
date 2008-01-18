/***************************************************************************
 *            ioNBA.h
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


#ifndef _IONBA_H_
#define _IONBA_H_

#include <libdodo/directives.h>

#include <poll.h>

#include <libdodo/ioNBAEx.h>
#include <libdodo/types.h>
#include <libdodo/tools.h>
#include <libdodo/threadGuard.h>

namespace dodo
{

	/**
	 * @struct __inOutDescriptors contains in and descriptors of the io* classes
	 */
	struct __inOutDescriptors
	{
		int position;   ///< identificator of pair

		int in;         ///< in stream
		int out;        ///< out stream
	};

	/**
	 * @class ioNBA
	 */
	class ioNBA : virtual public threadGuardHolder
	{

		private:

			/**
			 * copy constructor
			 * to prevent copying
			 */
			ioNBA(ioNBA &rt);

		public:

			/**
			 * constructor
			 */
			ioNBA();

			/**
			 * destructor
			 */
			virtual ~ioNBA();

			/**
			 * @return identificator of added stream
			 * @param fl is a reference on stream[ioDisk, ioSTD, ioSocket, ioSocketExchange]
			 */
			virtual int addFlush(const ioNBAInfo &fl);

			/**
			 * @param fl is identificator of stream to remove
			 */
			virtual void delFlush(int pos);

			/**
			 * @return true if io* is ready to read
			 * @param pos is identificator of stream
			 * @param timeout describes amount of time to wait for result[in milliseconds]
			 * @note if timeout is negative - infinit timeout
			 */
			virtual bool isReadable(int pos, int timeout = 100) const;

			/**
			 * @return vector of true if io* is ready to read
			 * @param pos is vector of identificators of streams
			 * @param timeout describes amount of time to wait for result[in milliseconds]
			 * @note if timeout is negative - infinit timeout
			 */
			virtual dodoArray<bool> isReadable(const dodoArray<int> &pos, int timeout = 100) const;

			/**
			 * @return vector of true if io* is ready to write
			 * @param pos is vector of identificators of streams
			 * @param timeout describes amount of time to wait for result[in milliseconds]
			 * @note if timeout is negative - infinit timeout
			 */
			virtual dodoArray<bool> isWritable(const dodoArray<int> &pos, int timeout = 100) const;

			/**
			 * @return true if io* is ready to write
			 * @param pos is identificator of stream
			 * @param timeout describes amount of time to wait for result[in milliseconds]
			 * @note if timeout is negative - infinit timeout
			 */
			virtual bool isWritable(int pos, int timeout = 100) const;

		protected:

			/**
			 * @param count decribes with how many 'false' fill
			 * @note fill tempRB with false values
			 */
			virtual void makeFalse(int count) const;

			dodoArray<__inOutDescriptors> desc; ///< io descriptors

			int descs;                          ///< descriptors counter

			mutable dodoArray<bool> tempRB;     ///< temporary to save results of avalaibility
	};

};

#endif
