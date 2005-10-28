/***************************************************************************
 *            flushSocket.cc
 *
 *  Thu Sep 20 01:43:24 2005
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
 
#include "flushSocket.h"

using namespace dodo;

flushSocket::flushSocket(unsigned long a_numberOfConn, 
						socketDomainEnum domain, 
						socketTransferTypeEnum type, 
						unsigned int protocol) : numberOfConn(a_numberOfConn)
{
	connections = new int [numberOfConn];
	makeSocket(domain,type,protocol);
}

//-------------------------------------------------------------------

flushSocket::flushSocket(socketDomainEnum domain, 
						socketTransferTypeEnum type, 
						unsigned int protocol) : numberOfConn(1)
{
	connections = new int [1];
	makeSocket(domain,type,protocol);
}

//-------------------------------------------------------------------

flushSocket::~flushSocket()
{
	delete [] connections;
}

//-------------------------------------------------------------------

void 
flushSocket::makeSocket(socketDomainEnum domain, 
						socketTransferTypeEnum type,
						unsigned int protocol)
{
	int real_domain, real_type;
	switch (domain)
	{
		case IPV4:
			real_domain = PF_INET;
			break;
		case IPX:
			real_domain = PF_IPX;
			break;
	#ifndef WIN
		case UNIX_SOCKET:
			real_domain = PF_UNIX;
			break;
		case IPV6:
			real_domain = PF_INET6;
			break;
		case PACKET:
			real_domain = PF_PACKET;
			break;
		case NETLINK:
			real_domain = PF_NETLINK;
			break;
	#endif
	}
	switch (type)
	{
		case STREAM:
			real_type = SOCK_STREAM;
			break;
		case DATAGRAM:
			real_type = SOCK_DGRAM;
			break;
		case RAW:
			real_type = SOCK_RAW;
			break;
	}	
	socket = ::socket(real_domain,real_type,protocol);
	if (socket == -1)
	#ifndef NO_EX
		switch (errno)
		{
			case EAFNOSUPPORT:
				throw flushSocketEx(FLUSHSOCKET_NOT_SUPPORTED_ADDR_FAMILY,(flushSocket *)this,__LINE__,__FILE__);
			case EACCES:
				throw flushSocketEx(FLUSHSOCKET_ACCESS_DENIED,(flushSocket *)this,__LINE__,__FILE__);
			case EINVAL:
				throw flushSocketEx(FLUSHSOCKET_UNKNOWN_PROTOCOL, (flushSocket *)this,__LINE__,__FILE__);
			case EMFILE:
			case ENFILE:
				throw flushSocketEx(FLUSHSOCKET_TOO_MANY_OPEN_FILE,(flushSocket *)this,__LINE__,__FILE__);
			case ENOBUFS:
			case ENOMEM:
				throw flushSocketEx(FLUSHSOCKET_MEMORY_OVER,(flushSocket *)this,__LINE__,__FILE__);
			case EPROTONOSUPPORT:
				throw flushSocketEx(FLUSHSOCKET_PROTO_NOT_SUPPORTED_WITHIN_DOMAIN,(flushSocket *)this,__LINE__,__FILE__);
		}
	#else
		switch (errno)
		{
			case EACCES:
			case EAFNOSUPPORT:
			case EINVAL:
			case EMFILE:
			case ENFILE:
			case ENOBUFS:
			case ENOMEM:
			case EPROTONOSUPPORT
				return false;					
		}							
	#endif	
}

//-------------------------------------------------------------------
