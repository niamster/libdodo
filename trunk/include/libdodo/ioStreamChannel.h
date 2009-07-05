/***************************************************************************
 *            ioStreamChannel.h
 *
 *  Sat Jun 13 12:26:57 2009
 *  Copyright  2009  Ni@m
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

#ifndef _IOSTREAMCHANNEL_H_
#define _IOSTREAMCHANNEL_H_ 1

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/ioChannel.h>

namespace dodo
{
	namespace io
	{
		namespace stream
		{
			/**
			 * @class channel
			 * @brief implements an interface for I/O operations on stream sources
			 */
			class channel : public io::channel
			{
			  public:

				/**
				 * constructor
				 * @param protection defines type of IO protection[see channelProtectionTypeEnum]
				 */
				channel(short protection);

				/**
				 * destructor
				 */
				virtual ~channel();

				/**
				 * @return read data
				 * @note not more then inSize
				 */
				virtual dodoString read() const;

				/**
				 * @param data defines data that will be written
				 * @note not more then outSize
				 */
				virtual void write(const dodoString &data);

				/**
				 * read from stream - '\0' or '\n' - terminated string
				 * @return read data
				 * @note not more then inSize
				 */
				virtual dodoString readStream() const;

				/**
				 * write to stream - '\0' - terminated string
				 * @param data defines data that will be written
				 * @note not more then outSize
				 */
				virtual void writeStream(const dodoString &data);
			};
		};
	};
};

#endif
