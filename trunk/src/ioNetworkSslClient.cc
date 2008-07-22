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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/ioNetworkSslClient.h>

#ifdef OPENSSL_EXT

using namespace dodo::io::network::ssl;

client::client(client &fs) : network::client(fs)
{
}

//-------------------------------------------------------------------

client::client(short a_family,
			   short a_type) : network::client(a_family,
			   								   a_type),
							   sslCtx(NULL),
							   sslHandle(NULL),
							   sslConnected(false)
{
#ifndef IO_WO_XEXEC

	execObject = XEXEC_OBJECT_IONETWORKSSLCLIENT;

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
}

//-------------------------------------------------------------------

void
client::removeSertificates()
{
	if (sslHandle != NULL)
	{
		if (sslConnected)
		{
			int err = SSL_shutdown(sslHandle);
			if (err < 0)
			{
				unsigned long nerr = ERR_get_error();
				throw baseEx(ERRMODULE_IONETWORKSSLCLIENT, CLIENTEX_REMOVESERTIFICATES, ERR_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
			}
			if (err == 0)
			{
				err = SSL_shutdown(sslHandle);
				if (err < 0)
				{
					unsigned long nerr = ERR_get_error();
					throw baseEx(ERRMODULE_IONETWORKSSLCLIENT, CLIENTEX_REMOVESERTIFICATES, ERR_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
				}
			}

			sslConnected = false;
		}

		SSL_free(sslHandle);

		sslHandle = NULL;
	}

	if (sslCtx != NULL)
	{
		SSL_CTX_free(sslCtx);

		sslCtx = NULL;
	}

	sslCtx = SSL_CTX_new(SSLv23_client_method());
	if (sslCtx == NULL)
		throw baseEx(ERRMODULE_IONETWORKSSLCLIENT, CLIENTEX_REMOVESERTIFICATES, ERR_LIBDODO, CLIENTEX_UNABLETOINITCONTEXT, IONETWORKSSLCLIENTEX_UNABLETOINITCONTEXT_STR, __LINE__, __FILE__);

	sslHandle = SSL_new(sslCtx);
	if (sslHandle == NULL)
		throw baseEx(ERRMODULE_IONETWORKSSLCLIENT, CLIENTEX_REMOVESERTIFICATES, ERR_LIBDODO, CLIENTEX_UNABLETOINITSSL, IONETWORKSSLCLIENTEX_UNABLETOINITSSL_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
client::setSertificates(const io::ssl::__certificates &certs)
{
	if (sslHandle != NULL)
	{
		if (sslConnected)
		{
			int err = SSL_shutdown(sslHandle);
			if (err < 0)
			{
				unsigned long nerr = ERR_get_error();
				throw baseEx(ERRMODULE_IONETWORKSSLCLIENT, CLIENTEX_SETSERTIFICATES, ERR_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
			}
			if (err == 0)
			{
				err = SSL_shutdown(sslHandle);
				if (err < 0)
				{
					unsigned long nerr = ERR_get_error();
					throw baseEx(ERRMODULE_IONETWORKSSLCLIENT, CLIENTEX_SETSERTIFICATES, ERR_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
				}
			}

			sslConnected = false;
		}

		SSL_free(sslHandle);

		sslHandle = NULL;
	}

	if (sslCtx != NULL)
	{
		SSL_CTX_free(sslCtx);

		sslCtx = NULL;
	}

	sslCtx = SSL_CTX_new(SSLv23_client_method());
	if (sslCtx == NULL)
		throw baseEx(ERRMODULE_IONETWORKSSLCLIENT, CLIENTEX_SETSERTIFICATES, ERR_LIBDODO, CLIENTEX_UNABLETOINITCONTEXT, IONETWORKSSLCLIENTEX_UNABLETOINITCONTEXT_STR, __LINE__, __FILE__);

	if (certs.cipher.size() > 0 && SSL_CTX_set_cipher_list(sslCtx, certs.cipher.c_str()) != 1)
	{
		unsigned long nerr = ERR_get_error();
		throw baseEx(ERRMODULE_IONETWORKSSLCLIENT, CLIENTEX_SETSERTIFICATES, ERR_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
	}

	if (certs.ca.size() > 0 && SSL_CTX_use_certificate_chain_file(sslCtx, certs.ca.c_str()) != 1)
	{
		unsigned long nerr = ERR_get_error();
		throw baseEx(ERRMODULE_IONETWORKSSLCLIENT, CLIENTEX_SETSERTIFICATES, ERR_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
	}

	if (certs.cert.size() > 0 && SSL_CTX_use_certificate_file(sslCtx, certs.cert.c_str(), SSL_FILETYPE_PEM) != 1)
	{
		unsigned long nerr = ERR_get_error();
		throw baseEx(ERRMODULE_IONETWORKSSLCLIENT, CLIENTEX_SETSERTIFICATES, ERR_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
	}

	if (certs.keyPassword.size() > 0)
		SSL_CTX_set_default_passwd_cb_userdata(sslCtx, (void *)certs.keyPassword.c_str());

	bool keySet = false;

	if (certs.key.size() > 0)
		switch (certs.keyType)
		{
			case io::ssl::KEYTYPE_PKEY:

				if (SSL_CTX_use_PrivateKey_file(sslCtx, certs.key.c_str(), SSL_FILETYPE_PEM) != 1)
				{
					unsigned long nerr = ERR_get_error();
					throw baseEx(ERRMODULE_IONETWORKSSLCLIENT, CLIENTEX_SETSERTIFICATES, ERR_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
				}

				keySet = true;

				break;

			case io::ssl::KEYTYPE_RSA:

				if (SSL_CTX_use_RSAPrivateKey_file(sslCtx, certs.key.c_str(), SSL_FILETYPE_PEM) != 1)
				{
					unsigned long nerr = ERR_get_error();
					throw baseEx(ERRMODULE_IONETWORKSSLCLIENT, CLIENTEX_SETSERTIFICATES, ERR_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
				}

				keySet = true;

				break;

			default:

				throw baseEx(ERRMODULE_IONETWORKSSLCLIENT, CLIENTEX_SETSERTIFICATES, ERR_LIBDODO, CLIENTEX_UNKNOWNKEYTYPE, IONETWORKSSLCLIENTEX_UNKNOWNKEYTYPE_STR, __LINE__, __FILE__);
		}
	else
	{
		if (certs.ca.size() > 0)
		{
			if (SSL_CTX_use_PrivateKey_file(sslCtx, certs.ca.c_str(), SSL_FILETYPE_PEM) != 1)
			{
				unsigned long nerr = ERR_get_error();
				throw baseEx(ERRMODULE_IONETWORKSSLCLIENT, CLIENTEX_SETSERTIFICATES, ERR_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
			}

			keySet = true;
		}
	}

	if (certs.caPath.size() > 0)
	{
		if (tools::filesystem::getFileInfo(certs.caPath).type == tools::FILESYSTEM_FILETYPE_DIRECTORY)
		{
			if (SSL_CTX_load_verify_locations(sslCtx, NULL, certs.caPath.c_str()) != 1)
			{
				unsigned long nerr = ERR_get_error();
				throw baseEx(ERRMODULE_IONETWORKSSLCLIENT, CLIENTEX_SETSERTIFICATES, ERR_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
			}
		}
		else
		{
			if (SSL_CTX_load_verify_locations(sslCtx, certs.caPath.c_str(), NULL) != 1)
			{
				unsigned long nerr = ERR_get_error();
				throw baseEx(ERRMODULE_IONETWORKSSLCLIENT, CLIENTEX_SETSERTIFICATES, ERR_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
			}
		}
	}

	if (keySet && SSL_CTX_check_private_key(sslCtx) != 1)
	{
		unsigned long nerr = ERR_get_error();
		throw baseEx(ERRMODULE_IONETWORKSSLCLIENT, CLIENTEX_SETSERTIFICATES, ERR_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
	}

	sslHandle = SSL_new(sslCtx);
	if (sslHandle == NULL)
		throw baseEx(ERRMODULE_IONETWORKSSLCLIENT, CLIENTEX_SETSERTIFICATES, ERR_LIBDODO, CLIENTEX_UNABLETOINITSSL, IONETWORKSSLCLIENTEX_UNABLETOINITSSL_STR, __LINE__, __FILE__);
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
	io::ssl::__openssl_init_object__.addEntropy();

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
			unsigned long nerr = ERR_get_error();

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

			throw baseEx(ERRMODULE_IONETWORKSSLCLIENT, CLIENTEX_CONNECTSSL, ERR_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
		}
	}

	sslConnected = true;
}

//-------------------------------------------------------------------

void
client::connect(const dodoString &host,
				int port,
				exchange &exchange)
{
#ifndef IO_WO_XEXEC
	operType = CLIENT_OPERATION_CONNECT;
	performXExec(preExec);
#endif

	initSsl();
	makeSocket();

	if (family == OPTIONS_PROTO_FAMILY_IPV4)
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
	else
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

	connectSsl();

	exchange.init(socket, sslHandle, blocked, blockInherited);

	socket = -1;
	sslHandle = NULL;

#ifndef IO_WO_XEXEC
	performXExec(postExec);
#endif
}

//-------------------------------------------------------------------

void
client::connectFrom(const dodoString &local,
					const dodoString &host,
					int port,
					exchange &exchange)
{
#ifndef IO_WO_XEXEC
	operType = CLIENT_OPERATION_CONNECTFROM;
	performXExec(preExec);
#endif

	initSsl();
	makeSocket();

	int sockFlag(1);
	if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &sockFlag, sizeof(int)) == -1)
		throw baseEx(ERRMODULE_IONETWORKSSLCLIENT, CLIENTEX_CONNECTFROM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	addFlag(socketOpts, 1 << OPTIONS_OPTION_REUSE_ADDRESS);

	if (family == OPTIONS_PROTO_FAMILY_IPV4)
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
	else
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

	connectSsl();

	exchange.init(socket, sslHandle, blocked, blockInherited);

	socket = -1;
	sslHandle = NULL;

#ifndef IO_WO_XEXEC
	performXExec(postExec);
#endif
}

//-------------------------------------------------------------------

void
client::connect(const dodoString &path,
				exchange &exchange)
{
#ifndef IO_WO_XEXEC
	operType = CLIENT_OPERATION_CONNECT_UNIX;
	performXExec(preExec);
#endif

	initSsl();
	makeSocket();

	struct sockaddr_un sa;

	unsigned long size = path.size();

	if (size >= 108)
		throw baseEx(ERRMODULE_IONETWORKSSLCLIENT, CLIENTEX_CONNECT, ERR_LIBDODO, CLIENTEX_LONGPATH, IONETWORKCLIENTEX_LONGPATH_STR, __LINE__, __FILE__);

	strncpy(sa.sun_path, path.c_str(), size);
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

#ifndef IO_WO_XEXEC
	performXExec(postExec);
#endif
}

//-------------------------------------------------------------------

#endif
