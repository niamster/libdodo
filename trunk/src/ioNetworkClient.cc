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

using namespace dodo::io::network;

#ifndef IONETWORKCLIENT_WO_XEXEC

__xexexIoNetworkClientCollectedData::__xexexIoNetworkClientCollectedData(int &a_operType,
											   void *a_executor) : operType(a_operType),
																   executor(a_executor)
{
}

#endif

//-------------------------------------------------------------------

client::client(client &fs)

#ifndef IONETWORKCLIENT_WO_XEXEC

: collectedData(operType,
										 (void *) this)

#endif

{
}

//-------------------------------------------------------------------

client::client(short a_family,
				   short a_type) : options(a_family, a_type),
								   blockInherited(false)
#ifndef IONETWORKCLIENT_WO_XEXEC

								   ,
								   collectedData(operType,
												 (void *) this)

#endif

{
}


//-------------------------------------------------------------------

client::~client()
{
}

//-------------------------------------------------------------------

#ifndef IONETWORKCLIENT_WO_XEXEC

int
client::addPostExec(inExec func,
					  void   *data)
{
	return _addPostExec(func, (void *)&collectedData, XEXEC_OBJECT_IONETWORKCLIENT, data);
}

//-------------------------------------------------------------------

int
client::addPreExec(inExec func,
					 void   *data)
{
	return _addPreExec(func, (void *)&collectedData, XEXEC_OBJECT_IONETWORKCLIENT, data);
}

//-------------------------------------------------------------------

#ifdef DL_EXT

int
client::addPostExec(const dodoString &module,
					  void             *data,
					  void             *toInit)
{
	return _addPostExec(module, (void *)&collectedData, XEXEC_OBJECT_IONETWORKCLIENT, data, toInit);
}

//-------------------------------------------------------------------

int
client::addPreExec(const dodoString &module,
					 void             *data,
					 void             *toInit)
{
	return _addPreExec(module, (void *)&collectedData, XEXEC_OBJECT_IONETWORKCLIENT, data, toInit);
}

//-------------------------------------------------------------------

dodo::__xexecCounts
client::addExec(const dodoString &module,
				  void             *data,
				  void             *toInit)
{
	return _addExec(module, (void *)&collectedData, XEXEC_OBJECT_IONETWORKCLIENT, data, toInit);
}

#endif

#endif

//-------------------------------------------------------------------

void
client::restoreOptions()
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
client::makeSocket()
{
	if (socket != -1)
	{
		::shutdown(socket, SHUT_RDWR);

		if (::close(socket) == -1)
			throw baseEx(ERRMODULE_IONETWORKCLIENT, CLIENTEX_MAKESOCKET, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		socket = -1;
	}

	int real_domain(PF_INET), real_type(SOCK_STREAM);

	switch (family)
	{
		case OPTIONS_PROTO_FAMILY_IPV4:

			real_domain = PF_INET;

			break;

		case OPTIONS_PROTO_FAMILY_IPV6:

			real_domain = PF_INET6;

			break;

		case OPTIONS_PROTO_FAMILY_UNIX_SOCKET:

			real_domain = PF_UNIX;

			break;

		default:

			throw baseEx(ERRMODULE_IONETWORKCLIENT, CLIENTEX_MAKESOCKET, ERR_LIBDODO, CLIENTEX_WRONGPARAMETER, CLIENTEX_WRONGPARAMETER_STR, __LINE__, __FILE__);
	}

	switch (type)
	{
		case OPTIONS_TRANSFER_TYPE_STREAM:

			real_type = SOCK_STREAM;

			break;

		case OPTIONS_TRANSFER_TYPE_DATAGRAM:

			real_type = SOCK_DGRAM;

			break;

		default:

			throw baseEx(ERRMODULE_IONETWORKCLIENT, CLIENTEX_MAKESOCKET, ERR_LIBDODO, CLIENTEX_WRONGPARAMETER, CLIENTEX_WRONGPARAMETER_STR, __LINE__, __FILE__);
	}

	socket = ::socket(real_domain, real_type, 0);
	if (socket == -1)
		throw baseEx(ERRMODULE_IONETWORKCLIENT, CLIENTEX_MAKESOCKET, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	restoreOptions();
}

//-------------------------------------------------------------------

void
client::connect(const dodoString &host,
				  int port,
				  exchange &exchange)
{
#ifndef IONETWORKCLIENT_WO_XEXEC
	operType = CLIENT_OPERATION_CONNECT;
	performXExec(preExec);
#endif

	makeSocket();

	if (family == OPTIONS_PROTO_FAMILY_IPV6)
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
				throw baseEx(ERRMODULE_IONETWORKCLIENT, CLIENTEX_CONNECT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			
			socket = -1;

			throw baseEx(ERRMODULE_IONETWORKCLIENT, CLIENTEX_CONNECT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
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
				throw baseEx(ERRMODULE_IONETWORKCLIENT, CLIENTEX_CONNECT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

			socket = -1;

			throw baseEx(ERRMODULE_IONETWORKCLIENT, CLIENTEX_CONNECT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
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
client::connect(const __connInfo &destinaton,
				  exchange &exchange)
{
	connect(destinaton.host, destinaton.port, exchange);
}

//-------------------------------------------------------------------

void
client::connectFrom(const dodoString &local,
					  const dodoString &host,
					  int port,
					  exchange &exchange)
{
#ifndef IONETWORKCLIENT_WO_XEXEC
	operType = CLIENT_OPERATION_CONNECTFROM;
	performXExec(preExec);
#endif
	
	makeSocket();

	int sockFlag(1);
	if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &sockFlag, sizeof(int)) == -1)
		throw baseEx(ERRMODULE_IONETWORKCLIENT, CLIENTEX_CONNECTFROM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	addFlag(socketOpts, 1 << OPTIONS_OPTION_REUSE_ADDRESS);

	if (family == OPTIONS_PROTO_FAMILY_IPV6)
	{
		struct sockaddr_in6 sa;
		sa.sin6_family = AF_INET6;
		sa.sin6_flowinfo = 0;
		sa.sin6_scope_id = 0;
		sa.sin6_port = htons(0);
		inet_pton(AF_INET6, local.c_str(), &sa.sin6_addr);

		if (::bind(socket, (struct sockaddr *)&sa, sizeof(sa)) == -1)
			throw baseEx(ERRMODULE_IONETWORKCLIENT, CLIENTEX_CONNECTFROM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		sa.sin6_port = htons(port);
		inet_pton(AF_INET6, host.c_str(), &sa.sin6_addr);

		if (::connect(socket, (struct sockaddr *)&sa, sizeof(sa)) == -1)
		{
			if (::close(socket) == -1)
				throw baseEx(ERRMODULE_IONETWORKCLIENT, CLIENTEX_CONNECTFROM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

			socket = -1;

			throw baseEx(ERRMODULE_IONETWORKCLIENT, CLIENTEX_CONNECTFROM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}
	}
	else
	{
		struct sockaddr_in sa;
		sa.sin_family = AF_INET;
		sa.sin_port = htons(0);
		inet_aton(local.c_str(), &sa.sin_addr);

		if (::bind(socket, (struct sockaddr *)&sa, sizeof(sa)) == -1)
			throw baseEx(ERRMODULE_IONETWORKCLIENT, CLIENTEX_CONNECTFROM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		sa.sin_port = htons(port);
		inet_aton(host.c_str(), &sa.sin_addr);

		if (::connect(socket, (struct sockaddr *)&sa, sizeof(sa)) == -1)
		{
			if (::close(socket) == -1)
				throw baseEx(ERRMODULE_IONETWORKCLIENT, CLIENTEX_CONNECTFROM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

			socket = -1;

			throw baseEx(ERRMODULE_IONETWORKCLIENT, CLIENTEX_CONNECTFROM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
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
client::connectFrom(const dodoString &local,
					  const __connInfo &destinaton,
					  exchange &exchange)
{
	connectFrom(local, destinaton.host, destinaton.port, exchange);
}

//-------------------------------------------------------------------

void
client::connect(const dodoString &path,
				  exchange &exchange)
{
#ifndef IONETWORKCLIENT_WO_XEXEC
	operType = CLIENT_OPERATION_CONNECT_UNIX;
	performXExec(preExec);
#endif

	makeSocket();

	struct sockaddr_un sa;

	strcpy(sa.sun_path, path.c_str());
	sa.sun_family = AF_UNIX;

	if (::connect(socket, (struct sockaddr *)&sa, path.size() + sizeof(sa.sun_family)) == -1)
	{
		if (::close(socket) == -1)
			throw baseEx(ERRMODULE_IONETWORKCLIENT, CLIENTEX_CONNECT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		socket = -1;

		throw baseEx(ERRMODULE_IONETWORKCLIENT, CLIENTEX_CONNECT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	exchange.blocked = blocked;
	exchange.init(socket, blockInherited);

	socket = -1;

#ifndef IONETWORKCLIENT_WO_XEXEC
	performXExec(postExec);
#endif
}

//-------------------------------------------------------------------

