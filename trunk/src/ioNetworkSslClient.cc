/***************************************************************************
 *            ioNetworkSslClient.cc
 *
 *  Tue Jun 10 17:00:47 2008
 *  Copyright  2008  Ni@m
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

#include <libdodo/ioNetworkSslClient.h>

#ifdef OPENSSL_EXT

using namespace dodo::io::network::ssl;

#ifndef IONETWORKSSLCLIENT_WO_XEXEC

__xexexIoNetworkClientCollectedData::__xexexIoNetworkClientCollectedData(int &a_operType,
																		 void *a_executor) : operType(a_operType),
																							 executor(a_executor)
{
}

#endif

//-------------------------------------------------------------------

client::client(client &fs)

#ifndef IONETWORKSSLCLIENT_WO_XEXEC

	: collectedData(operType,
					(void *) this)

#endif

{
}

//-------------------------------------------------------------------

client::client(short a_family,
			   short a_type) : family(a_family),
							   type(a_type),
							   blockInherited(false),
							   sslCtx(NULL),
							   sslHandle(NULL),
							   sslConnected(false)
#ifndef IONETWORKSSLCLIENT_WO_XEXEC

							   ,
							   collectedData(operType,
											 (void *) this)

#endif

{
#ifndef IONETWORKSSLCLIENT_WO_XEXEC

	execObject = XEXEC_OBJECT_IONETWORKSSLCLIENT;
	execObjectData = (void *)&collectedData;

#endif

}

//-------------------------------------------------------------------

client::~client()
{
	if (sslHandle != NULL)
	{
		if (sslConnected && SSL_shutdown(sslHandle) == 0)
			SSL_shutdown(sslHandle);

		SSL_free(sslHandle);
	}

	if (sslCtx != NULL)
		SSL_CTX_free(sslCtx);

	if (socket != -1)
	{
		::shutdown(socket, SHUT_RDWR);

		::close(socket);
	}
}

//-------------------------------------------------------------------

void
client::initSsl()
{
	if (sslCtx == NULL)
	{
		sslCtx = SSL_CTX_new(SSLv23_client_method());
		if (sslCtx == NULL)
			throw baseEx(ERRMODULE_IONETWORKSSLCLIENT, CLIENTEX_INITSSL, ERR_LIBDODO, CLIENTEX_UNABLETOINITCONTEXT, IONETWORKSSLCLIENTEX_UNABLETOINITCONTEXT_STR, __LINE__, __FILE__);
	}

	if (sslHandle == NULL)
	{
		sslHandle = SSL_new(sslCtx);
		if (sslHandle == NULL)
			throw baseEx(ERRMODULE_IONETWORKSSLCLIENT, CLIENTEX_INITSSL, ERR_LIBDODO, CLIENTEX_UNABLETOINITSSL, IONETWORKSSLCLIENTEX_UNABLETOINITSSL_STR, __LINE__, __FILE__);
	}
}

//-------------------------------------------------------------------

void
client::connectSsl()
{
	__openssl_init_object__.addEntropy();

	if (sslConnected)
	{
		int err = SSL_shutdown(sslHandle);
		if (err < 0)
		{
			unsigned long nerr = ERR_get_error();
			throw baseEx(ERRMODULE_IONETWORKSSLCLIENT, CLIENTEX_CONNECTSSL, ERR_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
		}
		if (err == 0)
		{
			err = SSL_shutdown(sslHandle);
			if (err < 0)
			{
				unsigned long nerr = ERR_get_error();
				throw baseEx(ERRMODULE_IONETWORKSSLCLIENT, CLIENTEX_CONNECTSSL, ERR_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
			}
		}

		sslConnected = false;
	}

	if (SSL_clear(sslHandle) == 0)
	{
		unsigned long nerr = ERR_get_error();
		throw baseEx(ERRMODULE_IONETWORKSSLCLIENT, CLIENTEX_CONNECTSSL, ERR_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
	}

	if (SSL_set_fd(sslHandle, socket) == 0)
	{
		unsigned long nerr = ERR_get_error();
		throw baseEx(ERRMODULE_IONETWORKSSLCLIENT, CLIENTEX_CONNECTSSL, ERR_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
	}

	int res = SSL_connect(sslHandle);
	switch (res)
	{
		case 1:
			break;

		case 0:
		{
			unsigned long nerr = ERR_get_error();
			throw baseEx(ERRMODULE_IONETWORKSSLCLIENT, CLIENTEX_CONNECTSSL, ERR_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
		}

		case - 1:
		{
			int nerr = SSL_get_error(sslHandle, res);
			if (nerr == SSL_ERROR_WANT_READ || nerr == SSL_ERROR_WANT_WRITE || nerr == SSL_ERROR_WANT_X509_LOOKUP)
				break;
		}

		default:
		{
			unsigned long nerr;

			int err = SSL_shutdown(sslHandle);
			if (err < 0)
			{
				nerr = ERR_get_error();
				throw baseEx(ERRMODULE_IONETWORKSSLCLIENT, CLIENTEX_CONNECTSSL, ERR_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
			}
			if (err == 0)
			{
				err = SSL_shutdown(sslHandle);
				if (err < 0)
				{
					nerr = ERR_get_error();
					throw baseEx(ERRMODULE_IONETWORKSSLCLIENT, CLIENTEX_CONNECTSSL, ERR_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
				}
			}


			nerr = ERR_get_error();
			throw baseEx(ERRMODULE_IONETWORKSSLCLIENT, CLIENTEX_CONNECTSSL, ERR_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
		}
	}

	sslConnected = true;
}

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
			throw baseEx(ERRMODULE_IONETWORKSSLCLIENT, CLIENTEX_MAKESOCKET, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

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

			throw baseEx(ERRMODULE_IONETWORKSSLCLIENT, CLIENTEX_MAKESOCKET, ERR_LIBDODO, CLIENTEX_WRONGPARAMETER, IONETWORKSSLCLIENTEX_WRONGPARAMETER_STR, __LINE__, __FILE__);
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

			throw baseEx(ERRMODULE_IONETWORKSSLCLIENT, CLIENTEX_MAKESOCKET, ERR_LIBDODO, CLIENTEX_WRONGPARAMETER, IONETWORKSSLCLIENTEX_WRONGPARAMETER_STR, __LINE__, __FILE__);
	}

	socket = ::socket(real_domain, real_type, 0);
	if (socket == -1)
		throw baseEx(ERRMODULE_IONETWORKSSLCLIENT, CLIENTEX_MAKESOCKET, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	restoreOptions();
}

//-------------------------------------------------------------------

void
client::connect(const dodoString &host,
				int port,
				exchange &exchange)
{
#ifndef IONETWORKSSLCLIENT_WO_XEXEC
	operType = CLIENT_OPERATION_CONNECT;
	performXExec(preExec);
#endif

	initSsl();
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
				throw baseEx(ERRMODULE_IONETWORKSSLCLIENT, CLIENTEX_CONNECT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

			socket = -1;

			throw baseEx(ERRMODULE_IONETWORKSSLCLIENT, CLIENTEX_CONNECT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
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
				throw baseEx(ERRMODULE_IONETWORKSSLCLIENT, CLIENTEX_CONNECT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

			socket = -1;

			throw baseEx(ERRMODULE_IONETWORKSSLCLIENT, CLIENTEX_CONNECT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}
	}

	connectSsl();

	exchange.init(socket, sslHandle, blocked, blockInherited);

	socket = -1;
	sslHandle = NULL;

#ifndef IONETWORKSSLCLIENT_WO_XEXEC
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
#ifndef IONETWORKSSLCLIENT_WO_XEXEC
	operType = CLIENT_OPERATION_CONNECTFROM;
	performXExec(preExec);
#endif

	initSsl();
	makeSocket();

	int sockFlag(1);
	if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &sockFlag, sizeof(int)) == -1)
		throw baseEx(ERRMODULE_IONETWORKSSLCLIENT, CLIENTEX_CONNECTFROM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

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
			throw baseEx(ERRMODULE_IONETWORKSSLCLIENT, CLIENTEX_CONNECTFROM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		sa.sin6_port = htons(port);
		inet_pton(AF_INET6, host.c_str(), &sa.sin6_addr);

		if (::connect(socket, (struct sockaddr *)&sa, sizeof(sa)) == -1)
		{
			if (::close(socket) == -1)
				throw baseEx(ERRMODULE_IONETWORKSSLCLIENT, CLIENTEX_CONNECTFROM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

			socket = -1;

			throw baseEx(ERRMODULE_IONETWORKSSLCLIENT, CLIENTEX_CONNECTFROM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}
	}
	else
	{
		struct sockaddr_in sa;
		sa.sin_family = AF_INET;
		sa.sin_port = htons(0);
		inet_aton(local.c_str(), &sa.sin_addr);

		if (::bind(socket, (struct sockaddr *)&sa, sizeof(sa)) == -1)
			throw baseEx(ERRMODULE_IONETWORKSSLCLIENT, CLIENTEX_CONNECTFROM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		sa.sin_port = htons(port);
		inet_aton(host.c_str(), &sa.sin_addr);

		if (::connect(socket, (struct sockaddr *)&sa, sizeof(sa)) == -1)
		{
			if (::close(socket) == -1)
				throw baseEx(ERRMODULE_IONETWORKSSLCLIENT, CLIENTEX_CONNECTFROM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

			socket = -1;

			throw baseEx(ERRMODULE_IONETWORKSSLCLIENT, CLIENTEX_CONNECTFROM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}
	}

	connectSsl();

	exchange.init(socket, sslHandle, blocked, blockInherited);

	socket = -1;
	sslHandle = NULL;

#ifndef IONETWORKSSLCLIENT_WO_XEXEC
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
#ifndef IONETWORKSSLCLIENT_WO_XEXEC
	operType = CLIENT_OPERATION_CONNECT_UNIX;
	performXExec(preExec);
#endif

	initSsl();
	makeSocket();

	struct sockaddr_un sa;

	strcpy(sa.sun_path, path.c_str());
	sa.sun_family = AF_UNIX;

	if (::connect(socket, (struct sockaddr *)&sa, path.size() + sizeof(sa.sun_family)) == -1)
	{
		if (::close(socket) == -1)
			throw baseEx(ERRMODULE_IONETWORKSSLCLIENT, CLIENTEX_CONNECT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		socket = -1;

		throw baseEx(ERRMODULE_IONETWORKSSLCLIENT, CLIENTEX_CONNECT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	connectSsl();

	exchange.init(socket, sslHandle, blocked, blockInherited);

	socket = -1;
	sslHandle = NULL;

#ifndef IONETWORKSSLCLIENT_WO_XEXEC
	performXExec(postExec);
#endif
}

//-------------------------------------------------------------------

#endif

