/***************************************************************************
 *            ioNetwork.cc
 *
 *  Thu Sep 20 01:43:24 2005
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

#include <libdodo/ioNetwork.h>

using namespace dodo;

__xexexIoNetworkCollectedData::__xexexIoNetworkCollectedData(int &a_operType,
											   void *a_executor) : operType(a_operType),
																   executor(a_executor)
{
}

//-------------------------------------------------------------------

ioNetwork::ioNetwork(ioNetwork &fs): collectedData(operType,
										 (void *) this)
{
}

//-------------------------------------------------------------------

ioNetwork::ioNetwork(bool a_server,
				   short a_family,
				   short a_type) : ioNetworkOptions(a_family, a_type),
								   blockInherited(false),
								   server(a_server),
								   collectedData(operType,
												 (void *) this)
{
	makeSocket();
}


//-------------------------------------------------------------------

ioNetwork::~ioNetwork()
{
	if (socket != -1)
	{
		::shutdown(socket, SHUT_RDWR);

		::close(socket);
	}

	if (server && unixSock.size() != 0)
		::unlink(unixSock.c_str());
}

//-------------------------------------------------------------------

#ifndef IONETWORK_WO_XEXEC

int
ioNetwork::addPostExec(inExec func,
					  void   *data)
{
	return _addPostExec(func, (void *)&collectedData, XEXEC_OBJECT_IONETWORK, data);
}

//-------------------------------------------------------------------

int
ioNetwork::addPreExec(inExec func,
					 void   *data)
{
	return _addPreExec(func, (void *)&collectedData, XEXEC_OBJECT_IONETWORK, data);
}

//-------------------------------------------------------------------

#ifdef DL_EXT

int
ioNetwork::addPostExec(const dodoString &module,
					  void             *data,
					  void             *toInit)
{
	return _addPostExec(module, (void *)&collectedData, XEXEC_OBJECT_IONETWORK, data, toInit);
}

//-------------------------------------------------------------------

int
ioNetwork::addPreExec(const dodoString &module,
					 void             *data,
					 void             *toInit)
{
	return _addPreExec(module, (void *)&collectedData, XEXEC_OBJECT_IONETWORK, data, toInit);
}

//-------------------------------------------------------------------

__xexecCounts
ioNetwork::addExec(const dodoString &module,
				  void             *data,
				  void             *toInit)
{
	return _addExec(module, (void *)&collectedData, XEXEC_OBJECT_IONETWORK, data, toInit);
}

#endif

#endif

//-------------------------------------------------------------------

void
ioNetwork::restoreOptions()
{
	setInBufferSize(inSocketBuffer);
	setOutBufferSize(outSocketBuffer);

	setInTimeout(inTimeout);
	setOutTimeout(outTimeout);

	setLingerOption(lingerOpts, lingerSeconds);

	block(blocked);
}

//-------------------------------------------------------------------

void
ioNetwork::makeSocket()
{
	if (socket != -1)
	{
		::shutdown(socket, SHUT_RDWR);

		if (::close(socket) == -1)
			throw baseEx(ERRMODULE_IONETWORK, IONETWORKEX_BINDNLISTEN, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		socket = -1;
	}

	int real_domain(PF_INET), real_type(SOCK_STREAM);

	switch (family)
	{
		case IONETWORKOPTIONS_PROTO_FAMILY_IPV4:

			real_domain = PF_INET;

			break;

		case IONETWORKOPTIONS_PROTO_FAMILY_IPV6:

			real_domain = PF_INET6;

			break;

		case IONETWORKOPTIONS_PROTO_FAMILY_UNIX_SOCKET:

			real_domain = PF_UNIX;

			break;

		default:

			throw baseEx(ERRMODULE_IONETWORK, IONETWORKEX_MAKESOCKET, ERR_LIBDODO, IONETWORKEX_WRONGPARAMETER, IONETWORKEX_WRONGPARAMETER_STR, __LINE__, __FILE__);
	}

	switch (type)
	{
		case IONETWORKOPTIONS_TRANSFER_TYPE_STREAM:

			real_type = SOCK_STREAM;

			break;

		case IONETWORKOPTIONS_TRANSFER_TYPE_DATAGRAM:

			real_type = SOCK_DGRAM;

			break;

		default:

			throw baseEx(ERRMODULE_IONETWORK, IONETWORKEX_MAKESOCKET, ERR_LIBDODO, IONETWORKEX_WRONGPARAMETER, IONETWORKEX_WRONGPARAMETER_STR, __LINE__, __FILE__);
	}

	socket = ::socket(real_domain, real_type, 0);
	if (socket == -1)
		throw baseEx(ERRMODULE_IONETWORK, IONETWORKEX_MAKESOCKET, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	restoreOptions();
}

//-------------------------------------------------------------------

void
ioNetwork::connect(const dodoString &host,
				  int port,
				  ioNetworkExchange &exchange)
{
#ifndef IONETWORK_WO_XEXEC
	operType = IONETWORK_OPERATION_CONNECT;
	performXExec(preExec);
#endif

	if (server)
		throw baseEx(ERRMODULE_IONETWORK, IONETWORKEX_CONNECT, ERR_LIBDODO, IONETWORKEX_CANNOTCONNECT, IONETWORKEX_CANNOTCONNECT_STR, __LINE__, __FILE__);

	makeSocket();

	if (family == IONETWORKOPTIONS_PROTO_FAMILY_IPV6)
	{
		struct sockaddr_in6 sa;
		sa.sin6_family = AF_INET6;
		sa.sin6_port = htons(port);
		sa.sin6_flowinfo = 0;
		sa.sin6_scope_id = 0;
		inet_pton(AF_INET6, host.c_str(), &sa.sin6_addr);

		if (::connect(socket, (struct sockaddr *)&sa, sizeof(sa)) == -1)
		{
			if (::close(socket) == -1)
				throw baseEx(ERRMODULE_IONETWORK, IONETWORKEX_CONNECT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			
			socket = -1;

			throw baseEx(ERRMODULE_IONETWORK, IONETWORKEX_CONNECT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}
	}
	else
	{
		struct sockaddr_in sa;
		sa.sin_family = AF_INET;
		sa.sin_port = htons(port);
		inet_aton(host.c_str(), &sa.sin_addr);

		if (::connect(socket, (struct sockaddr *)&sa, sizeof(sa)) == -1)
		{
			if (::close(socket) == -1)
				throw baseEx(ERRMODULE_IONETWORK, IONETWORKEX_CONNECT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

			socket = -1;

			throw baseEx(ERRMODULE_IONETWORK, IONETWORKEX_CONNECT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}
	}

	exchange.blocked = blocked;
	exchange.init(socket, blockInherited);

	socket = -1;

#ifndef IONETWORK_WO_XEXEC
	performXExec(postExec);
#endif
}

//-------------------------------------------------------------------

void
ioNetwork::connect(const __connInfo &destinaton,
				  ioNetworkExchange &exchange)
{
	connect(destinaton.host, destinaton.port, exchange);
}

//-------------------------------------------------------------------

void
ioNetwork::connectFrom(const dodoString &local,
					  const dodoString &host,
					  int port,
					  ioNetworkExchange &exchange)
{
#ifndef IONETWORK_WO_XEXEC
	operType = IONETWORK_OPERATION_CONNECTFROM;
	performXExec(preExec);
#endif

	if (server)
		throw baseEx(ERRMODULE_IONETWORK, IONETWORKEX_CONNECTFROM, ERR_LIBDODO, IONETWORKEX_CANNOTCONNECT, IONETWORKEX_CANNOTCONNECT_STR, __LINE__, __FILE__);

	makeSocket();

	int sockFlag(1);
	if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &sockFlag, sizeof(int)) == -1)
		throw baseEx(ERRMODULE_IONETWORK, IONETWORKEX_CONNECTFROM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	addFlag(socketOpts, 1 << IONETWORKOPTIONS_OPTION_REUSE_ADDRESS);

	if (family == IONETWORKOPTIONS_PROTO_FAMILY_IPV6)
	{
		struct sockaddr_in6 sa;
		sa.sin6_family = AF_INET6;
		sa.sin6_flowinfo = 0;
		sa.sin6_scope_id = 0;
		sa.sin6_port = htons(0);
		inet_pton(AF_INET6, local.c_str(), &sa.sin6_addr);

		if (::bind(socket, (struct sockaddr *)&sa, sizeof(sa)) == -1)
			throw baseEx(ERRMODULE_IONETWORK, IONETWORKEX_CONNECTFROM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		sa.sin6_port = htons(port);
		inet_pton(AF_INET6, host.c_str(), &sa.sin6_addr);

		if (::connect(socket, (struct sockaddr *)&sa, sizeof(sa)) == -1)
		{
			if (::close(socket) == -1)
				throw baseEx(ERRMODULE_IONETWORK, IONETWORKEX_CONNECTFROM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

			socket = -1;

			throw baseEx(ERRMODULE_IONETWORK, IONETWORKEX_CONNECTFROM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}
	}
	else
	{
		struct sockaddr_in sa;
		sa.sin_family = AF_INET;
		sa.sin_port = htons(0);
		inet_aton(local.c_str(), &sa.sin_addr);

		if (::bind(socket, (struct sockaddr *)&sa, sizeof(sa)) == -1)
			throw baseEx(ERRMODULE_IONETWORK, IONETWORKEX_CONNECTFROM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		sa.sin_port = htons(port);
		inet_aton(host.c_str(), &sa.sin_addr);

		if (::connect(socket, (struct sockaddr *)&sa, sizeof(sa)) == -1)
		{
			if (::close(socket) == -1)
				throw baseEx(ERRMODULE_IONETWORK, IONETWORKEX_CONNECTFROM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

			socket = -1;

			throw baseEx(ERRMODULE_IONETWORK, IONETWORKEX_CONNECTFROM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}
	}

	exchange.blocked = blocked;
	exchange.init(socket, blockInherited);

	socket = -1;

#ifndef IONETWORK_WO_XEXEC
	performXExec(postExec);
#endif
}

//-------------------------------------------------------------------

void
ioNetwork::connectFrom(const dodoString &local,
					  const __connInfo &destinaton,
					  ioNetworkExchange &exchange)
{
	connectFrom(local, destinaton.host, destinaton.port, exchange);
}

//-------------------------------------------------------------------

void
ioNetwork::connect(const dodoString &path,
				  ioNetworkExchange &exchange)
{
#ifndef IONETWORK_WO_XEXEC
	operType = IONETWORK_OPERATION_CONNECT_UNIX;
	performXExec(preExec);
#endif

	if (server)
		throw baseEx(ERRMODULE_IONETWORK, IONETWORKEX_CONNECT, ERR_LIBDODO, IONETWORKEX_CANNOTCONNECT, IONETWORKEX_CANNOTCONNECT_STR, __LINE__, __FILE__);
	makeSocket();

	struct sockaddr_un sa;

	strcpy(sa.sun_path, path.c_str());
	sa.sun_family = AF_UNIX;

	if (::connect(socket, (struct sockaddr *)&sa, path.size() + sizeof(sa.sun_family)) == -1)
	{
		if (::close(socket) == -1)
			throw baseEx(ERRMODULE_IONETWORK, IONETWORKEX_CONNECT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		socket = -1;

		throw baseEx(ERRMODULE_IONETWORK, IONETWORKEX_CONNECT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	exchange.blocked = blocked;
	exchange.init(socket, blockInherited);

	socket = -1;

#ifndef IONETWORK_WO_XEXEC
	performXExec(postExec);
#endif
}

//-------------------------------------------------------------------

void
ioNetwork::bindNListen(const dodoString &host,
					  int port,
					  int numberOfConnections)
{
#ifndef IONETWORK_WO_XEXEC
	operType = IONETWORK_OPERATION_BINDNLISTEN;
	performXExec(preExec);
#endif

	if (!server)
		throw baseEx(ERRMODULE_IONETWORK, IONETWORKEX_BINDNLISTEN, ERR_LIBDODO, IONETWORKEX_CANNOTBIND, IONETWORKEX_CANNOTBIND_STR, __LINE__, __FILE__);

	if (opened)
	{
		if (::shutdown(socket, SHUT_RDWR) == -1)
			throw baseEx(ERRMODULE_IONETWORK, IONETWORKEX_BINDNLISTEN, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		if (::close(socket) == -1)
			throw baseEx(ERRMODULE_IONETWORK, IONETWORKEX_BINDNLISTEN, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		socket = -1;

		opened = false;
	}

	makeSocket();

	int sockFlag(1);
	if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &sockFlag, sizeof(int)) == 1)
		throw baseEx(ERRMODULE_IONETWORK, IONETWORKEX_CONNECTFROM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	addFlag(socketOpts, 1 << IONETWORKOPTIONS_OPTION_REUSE_ADDRESS);

	setLingerOption(IONETWORKOPTIONS_SOCKET_LINGER_OPTION, IONETWORKOPTIONS_SOCKET_LINGER_PERIOD);

	if (family == IONETWORKOPTIONS_PROTO_FAMILY_IPV6)
	{
		struct sockaddr_in6 sa;
		sa.sin6_family = AF_INET6;
		sa.sin6_port = htons(port);
		sa.sin6_flowinfo = 0;
		sa.sin6_scope_id = 0;
		if (host == "*")
			sa.sin6_addr = in6addr_any;
		else
			inet_pton(AF_INET6, host.c_str(), &sa.sin6_addr);

		if (::bind(socket, (struct sockaddr *)&sa, sizeof(sa)) == -1)
			throw baseEx(ERRMODULE_IONETWORK, IONETWORKEX_BINDNLISTEN, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}
	else
	{
		struct sockaddr_in sa;

		sa.sin_family = AF_INET;
		sa.sin_port = htons(port);
		if (host == "*")
			sa.sin_addr.s_addr = htonl(INADDR_ANY);
		else
			inet_pton(AF_INET, host.c_str(), &sa.sin_addr);

		if (::bind(socket, (struct sockaddr *)&sa, sizeof(sa)) == -1)
			throw baseEx(ERRMODULE_IONETWORK, IONETWORKEX_BINDNLISTEN, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	if (type == IONETWORKOPTIONS_TRANSFER_TYPE_STREAM)
		if (::listen(socket, numberOfConnections) == -1)
			throw baseEx(ERRMODULE_IONETWORK, IONETWORKEX_BINDNLISTEN, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	opened = true;

#ifndef IONETWORK_WO_XEXEC
	performXExec(postExec);
#endif

}

//-------------------------------------------------------------------

void
ioNetwork::bindNListen(const __connInfo &destinaton,
					  int numberOfConnections)
{
	bindNListen(destinaton.host, destinaton.port, numberOfConnections);
}

//-------------------------------------------------------------------

void
ioNetwork::bindNListen(const dodoString &path,
					  int numberOfConnections,
					  bool force)
{
#ifndef IONETWORK_WO_XEXEC
	operType = IONETWORK_OPERATION_BINDNLISTEN_UNIX;
	performXExec(preExec);
#endif

	if (!server)
		throw baseEx(ERRMODULE_IONETWORK, IONETWORKEX_BINDNLISTEN, ERR_LIBDODO, IONETWORKEX_CANNOTBIND, IONETWORKEX_CANNOTBIND_STR, __LINE__, __FILE__);

	if (opened)
	{
		if (::shutdown(socket, SHUT_RDWR) == -1)
			throw baseEx(ERRMODULE_IONETWORK, IONETWORKEX_BINDNLISTEN, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		if (::close(socket) == -1)
			throw baseEx(ERRMODULE_IONETWORK, IONETWORKEX_BINDNLISTEN, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		socket = -1;

		opened = false;
	}

	makeSocket();

	if (force)
	{
		struct stat st;
		if (::lstat(path.c_str(), &st) != -1)
			if (S_ISSOCK(st.st_mode))
				::unlink(path.c_str());
			else
				throw baseEx(ERRMODULE_IONETWORK, IONETWORKEX_MAKEUNIXSOCKET, ERR_LIBDODO, IONETWORKEX_WRONGFILENAME, IONETWORKEX_WRONGFILENAME_STR, __LINE__, __FILE__);
	}

	int sockFlag(1);
	if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &sockFlag, sizeof(int)) == -1)
		throw baseEx(ERRMODULE_IONETWORK, IONETWORKEX_CONNECTFROM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	addFlag(socketOpts, 1 << IONETWORKOPTIONS_OPTION_REUSE_ADDRESS);

	setLingerOption(IONETWORKOPTIONS_SOCKET_LINGER_OPTION, IONETWORKOPTIONS_SOCKET_LINGER_PERIOD);

	struct sockaddr_un sa;

	strcpy(sa.sun_path, path.c_str());
	sa.sun_family = AF_UNIX;

	if (::bind(socket, (struct sockaddr *)&sa, path.size() + sizeof(sa.sun_family)) == -1)
		throw baseEx(ERRMODULE_IONETWORK, IONETWORKEX_BINDNLISTEN, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	if (::listen(socket, numberOfConnections) == -1)
		throw baseEx(ERRMODULE_IONETWORK, IONETWORKEX_BINDNLISTEN, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	unixSock = path;

	opened = true;

#ifndef IONETWORK_WO_XEXEC
	performXExec(postExec);
#endif
}

//-------------------------------------------------------------------

bool
ioNetwork::accept(__initialAccept &init,
				 __connInfo &info)
{
#ifndef IONETWORK_WO_XEXEC
	operType = IONETWORK_OPERATION_ACCEPT;
	performXExec(preExec);
#endif

	if (!server)
		throw baseEx(ERRMODULE_IONETWORK, IONETWORKEX_ACCEPT, ERR_LIBDODO, IONETWORKEX_CANNOTACCEPT, IONETWORKEX_CANNOTACCEPT_STR, __LINE__, __FILE__);

	if (type != IONETWORKOPTIONS_TRANSFER_TYPE_STREAM)
	{
		init.socket = socket;
		init.type = type;
		init.family = family;
		init.blocked = blocked;
		init.blockInherited = blockInherited;

		return true;
	}

	if (!opened)
		throw baseEx(ERRMODULE_IONETWORK, IONETWORKEX_ACCEPT, ERR_LIBDODO, IONETWORKEX_ACCEPTWOBIND, IONETWORKEX_ACCEPTWOBIND_STR, __LINE__, __FILE__);

	int sock(-1);
	info.host.clear();

	switch (family)
	{
		case IONETWORKOPTIONS_PROTO_FAMILY_IPV4:
		{
			struct sockaddr_in sa;
			socklen_t len = sizeof(sockaddr_in);
			sock = ::accept(socket, (sockaddr *)&sa, &len);

			if (sock == -1)
			{
				if (errno == EAGAIN)
					return false;
				else
					throw baseEx(ERRMODULE_IONETWORK, IONETWORKEX_ACCEPT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			}

			char temp[INET_ADDRSTRLEN];
			if (inet_ntop(AF_INET, &(sa.sin_addr), temp, INET_ADDRSTRLEN) != NULL)
				info.host.assign(temp);
			info.port = ntohs(sa.sin_port);
		}

			break;

		case IONETWORKOPTIONS_PROTO_FAMILY_IPV6:
		{
			struct sockaddr_in6 sa;
			socklen_t len = sizeof(sockaddr_in6);

			sock = ::accept(socket, (sockaddr *)&sa, &len);

			if (sock == -1)
			{
				if (errno == EAGAIN)
					return false;
				else
					throw baseEx(ERRMODULE_IONETWORK, IONETWORKEX_ACCEPT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			}

			char temp[INET6_ADDRSTRLEN];
			if (inet_ntop(AF_INET6, &(sa.sin6_addr), temp, INET6_ADDRSTRLEN) != NULL)
				info.host.assign(temp);
			info.port = ntohs(sa.sin6_port);
		}
			break;

		case IONETWORKOPTIONS_PROTO_FAMILY_UNIX_SOCKET:
			sock = ::accept(socket, NULL, NULL);
			if (sock == -1)
			{
				if (errno == EAGAIN)
					return false;
				else
					throw baseEx(ERRMODULE_IONETWORK, IONETWORKEX_ACCEPT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			}
			break;

		default:
			throw baseEx(ERRMODULE_IONETWORK, IONETWORKEX_ACCEPT, ERR_LIBDODO, IONETWORKEX_WRONGPARAMETER, IONETWORKEX_WRONGPARAMETER_STR, __LINE__, __FILE__);
	}

	init.socket = sock;
	init.type = type;
	init.family = family;
	init.blocked = blocked;
	init.blockInherited = blockInherited;

#ifndef IONETWORK_WO_XEXEC
	performXExec(postExec);
#endif

	return true;
}

//-------------------------------------------------------------------

bool
ioNetwork::accept(__initialAccept &init)
{
#ifndef IONETWORK_WO_XEXEC
	operType = IONETWORK_OPERATION_ACCEPT;
	performXExec(preExec);
#endif

	if (!server)
		throw baseEx(ERRMODULE_IONETWORK, IONETWORKEX_ACCEPT, ERR_LIBDODO, IONETWORKEX_CANNOTACCEPT, IONETWORKEX_CANNOTACCEPT_STR, __LINE__, __FILE__);

	if (type != IONETWORKOPTIONS_TRANSFER_TYPE_STREAM)
	{
		init.socket = socket;
		init.type = type;
		init.family = family;
		init.blocked = blocked;
		init.blockInherited = blockInherited;

		return true;
	}

	if (!opened)
		throw baseEx(ERRMODULE_IONETWORK, IONETWORKEX_ACCEPT, ERR_LIBDODO, IONETWORKEX_ACCEPTWOBIND, IONETWORKEX_ACCEPTWOBIND_STR, __LINE__, __FILE__);

	int sock = ::accept(socket, NULL, NULL);
	if (sock == -1)
	{
		if (errno == EAGAIN)
			return false;
		else
			throw baseEx(ERRMODULE_IONETWORK, IONETWORKEX_ACCEPT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	init.socket = sock;
	init.type = type;
	init.family = family;
	init.blocked = blocked;
	init.blockInherited = blockInherited;

#ifndef IONETWORK_WO_XEXEC
	performXExec(postExec);
#endif

	return true;
}

//-------------------------------------------------------------------

int
ioNetwork::getInDescriptor() const
{
	return socket;
}

//-------------------------------------------------------------------

int
ioNetwork::getOutDescriptor() const
{
	return socket;
}

//-------------------------------------------------------------------

