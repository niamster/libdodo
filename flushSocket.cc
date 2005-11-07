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

flushDisk dummyDiskExSocket;///to throw ex in static members
flushSocket dummySocketEx;///to throw ex in static members

//-------------------------------------------------------------------

dodoBase *
flushSocket::getSelf()
{
	return dynamic_cast<dodoBase *>(this);
}

//-------------------------------------------------------------------

flushSocket::flushSocket() : numberOfConn(-1),
						socket(-1)
{
}

//-------------------------------------------------------------------

flushSocket::flushSocket(unsigned long a_numberOfConn, 
						socketProtoFamilyEnum a_family, 
						socketTransferTypeEnum a_type, 
						unsigned int a_protocol) : numberOfConn(a_numberOfConn),
						family(a_family),
						type(a_type),
						protocol(a_protocol),
						socket(-1),
						inTimeout(RECIEVE_TIMEOUT),
						outTimeout(SEND_TIMEOUT),
						inSocketBuffer(SOCKET_INSIZE),
						outSocketBuffer(SOCKET_OUTSIZE)
{
	connections = new int [numberOfConn];
}

//-------------------------------------------------------------------

flushSocket::flushSocket(socketProtoFamilyEnum a_family, 
						socketTransferTypeEnum a_type, 
						unsigned int a_protocol) : numberOfConn(-1),
						family(a_family),
						type(a_type),
						protocol(a_protocol),
						socket(-1),
						inTimeout(RECIEVE_TIMEOUT),
						outTimeout(SEND_TIMEOUT),
						inSocketBuffer(SOCKET_INSIZE),
						outSocketBuffer(SOCKET_OUTSIZE)
						
{
	connections = new int [0];
}

//-------------------------------------------------------------------

flushSocket::~flushSocket()
{
	delete [] connections;
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
flushSocket::init()
{
	makeSocket(family,type,protocol);
	
	setInBufferSize(inSocketBuffer);
	setOutBufferSize(outSocketBuffer);
	
	setInTimeout(inTimeout);	
	setOutTimeout(outTimeout);
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
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
			default:
				throw flushSocketEx(FLUSHSOCKET_UNKNOWN,(flushSocket *)this,__LINE__,__FILE__);
		}
	#else
		return false;			
	#endif	
		
	#ifdef NO_EX
		return true;
	#endif
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
flushSocket::connect(const std::string &host, 
					unsigned int port)
{
	if (socket == -1)
		#ifdef NO_EX
			return false;
		#else
			throw flushSocketEx(FLUSHSOCKET_NOT_A_SOCKET,(flushSocket *)this,__LINE__,__FILE__);
		#endif
	
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
				default:
					throw flushSocketEx(FLUSHSOCKET_UNKNOWN,(flushSocket *)this,__LINE__,__FILE__);
			}
		#else			
			return false;		
		#endif
	
	opened = true;
		
	#ifdef NO_EX
		return true;
	#endif
}

//-------------------------------------------------------------------

#ifndef WIN

	#ifndef NO_EX
		void 
	#else
		bool
	#endif
	flushSocket::mkUnixSocket(const std::string &path)
	{
		struct stat st;
		if (::lstat(path.c_str(),&st) == -1)
			#ifndef NO_EX
				switch (errno)
				{
					case EACCES:
						throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,&dummyDiskExSocket,__LINE__,__FILE__);
					case EIO:
						throw flushDiskEx(FLUSHDISK_IOERROR,&dummyDiskExSocket,__LINE__,__FILE__);
					case EINTR:
						throw flushDiskEx(FLUSHDISK_INTERRUPTED, &dummyDiskExSocket,__LINE__,__FILE__);
					case ENAMETOOLONG:
						throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,&dummyDiskExSocket,__LINE__,__FILE__);
					case ELOOP:
					case ENOTDIR:
						throw flushDiskEx(FLUSHDISK_WRONG_FILE_NAME,&dummyDiskExSocket,__LINE__,__FILE__);	
					case ENOMEM:
						throw flushDiskEx(FLUSHDISK_MEMORY_OVER,&dummyDiskExSocket,__LINE__,__FILE__);
					default:
						throw flushDiskEx(FLUSHDISK_UNKNOWN,&dummyDiskExSocket,__LINE__,__FILE__);
				}
			#else
				return false;		
			#endif
		if (errno != ENOENT)
		{
			if (!S_ISSOCK(st.st_mode))
				#ifndef NO_EX
					throw flushSocketEx(FLUSHSOCKET_NOT_A_SOCKET,(flushSocket *)this,__LINE__,__FILE__);
				#else
					return false;
				#endif
		}
		else
			if (mknod(path.c_str(),flushDisk::getPermission(UNIX_SOCKET_PERM),S_IFSOCK)==-1)
				#ifndef NO_EX
					switch (errno)
					{
						case EACCES:
							throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,&dummyDiskExSocket,__LINE__,__FILE__);
						case EIO:
							throw flushDiskEx(FLUSHDISK_IOERROR,&dummyDiskExSocket,__LINE__,__FILE__);
						case EINTR:
							throw flushDiskEx(FLUSHDISK_INTERRUPTED,&dummyDiskExSocket,__LINE__,__FILE__);
						case ENOSPC:
						case ENOMEM:
							throw flushDiskEx(FLUSHDISK_NOT_ENOUGH_FREE_SPACE,&dummyDiskExSocket,__LINE__,__FILE__);
						case EMFILE:
						case ENFILE:
							throw flushDiskEx(FLUSHDISK_TOO_MANY_OPEN_FILE,&dummyDiskExSocket,__LINE__,__FILE__);
						case ENAMETOOLONG:
							throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,&dummyDiskExSocket,__LINE__,__FILE__);
						case ENOENT:
							throw flushDiskEx(FLUSHDISK_NO_SUCH_FILE,&dummyDiskExSocket,__LINE__,__FILE__);	
						case EROFS:
							throw flushDiskEx(FLUSHDISK_READ_ONLY_FS,&dummyDiskExSocket,__LINE__,__FILE__);	
						default:
							throw flushDiskEx(FLUSHDISK_UNKNOWN,&dummyDiskExSocket,__LINE__,__FILE__);	
					}
				#else
					return false;
				#endif				
		
		#ifdef NO_EX
			return true;
		#endif			
	}

//-------------------------------------------------------------------
	
	#ifndef NO_EX
		void 
	#else
		bool
	#endif 
	flushSocket::connect(const std::string &path)
	{
		if (socket == -1)
			#ifdef NO_EX
				return false;
			#else
				throw flushSocketEx(FLUSHSOCKET_NOT_A_SOCKET,(flushSocket *)this,__LINE__,__FILE__);
			#endif
	
		#ifdef NO_EX
			if (!mkUnixSocket(path))
				return false;
		#else
			mkUnixSocket(path);
		#endif
		
		struct sockaddr_un sa;
		
		strcpy(sa.sun_path,path.c_str());
		sa.sun_family = AF_UNIX;
	
		if (::connect(socket,(struct sockaddr *)&sa,path.size()+sizeof(sa.sun_family))==-1)
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
					default:
						throw flushSocketEx(FLUSHSOCKET_UNKNOWN,(flushSocket *)this,__LINE__,__FILE__);
				}
			#else			
				return false;		
			#endif	
		
		opened = true;
		
		#ifdef NO_EX
			return true;
		#endif	
	}

#endif

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
flushSocket::setInBufferSize(int bytes)
{
	if (socket == -1)
		#ifdef NO_EX
			return false;
		#else
			throw flushSocketEx(FLUSHSOCKET_NOT_A_SOCKET,(flushSocket *)this,__LINE__,__FILE__);
		#endif
	
	inSocketBuffer = bytes;
	
	if (setsockopt(socket,SOL_SOCKET,SO_RCVBUF,&inSocketBuffer,size_of_long)==-1)
		#ifndef NO_EX
			switch (errno)
			{
				case EBADF:
				case EFAULT:
				case ENOPROTOOPT:
					throw flushSocketEx(FLUSHSOCKET_SYSTEM_FAULT,(flushSocket *)this,__LINE__,__FILE__);
				case ENOTSOCK:
					throw flushSocketEx(FLUSHSOCKET_NOT_A_SOCKET,(flushSocket *)this,__LINE__,__FILE__);
				default:
					throw flushSocketEx(FLUSHSOCKET_UNKNOWN,(flushSocket *)this,__LINE__,__FILE__);
			}
		#else
			return false;		
		#endif	
		
	#ifdef NO_EX
		return true;
	#endif			
}

//-------------------------------------------------------------------

int
flushSocket::getInBufferSize()
{
	return inSocketBuffer;
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
flushSocket::setOutBufferSize(int bytes)
{
	if (socket == -1)
		#ifdef NO_EX
			return false;
		#else
			throw flushSocketEx(FLUSHSOCKET_NOT_A_SOCKET,(flushSocket *)this,__LINE__,__FILE__);
		#endif
	
	outSocketBuffer = bytes;
	
	if (setsockopt(socket,SOL_SOCKET,SO_SNDBUF,&outSocketBuffer,size_of_long)==-1)
		#ifndef NO_EX
			switch (errno)
			{
				case EBADF:
				case EFAULT:
				case ENOPROTOOPT:
					throw flushSocketEx(FLUSHSOCKET_SYSTEM_FAULT,(flushSocket *)this,__LINE__,__FILE__);
				case ENOTSOCK:
					throw flushSocketEx(FLUSHSOCKET_NOT_A_SOCKET,(flushSocket *)this,__LINE__,__FILE__);
				default:
					throw flushSocketEx(FLUSHSOCKET_UNKNOWN,(flushSocket *)this,__LINE__,__FILE__);
			}
		#else
			return false;		
		#endif	
		
	#ifdef NO_EX
		return true;
	#endif				
}

//-------------------------------------------------------------------

int
flushSocket::getOutBufferSize()
{
	return outSocketBuffer;
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif 
flushSocket::setInTimeout(unsigned long microseconds)
{
	if (socket == -1)
		#ifdef NO_EX
			return false;
		#else
			throw flushSocketEx(FLUSHSOCKET_NOT_A_SOCKET,(flushSocket *)this,__LINE__,__FILE__);
		#endif

	inTimeout = microseconds;

	timeval val;
	val.tv_sec = inTimeout/100;
	val.tv_usec = inTimeout%100;
	
	if (setsockopt(socket,SOL_SOCKET,SO_SNDTIMEO,&val,size_of_long)==-1)
		#ifndef NO_EX
			switch (errno)
			{
				case EBADF:
				case EFAULT:
				case ENOPROTOOPT:
					throw flushSocketEx(FLUSHSOCKET_SYSTEM_FAULT,(flushSocket *)this,__LINE__,__FILE__);
				case ENOTSOCK:
					throw flushSocketEx(FLUSHSOCKET_NOT_A_SOCKET,(flushSocket *)this,__LINE__,__FILE__);
				default:
					throw flushSocketEx(FLUSHSOCKET_UNKNOWN,(flushSocket *)this,__LINE__,__FILE__);
			}
		#else
			return false;	
		#endif	
		
	#ifdef NO_EX
		return true;
	#endif	
}


//-------------------------------------------------------------------

unsigned long 
flushSocket::getInTimeout()
{
	return inTimeout;
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
flushSocket::setOutTimeout(unsigned long microseconds)
{
	if (socket == -1)
		#ifdef NO_EX
			return false;
		#else
			throw flushSocketEx(FLUSHSOCKET_NOT_A_SOCKET,(flushSocket *)this,__LINE__,__FILE__);
		#endif

	outTimeout = microseconds;

	timeval val;
	val.tv_sec = inTimeout/100;
	val.tv_usec = inTimeout%100;
	
	if (setsockopt(socket,SOL_SOCKET,SO_SNDTIMEO,&val,size_of_long)==-1)
		#ifndef NO_EX
			switch (errno)
			{
				case EBADF:
				case EFAULT:
				case ENOPROTOOPT:
					throw flushSocketEx(FLUSHSOCKET_SYSTEM_FAULT,(flushSocket *)this,__LINE__,__FILE__);
				case ENOTSOCK:
					throw flushSocketEx(FLUSHSOCKET_NOT_A_SOCKET,(flushSocket *)this,__LINE__,__FILE__);
				default:
					throw flushSocketEx(FLUSHSOCKET_UNKNOWN,(flushSocket *)this,__LINE__,__FILE__);
			}
		#else
			return false;		
		#endif	
		
	#ifdef NO_EX
		return true;
	#endif	
}


//-------------------------------------------------------------------

unsigned long 
flushSocket::getOutTimeout()
{
	return outTimeout;
}

//-------------------------------------------------------------------

bool
flushSocket::getSocketOpts(int option)
{
	if  ( (option&socketOpts) == option)
		return true;
	return false;
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
flushSocket::setSockOption(socketOptionsEnum option, 
							bool flag)
{
	if (socket == -1)
		#ifdef NO_EX
			return false;
		#else
			throw flushSocketEx(FLUSHSOCKET_NOT_A_SOCKET,(flushSocket *)this,__LINE__,__FILE__);
		#endif
	
	register int sockFlag(1);
	
	if (!flag)
		sockFlag = 0;
	
	register int real_option;
	
	switch (option)
	{
		case SOCKET_KEEP_ALIVE:
			real_option = SO_KEEPALIVE;
			break;
		case SOCKET_REUSE_ADDRESS:
			real_option = SO_REUSEADDR;
			break;
		case SOCKET_DONOT_USE_GATEWAY:
			real_option = SO_DONTROUTE;
			break;
		case SOCKET_BROADCAST:
			real_option = SO_BROADCAST;
			break;
		case SOCKET_OOB_INLINE:
			real_option = SO_OOBINLINE;
			break;
		#ifdef SO_REUSEPORT	
			case SOCKET_REUSE_PORT:
				real_option = SO_REUSEPORT;
				break;
		#endif
	}
	
		
	if (setsockopt(socket,SOL_SOCKET,real_option,&sockFlag,size_of_int)==-1)
		#ifndef NO_EX
			switch (errno)
			{
				case EBADF:
				case EFAULT:
				case ENOPROTOOPT:
					throw flushSocketEx(FLUSHSOCKET_SYSTEM_FAULT,(flushSocket *)this,__LINE__,__FILE__);
				case ENOTSOCK:
					throw flushSocketEx(FLUSHSOCKET_NOT_A_SOCKET,(flushSocket *)this,__LINE__,__FILE__);
				default:
					throw flushSocketEx(FLUSHSOCKET_UNKNOWN,(flushSocket *)this,__LINE__,__FILE__);
			}
		#else
			return false;	
		#endif	
	
	if (!flag)
		removeF(socketOpts,option);
	else
		addF(socketOpts,option);	
		
	#ifdef NO_EX
		return true;
	#endif					
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
flushSocket::setLingerSocketOption(socketLingerOption option,
									int seconds)
{	
	if (socket == -1)
		#ifdef NO_EX
			return false;
		#else
			throw flushSocketEx(FLUSHSOCKET_NOT_A_SOCKET,(flushSocket *)this,__LINE__,__FILE__);
		#endif
		
	linger lin;
	
	switch (option)
	{
		case SOCKET_GRACEFUL_CLOSE:
			lin.l_onoff = 0;
			break;
		case SOCKET_HARD_CLOSE:
			lin.l_onoff = 1;
			lin.l_linger = 0;
			break;
		case SOCKET_WAIT_CLOSE:
			lin.l_onoff = 1;
			lin.l_linger = seconds;
			break;
	}

	if (setsockopt(socket,SOL_SOCKET,SO_LINGER,&lin,sizeof(linger))==-1)
		#ifndef NO_EX
			switch (errno)
			{
				case EBADF:
				case EFAULT:
				case ENOPROTOOPT:
					throw flushSocketEx(FLUSHSOCKET_SYSTEM_FAULT,(flushSocket *)this,__LINE__,__FILE__);
				case ENOTSOCK:
					throw flushSocketEx(FLUSHSOCKET_NOT_A_SOCKET,(flushSocket *)this,__LINE__,__FILE__);
				default:
					throw flushSocketEx(FLUSHSOCKET_UNKNOWN,(flushSocket *)this,__LINE__,__FILE__);
			}
		#else
			return false;	
		#endif
	
	removeF(socketOpts,SOCKET_GRACEFUL_CLOSE);
	removeF(socketOpts,SOCKET_HARD_CLOSE);
	removeF(socketOpts,SOCKET_WAIT_CLOSE);
	
	addF(socketOpts,option);	
		
	#ifdef NO_EX
		return true;
	#endif	
}

//-------------------------------------------------------------------

__hostInfo 
flushSocket::getHostInfo(std::string &host)
{
	hostent *ent = gethostbyname(host.c_str());
	
	__hostInfo info;
	
	if (ent == NULL)
		#ifndef NO_EX
			switch (h_errno)
			{
				case HOST_NOT_FOUND:
				case NO_ADDRESS:
					throw flushSocketEx(FLUSHSOCKET_NOT_FOUND,&dummySocketEx,__LINE__,__FILE__);
				case NO_RECOVERY:
				case TRY_AGAIN:
					throw flushSocketEx(FLUSHSOCKET_NAMESERVER_ERR,&dummySocketEx,__LINE__,__FILE__);
				default:
					throw flushSocketEx(FLUSHSOCKET_UNKNOWN,&dummySocketEx,__LINE__,__FILE__);
			}
		#else
			return info;		
		#endif		
	
	info.name = ent->h_name;

	register int i(0);

	while (ent->h_aliases[i] != NULL)	
		info.aliases.push_back(ent->h_aliases[i++]);
	
	i = 0;	
	while (ent->h_addr_list[i] != NULL)	
		info.addresses.push_back(ent->h_addr_list[i++]);
	
	return info;	
}

//-------------------------------------------------------------------

__servInfo 
flushSocket::getServiceInfo(std::string &host, 
						std::string &protocol)
{
	servent *ent = getservbyname(host.c_str(), protocol.c_str());
	
	__servInfo info;
	
	if (ent == NULL)
		return info;
	
	info.name = ent->s_name;
	info.port = ent->s_port;
		
		register int i(0);

	while (ent->s_aliases[i] != NULL)	
		info.aliases.push_back(ent->s_aliases[i++]);
		
	return info;	
}

//-------------------------------------------------------------------

__servInfo 
flushSocket::getServiceInfo(int port, 
						std::string &protocol)
{
	servent *ent = getservbyport(port, protocol.c_str());
	
	__servInfo info;
	
	if (ent == NULL)
		return info;
	
	info.name = ent->s_name;
	info.port = ent->s_port;
		
		register int i(0);

	while (ent->s_aliases[i] != NULL)	
		info.aliases.push_back(ent->s_aliases[i++]);
		
	return info;	
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
flushSocket::bindNListen(const std::string &host, unsigned int port)
{
	if (socket == -1)
		#ifdef NO_EX
			return false;
		#else
			throw flushSocketEx(FLUSHSOCKET_NOT_A_SOCKET,(flushSocket *)this,__LINE__,__FILE__);
		#endif
	
	struct sockaddr_in sa;
	
	if (family == PROTO_FAMILY_IPV6)
		sa.sin_family = AF_INET6;
	else
		sa.sin_family = AF_INET;
	sa.sin_port = htonl(port);
	inet_aton(host.c_str(),&sa.sin_addr);
	
	if (::bind(socket,(struct sockaddr *)&sa,sizeof(sa))==-1)
		#ifndef NO_EX
			switch (errno)
			{
				case EAFNOSUPPORT:
					throw flushSocketEx(FLUSHSOCKET_NOT_SUPPORTED_ADDR_FAMILY,(flushSocket *)this,__LINE__,__FILE__);
				case EACCES:
					throw flushSocketEx(FLUSHSOCKET_ACCESS_DENIED,(flushSocket *)this,__LINE__,__FILE__);
				case EINVAL:
				case EADDRINUSE:
					throw flushSocketEx(FLUSHSOCKET_ALREADY_USED,(flushSocket *)this,__LINE__,__FILE__);
				case EBADF:
					throw flushSocketEx(FLUSHSOCKET_SYSTEM_FAULT,(flushSocket *)this,__LINE__,__FILE__);
				case EADDRNOTAVAIL:
					throw flushSocketEx(FLUSHSOCKET_NETWORK_UNREACHABLE,(flushSocket *)this,__LINE__,__FILE__);
				case ENOTSOCK:
					throw flushSocketEx(FLUSHSOCKET_NOT_A_SOCKET,(flushSocket *)this,__LINE__,__FILE__);
				default:
					throw flushSocketEx(FLUSHSOCKET_UNKNOWN,(flushSocket *)this,__LINE__,__FILE__);
			}
		#else
			return false;
		#endif

	if (::listen(socket,numberOfConn)==-1)
		#ifndef NO_EX
			switch (errno)
			{
				case EAFNOSUPPORT:
					throw flushSocketEx(FLUSHSOCKET_NOT_SUPPORTED_ADDR_FAMILY,(flushSocket *)this,__LINE__,__FILE__);
				case EACCES:
					throw flushSocketEx(FLUSHSOCKET_ACCESS_DENIED,(flushSocket *)this,__LINE__,__FILE__);
				case EINVAL:
				case EADDRINUSE:
					throw flushSocketEx(FLUSHSOCKET_ALREADY_USED,(flushSocket *)this,__LINE__,__FILE__);
				case EBADF:
					throw flushSocketEx(FLUSHSOCKET_SYSTEM_FAULT,(flushSocket *)this,__LINE__,__FILE__);
				case EADDRNOTAVAIL:
					throw flushSocketEx(FLUSHSOCKET_NETWORK_UNREACHABLE,(flushSocket *)this,__LINE__,__FILE__);
				case ENOTSOCK:
					throw flushSocketEx(FLUSHSOCKET_NOT_A_SOCKET,(flushSocket *)this,__LINE__,__FILE__);
				default:
					throw flushSocketEx(FLUSHSOCKET_UNKNOWN,(flushSocket *)this,__LINE__,__FILE__);
			}
		#else
			return false;
		#endif	
	
	opened = true;
		
	#ifdef NO_EX
		return true;
	#endif	
}

//-------------------------------------------------------------------
