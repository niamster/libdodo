/***************************************************************************
 *            ioNetworkSslServer.cc
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

#include <libdodo/ioNetworkSslServer.h>

#ifdef OPENSSL_EXT

using namespace dodo::io::network::ssl;

server::server(server &fs) : network::server(fs)
{
}

//-------------------------------------------------------------------

server::server(short a_family,
			   short a_type) : network::server(a_family,
											   a_type),
							   sslCtx(NULL)
{
#ifndef IO_WO_XEXEC

	collectedData.executor = (void *)this;
	execObject = XEXEC_OBJECT_IONETWORKSSLSERVER;

#endif
}


//-------------------------------------------------------------------

server::~server()
{
	if (sslCtx != NULL)
		SSL_CTX_free(sslCtx);
}


//-------------------------------------------------------------------

void
server::removeSertificates()
{
	if (sslCtx != NULL)
	{
		SSL_CTX_free(sslCtx);

		sslCtx = NULL;
	}

	sslCtx = SSL_CTX_new(SSLv23_server_method());
	if (sslCtx == NULL)
		throw exception::basic(exception::ERRMODULE_IONETWORKSSLSERVER, SERVEREX_REMOVESERTIFICATES, exception::ERRNO_LIBDODO, SERVEREX_UNABLETOINITCONTEXT, IONETWORKSSLSERVEREX_UNABLETOINITCONTEXT_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
server::setSertificates(const io::ssl::__certificates &certs)
{
	if (sslCtx != NULL)
		SSL_CTX_free(sslCtx);

	sslCtx = SSL_CTX_new(SSLv23_server_method());
	if (sslCtx == NULL)
		throw exception::basic(exception::ERRMODULE_IONETWORKSSLSERVER, SERVEREX_SETSERTIFICATES, exception::ERRNO_LIBDODO, SERVEREX_UNABLETOINITCONTEXT, IONETWORKSSLSERVEREX_UNABLETOINITCONTEXT_STR, __LINE__, __FILE__);

	if (certs.cipher.size() > 0 && SSL_CTX_set_cipher_list(sslCtx, certs.cipher.c_str()) != 1)
	{
		unsigned long nerr = ERR_get_error();
		throw exception::basic(exception::ERRMODULE_IONETWORKSSLSERVER, SERVEREX_SETSERTIFICATES, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
	}

	if (certs.ca.size() > 0 && SSL_CTX_use_certificate_chain_file(sslCtx, certs.ca.c_str()) != 1)
	{
		unsigned long nerr = ERR_get_error();
		throw exception::basic(exception::ERRMODULE_IONETWORKSSLSERVER, SERVEREX_SETSERTIFICATES, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
	}

	if (certs.cert.size() > 0 && SSL_CTX_use_certificate_file(sslCtx, certs.cert.c_str(), SSL_FILETYPE_PEM) != 1)
	{
		unsigned long nerr = ERR_get_error();
		throw exception::basic(exception::ERRMODULE_IONETWORKSSLSERVER, SERVEREX_SETSERTIFICATES, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
	}

	if (certs.keyPassword.size() > 0)
		SSL_CTX_set_default_passwd_cb_userdata(sslCtx, (void *)certs.keyPassword.c_str());

	bool keySet = false;

	if (certs.key.size() > 0)
	{
		switch (certs.keyType)
		{
			case io::ssl::KEYTYPE_PKEY:

				if (SSL_CTX_use_PrivateKey_file(sslCtx, certs.key.c_str(), SSL_FILETYPE_PEM) != 1)
				{
					unsigned long nerr = ERR_get_error();
					throw exception::basic(exception::ERRMODULE_IONETWORKSSLSERVER, SERVEREX_SETSERTIFICATES, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
				}

				keySet = true;

				break;

			case io::ssl::KEYTYPE_RSA:

				if (SSL_CTX_use_RSAPrivateKey_file(sslCtx, certs.key.c_str(), SSL_FILETYPE_PEM) != 1)
				{
					unsigned long nerr = ERR_get_error();
					throw exception::basic(exception::ERRMODULE_IONETWORKSSLSERVER, SERVEREX_SETSERTIFICATES, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
				}

				keySet = true;

				break;

			default:

				throw exception::basic(exception::ERRMODULE_IONETWORKSSLSERVER, SERVEREX_SETSERTIFICATES, exception::ERRNO_LIBDODO, SERVEREX_UNKNOWNKEYTYPE, IONETWORKSSLSERVEREX_UNKNOWNKEYTYPE_STR, __LINE__, __FILE__);
		}
	}
	else
	{
		if (certs.ca.size() > 0)
		{
			if (SSL_CTX_use_PrivateKey_file(sslCtx, certs.ca.c_str(), SSL_FILETYPE_PEM) != 1)
			{
				unsigned long nerr = ERR_get_error();
				throw exception::basic(exception::ERRMODULE_IONETWORKSSLSERVER, SERVEREX_SETSERTIFICATES, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
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
				throw exception::basic(exception::ERRMODULE_IONETWORKSSLSERVER, SERVEREX_SETSERTIFICATES, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
			}
		}
		else
		{
			if (SSL_CTX_load_verify_locations(sslCtx, certs.caPath.c_str(), NULL) != 1)
			{
				unsigned long nerr = ERR_get_error();
				throw exception::basic(exception::ERRMODULE_IONETWORKSSLSERVER, SERVEREX_SETSERTIFICATES, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
			}
		}
	}

	if (keySet && SSL_CTX_check_private_key(sslCtx) != 1)
	{
		unsigned long nerr = ERR_get_error();
		throw exception::basic(exception::ERRMODULE_IONETWORKSSLSERVER, SERVEREX_SETSERTIFICATES, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
	}
}

//-------------------------------------------------------------------

void
server::initSsl()
{
	if (sslCtx == NULL)
	{
		sslCtx = SSL_CTX_new(SSLv23_server_method());
		if (sslCtx == NULL)
			throw exception::basic(exception::ERRMODULE_IONETWORKSSLSERVER, SERVEREX_INITSSL, exception::ERRNO_LIBDODO, SERVEREX_UNABLETOINITCONTEXT, IONETWORKSSLSERVEREX_UNABLETOINITCONTEXT_STR, __LINE__, __FILE__);
	}
}

//-------------------------------------------------------------------

void
server::acceptSsl(__initialAccept &init)
{
	io::ssl::__openssl_init_object__.addEntropy();

	init.sslHandle = SSL_new(sslCtx);
	if (init.sslHandle == NULL)
		throw exception::basic(exception::ERRMODULE_IONETWORKSSLSERVER, SERVEREX_INITSSL, exception::ERRNO_LIBDODO, SERVEREX_UNABLETOINITSSL, IONETWORKSSLSERVEREX_UNABLETOINITSSL_STR, __LINE__, __FILE__);

	if (SSL_set_fd(init.sslHandle, init.socket) == 0)
	{
		unsigned long nerr = ERR_get_error();
		throw exception::basic(exception::ERRMODULE_IONETWORKSSLSERVER, SERVEREX_ACCEPTSSL, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
	}

	int res = SSL_accept(init.sslHandle);
	switch (res)
	{
		case 1:
			break;

		case 0:
		{
			unsigned long nerr = ERR_get_error();
			throw exception::basic(exception::ERRMODULE_IONETWORKSSLSERVER, SERVEREX_ACCEPTSSL, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
		}

		case - 1:
		{
			int nerr = SSL_get_error(init.sslHandle, res);
			if (nerr == SSL_ERROR_WANT_READ || nerr == SSL_ERROR_WANT_WRITE || nerr == SSL_ERROR_WANT_X509_LOOKUP)
				break;
		}

		default:
		{
			unsigned long nerr = ERR_get_error();

			int err = SSL_shutdown(init.sslHandle);
			if (err < 0)
			{
				nerr = ERR_get_error();
				throw exception::basic(exception::ERRMODULE_IONETWORKSSLSERVER, SERVEREX_ACCEPTSSL, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
			}
			if (err == 0)
			{
				err = SSL_shutdown(init.sslHandle);
				if (err < 0)
				{
					nerr = ERR_get_error();
					throw exception::basic(exception::ERRMODULE_IONETWORKSSLSERVER, SERVEREX_ACCEPTSSL, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
				}
			}

			throw exception::basic(exception::ERRMODULE_IONETWORKSSLSERVER, SERVEREX_ACCEPTSSL, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
		}
	}
}

//-------------------------------------------------------------------

void
server::serve(const dodoString &host,
					int port,
					int numberOfConnections)
{
#ifndef IO_WO_XEXEC
	operType = SERVER_OPERATION_BINDNLISTEN;
	performXExec(preExec);
#endif

	initSsl();
	makeSocket();

	int sockFlag(1);
	if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &sockFlag, sizeof(int)) == 1)
		throw exception::basic(exception::ERRMODULE_IONETWORKSSLSERVER, SERVEREX_BINDNLISTEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	addFlag(socketOpts, 1 << CONNECTION_OPTION_REUSE_ADDRESS);

	setLingerOption(IONETWORKCONNECTION_SOCKET_LINGER_OPTION, IONETWORKCONNECTION_SOCKET_LINGER_PERIOD);

	if (family == CONNECTION_PROTO_FAMILY_IPV6)
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
			throw exception::basic(exception::ERRMODULE_IONETWORKSSLSERVER, SERVEREX_BINDNLISTEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
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
			throw exception::basic(exception::ERRMODULE_IONETWORKSSLSERVER, SERVEREX_BINDNLISTEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	if (type == CONNECTION_TRANSFER_TYPE_STREAM)
		if (::listen(socket, numberOfConnections) == -1)
			throw exception::basic(exception::ERRMODULE_IONETWORKSSLSERVER, SERVEREX_BINDNLISTEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

#ifndef IO_WO_XEXEC
	performXExec(postExec);
#endif

}

//-------------------------------------------------------------------

void
server::serve(const dodoString &path,
					int numberOfConnections,
					bool force)
{
#ifndef IO_WO_XEXEC
	operType = SERVER_OPERATION_BINDNLISTEN_UNIX;
	performXExec(preExec);
#endif

	initSsl();
	makeSocket();

	if (force)
	{
		struct stat st;
		if (::lstat(path.c_str(), &st) != -1)
			if (S_ISSOCK(st.st_mode))
				::unlink(path.c_str());
			else
				throw exception::basic(exception::ERRMODULE_IONETWORKSSLSERVER, SERVEREX_BINDNLISTEN, exception::ERRNO_LIBDODO, SERVEREX_WRONGFILENAME, IONETWORKSSLSERVEREX_WRONGFILENAME_STR, __LINE__, __FILE__);
	}

	int sockFlag(1);
	if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &sockFlag, sizeof(int)) == -1)
		throw exception::basic(exception::ERRMODULE_IONETWORKSSLSERVER, SERVEREX_BINDNLISTEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	addFlag(socketOpts, 1 << CONNECTION_OPTION_REUSE_ADDRESS);

	setLingerOption(IONETWORKCONNECTION_SOCKET_LINGER_OPTION, IONETWORKCONNECTION_SOCKET_LINGER_PERIOD);

	struct sockaddr_un sa;

	unsigned long size = path.size();

	if (size >= 108)
		throw exception::basic(exception::ERRMODULE_IONETWORKSSLSERVER, SERVEREX_BINDNLISTEN, exception::ERRNO_LIBDODO, SERVEREX_LONGPATH, IONETWORKSSLSERVEREX_LONGPATH_STR, __LINE__, __FILE__);

	strncpy(sa.sun_path, path.c_str(), size);
	sa.sun_family = AF_UNIX;

	if (::bind(socket, (struct sockaddr *)&sa, path.size() + sizeof(sa.sun_family)) == -1)
		throw exception::basic(exception::ERRMODULE_IONETWORKSSLSERVER, SERVEREX_BINDNLISTEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	if (::listen(socket, numberOfConnections) == -1)
		throw exception::basic(exception::ERRMODULE_IONETWORKSSLSERVER, SERVEREX_BINDNLISTEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	unixSock = path;

#ifndef IO_WO_XEXEC
	performXExec(postExec);
#endif
}

//-------------------------------------------------------------------

bool
server::accept(__initialAccept &init,
			   __peerInfo &info)
{
#ifndef IO_WO_XEXEC
	operType = SERVER_OPERATION_ACCEPT;
	performXExec(preExec);
#endif

	if (type != CONNECTION_TRANSFER_TYPE_STREAM)
	{
		init.socket = socket;
		init.blocked = blocked;
		init.blockInherited = blockInherited;

		return true;
	}

	int sock(-1);
	info.host.clear();

	switch (family)
	{
		case CONNECTION_PROTO_FAMILY_IPV4:
		{
			struct sockaddr_in sa;
			socklen_t len = sizeof(sockaddr_in);
			sock = ::accept(socket, (sockaddr *)&sa, &len);

			if (sock == -1)
			{
				if (errno == EAGAIN)
					return false;
				else
					throw exception::basic(exception::ERRMODULE_IONETWORKSSLSERVER, SERVEREX_ACCEPT, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			}

			char temp[INET_ADDRSTRLEN];
			if (inet_ntop(AF_INET, &(sa.sin_addr), temp, INET_ADDRSTRLEN) != NULL)
				info.host.assign(temp);
			info.port = ntohs(sa.sin_port);

			break;
		}

		case CONNECTION_PROTO_FAMILY_IPV6:
		{
			struct sockaddr_in6 sa;
			socklen_t len = sizeof(sockaddr_in6);

			sock = ::accept(socket, (sockaddr *)&sa, &len);

			if (sock == -1)
			{
				if (errno == EAGAIN)
					return false;
				else
					throw exception::basic(exception::ERRMODULE_IONETWORKSSLSERVER, SERVEREX_ACCEPT, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			}

			char temp[INET6_ADDRSTRLEN];
			if (inet_ntop(AF_INET6, &(sa.sin6_addr), temp, INET6_ADDRSTRLEN) != NULL)
				info.host.assign(temp);
			info.port = ntohs(sa.sin6_port);

			break;
		}

		case CONNECTION_PROTO_FAMILY_UNIX_SOCKET:

			sock = ::accept(socket, NULL, NULL);
			if (sock == -1)
			{
				if (errno == EAGAIN)
					return false;
				else
					throw exception::basic(exception::ERRMODULE_IONETWORKSSLSERVER, SERVEREX_ACCEPT, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			}

			break;

		default:

			throw exception::basic(exception::ERRMODULE_IONETWORKSSLSERVER, SERVEREX_ACCEPT, exception::ERRNO_LIBDODO, SERVEREX_WRONGPARAMETER, IONETWORKSSLSERVEREX_WRONGPARAMETER_STR, __LINE__, __FILE__);
	}

	init.socket = sock;
	init.blocked = blocked;
	init.blockInherited = blockInherited;

	acceptSsl(init);

#ifndef IO_WO_XEXEC
	performXExec(postExec);
#endif

	return true;
}

//-------------------------------------------------------------------

bool
server::accept(__initialAccept &init)
{
#ifndef IO_WO_XEXEC
	operType = SERVER_OPERATION_ACCEPT;
	performXExec(preExec);
#endif

	if (type != CONNECTION_TRANSFER_TYPE_STREAM)
	{
		init.socket = socket;
		init.blocked = blocked;
		init.blockInherited = blockInherited;

		return true;
	}

	int sock = ::accept(socket, NULL, NULL);
	if (sock == -1)
	{
		if (errno == EAGAIN)
			return false;
		else
			throw exception::basic(exception::ERRMODULE_IONETWORKSSLSERVER, SERVEREX_ACCEPT, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	init.socket = sock;
	init.blocked = blocked;
	init.blockInherited = blockInherited;

	acceptSsl(init);

#ifndef IO_WO_XEXEC
	performXExec(postExec);
#endif

	return true;
}

//-------------------------------------------------------------------

#endif

