/***************************************************************************
 *            ioNetworkClient.cc
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

#include <libdodo/ioNetworkClient.h>

using namespace dodo;

__xexexIoNetworkClientCollectedData::__xexexIoNetworkClientCollectedData(int &a_operType,
											   void *a_executor) : operType(a_operType),
																   executor(a_executor)
{
}

//-------------------------------------------------------------------

ioNetworkClient::ioNetworkClient(ioNetworkClient &fs): collectedData(operType,
										 (void *) this)
{
}

//-------------------------------------------------------------------

ioNetworkClient::ioNetworkClient(short a_family,
				   short a_type) : ioNetworkOptions(a_family, a_type),
								   blockInherited(false),
								   collectedData(operType,
												 (void *) this),
									opened(false)
{
}


//-------------------------------------------------------------------

ioNetworkClient::~ioNetworkClient()
{
	if (socket != -1)
	{
		::shutdown(socket, SHUT_RDWR);

		::close(socket);
	}
}

//-------------------------------------------------------------------

#ifndef IONETWORKCLIENT_WO_XEXEC

int
ioNetworkClient::addPostExec(inExec func,
					  void   *data)
{
	return _addPostExec(func, (void *)&collectedData, XEXEC_OBJECT_IONETWORKCLIENT, data);
}

//-------------------------------------------------------------------

int
ioNetworkClient::addPreExec(inExec func,
					 void   *data)
{
	return _addPreExec(func, (void *)&collectedData, XEXEC_OBJECT_IONETWORKCLIENT, data);
}

//-------------------------------------------------------------------

#ifdef DL_EXT

int
ioNetworkClient::addPostExec(const dodoString &module,
					  void             *data,
					  void             *toInit)
{
	return _addPostExec(module, (void *)&collectedData, XEXEC_OBJECT_IONETWORKCLIENT, data, toInit);
}

//-------------------------------------------------------------------

int
ioNetworkClient::addPreExec(const dodoString &module,
					 void             *data,
					 void             *toInit)
{
	return _addPreExec(module, (void *)&collectedData, XEXEC_OBJECT_IONETWORKCLIENT, data, toInit);
}

//-------------------------------------------------------------------

__xexecCounts
ioNetworkClient::addExec(const dodoString &module,
				  void             *data,
				  void             *toInit)
{
	return _addExec(module, (void *)&collectedData, XEXEC_OBJECT_IONETWORKCLIENT, data, toInit);
}

#endif

#endif

//-------------------------------------------------------------------

void
ioNetworkClient::restoreOptions()
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
ioNetworkClient::makeSocket()
{
	if (socket != -1)
	{
		::shutdown(socket, SHUT_RDWR);

		if (::close(socket) == -1)
			throw baseEx(ERRMODULE_IONETWORKCLIENT, IONETWORKCLIENTEX_MAKESOCKET, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

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

			throw baseEx(ERRMODULE_IONETWORKCLIENT, IONETWORKCLIENTEX_MAKESOCKET, ERR_LIBDODO, IONETWORKCLIENTEX_WRONGPARAMETER, IONETWORKCLIENTEX_WRONGPARAMETER_STR, __LINE__, __FILE__);
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

			throw baseEx(ERRMODULE_IONETWORKCLIENT, IONETWORKCLIENTEX_MAKESOCKET, ERR_LIBDODO, IONETWORKCLIENTEX_WRONGPARAMETER, IONETWORKCLIENTEX_WRONGPARAMETER_STR, __LINE__, __FILE__);
	}

	socket = ::socket(real_domain, real_type, 0);
	if (socket == -1)
		throw baseEx(ERRMODULE_IONETWORKCLIENT, IONETWORKCLIENTEX_MAKESOCKET, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	restoreOptions();
}

//-------------------------------------------------------------------

void
ioNetworkClient::connect(const dodoString &host,
				  int port,
				  ioNetworkExchange &exchange)
{
#ifndef IONETWORKCLIENT_WO_XEXEC
	operType = IONETWORKCLIENT_OPERATION_CONNECT;
	performXExec(preExec);
#endif

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
				throw baseEx(ERRMODULE_IONETWORKCLIENT, IONETWORKCLIENTEX_CONNECT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			
			socket = -1;

			throw baseEx(ERRMODULE_IONETWORKCLIENT, IONETWORKCLIENTEX_CONNECT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
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
				throw baseEx(ERRMODULE_IONETWORKCLIENT, IONETWORKCLIENTEX_CONNECT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

			socket = -1;

			throw baseEx(ERRMODULE_IONETWORKCLIENT, IONETWORKCLIENTEX_CONNECT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}
	}

	exchange.blocked = blocked;
	exchange.init(socket, blockInherited);

	socket = -1;

#ifndef IONETWORKCLIENT_WO_XEXEC
	performXExec(postExec);
#endif
}

//-------------------------------------------------------------------

void
ioNetworkClient::connect(const __connInfo &destinaton,
				  ioNetworkExchange &exchange)
{
	connect(destinaton.host, destinaton.port, exchange);
}

//-------------------------------------------------------------------

void
ioNetworkClient::connectFrom(const dodoString &local,
					  const dodoString &host,
					  int port,
					  ioNetworkExchange &exchange)
{
#ifndef IONETWORKCLIENT_WO_XEXEC
	operType = IONETWORKCLIENT_OPERATION_CONNECTFROM;
	performXExec(preExec);
#endif
	
	makeSocket();

	int sockFlag(1);
	if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &sockFlag, sizeof(int)) == -1)
		throw baseEx(ERRMODULE_IONETWORKCLIENT, IONETWORKCLIENTEX_CONNECTFROM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

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
			throw baseEx(ERRMODULE_IONETWORKCLIENT, IONETWORKCLIENTEX_CONNECTFROM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		sa.sin6_port = htons(port);
		inet_pton(AF_INET6, host.c_str(), &sa.sin6_addr);

		if (::connect(socket, (struct sockaddr *)&sa, sizeof(sa)) == -1)
		{
			if (::close(socket) == -1)
				throw baseEx(ERRMODULE_IONETWORKCLIENT, IONETWORKCLIENTEX_CONNECTFROM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

			socket = -1;

			throw baseEx(ERRMODULE_IONETWORKCLIENT, IONETWORKCLIENTEX_CONNECTFROM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}
	}
	else
	{
		struct sockaddr_in sa;
		sa.sin_family = AF_INET;
		sa.sin_port = htons(0);
		inet_aton(local.c_str(), &sa.sin_addr);

		if (::bind(socket, (struct sockaddr *)&sa, sizeof(sa)) == -1)
			throw baseEx(ERRMODULE_IONETWORKCLIENT, IONETWORKCLIENTEX_CONNECTFROM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		sa.sin_port = htons(port);
		inet_aton(host.c_str(), &sa.sin_addr);

		if (::connect(socket, (struct sockaddr *)&sa, sizeof(sa)) == -1)
		{
			if (::close(socket) == -1)
				throw baseEx(ERRMODULE_IONETWORKCLIENT, IONETWORKCLIENTEX_CONNECTFROM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

			socket = -1;

			throw baseEx(ERRMODULE_IONETWORKCLIENT, IONETWORKCLIENTEX_CONNECTFROM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}
	}

	exchange.blocked = blocked;
	exchange.init(socket, blockInherited);

	socket = -1;

#ifndef IONETWORKCLIENT_WO_XEXEC
	performXExec(postExec);
#endif
}

//-------------------------------------------------------------------

void
ioNetworkClient::connectFrom(const dodoString &local,
					  const __connInfo &destinaton,
					  ioNetworkExchange &exchange)
{
	connectFrom(local, destinaton.host, destinaton.port, exchange);
}

//-------------------------------------------------------------------

void
ioNetworkClient::connect(const dodoString &path,
				  ioNetworkExchange &exchange)
{
#ifndef IONETWORKCLIENT_WO_XEXEC
	operType = IONETWORKCLIENT_OPERATION_CONNECT_UNIX;
	performXExec(preExec);
#endif

	makeSocket();

	struct sockaddr_un sa;

	strcpy(sa.sun_path, path.c_str());
	sa.sun_family = AF_UNIX;

	if (::connect(socket, (struct sockaddr *)&sa, path.size() + sizeof(sa.sun_family)) == -1)
	{
		if (::close(socket) == -1)
			throw baseEx(ERRMODULE_IONETWORKCLIENT, IONETWORKCLIENTEX_CONNECT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		socket = -1;

		throw baseEx(ERRMODULE_IONETWORKCLIENT, IONETWORKCLIENTEX_CONNECT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	exchange.blocked = blocked;
	exchange.init(socket, blockInherited);

	socket = -1;

#ifndef IONETWORKCLIENT_WO_XEXEC
	performXExec(postExec);
#endif
}

//-------------------------------------------------------------------

