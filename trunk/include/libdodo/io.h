/***************************************************************************
 *            io.h
 *
 *  Tue Oct 11 00:19:57 2005
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

#ifndef _IO_H_
#define _IO_H_

#include <libdodo/directives.h>

#include <libdodo/xexec.h>
#include <libdodo/types.h>

namespace dodo
{
	/**
	 * @struct __xexexIoCollectedData contains data that could be retrieved from class(to modificate)[contains references]
	 */
	struct __xexexIoCollectedData
	{
		/**
		 * constructor
		 * initiates references
		 */
		__xexexIoCollectedData(dodoString &buffer,
							   int &operType,
							   void *executor);

		dodoString &buffer;         ///< buffer where data is stored

		int &operType;              ///< operation type set by main action; can be used in hook to determine type of action

		void *executor;             ///< class that executed hook
	};

	/**
	 * @class io is a base class for I/O operations.
	 * all I/O operations are blockable => read/write inSize/outSize block.
	 */
	class io : public xexec
	{
			friend class ioNBA;

		public:

			/**
			 * constructor
			 */
			io();

			/**
			 * destructor
			 */
			virtual ~io() = 0;

			unsigned long inSize;   ///< size of data block;
			unsigned long outSize;  ///< size of data block;

		protected:

			bool opened; ///< indicates whether file(connection) opened or not

			/**
			 * @return descriptor of input stream
			 */
			virtual int getInDescriptor() const = 0;

			/**
			 * @return descriptor of output stream
			 */
			virtual int getOutDescriptor() const = 0;

			dodoString buffer;                      ///< buffer where data is stored

			__xexexIoCollectedData collectedData;   ///< data collected for xexec
	};

};

#endif
