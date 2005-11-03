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

flushDisk dummySockEx(REG_FILE);///to throw ex in this class

flushSocket::flushSocket(unsigned long a_numberOfConn, 
						socketProtoFamilyEnum a_family, 
						socketTransferTypeEnum a_type, 
						unsigned int a_protocol) : numberOfConn(a_numberOfConn),
						family(a_family),
						type(a_type),
						protocol(a_protocol),
						socket(-1),
						recieveTimeout(RECIEVE_TIMEOUT),
						sendTimeout(SEND_TIMEOUT),
						inSocketBuffer(SOCKET_INSIZE),
						outSocketBuffer(SOCKET_OUTSIZE)
{
	connections = new int [numberOfConn];
	makeSocket(a_family,a_type,a_protocol);
	#ifndef SOCKET_SYSTEM_DEFAULT_BUFFER_SIZE
		acceptBufferSize();
	#endif
	#ifndef SOCKET_SYSTEM_DEFAULT_TIMEOUT
		acceptTimeout();
	#endif
}

//-------------------------------------------------------------------

flushSocket::flushSocket(socketProtoFamilyEnum a_family, 
						socketTransferTypeEnum a_type, 
						unsigned int a_protocol) : numberOfConn(-1),
						family(a_family),
						type(a_type),
						protocol(a_protocol),
						socket(-1),
						recieveTimeout(RECIEVE_TIMEOUT),
						sendTimeout(SEND_TIMEOUT),
						inSocketBuffer(SOCKET_INSIZE),
						outSocketBuffer(SOCKET_OUTSIZE)
						
{
	connections = new int [1];
	makeSocket(a_family,a_type,a_protocol);
	#ifndef SOCKET_SYSTEM_DEFAULT_BUFFER_SIZE
		acceptBufferSize();
	#endif
	#ifndef SOCKET_SYSTEM_DEFAULT_TIMEOUT
		acceptTimeout();
	#endif
}

//-------------------------------------------------------------------

flushSocket::~flushSocket()
{
	delete [] connections;
}

//-------------------------------------------------------------------

void 
flushSocket::makeSocket(socketProtoFamilyEnum domain, 
						socketTransferTypeEnum type,
						unsigned int protocol)
{
	int real_domain, real_type;
	
	switch (domain)
	{
		case PROTO_FAMILY_IPV4:
			real_domain = PF_INET;
			break;
		case PROTO_FAMILY_IPV6:
			real_domain = PF_INET6;
			break;
		case PROTO_FAMILY_PACKET:
			real_domain = PF_PACKET;
			break;
	#ifndef WIN
		case PROTO_FAMILY_UNIX_SOCKET:
			real_domain = PF_UNIX;
			break;
	#endif
	}
	switch (type)
	{
		case TRANSFER_TYPE_STREAM:
			real_type = SOCK_STREAM;
			break;
		case TRANSFER_TYPE_DATAGRAM:
			real_type = SOCK_DGRAM;
			break;
		case TRANSFER_TYPE_RAW:
			real_type = SOCK_RAW;
			break;
	#ifndef WIN			
		case TRANSFER_TYPE_PACKET:
			real_type = SOCK_PACKET;
	#endif
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

bool 
flushSocket::connect(const std::string &host, 
					unsigned int port)
{
	struct sockaddr_in sa;
	
	if (family == PROTO_FAMILY_IPV6)
		sa.sin_family = AF_INET6;
	else
		sa.sin_family = AF_INET;
	sa.sin_port = htonl(port);
	inet_aton(host.c_str(),&sa.sin_addr);
	

	if (::connect(socket,(struct sockaddr *)&sa,sizeof(sa))==-1)
	#ifndef NO_EX
		switch (errno)
		{
			case EAFNOSUPPORT:
				throw flushSocketEx(FLUSHSOCKET_NOT_SUPPORTED_ADDR_FAMILY,(flushSocket *)this,__LINE__,__FILE__);
			case EACCES:
			case EPERM:
				throw flushSocketEx(FLUSHSOCKET_ACCESS_DENIED,(flushSocket *)this,__LINE__,__FILE__);
			case EADDRINUSE:
				throw flushSocketEx(FLUSHSOCKET_ALREADY_USED,(flushSocket *)this,__LINE__,__FILE__);
			case EAGAIN:
				throw flushSocketEx(FLUSHSOCKET_NO_FREE_PORTS,(flushSocket *)this,__LINE__,__FILE__);	
			case EALREADY:
			case EBADF:
			case EFAULT:
			case ETIMEDOUT:
			case EINPROGRESS:
				throw flushSocketEx(FLUSHSOCKET_SYSTEM_FAULT,(flushSocket *)this,__LINE__,__FILE__);
			case ECONNREFUSED:
				throw flushSocketEx(FLUSHSOCKET_CONNECTION_REFUSED,(flushSocket *)this,__LINE__,__FILE__);
			case EISCONN:
				throw flushSocketEx(FLUSHSOCKET_ALREADY_CONNECTED,(flushSocket *)this,__LINE__,__FILE__);
			case ENETUNREACH:
				throw flushSocketEx(FLUSHSOCKET_NETWORK_UNREACHABLE,(flushSocket *)this,__LINE__,__FILE__);
			case ENOTSOCK:
				throw flushSocketEx(FLUSHSOCKET_NOT_A_SOCKET,(flushSocket *)this,__LINE__,__FILE__);
		}
	#else
		switch (errno)
		{
			case EAFNOSUPPORT:
			case EACCES:
			case EPERM:
			case EADDRINUSE:
			case EAGAIN:
			case EALREADY:
			case EBADF:
			case EFAULT:
			case ETIMEDOUT:
			case EINPROGRESS:
			case ECONNREFUSED:
			case EISCONN:
			case ENETUNREACH:
			case ENOTSOCK:				
				return false;					
		}							
	#endif		
}

//-------------------------------------------------------------------

#ifndef WIN
	
	bool 
	flushSocket::connect(const std::string &path)
	{
	
		struct stat st;
		if (::lstat(path.c_str(),&st) == -1)
		#ifndef NO_EX
			switch (errno)
			{
				case EACCES:
					throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,&dummySockEx,__LINE__,__FILE__);
				case EIO:
					throw flushDiskEx(FLUSHDISK_IOERROR,&dummySockEx,__LINE__,__FILE__);
				case EINTR:
					throw flushDiskEx(FLUSHDISK_INTERRUPTED, &dummySockEx,__LINE__,__FILE__);
				case ENAMETOOLONG:
					throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,&dummySockEx,__LINE__,__FILE__);
				case ELOOP:
				case ENOTDIR:
					throw flushDiskEx(FLUSHDISK_WRONG_FILE_NAME,&dummySockEx,__LINE__,__FILE__);	
				case ENOMEM:
					throw flushDiskEx(FLUSHDISK_MEMORY_OVER,&dummySockEx,__LINE__,__FILE__);
			}
		#else
			switch (errno)
			{					
				case EACCES:
				case EIO:
				case EINTR:
				case ENAMETOOLONG:
				case ELOOP:
				case ENOTDIR:
				case ENOMEM:
					return false;					
			}							
		#endif
		if (errno != ENOENT)
		{
			if (!S_ISSOCK(st.st_mode))
				throw flushSocketEx(FLUSHSOCKET_NOT_A_SOCKET,(flushSocket *)this,__LINE__,__FILE__);
		}
		else
		{
			if (mknod(path.c_str(),flushDisk::getPermission(UNIX_SOCKET_PERM),S_IFSOCK)==-1)
			#ifndef NO_EX
				switch (errno)
				{
					case EACCES:
						throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,&dummySockEx,__LINE__,__FILE__);
					case EIO:
						throw flushDiskEx(FLUSHDISK_IOERROR,&dummySockEx,__LINE__,__FILE__);
					case EINTR:
						throw flushDiskEx(FLUSHDISK_INTERRUPTED,&dummySockEx,__LINE__,__FILE__);
					case ENOSPC:
					case ENOMEM:
						throw flushDiskEx(FLUSHDISK_NOT_ENOUGH_FREE_SPACE,&dummySockEx,__LINE__,__FILE__);
					case EMFILE:
					case ENFILE:
						throw flushDiskEx(FLUSHDISK_TOO_MANY_OPEN_FILE,&dummySockEx,__LINE__,__FILE__);
					case ENAMETOOLONG:
						throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,&dummySockEx,__LINE__,__FILE__);
					case ENOENT:
						throw flushDiskEx(FLUSHDISK_NO_SUCH_FILE,&dummySockEx,__LINE__,__FILE__);	
					case EROFS:
						throw flushDiskEx(FLUSHDISK_READ_ONLY_FS,&dummySockEx,__LINE__,__FILE__);		
				}
			#else
				switch (errno)
				{
					case EROFS:
					case EACCES:
					case EIO:
					case EINTR:
					case ENOSPC:
					case ENOMEM:
					case EMFILE:
					case ENFILE:
					case ENAMETOOLONG:
					case ENOENT:
						return false;					
				}
			#endif				
		}	
		
		struct sockaddr_un sa;
		
		strcpy(sa.sun_path,path.c_str());
		sa.sun_family = AF_UNIX;
	
		if (::connect(socket,(struct sockaddr *)&sa,path.size()+sizeof(sa))==-1)
		#ifndef NO_EX
			switch (errno)
			{
				case EAFNOSUPPORT:
					throw flushSocketEx(FLUSHSOCKET_NOT_SUPPORTED_ADDR_FAMILY,(flushSocket *)this,__LINE__,__FILE__);
				case EACCES:
				case EPERM:
					throw flushSocketEx(FLUSHSOCKET_ACCESS_DENIED,(flushSocket *)this,__LINE__,__FILE__);
				case EADDRINUSE:
					throw flushSocketEx(FLUSHSOCKET_ALREADY_USED,(flushSocket *)this,__LINE__,__FILE__);
				case EAGAIN:
					throw flushSocketEx(FLUSHSOCKET_NO_FREE_PORTS,(flushSocket *)this,__LINE__,__FILE__);	
				case EALREADY:
				case EBADF:
				case EFAULT:
				case ETIMEDOUT:
				case EINPROGRESS:
					throw flushSocketEx(FLUSHSOCKET_SYSTEM_FAULT,(flushSocket *)this,__LINE__,__FILE__);
				case ECONNREFUSED:
					throw flushSocketEx(FLUSHSOCKET_CONNECTION_REFUSED,(flushSocket *)this,__LINE__,__FILE__);
				case EISCONN:
					throw flushSocketEx(FLUSHSOCKET_ALREADY_CONNECTED,(flushSocket *)this,__LINE__,__FILE__);
				case ENETUNREACH:
					throw flushSocketEx(FLUSHSOCKET_NETWORK_UNREACHABLE,(flushSocket *)this,__LINE__,__FILE__);
				case ENOTSOCK:
					throw flushSocketEx(FLUSHSOCKET_NOT_A_SOCKET,(flushSocket *)this,__LINE__,__FILE__);
			}
		#else
			switch (errno)
			{
				case EAFNOSUPPORT:
				case EACCES:
				case EPERM:
				case EADDRINUSE:
				case EAGAIN:
				case EALREADY:
				case EBADF:
				case EFAULT:
				case ETIMEDOUT:
				case EINPROGRESS:
				case ECONNREFUSED:
				case EISCONN:
				case ENETUNREACH:
				case ENOTSOCK:				
					return false;					
			}							
		#endif		
	}

#endif

//-------------------------------------------------------------------

void 
flushSocket::acceptBufferSize()
{
	if (socket!=-1)
	{
		if (setsockopt(socket,SOL_SOCKET,SO_SNDBUF,&outSocketBuffer,sizeof(unsigned long))==-1)
		#ifndef NO_EX
			switch (errno)
			{
				case EBADF:
				case EFAULT:
				case ENOPROTOOPT:
					throw flushSocketEx(FLUSHSOCKET_SYSTEM_FAULT,(flushSocket *)this,__LINE__,__FILE__);
				case ENOTSOCK:
					throw flushSocketEx(FLUSHSOCKET_NOT_A_SOCKET,(flushSocket *)this,__LINE__,__FILE__);
			}
		#else
			switch (errno)
			{
				case EBADF:
				case EFAULT:
				case ENOPROTOOPT:
				case ENOTSOCK:
					return ;					
			}							
		#endif
		
		if (setsockopt(socket,SOL_SOCKET,SO_RCVBUF,&inSocketBuffer,sizeof(unsigned long))==-1)
		#ifndef NO_EX
			switch (errno)
			{
				case EBADF:
				case EFAULT:
				case ENOPROTOOPT:
					throw flushSocketEx(FLUSHSOCKET_SYSTEM_FAULT,(flushSocket *)this,__LINE__,__FILE__);
				case ENOTSOCK:
					throw flushSocketEx(FLUSHSOCKET_NOT_A_SOCKET,(flushSocket *)this,__LINE__,__FILE__);
			}
		#else
			switch (errno)
			{
				case EBADF:
				case EFAULT:
				case ENOPROTOOPT:
				case ENOTSOCK:
					return ;					
			}							
		#endif			
		
	}
}

//-------------------------------------------------------------------

void 
flushSocket::acceptTimeout()
{
	if (socket!=-1)
	{
		timeval val;
		
		val.tv_sec = sendTimeout/100;
		val.tv_usec = sendTimeout%100;
		if (setsockopt(socket,SOL_SOCKET,SO_SNDTIMEO,&val,sizeof(unsigned long))==-1)
		#ifndef NO_EX
			switch (errno)
			{
				case EBADF:
				case EFAULT:
				case ENOPROTOOPT:
					throw flushSocketEx(FLUSHSOCKET_SYSTEM_FAULT,(flushSocket *)this,__LINE__,__FILE__);
				case ENOTSOCK:
					throw flushSocketEx(FLUSHSOCKET_NOT_A_SOCKET,(flushSocket *)this,__LINE__,__FILE__);
			}
		#else
			switch (errno)
			{
				case EBADF:
				case EFAULT:
				case ENOPROTOOPT:
				case ENOTSOCK:
					return ;					
			}							
		#endif
		
		val.tv_sec = recieveTimeout/100;
		val.tv_usec = recieveTimeout%100;
		if (setsockopt(socket,SOL_SOCKET,SO_RCVBUF,&val,sizeof(unsigned long))==-1)
		#ifndef NO_EX
			switch (errno)
			{
				case EBADF:
				case EFAULT:
				case ENOPROTOOPT:
					throw flushSocketEx(FLUSHSOCKET_SYSTEM_FAULT,(flushSocket *)this,__LINE__,__FILE__);
				case ENOTSOCK:
					throw flushSocketEx(FLUSHSOCKET_NOT_A_SOCKET,(flushSocket *)this,__LINE__,__FILE__);
			}
		#else
			switch (errno)
			{
				case EBADF:
				case EFAULT:
				case ENOPROTOOPT:
				case ENOTSOCK:
					return ;					
			}							
		#endif			
		
	}	
}

//-------------------------------------------------------------------
