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
 
#include <flushSocket.h>

using namespace dodo;

flushSocket::flushSocket(flushSocket &fs)
{
}

//-------------------------------------------------------------------

flushSocket::flushSocket(bool a_server, 
						socketProtoFamilyEnum a_family, 
						socketTransferTypeEnum a_type) : flushSocketOptions(a_family,a_type),
						blockInherited(false),
						server(a_server)
{
	makeSocket();
}


//-------------------------------------------------------------------

flushSocket::~flushSocket()
{
	if (server && type==TRANSFER_TYPE_STREAM)
		if (opened)
		{
			::shutdown(socket,SHUT_RDWR);		
			::close(socket);
		}
	
	if (server && unixSock.size()!=0)
		flushDiskTools::unlink(unixSock);
}

//-------------------------------------------------------------------

#ifndef FLUSH_SOCKET_WO_XEXEC

	int 
	flushSocket::addPostExec(inExec func, 
						void *data) const
	{
		return _addPostExec(func, (void *)this, XEXECOBJ_FLUSHSOCKET, data);
	}
	
	//-------------------------------------------------------------------
	
	int 
	flushSocket::addPreExec(inExec func, 
						void *data) const
	{
		return _addPreExec(func, (void *)this, XEXECOBJ_FLUSHSOCKET, data);
	}
	
	//-------------------------------------------------------------------
	
	#ifdef DL_EXT
	
		int 
		flushSocket::addPostExec(const std::string &module, 
							void *data,
							void *toInit) const
		{
			return _addPostExec(module, (void *)this, XEXECOBJ_FLUSHSOCKET, data, toInit);
		}
		
		//-------------------------------------------------------------------
		
		int 
		flushSocket::addPreExec(const std::string &module, 
							void *data,
							void *toInit) const
		{
			return _addPreExec(module, (void *)this, XEXECOBJ_FLUSHSOCKET, data, toInit);
		}
		
		//-------------------------------------------------------------------
		
		xexecCounts 
		flushSocket::addExec(const std::string &module, 
							void *data,
							void *toInit) const
		{
			return _addExec(module, (void *)this, XEXECOBJ_FLUSHSOCKET, data, toInit);
		}
	
	#endif

#endif

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
flushSocket::makeSocket()
{
	int real_domain(PF_INET), real_type(TRANSFER_TYPE_STREAM);
	
	switch (family)
	{
		case PROTO_FAMILY_IPV4:
			real_domain = PF_INET;
			break;
		case PROTO_FAMILY_IPV6:
			real_domain = PF_INET6;
			break;
		case PROTO_FAMILY_UNIX_SOCKET:
			real_domain = PF_UNIX;
			break;
		default:
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_MAKESOCKET,ERR_LIBDODO,FLUSHSOCKET_WRONG_PARAMETHER,FLUSHSOCKET_WRONG_PARAMETHER_STR,__LINE__,__FILE__);
			#else
				return false;			
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
		default:
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_MAKESOCKET,ERR_LIBDODO,FLUSHSOCKET_WRONG_PARAMETHER,FLUSHSOCKET_WRONG_PARAMETHER_STR,__LINE__,__FILE__);
			#else
				return false;			
			#endif	
	}	
	
	socket = ::socket(real_domain,real_type,0);
	if (socket == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_MAKESOCKET,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
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
					int port, 
					flushSocketExchange &exchange)
{			
	#ifndef FLUSH_SOCKET_WO_XEXEC
		operType = FLUSHSOCKET_OPER_CONNECT;
		performXExec(preExec);
	#endif
	
	if (server)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_CONNECT,ERR_LIBDODO,FLUSHSOCKET_CANNOT_CONNECT,FLUSHSOCKET_CANNOT_CONNECT_STR,__LINE__,__FILE__);
		#else
			return false;
		#endif
	
	if (opened)
	{
		opened = false;
		makeSocket();
	}
	
	if (family == PROTO_FAMILY_IPV6)
	{
		struct sockaddr_in6 sa;
		sa.sin6_family = AF_INET6;
		sa.sin6_port = htons(port);
		sa.sin6_flowinfo = 0;
		sa.sin6_scope_id = 0;
		inet_pton(AF_INET6,host.c_str(),&sa.sin6_addr);
		
		if (::connect(socket,(struct sockaddr *)&sa,sizeof(sa))==-1)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_CONNECT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else			
				return false;		
			#endif		
	}
	else
	{
		struct sockaddr_in sa;
		sa.sin_family = AF_INET;
		sa.sin_port = htons(port);
		inet_aton(host.c_str(),&sa.sin_addr);
		
		if (::connect(socket,(struct sockaddr *)&sa,sizeof(sa))==-1)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_CONNECT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else			
				return false;		
			#endif
	}
	
	exchange.blocked = blocked;
	exchange.init(socket,blockInherited);
	
	opened = true;
			
	#ifndef FLUSH_SOCKET_WO_XEXEC		
		performXExec(postExec);
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
flushSocket::connect(const __connInfo &destinaton, 
					flushSocketExchange &exchange)
{
	#ifdef NO_EX
		if (!connect(destinaton.host,destinaton.port,exchange))
			return false;
	#else
		connect(destinaton.host,destinaton.port,exchange);
	#endif
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif 
flushSocket::connect(const std::string &path, 
				flushSocketExchange &exchange)
{		
	#ifndef FLUSH_SOCKET_WO_XEXEC
		operType = FLUSHSOCKET_OPER_CONNECT_UNIX;
		performXExec(preExec);
	#endif
	
	if (server)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_CONNECT,ERR_LIBDODO,FLUSHSOCKET_CANNOT_CONNECT,FLUSHSOCKET_CANNOT_CONNECT_STR,__LINE__,__FILE__);
		#else
			return false;
		#endif

	if (opened)
	{
		opened = false;
		makeSocket();
	}	
	
	struct sockaddr_un sa;
	
	strcpy(sa.sun_path,path.c_str());
	sa.sun_family = AF_UNIX;

	if (::connect(socket,(struct sockaddr *)&sa,path.size()+sizeof(sa.sun_family))==-1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_CONNECT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else			
			return false;		
		#endif	
		
	exchange.blocked = blocked;
	exchange.init(socket,blockInherited);

	opened = true;
		
	#ifndef FLUSH_SOCKET_WO_XEXEC		
		performXExec(postExec);
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
flushSocket::bindNListen(const std::string &host, 
						int port,
						int numberOfConnections)
{		
	#ifndef FLUSH_SOCKET_WO_XEXEC
		operType = FLUSHSOCKET_OPER_BINDNLISTEN;
		performXExec(preExec);
	#endif
			
	if (!server)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_BINDNLISTEN,ERR_LIBDODO,FLUSHSOCKET_CANNOT_BIND,FLUSHSOCKET_CANNOT_BIND_STR,__LINE__,__FILE__);
		#else
			return false;
		#endif

	if (opened)
	{
		opened = false;
		makeSocket();
	}
	
	setLingerSockOption(SOCKET_LINGER_OPTION,SOCKET_LINGER_PERIOD);
	
	if (family == PROTO_FAMILY_IPV6)
	{
		struct sockaddr_in6 sa;
		sa.sin6_family = AF_INET6;
		sa.sin6_port = htons(port);
		sa.sin6_flowinfo = 0;
		sa.sin6_scope_id = 0;
		if (strcmp(host.c_str(),"*") == 0)
			sa.sin6_addr = in6addr_any;
		else
			inet_pton(AF_INET6,host.c_str(),&sa.sin6_addr);
		
		if (::bind(socket,(struct sockaddr *)&sa,sizeof(sa))==-1)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_BINDNLISTEN,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;
			#endif
	}
	else
	{
		struct sockaddr_in sa;
		
		sa.sin_family = AF_INET;
		sa.sin_port = htons(port);
		if (strcmp(host.c_str(),"*") == 0)
			sa.sin_addr.s_addr = htonl(INADDR_ANY);
		else
			inet_pton(AF_INET,host.c_str(),&sa.sin_addr);

		if (::bind(socket,(struct sockaddr *)&sa,sizeof(sa))==-1)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_BINDNLISTEN,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;
			#endif			
	}	

	if (type == TRANSFER_TYPE_STREAM)
		if (::listen(socket,numberOfConnections)==-1)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_BINDNLISTEN,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;
			#endif		
	
	opened = true;
			
	#ifndef FLUSH_SOCKET_WO_XEXEC		
		performXExec(postExec);
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
flushSocket::bindNListen(const __connInfo &destinaton, 
						int numberOfConnections) 
{
	#ifdef NO_EX
		if (!bindNListen(destinaton.host,destinaton.port,numberOfConnections))
			return false;
	#else
		bindNListen(destinaton.host,destinaton.port,numberOfConnections);
	#endif
}

//-------------------------------------------------------------------
	
#ifndef NO_EX
	void 
#else
	bool
#endif
flushSocket::bindNListen(const std::string &path,
							int numberOfConnections,
							bool force)
	{		
		#ifndef FLUSH_SOCKET_WO_XEXEC
			operType = FLUSHSOCKET_OPER_CONNECT_UNIX;
			performXExec(preExec);
		#endif
		
		if (!server)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_BINDNLISTEN,ERR_LIBDODO,FLUSHSOCKET_CANNOT_BIND,FLUSHSOCKET_CANNOT_BIND_STR,__LINE__,__FILE__);
			#else
				return false;
			#endif

		if (opened)
		{
			opened = false;
			makeSocket();
		}

		if (force)
		{
			struct stat st;
            if (::lstat(path.c_str(),&st) != -1)
				if (S_ISSOCK(st.st_mode))
					flushDiskTools::unlink(path,true);
				else
					#ifndef NO_EX
						throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_MAKEUNIXSOCKET,ERR_LIBDODO,FLUSHSOCKET_WRONG_FILENAME,FLUSHSOCKET_WRONG_FILENAME_STR,__LINE__,__FILE__);
					#else
						return false;	
					#endif
		}
		
		setLingerSockOption(SOCKET_LINGER_OPTION,SOCKET_LINGER_PERIOD);
		
		struct sockaddr_un sa;
		
		strcpy(sa.sun_path,path.c_str());
		sa.sun_family = AF_UNIX;	
		
		if (::bind(socket,(struct sockaddr *)&sa,path.size()+sizeof(sa.sun_family))==-1)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_BINDNLISTEN,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;
			#endif
	
		if (::listen(socket,numberOfConnections)==-1)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_BINDNLISTEN,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;
			#endif	
		
		unixSock = path;
		
		opened = true;
			
		#ifndef FLUSH_SOCKET_WO_XEXEC		
			performXExec(postExec);
		#endif
			
		#ifdef NO_EX
			return true;
		#endif		
	}

//-------------------------------------------------------------------

bool 
flushSocket::accept(__initialAccept &init, 
					__connInfo &info) const
{		
	#ifndef FLUSH_SOCKET_WO_XEXEC
		operType = FLUSHSOCKET_OPER_ACCEPT;
		performXExec(preExec);
	#endif
	
	if (!server)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_ACCEPT,ERR_LIBDODO,FLUSHSOCKET_CANNOT_ACCEPT,FLUSHSOCKET_CANNOT_ACCEPT_STR,__LINE__,__FILE__);
		#else
			return false;
		#endif
		
	if (type != TRANSFER_TYPE_STREAM)
	{
		init.socket = socket;
		init.type = type;
		init.family = family;
		init.blocked = blocked;
		init.blockInherited = blockInherited;
			
		return true;
	}
			
	if (!opened)	
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_ACCEPT,ERR_LIBDODO,FLUSHSOCKET_ACCEPT_WO_BIND,FLUSHSOCKET_ACCEPT_WO_BIND_STR,__LINE__,__FILE__);
		#else
			return false;
		#endif
	
	register int sock(-1);
	info.host.clear();
	
	switch (family)
	{
		case PROTO_FAMILY_IPV4:
			{
				struct sockaddr_in sa;
				register socklen_t len = sizeof(sockaddr_in);
				sock = ::accept(socket,(sockaddr *)&sa,&len);
				
				if (sock == -1)
					#ifndef NO_EX
					{
						if (errno == EINVAL || errno == EWOULDBLOCK)
							return false;
						else
							throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_ACCEPT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
					}
					#else
						return false;
					#endif
				
				char temp[INET_ADDRSTRLEN];
				if (inet_ntop(AF_INET,&(sa.sin_addr),temp,INET_ADDRSTRLEN) != NULL)
					info.host.assign(temp);
				info.port = ntohs(sa.sin_port);
			}
				
			break;
			
		case PROTO_FAMILY_IPV6:
			{
				struct sockaddr_in6 sa;
				register socklen_t len = sizeof(sockaddr_in6);
				
				sock = ::accept(socket,(sockaddr *)&sa,&len);
	
				if (sock == -1)
					#ifndef NO_EX
					{
						if (errno == EINVAL || errno == EWOULDBLOCK)
							return false;
						else
							throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_ACCEPT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
					}
					#else
						return false;
					#endif
									
				char temp[INET6_ADDRSTRLEN];
				if (inet_ntop(AF_INET6,&(sa.sin6_addr),temp,INET6_ADDRSTRLEN) != NULL)
					info.host.assign(temp);
				info.port = ntohs(sa.sin6_port);	
			}
			break;
			
		case PROTO_FAMILY_UNIX_SOCKET:
			sock = ::accept(socket,NULL,NULL);
			if (sock == -1)
				#ifndef NO_EX
					{
						if (errno == EINVAL || errno == EWOULDBLOCK)
							return false;
						else
							throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_ACCEPT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
					}
				#else
					return false;
				#endif				
			break;
			
		default:
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_ACCEPT,ERR_LIBDODO,FLUSHSOCKET_WRONG_PARAMETHER,FLUSHSOCKET_WRONG_PARAMETHER_STR,__LINE__,__FILE__);
			#else
				return false;			
			#endif	
	}
	
	init.socket = sock;
	init.type = type;
	init.family = family;
	init.blocked = blocked;
	init.blockInherited = blockInherited;
					
	#ifndef FLUSH_SOCKET_WO_XEXEC		
		performXExec(postExec);
	#endif
	
	return true;
}

//-------------------------------------------------------------------

bool 
flushSocket::accept(__initialAccept &init) const
{		
	#ifndef FLUSH_SOCKET_WO_XEXEC
		operType = FLUSHSOCKET_OPER_ACCEPT;
		performXExec(preExec);
	#endif
	
	if (!server)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_ACCEPT,ERR_LIBDODO,FLUSHSOCKET_CANNOT_ACCEPT,FLUSHSOCKET_CANNOT_ACCEPT_STR,__LINE__,__FILE__);
		#else
			return false;
		#endif
		
	if (type != TRANSFER_TYPE_STREAM)
	{
		init.socket = socket;
		init.type = type;
		init.family = family;
		init.blocked = blocked;
		init.blockInherited = blockInherited;
		
		return true;
	}			
	
	if (!opened)	
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_ACCEPT,ERR_LIBDODO,FLUSHSOCKET_ACCEPT_WO_BIND,FLUSHSOCKET_ACCEPT_WO_BIND_STR,__LINE__,__FILE__);
		#else
			return false;
		#endif
	
	register int sock = ::accept(socket,NULL,NULL);
	if (sock == -1)
		#ifndef NO_EX
		{
			if (errno == EINVAL || errno == EWOULDBLOCK)
				return false;
			else
				throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_ACCEPT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		}
		#else
			return false;
		#endif
	
	init.socket = sock;
	init.type = type;
	init.family = family;
	init.blocked = blocked;
	init.blockInherited = blockInherited;
	
	#ifndef FLUSH_SOCKET_WO_XEXEC		
		performXExec(postExec);
	#endif
	
	return true;
}

//-------------------------------------------------------------------
