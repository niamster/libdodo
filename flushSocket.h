/***************************************************************************
 *            flushSocket.h
 *
 *  Thu Oct 04 02:02:24 2005
 *  Copyright  2005  Ni@m
 *  niam.niam@gmail.com
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#ifndef _FLUSHSOCKET_H_
#define _FLUSHSOCKET_H_

#ifdef WIN
	#include <winsock.h>
#else
	#include <sys/socket.h>
#endif

namespace dodo
{
	/**
	 * type of service
	 */
	enum socketModeEnum
	{
		CLIENT_MODE,
		SERVER_MODE
	};
	
	enum socketTransferTypeEnum
	{
		TCP,
		UDP
	};
	/**
	 * class that takes ugly routine with sockets
	 * 
	 */
	class flushSocket
	{
		public:
			/**
			 * constructors/destructors
			 */
			socketpp();
			~socketpp();
			/**
			 * set buffer size
			 */	
			virtual void setBufferSize(unsigned long buffSize);

			/**
			 * send, recieve
			 */
			virtual bool send(void *data);
			virtual bool recieve(void *data);
			
			/**
			 * number of connections that can recieve
			 */
			 unsigned long numberOfConn;
		protected:
		
			/**
			 * size of buffer for transfers
			 * in bytes. default - 1024 bytes
			 */		
			unsigned long buffSize;					
	};
};

#endif /*SOCKETPP_H_*/
