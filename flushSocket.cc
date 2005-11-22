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

__initialAccept::__initialAccept() : socket(-1), 
									family((socketProtoFamilyEnum)-1), 
									type((socketTransferTypeEnum)-1)
{
}

//-------------------------------------------------------------------

__initialAccept::__initialAccept(__initialAccept &init) : socket(init.socket), family(init.family), type(init.type)
{
	init.socket = -1;
}

//-------------------------------------------------------------------

dodoBase * const 
flushSocket::getSelf()
{
	return dynamic_cast<dodoBase *>(this);
}

//-------------------------------------------------------------------

flushSocket::flushSocket(flushSocket &fs)
{
}

//-------------------------------------------------------------------

flushSocket::flushSocket(unsigned long a_numberOfConn, 
						socketProtoFamilyEnum a_family, 
						socketTransferTypeEnum a_type) : flushSocketOptions(a_family,a_type),
						numberOfConn(a_numberOfConn),
						accepted(a_numberOfConn)
{
	if (type == TRANSFER_TYPE_DATAGRAM)
		numberOfConn = 1;
	
	#ifdef WIN
		WSADATA wsaData;
  		WSAStartup(MAKEWORD(2,2), &wsaData);
  	#endif
}

//-------------------------------------------------------------------

flushSocket::flushSocket(socketProtoFamilyEnum a_family, 
						socketTransferTypeEnum a_type) : flushSocketOptions(a_family,a_type),
						numberOfConn(-1),
						accepted(0)
{
	#ifdef WIN
		WSADATA wsaData;
  		WSAStartup(MAKEWORD(2,2), &wsaData);
  	#endif	
}

//-------------------------------------------------------------------

flushSocket::~flushSocket()
{
	if (numberOfConn!=-1 && type==TRANSFER_TYPE_STREAM)
		if (opened)
			_close(socket);
	
	#ifdef WIN
		WSACleanup();
	#endif
}

//-------------------------------------------------------------------

int 
flushSocket::addPostExec(inExec func, 
					void *data) const
{
	return _addPostExec(func, (dodoBase *)this, data);
}

//-------------------------------------------------------------------

int 
flushSocket::addPreExec(inExec func, 
					void *data) const
{
	return _addPreExec(func, (dodoBase *)this, data);
}

//-------------------------------------------------------------------

int 
flushSocket::addPostExec(const std::string &module, 
					void *data) const
{
	return _addPostExec(module, (dodoBase *)this, data);
}

//-------------------------------------------------------------------

int 
flushSocket::addPreExec(const std::string &module, 
					void *data) const
{
	return _addPreExec(module, (dodoBase *)this, data);
}

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
		#ifndef WIN
			case PROTO_FAMILY_UNIX_SOCKET:
				real_domain = PF_UNIX;
				break;
		#endif
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
	#ifndef WIN
		if (socket == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_MAKESOCKET,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;			
		#endif	
	#else
		if (socket == INVALID_SOCKET)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_MAKESOCKET,ERR_WINSOCK,WSAGetLastError(),"Error at socket()",__LINE__,__FILE__);
		#else
			return false;			
		#endif	
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
					unsigned int port, 
					flushSocketExchange &exchange)
{			
	#ifndef FLUSH_SOCKET_WO_XEXEC
		operType = FLUSHSOCKET_OPER_CONNECT;
	#endif
	
	#ifndef FLUSH_SOCKET_WO_XEXEC
		performXExec(preExec);
	#endif
	
	if (numberOfConn != -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_CONNECT,ERR_LIBDODO,FLUSHSOCKET_CANNOT_CONNECT,FLUSHSOCKET_CANNOT_CONNECT_STR,__LINE__,__FILE__);
		#else
			return false;
		#endif
			
	#ifdef NO_EX
		if (!makeSocket())
			return false;
	#else
		makeSocket();
	#endif
	
	if (family == PROTO_FAMILY_IPV6)
	{
		struct sockaddr_in6 sa;
		sa.sin6_family = AF_INET6;
		sa.sin6_port = htons(port);
		sa.sin6_flowinfo = 0;
		sa.sin6_scope_id = 0;
		inet_pton(AF_INET6,host.c_str(),&sa.sin6_addr);
		
		#ifndef WIN
			if (::connect(socket,(struct sockaddr *)&sa,sizeof(sa))==-1)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_CONNECT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else			
					return false;		
				#endif		
		#else
			if (::connect(socket,(SOCKADDR *)&sa,sizeof(sa))==SOCKET_ERROR)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_CONNECT,ERR_WINSOCK,WSAGetLastError(),"Error at connect()",__LINE__,__FILE__);
				#else			
					return false;		
				#endif		
		#endif
	}
	else
	{
		struct sockaddr_in sa;
		sa.sin_family = AF_INET;
		sa.sin_port = htons(port);
		inet_aton(host.c_str(),&sa.sin_addr);
		
		#ifndef WIN
			if (::connect(socket,(struct sockaddr *)&sa,sizeof(sa))==-1)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_CONNECT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else			
					return false;		
				#endif
		#else
			if (::connect(socket,(SOCKADDR *)&sa,sizeof(sa))==SOCKET_ERROR)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_CONNECT,ERR_WINSOCK,WSAGetLastError(),"Error at connect()",__LINE__,__FILE__);
				#else			
					return false;		
				#endif		
		#endif
	}
	
	exchange.init(socket);
	
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

#ifndef WIN

	#ifndef NO_EX
		void 
	#else
		bool
	#endif
	flushSocket::makeUnixSocket(const std::string &path)
	{
		struct stat st;
		if (::lstat(path.c_str(),&st) == -1)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_MAKEUNIXSOCKET,ERR_LIBDODO,FLUSHSOCKET_NO_SOCKET_CREATED,FLUSHSOCKET_NO_SOCKET_CREATED_STR,__LINE__,__FILE__);
			#else
				return false;		
			#endif
		if (errno != ENOENT)
		{
			if (!S_ISSOCK(st.st_mode))
				#ifndef NO_EX
					throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_MAKEUNIXSOCKET,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else
					return false;
				#endif
		}
		else
			if (mknod(path.c_str(),flushDisk::getPermission(UNIX_SOCKET_PERM),S_IFSOCK)==-1)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_MAKEUNIXSOCKET,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
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
	flushSocket::connect(const std::string &path, 
					flushSocketExchange &exchange)
	{		
		#ifndef FLUSH_SOCKET_WO_XEXEC
			operType = FLUSHSOCKET_OPER_CONNECT_UNIX;
		#endif
		
		#ifndef FLUSH_SOCKET_WO_XEXEC
			performXExec(preExec);
		#endif
		
		if (numberOfConn != -1)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_CONNECT,ERR_LIBDODO,FLUSHSOCKET_CANNOT_CONNECT,FLUSHSOCKET_CANNOT_CONNECT_STR,__LINE__,__FILE__);
			#else
				return false;
			#endif
				
		#ifdef NO_EX
			if (!makeSocket())
				return false;
		#else
			makeSocket();
		#endif
	
		#ifdef NO_EX
			if (!makeUnixSocket(path))
				return false;
		#else
			makeUnixSocket(path);
		#endif
		
		struct sockaddr_un sa;
		
		strcpy(sa.sun_path,path.c_str());
		sa.sun_family = AF_UNIX;
	
		if (::connect(socket,(struct sockaddr *)&sa,path.size()+sizeof(sa.sun_family))==-1)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_CONNECT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else			
				return false;		
			#endif	
		
		exchange.init(socket);
	
		opened = true;
			
		#ifndef FLUSH_SOCKET_WO_XEXEC		
			performXExec(postExec);
		#endif
		
		#ifdef NO_EX
			return true;
		#endif	
	}

#endif

//-------------------------------------------------------------------

__hostInfo 
flushSocket::getHostInfo(const std::string &host)
{
	hostent *ent = gethostbyname(host.c_str());
	
	__hostInfo info;
	
	if (ent == NULL)
		#ifndef NO_EX
			#ifndef WIN
				throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_GETHOSTINFO,ERR_H_ERRNO,h_errno,hstrerror(h_errno),__LINE__,__FILE__);
			#else
				throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_GETHOSTINFO,ERR_WINSOCK,WSAGetLastError(),"Error at gethostbyname",__LINE__,__FILE__);
			#endif
		#else
			return info;		
		#endif		
	
	info.name = ent->h_name;

	register int i(0);

	while (ent->h_aliases[i] != NULL)	
		info.aliases.push_back(ent->h_aliases[i++]);
	
	i = 0;	
	char temp[INET6_ADDRSTRLEN];
	while (ent->h_addr_list[i] != NULL)
	{
		if (inet_ntop(AF_INET,ent->h_addr_list[i++],temp,15)==NULL)
			if (errno == ENOSPC)
			{
				if (inet_ntop(AF_INET6,ent->h_addr_list[i++],temp,INET6_ADDRSTRLEN) == NULL)
					continue;
			}
			else
				continue;			
		info.addresses.push_back(temp);
	}
	
	return info;	
}

//-------------------------------------------------------------------

__servInfo 
flushSocket::getServiceInfo(const std::string &host, 
						const std::string &protocol)
{
	servent *ent = getservbyname(host.c_str(), protocol.c_str());
	
	__servInfo info;
	
	if (ent == NULL)
		#ifdef WIN
			#ifndef NO_EX
					throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_GETSERVICEINFO,ERR_WINSOCK,WSAGetLastError(),"Error at gethostbyname",__LINE__,__FILE__);
			#endif
		#else		
			return info;
		#endif
	
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
						const std::string &protocol)
{
	servent *ent = getservbyport(port, protocol.c_str());
	
	__servInfo info;
	
	if (ent == NULL)
		#ifdef WIN
			#ifndef NO_EX
					throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_GETSERVICEINFO,ERR_WINSOCK,WSAGetLastError(),"Error at gethostbyname",__LINE__,__FILE__);
			#endif
		#else		
			return info;
		#endif
	
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
	#ifndef FLUSH_SOCKET_WO_XEXEC
		operType = FLUSHSOCKET_OPER_BINDNLISTEN;
	#endif
	
	#ifndef FLUSH_SOCKET_WO_XEXEC
		performXExec(preExec);
	#endif
	
	if (opened)
		#ifndef NO_EX
			return ;
		#else
			return false;
		#endif
			
	if (numberOfConn == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_BINDNLISTEN,ERR_LIBDODO,FLUSHSOCKET_CANNOT_BIND,FLUSHSOCKET_CANNOT_BIND_STR,__LINE__,__FILE__);
		#else
			return false;
		#endif
			
	#ifdef NO_EX
		if (!makeSocket())
			return false;
	#else
		makeSocket();
	#endif
	
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
		
		#ifndef WIN	
			if (::bind(socket,(struct sockaddr *)&sa,sizeof(sa))==-1)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_BINDNLISTEN,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else
					return false;
				#endif
		#else
			if (::bind(socket,(struct sockaddr *)&sa,sizeof(sa))==SOCKET_ERROR)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_BINDNLISTEN,ERR_WINSOCK,WSAGetLastError(),"Error at bind()",__LINE__,__FILE__);
				#else
					return false;
				#endif		
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
	{
		#ifndef WIN
			if (::listen(socket,numberOfConn)==-1)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_BINDNLISTEN,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else
					return false;
				#endif
		#else
			if (::listen(socket,numberOfConn)==SOCKET_ERROR)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_BINDNLISTEN,ERR_WINSOCK,WSAGetLastError(),"Error at listen()",__LINE__,__FILE__);
				#else
					return false;
				#endif		
		#endif			
	}
	
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
flushSocket::bindNListen(const __connInfo &destinaton)
{
	#ifdef NO_EX
		if (!bindNListen(destinaton.host,destinaton.port))
			return false;
	#else
		bindNListen(destinaton.host,destinaton.port);
	#endif
}

//-------------------------------------------------------------------

#ifndef WIN
	
	#ifndef NO_EX
		void 
	#else
		bool
	#endif
	flushSocket::bindNListen(const std::string &path)
	{		
		#ifndef FLUSH_SOCKET_WO_XEXEC
			operType = FLUSHSOCKET_OPER_CONNECT_UNIX;
		#endif
		
		#ifndef FLUSH_SOCKET_WO_XEXEC
			performXExec(preExec);
		#endif
		
		if (opened)
			#ifndef NO_EX
				return ;
			#else
				return false;
			#endif
		
		if (numberOfConn == -1)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_BINDNLISTEN,ERR_LIBDODO,FLUSHSOCKET_CANNOT_BIND,FLUSHSOCKET_CANNOT_BIND_STR,__LINE__,__FILE__);
			#else
				return false;
			#endif
			
		#ifdef NO_EX
			if (!makeSocket())
				return false;
		#else
			makeSocket();
		#endif

		#ifdef NO_EX
			if (!makeUnixSocket(path))
				return false;
		#else
			makeUnixSocket(path);
		#endif
		
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
	
		if (::listen(socket,numberOfConn)==-1)
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
	
#endif

//-------------------------------------------------------------------

std::string 
flushSocket::getLocalName()
{
	std::string temp0;
	char *temp1 = new char[256];
	#ifndef WIN
		if (::gethostname(temp1,255)==-1)
	#else
		if (::gethostname(temp1,255)==SOCKET_ERROR)
	#endif
	#ifndef NO_EX
		#ifndef WIN
			throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_GETLOCALNAME,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_GETLOCALNAME,ERR_WINSOCK,WSAGetLastError(),"Error at gethostname()",__LINE__,__FILE__);
		#endif
	#else
		return temp0;
	#endif		
	
	temp0.assign(temp1,255);
	
	delete [] temp1;
	return temp0;
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
flushSocket::setLocalName(const std::string &host)
{
	#ifndef WIN
		if (::sethostname(host.c_str(),host.size())==-1)
	#else
		if (::sethostname(host.c_str(),host.size())==SOCKET_ERROR)	
	#endif
		#ifndef NO_EX
			#ifndef WIN
				throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_SETLOCALNAME,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_SETLOCALNAME,ERR_WINSOCK,WSAGetLastError(),"Error at sethostname()",__LINE__,__FILE__);
			#endif
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
flushSocket::_close(int socket)
{
	#ifndef WIN
		if (::shutdown(socket,SHUT_RDWR)==-1)
	#else
		if (::shutdown(socket,SD_BOTH)==SOCKET_ERROR)
	#endif
		#ifndef NO_EX
			#ifndef WIN
				throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET__CLOSE,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET__CLOSE,ERR_WINSOCK,WSAGetLastError(),"Error at shutdown()",__LINE__,__FILE__);
			#endif
		#else
			return false;
		#endif			
	
	#ifndef WIN
		if (::close(socket)==-1)
	#else
		if (::closesocket(socket)==SOCKET_ERROR)
	#endif
		#ifndef NO_EX
			#ifndef WIN
				throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET__CLOSE,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET__CLOSE,ERR_WINSOCK,WSAGetLastError(),"Error at closesocket()",__LINE__,__FILE__);
			#endif
		#else
			return false;
		#endif
		
	#ifdef NO_EX
		return true;
	#endif			
}

//-------------------------------------------------------------------

bool 
flushSocket::accept(__initialAccept &init, 
					__connInfo &info)
{		
	#ifndef FLUSH_SOCKET_WO_XEXEC
		operType = FLUSHSOCKET_OPER_ACCEPT;
	#endif
	
	#ifndef FLUSH_SOCKET_WO_XEXEC
		performXExec(preExec);
	#endif
	
	if (numberOfConn == -1)
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
			
		return true;
	}
	
	if (accepted == 0)
		return false;
			
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
				
				#ifndef WIN
					if (sock == -1)
				#else
					if (sock == INVALID_SOCKET)
				#endif
					#ifndef NO_EX
						#ifndef WIN
							throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_ACCEPT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
						#else
							throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_ACCEPT,ERR_WINSOCK,WSAGetLastError(),"Error at accept()",__LINE__,__FILE__);
						#endif
					#else
						return false;
					#endif
				
				char temp[15];
				if (inet_ntop(AF_INET,&(sa.sin_addr),temp,15) != NULL)
					info.host.assign(temp);
				info.port = ntohs(sa.sin_port);
			}
				
			break;
		case PROTO_FAMILY_IPV6:
			{
				struct sockaddr_in6 sa;
				register socklen_t len = sizeof(sockaddr_in6);
				
				sock = ::accept(socket,(sockaddr *)&sa,&len);
	
				#ifndef WIN
					if (sock == -1)
				#else
					if (sock == INVALID_SOCKET)
				#endif
					#ifndef NO_EX
						#ifndef WIN
							throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_ACCEPT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
						#else
							throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_ACCEPT,ERR_WINSOCK,WSAGetLastError(),"Error at accept()",__LINE__,__FILE__);
						#endif
					#else
						return false;
					#endif
									
				char temp[INET6_ADDRSTRLEN];
				if (inet_ntop(AF_INET6,&(sa.sin6_addr),temp,INET6_ADDRSTRLEN) != NULL)
					info.host.assign(temp);
				info.port = ntohs(sa.sin6_port);	
			}
			break;
		#ifndef WIN
			case PROTO_FAMILY_UNIX_SOCKET:
				sock = ::accept(socket,NULL,NULL);
				if (sock == -1)
					#ifndef NO_EX
						throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_ACCEPT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
					#else
						return false;
					#endif				
				break;
		#endif
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
	
	--accepted;
			
	#ifndef FLUSH_SOCKET_WO_XEXEC		
		performXExec(postExec);
	#endif
	
	return true;
}

//-------------------------------------------------------------------

bool 
flushSocket::accept(__initialAccept &init)
{		
	#ifndef FLUSH_SOCKET_WO_XEXEC
		operType = FLUSHSOCKET_OPER_ACCEPT;
	#endif
	
	#ifndef FLUSH_SOCKET_WO_XEXEC
		performXExec(preExec);
	#endif
	
	if (numberOfConn == -1)
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
		
		return true;
	}			
	
	if (accepted == 0)
		return false;
	
	if (!opened)	
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_ACCEPT,ERR_LIBDODO,FLUSHSOCKET_ACCEPT_WO_BIND,FLUSHSOCKET_ACCEPT_WO_BIND_STR,__LINE__,__FILE__);
		#else
			return false;
		#endif
	
	register int sock = ::accept(socket,NULL,NULL);

	#ifndef WIN
		if (sock == -1)
	#else
		if (sock == INVALID_SOCKET)
	#endif
		#ifndef NO_EX
			#ifndef WIN
				throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_ACCEPT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_ACCEPT,ERR_WINSOCK,WSAGetLastError(),"Error at accept()",__LINE__,__FILE__);
			#endif
		#else
			return false;
		#endif
	
	init.socket = sock;
	init.type = type;
	init.family = family;
		
	--accepted;
			
	#ifndef FLUSH_SOCKET_WO_XEXEC		
		performXExec(postExec);
	#endif
	
	return true;
}

//-------------------------------------------------------------------

flushSocketExchange::flushSocketExchange(flushSocketExchange &fse)
{
	socket = fse.socket;
	
	socketOpts = fse.socketOpts;
	inTimeout = fse.inTimeout;
	outTimeout = fse.outTimeout;
	inSocketBuffer = fse.inSocketBuffer;
	outSocketBuffer = fse.outSocketBuffer;
	lingerOpts = fse.lingerOpts;
	
	opened = fse.opened;
	
	fse.opened = false;
}

//-------------------------------------------------------------------

flushSocketOptions::flushSocketOptions(socketProtoFamilyEnum a_family, 
										socketTransferTypeEnum a_type): family(a_family),
																	type(a_type),
																	lingerOpts(SOCKET_LINGER_OPTION),
																	lingerSeconds(SOCKET_LINGER_PERIOD),
																	inTimeout(RECIEVE_TIMEOUT),
																	outTimeout(SEND_TIMEOUT),
																	inSocketBuffer(SOCKET_INSIZE),
																	outSocketBuffer(SOCKET_OUTSIZE),
																	socket(-1)
{
}

//-------------------------------------------------------------------

flushSocketOptions::flushSocketOptions(): lingerOpts(SOCKET_LINGER_OPTION),
										lingerSeconds(SOCKET_LINGER_PERIOD),
										inTimeout(RECIEVE_TIMEOUT),
										outTimeout(SEND_TIMEOUT),
										inSocketBuffer(SOCKET_INSIZE),
										outSocketBuffer(SOCKET_OUTSIZE),
										socket(-1)
{
}

//-------------------------------------------------------------------

flushSocketOptions::~flushSocketOptions()
{
}

//-------------------------------------------------------------------

dodoBase * const 
flushSocketExchange::getSelf()
{
	return dynamic_cast<dodoBase *>(this);
}

//-------------------------------------------------------------------

flushSocketExchange::flushSocketExchange()
{
}

//-------------------------------------------------------------------

flushSocketExchange::flushSocketExchange(__initialAccept &a_init)
{
	init(a_init.socket);
}
//-------------------------------------------------------------------

flushSocketExchange::~flushSocketExchange()
{
	close();
}

//-------------------------------------------------------------------

void
flushSocketExchange::init(__initialAccept &a_init)
{
	family = a_init.family;
	type = a_init.type;
	
	init(a_init.socket);
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
flushSocketOptions::setInBufferSize(int bytes)
{
	if (socket == -1)
		#ifdef NO_EX
			return false;
		#else
			throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_SETINBUFFERSIZE,ERR_LIBDODO,FLUSHSOCKET_NO_SOCKET_CREATED,FLUSHSOCKET_NO_SOCKET_CREATED_STR,__LINE__,__FILE__);
		#endif
	
	inSocketBuffer = bytes;
	
	#ifndef WIN
		if (setsockopt(socket,SOL_SOCKET,SO_RCVBUF,&inSocketBuffer,size_of_long)==-1)
	#else
		if (setsockopt(socket,SOL_SOCKET,SO_RCVBUF,&inSocketBuffer,size_of_long)==SOCKET_ERROR)
	#endif
		#ifndef NO_EX
			#ifndef WIN
				throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_SETINBUFFERSIZE,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_SETINBUFFERSIZE,ERR_WINSOCK,WSAGetLastError(),"Error at setsockopt()",__LINE__,__FILE__);
			#endif
		#else
			return false;		
		#endif	
		
	#ifdef NO_EX
		return true;
	#endif			
}

//-------------------------------------------------------------------

int
flushSocketOptions::getInBufferSize()
{
	return inSocketBuffer;
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
flushSocketOptions::setOutBufferSize(int bytes)
{
	if (socket == -1)
		#ifdef NO_EX
			return false;
		#else
			throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_SETOUTBUFFERSIZE,ERR_LIBDODO,FLUSHSOCKET_NO_SOCKET_CREATED,FLUSHSOCKET_NO_SOCKET_CREATED_STR,__LINE__,__FILE__);
		#endif
	
	outSocketBuffer = bytes;
	
	#ifndef WIN
		if (setsockopt(socket,SOL_SOCKET,SO_SNDBUF,&outSocketBuffer,size_of_long)==-1)
	#else
		if (setsockopt(socket,SOL_SOCKET,SO_SNDBUF,&outSocketBuffer,size_of_long)==SOCKET_ERROR)	
	#endif
		#ifndef NO_EX
			#ifndef WIN
				throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_SETOUTBUFFERSIZE,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_SETOUTBUFFERSIZE,ERR_WINSOCK,WSAGetLastError(),"Error at setsockopt()",__LINE__,__FILE__);
			#endif
		#else
			return false;		
		#endif	
		
	#ifdef NO_EX
		return true;
	#endif				
}

//-------------------------------------------------------------------

int
flushSocketOptions::getOutBufferSize()
{
	return outSocketBuffer;
}

//-------------------------------------------------------------------

#ifndef WIN

	#ifndef NO_EX
		void 
	#else
		bool
	#endif 
	flushSocketOptions::setInTimeout(unsigned long microseconds)
	{
		if (socket == -1)
			#ifdef NO_EX
				return false;
			#else
				throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_SETINTIMEOUT,ERR_LIBDODO,FLUSHSOCKET_NO_SOCKET_CREATED,FLUSHSOCKET_NO_SOCKET_CREATED_STR,__LINE__,__FILE__);
			#endif
	
		inTimeout = microseconds;
	
		timeval val;
		val.tv_sec = inTimeout/100;
		val.tv_usec = inTimeout%100;
		
		if (setsockopt(socket,SOL_SOCKET,SO_RCVTIMEO,&val,sizeof(val))==-1)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_SETINTIMEOUT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;	
			#endif	
			
		#ifdef NO_EX
			return true;
		#endif	
	}
	
	
	//-------------------------------------------------------------------
	
	unsigned long 
	flushSocketOptions::getInTimeout()
	{
		return inTimeout;
	}
	
	//-------------------------------------------------------------------
	
	#ifndef NO_EX
		void 
	#else
		bool
	#endif
	flushSocketOptions::setOutTimeout(unsigned long microseconds)
	{
		if (socket == -1)
			#ifdef NO_EX
				return false;
			#else
				throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_SETOUTTIMEOUT,ERR_LIBDODO,FLUSHSOCKET_NO_SOCKET_CREATED,FLUSHSOCKET_NO_SOCKET_CREATED_STR,__LINE__,__FILE__);
			#endif
	
		outTimeout = microseconds;
	
		timeval val;
		val.tv_sec = outTimeout/100;
		val.tv_usec = outTimeout%100;
		
		if (setsockopt(socket,SOL_SOCKET,SO_SNDTIMEO,&val,sizeof(val))==-1)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_SETOUTTIMEOUT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;		
			#endif	
			
		#ifdef NO_EX
			return true;
		#endif	
	}
	
	
	//-------------------------------------------------------------------
	
	unsigned long 
	flushSocketOptions::getOutTimeout()
	{
		return outTimeout;
	}

#endif

//-------------------------------------------------------------------

bool
flushSocketOptions::getSocketOpts(int option)
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
flushSocketOptions::setSockOption(socketOptionsEnum option, 
							bool flag)
{
	if (socket == -1)
		#ifdef NO_EX
			return false;
		#else
			throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_SETSOCKOPT,ERR_LIBDODO,FLUSHSOCKET_NO_SOCKET_CREATED,FLUSHSOCKET_NO_SOCKET_CREATED_STR,__LINE__,__FILE__);
		#endif
	
	register int sockFlag(1);
	
	if (!flag)
		sockFlag = 0;
	
	register int real_option(0);
	
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
		default:
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_SETSOCKOPT,ERR_LIBDODO,FLUSHSOCKET_WRONG_PARAMETHER,FLUSHSOCKET_WRONG_PARAMETHER_STR,__LINE__,__FILE__);
			#else
				return false;			
			#endif	
	}
	
	#ifndef WIN
		if (setsockopt(socket,SOL_SOCKET,real_option,&sockFlag,size_of_int)==-1)
	#else
		if (setsockopt(socket,SOL_SOCKET,real_option,&sockFlag,size_of_int)==SOCKET_ERROR)
	#endif
		#ifndef NO_EX
			#ifndef WIN
				throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_SETSOCKOPT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_SETSOCKOPT,ERR_WINSOCK,WSAGetLastError(),"Error at setsockopt()",__LINE__,__FILE__);
			#endif
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
flushSocketOptions::setLingerSockOption(socketLingerOption option,
									int seconds)
{	
	if (socket == -1)
		#ifdef NO_EX
			return false;
		#else
			throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_SETLINGERSOCKOPT,ERR_LIBDODO,FLUSHSOCKET_NO_SOCKET_CREATED,FLUSHSOCKET_NO_SOCKET_CREATED_STR,__LINE__,__FILE__);
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
		default:
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_SETLINGERSOCKOPT,ERR_LIBDODO,FLUSHSOCKET_WRONG_PARAMETHER,FLUSHSOCKET_WRONG_PARAMETHER_STR,__LINE__,__FILE__);
			#else
				return false;			
			#endif	
	}

	#ifndef WIN
		if (setsockopt(socket,SOL_SOCKET,SO_LINGER,&lin,sizeof(linger))==-1)
	#else
		if (setsockopt(socket,SOL_SOCKET,SO_LINGER,&lin,sizeof(linger))==SOCKET_ERROR)	
	#endif
		#ifndef NO_EX
			#ifndef WIN
				throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_SETLINGERSOCKOPT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_SETLINGERSOCKOPT,ERR_WINSOCK,WSAGetLastError(),"Error at setsockopt()",__LINE__,__FILE__);
			#endif
		#else
			return false;	
		#endif
		
	lingerOpts = option;
	lingerSeconds = seconds;		
	
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
flushSocketExchange::close()
{		
	#ifndef FLUSH_SOCKET_WO_XEXEC
		operType = FLUSHSOCKET_OPER_CLOSE;
	#endif
	
	#ifndef FLUSH_SOCKET_WO_XEXEC
		performXExec(preExec);
	#endif
	
	if (!opened)
	#ifndef NO_EX
		return ;
	#else
		return true;
	#endif
	
	#ifdef NO_EX
		register bool result = 
	#endif
	flushSocket::_close(socket);
	
	opened = false;
			
	#ifndef FLUSH_SOCKET_WO_XEXEC		
		performXExec(postExec);
	#endif
			
	#ifdef NO_EX
		return result;
	#endif			
}
//-------------------------------------------------------------------

void 
flushSocketExchange::init(int a_socket)
{
	if (socket != a_socket)
	{
		close();
	
		socket = a_socket;
	
		setInBufferSize(inSocketBuffer);
		setOutBufferSize(outSocketBuffer);
		
		setInTimeout(inTimeout);
		setOutTimeout(outTimeout);
		
		setLingerSockOption(lingerOpts,lingerSeconds);	
	
		opened = true;
	}
}

//-------------------------------------------------------------------

bool 
flushSocketExchange::alive()
{
	return opened;
}

//-------------------------------------------------------------------

socketLingerOption 
flushSocketOptions::getLingerOption()
{
	return lingerOpts;
}

//-------------------------------------------------------------------

int 
flushSocketOptions::getLingerPeriod()
{
	return lingerSeconds;
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
flushSocketExchange::send(const char * const data, 
						bool urgent)
{
		
	#ifndef FLUSH_SOCKET_WO_XEXEC
		operType = FLUSHSOCKET_OPER_SEND;
	#endif

	register long oldOutSize = outSize;
	
	if (autoOutSize)
		outSize = strlen(data);
		
	buffer.assign(data,outSize);
				
	#ifndef FLUSH_SOCKET_WO_XEXEC
		performXExec(preExec);
	#endif	

	if (autoOutSize)
		outSize = buffer.size();		
	
	register long iter = outSize/outSocketBuffer, rest = outSize%outSocketBuffer;
	register long n(0), sent(0);	
		
	register int flag = 0;	
	if (urgent)	
		flag = MSG_OOB;
		
	for (register long i=0;i<iter;++i)
	{
		n = 0;
		while (sent<outSize)
		{
			n = ::send(socket,buffer.c_str()+sent,inSocketBuffer,flag);
			#ifndef WIN
				if (n==-1)
			#else
				if (n==SOCKET_ERROR)
			#endif
				#ifndef NO_EX
					#ifndef WIN
						throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_SEND,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
					#else
						throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_SEND,ERR_WINSOCK,WSAGetLastError(),"Error at send()",__LINE__,__FILE__);
					#endif
				#else
					return false;	
				#endif
			sent += inSocketBuffer;
		}
	}
	
	if (rest>0)
	{
		n = 0;
		while (sent<rest)
		{
			n = ::send(socket,buffer.c_str()+sent,rest,flag);
			#ifndef WIN
				if (n==-1)
			#else
				if (n==SOCKET_ERROR)
			#endif
				#ifndef NO_EX
					#ifndef WIN
						throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_SEND,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
					#else
						throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_SEND,ERR_WINSOCK,WSAGetLastError(),"Error at send()",__LINE__,__FILE__);
					#endif
				#else
					return false;	
				#endif
			sent += n;
		}		
	}
			
	#ifndef FLUSH_SOCKET_WO_XEXEC		
		performXExec(postExec);
	#endif
		
	outSize = oldOutSize;	
		
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
flushSocketExchange::sendString(const std::string &data, 
						bool urgent)
{
	return this->send(data.c_str(),urgent);
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
flushSocketExchange::recieve(char * const data, 
							bool urgent)
{
		
	#ifndef FLUSH_SOCKET_WO_XEXEC
		operType = FLUSHSOCKET_OPER_RECIEVE;
	#endif
	
	#ifndef FLUSH_SOCKET_WO_XEXEC
		performXExec(preExec);
	#endif	
				
	register long iter = inSize/inSocketBuffer, rest = inSize%inSocketBuffer;
	register long n(0), recieved(0);
		
	register int flag = 0;	
	if (urgent)	
		flag = MSG_OOB;
			
	for (register long i=0;i<iter;++i)
	{
		n = 0;
		n = ::recv(socket,data+recieved,inSocketBuffer,flag);
		#ifndef WIN
			if (n==-1)
		#else
			if (n==SOCKET_ERROR)
		#endif
			#ifndef NO_EX
				#ifndef WIN
					throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_RECIEVE,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else
					throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_RECIEVE,ERR_WINSOCK,WSAGetLastError(),"Error at recv()",__LINE__,__FILE__);
				#endif
			#else
				return false;	
			#endif
		recieved += n;
	}
	
	if (rest>0)
		#ifndef WIN
			if (::recv(socket,data+recieved,rest,flag)==-1)
		#else
			if (::recv(socket,data+recieved,rest,flag)==SOCKET_ERROR)
		#endif
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_RECIEVE,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;	
			#endif
	
	buffer.assign(data,inSize);
			
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
flushSocketExchange::recieveString(std::string &data, 
								bool urgent)
{	
	register char *t_data = new char[inSize+1];
	if (t_data == NULL)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHSOCKET_RECIEVESTRING,ERR_LIBDODO,FLUSHSOCKET_MEMORY_OVER,FLUSHSOCKET_MEMORY_OVER_STR,__LINE__,__FILE__);
		#else
			return false;
		#endif
		
	memset(t_data,0,inSize);

	#ifdef NO_EX
		register bool result = 
	#endif
	
	this->recieve(t_data,urgent);
	data.assign(t_data,inSize);
	delete [] t_data;
	
	#ifdef NO_EX	
		return result;
	#endif
}

//-------------------------------------------------------------------

int 
flushSocketExchange::addPostExec(inExec func, 
					void *data) const
{
	return _addPostExec(func, (dodoBase *)this, data);
}

//-------------------------------------------------------------------

int 
flushSocketExchange::addPreExec(inExec func, 
					void *data) const
{
	return _addPreExec(func, (dodoBase *)this, data);
}

//-------------------------------------------------------------------

int 
flushSocketExchange::addPostExec(const std::string &module, 
					void *data) const
{
	return _addPostExec(module, (dodoBase *)this, data);
}

//-------------------------------------------------------------------

int 
flushSocketExchange::addPreExec(const std::string &module, 
					void *data) const
{
	return _addPreExec(module, (dodoBase *)this, data);
}

//-------------------------------------------------------------------
