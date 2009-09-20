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

#include <libdodo/directives.h>

#ifdef OPENSSL_EXT
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "ioSsl.inline"

#include <libdodo/ioNetworkSslClient.h>
#include <libdodo/toolsFilesystem.h>
#include <libdodo/ioNetworkSslClientEx.h>
#include <libdodo/ioSsl.h>
#include <libdodo/types.h>
#include <libdodo/ioNetworkSslExchange.h>
#include <libdodo/xexec.h>

using namespace dodo::io::network::ssl;

client::client(client &fs) : network::client(fs)
{
}

//-------------------------------------------------------------------

client::client(short a_family,
			   short a_type) : network::client(a_family,
											   a_type),
							   handle(new io::ssl::__connection__),
							   ctx(new io::ssl::__context__),
							   sslConnected(false)
{
#ifndef IO_WO_XEXEC
	collectedData.setExecObject(xexec::OBJECT_IONETWORKSSLCLIENT);
#endif

	ctx->ctx = NULL;
	handle->handle = NULL;
}

//-------------------------------------------------------------------

client::~client()
{
	if (handle->handle != NULL) {
		if (sslConnected && SSL_shutdown(handle->handle) == 0)
			SSL_shutdown(handle->handle);

		SSL_free(handle->handle);
	}

	if (ctx->ctx != NULL)
		SSL_CTX_free(ctx->ctx);

	delete handle;
	delete ctx;
}

//-------------------------------------------------------------------

void
client::removeSertificates()
{
	if (handle->handle != NULL) {
		if (sslConnected) {
			int err = SSL_shutdown(handle->handle);
			if (err < 0) {
				unsigned long nerr = ERR_get_error();
				throw exception::basic(exception::MODULE_IONETWORKSSLCLIENT, CLIENTEX_REMOVESERTIFICATES, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
			}
			if (err == 0) {
				err = SSL_shutdown(handle->handle);
				if (err < 0) {
					unsigned long nerr = ERR_get_error();
					throw exception::basic(exception::MODULE_IONETWORKSSLCLIENT, CLIENTEX_REMOVESERTIFICATES, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
				}
			}

			sslConnected = false;
		}

		SSL_free(handle->handle);

		handle->handle = NULL;
	}

	if (ctx->ctx != NULL) {
		SSL_CTX_free(ctx->ctx);

		ctx->ctx = NULL;
	}

	ctx->ctx = SSL_CTX_new(SSLv23_client_method());
	if (ctx->ctx == NULL)
		throw exception::basic(exception::MODULE_IONETWORKSSLCLIENT, CLIENTEX_REMOVESERTIFICATES, exception::ERRNO_LIBDODO, CLIENTEX_UNABLETOINITCONTEXT, IONETWORKSSLCLIENTEX_UNABLETOINITCONTEXT_STR, __LINE__, __FILE__);

	handle->handle = SSL_new(ctx->ctx);
	if (handle->handle == NULL)
		throw exception::basic(exception::MODULE_IONETWORKSSLCLIENT, CLIENTEX_REMOVESERTIFICATES, exception::ERRNO_LIBDODO, CLIENTEX_UNABLETOINITSSL, IONETWORKSSLCLIENTEX_UNABLETOINITSSL_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
client::setSertificates(const io::ssl::__certificates__ &certs)
{
	if (handle->handle != NULL) {
		if (sslConnected) {
			int err = SSL_shutdown(handle->handle);
			if (err < 0) {
				unsigned long nerr = ERR_get_error();
				throw exception::basic(exception::MODULE_IONETWORKSSLCLIENT, CLIENTEX_SETSERTIFICATES, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
			}
			if (err == 0) {
				err = SSL_shutdown(handle->handle);
				if (err < 0) {
					unsigned long nerr = ERR_get_error();
					throw exception::basic(exception::MODULE_IONETWORKSSLCLIENT, CLIENTEX_SETSERTIFICATES, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
				}
			}

			sslConnected = false;
		}

		SSL_free(handle->handle);

		handle->handle = NULL;
	}

	if (ctx->ctx != NULL) {
		SSL_CTX_free(ctx->ctx);

		ctx->ctx = NULL;
	}

	ctx->ctx = SSL_CTX_new(SSLv23_client_method());
	if (ctx->ctx == NULL)
		throw exception::basic(exception::MODULE_IONETWORKSSLCLIENT, CLIENTEX_SETSERTIFICATES, exception::ERRNO_LIBDODO, CLIENTEX_UNABLETOINITCONTEXT, IONETWORKSSLCLIENTEX_UNABLETOINITCONTEXT_STR, __LINE__, __FILE__);

	if (certs.cipher.size() > 0 && SSL_CTX_set_cipher_list(ctx->ctx, certs.cipher.data()) != 1) {
		unsigned long nerr = ERR_get_error();
		throw exception::basic(exception::MODULE_IONETWORKSSLCLIENT, CLIENTEX_SETSERTIFICATES, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
	}

	if (certs.ca.size() > 0 && SSL_CTX_use_certificate_chain_file(ctx->ctx, certs.ca.data()) != 1) {
		unsigned long nerr = ERR_get_error();
		throw exception::basic(exception::MODULE_IONETWORKSSLCLIENT, CLIENTEX_SETSERTIFICATES, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
	}

	if (certs.cert.size() > 0 && SSL_CTX_use_certificate_file(ctx->ctx, certs.cert.data(), SSL_FILETYPE_PEM) != 1) {
		unsigned long nerr = ERR_get_error();
		throw exception::basic(exception::MODULE_IONETWORKSSLCLIENT, CLIENTEX_SETSERTIFICATES, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
	}

	if (certs.keyPassword.size() > 0)
		SSL_CTX_set_default_passwd_cb_userdata(ctx->ctx, (void *)certs.keyPassword.data());

	bool keySet = false;

	if (certs.key.size() > 0) {
		switch (certs.keyType) {
			case io::ssl::KEY_PKEY:

				if (SSL_CTX_use_PrivateKey_file(ctx->ctx, certs.key.data(), SSL_FILETYPE_PEM) != 1) {
					unsigned long nerr = ERR_get_error();
					throw exception::basic(exception::MODULE_IONETWORKSSLCLIENT, CLIENTEX_SETSERTIFICATES, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
				}

				keySet = true;

				break;

			case io::ssl::KEY_RSA:

				if (SSL_CTX_use_RSAPrivateKey_file(ctx->ctx, certs.key.data(), SSL_FILETYPE_PEM) != 1) {
					unsigned long nerr = ERR_get_error();
					throw exception::basic(exception::MODULE_IONETWORKSSLCLIENT, CLIENTEX_SETSERTIFICATES, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
				}

				keySet = true;

				break;

			default:

				throw exception::basic(exception::MODULE_IONETWORKSSLCLIENT, CLIENTEX_SETSERTIFICATES, exception::ERRNO_LIBDODO, CLIENTEX_UNKNOWNKEYTYPE, IONETWORKSSLCLIENTEX_UNKNOWNKEY_STR, __LINE__, __FILE__);
		}
	} else {
		if (certs.ca.size() > 0) {
			if (SSL_CTX_use_PrivateKey_file(ctx->ctx, certs.ca.data(), SSL_FILETYPE_PEM) != 1) {
				unsigned long nerr = ERR_get_error();
				throw exception::basic(exception::MODULE_IONETWORKSSLCLIENT, CLIENTEX_SETSERTIFICATES, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
			}

			keySet = true;
		}
	}

	if (certs.caPath.size() > 0) {
		if (tools::filesystem::file(certs.caPath).type == tools::filesystem::FILE_DIRECTORY) {
			if (SSL_CTX_load_verify_locations(ctx->ctx, NULL, certs.caPath.data()) != 1) {
				unsigned long nerr = ERR_get_error();
				throw exception::basic(exception::MODULE_IONETWORKSSLCLIENT, CLIENTEX_SETSERTIFICATES, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
			}
		} else {
			if (SSL_CTX_load_verify_locations(ctx->ctx, certs.caPath.data(), NULL) != 1) {
				unsigned long nerr = ERR_get_error();
				throw exception::basic(exception::MODULE_IONETWORKSSLCLIENT, CLIENTEX_SETSERTIFICATES, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
			}
		}
	}

	if (keySet && SSL_CTX_check_private_key(ctx->ctx) != 1) {
		unsigned long nerr = ERR_get_error();
		throw exception::basic(exception::MODULE_IONETWORKSSLCLIENT, CLIENTEX_SETSERTIFICATES, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
	}

	handle->handle = SSL_new(ctx->ctx);
	if (handle->handle == NULL)
		throw exception::basic(exception::MODULE_IONETWORKSSLCLIENT, CLIENTEX_SETSERTIFICATES, exception::ERRNO_LIBDODO, CLIENTEX_UNABLETOINITSSL, IONETWORKSSLCLIENTEX_UNABLETOINITSSL_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
client::initSsl()
{
	if (ctx->ctx == NULL) {
		ctx->ctx = SSL_CTX_new(SSLv23_client_method());
		if (ctx->ctx == NULL)
			throw exception::basic(exception::MODULE_IONETWORKSSLCLIENT, CLIENTEX_INITSSL, exception::ERRNO_LIBDODO, CLIENTEX_UNABLETOINITCONTEXT, IONETWORKSSLCLIENTEX_UNABLETOINITCONTEXT_STR, __LINE__, __FILE__);
	}

	if (handle->handle == NULL) {
		handle->handle = SSL_new(ctx->ctx);
		if (handle->handle == NULL)
			throw exception::basic(exception::MODULE_IONETWORKSSLCLIENT, CLIENTEX_INITSSL, exception::ERRNO_LIBDODO, CLIENTEX_UNABLETOINITSSL, IONETWORKSSLCLIENTEX_UNABLETOINITSSL_STR, __LINE__, __FILE__);
	}
}

//-------------------------------------------------------------------

void
client::connectSsl()
{
	io::ssl::__openssl___init_object__.addEntropy();

	if (sslConnected) {
		int err = SSL_shutdown(handle->handle);
		if (err < 0) {
			unsigned long nerr = ERR_get_error();
			throw exception::basic(exception::MODULE_IONETWORKSSLCLIENT, CLIENTEX_CONNECTSSL, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
		}
		if (err == 0) {
			err = SSL_shutdown(handle->handle);
			if (err < 0) {
				unsigned long nerr = ERR_get_error();
				throw exception::basic(exception::MODULE_IONETWORKSSLCLIENT, CLIENTEX_CONNECTSSL, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
			}
		}

		sslConnected = false;
	}

	if (SSL_clear(handle->handle) == 0) {
		unsigned long nerr = ERR_get_error();
		throw exception::basic(exception::MODULE_IONETWORKSSLCLIENT, CLIENTEX_CONNECTSSL, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
	}

	if (SSL_set_fd(handle->handle, socket) == 0) {
		unsigned long nerr = ERR_get_error();
		throw exception::basic(exception::MODULE_IONETWORKSSLCLIENT, CLIENTEX_CONNECTSSL, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
	}

	int res = SSL_connect(handle->handle);
	switch (res) {
		case 1:
			break;

		case 0:
		{
			unsigned long nerr = ERR_get_error();
			throw exception::basic(exception::MODULE_IONETWORKSSLCLIENT, CLIENTEX_CONNECTSSL, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
		}

		case - 1:
		{
			int nerr = SSL_get_error(handle->handle, res);
			if (nerr == SSL_ERROR_WANT_READ || nerr == SSL_ERROR_WANT_WRITE || nerr == SSL_ERROR_WANT_X509_LOOKUP)
				break;
		}

		default:
		{
			unsigned long nerr = ERR_get_error();

			int err = SSL_shutdown(handle->handle);
			if (err < 0) {
				nerr = ERR_get_error();
				throw exception::basic(exception::MODULE_IONETWORKSSLCLIENT, CLIENTEX_CONNECTSSL, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
			}
			if (err == 0) {
				err = SSL_shutdown(handle->handle);
				if (err < 0) {
					nerr = ERR_get_error();
					throw exception::basic(exception::MODULE_IONETWORKSSLCLIENT, CLIENTEX_CONNECTSSL, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
				}
			}

			throw exception::basic(exception::MODULE_IONETWORKSSLCLIENT, CLIENTEX_CONNECTSSL, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
		}
	}

	sslConnected = true;
}

//-------------------------------------------------------------------

void
client::connect(const dodoString &host,
				int              port,
				network::exchange         &ex)
{
#ifndef IO_WO_XEXEC
	performPreExec(OPERATION_CONNECT);
#endif

	initSsl();
	makeSocket();

	if (family == PROTOCOL_FAMILY_IPV4) {
		struct sockaddr_in sa;
		sa.sin_family = AF_INET;
		sa.sin_port = htons(port);
		inet_aton(host.data(), &sa.sin_addr);

		if (::connect(socket, (struct sockaddr *)&sa, sizeof(sa)) == -1) {
			if (::close(socket) == -1)
				throw exception::basic(exception::MODULE_IONETWORKSSLCLIENT, CLIENTEX_CONNECT, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

			socket = -1;

			throw exception::basic(exception::MODULE_IONETWORKSSLCLIENT, CLIENTEX_CONNECT, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}
	} else {
		struct sockaddr_in6 sa;
		sa.sin6_family = AF_INET6;
		sa.sin6_port = htons(port);
		sa.sin6_flowinfo = 0;
		sa.sin6_scope_id = 0;
		inet_pton(AF_INET6, host.data(), &sa.sin6_addr);

		if (::connect(socket, (struct sockaddr *)&sa, sizeof(sa)) == -1) {
			if (::close(socket) == -1)
				throw exception::basic(exception::MODULE_IONETWORKSSLCLIENT, CLIENTEX_CONNECT, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

			socket = -1;

			throw exception::basic(exception::MODULE_IONETWORKSSLCLIENT, CLIENTEX_CONNECT, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}
	}

	connectSsl();

	exchange &sslEx = dynamic_cast<exchange &>(ex);
	sslEx.init(socket, handle, blocked, blockInherited);

	socket = -1;
	handle->handle = NULL;

#ifndef IO_WO_XEXEC
	performPostExec(OPERATION_CONNECT);
#endif
}

//-------------------------------------------------------------------

void
client::connectFrom(const dodoString &local,
					const dodoString &host,
					int              port,
					network::exchange         &ex)
{
#ifndef IO_WO_XEXEC
	performPreExec(OPERATION_CONNECTFROM);
#endif

	initSsl();
	makeSocket();

	int sockFlag(1);
	if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &sockFlag, sizeof(int)) == -1)
		throw exception::basic(exception::MODULE_IONETWORKSSLCLIENT, CLIENTEX_CONNECTFROM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	addFlag(socketOpts, 1 << OPTION_REUSE_ADDRESS);

	if (family == PROTOCOL_FAMILY_IPV4) {
		struct sockaddr_in sa;
		sa.sin_family = AF_INET;
		sa.sin_port = htons(0);
		inet_aton(local.data(), &sa.sin_addr);

		if (::bind(socket, (struct sockaddr *)&sa, sizeof(sa)) == -1)
			throw exception::basic(exception::MODULE_IONETWORKSSLCLIENT, CLIENTEX_CONNECTFROM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		sa.sin_port = htons(port);
		inet_aton(host.data(), &sa.sin_addr);

		if (::connect(socket, (struct sockaddr *)&sa, sizeof(sa)) == -1) {
			if (::close(socket) == -1)
				throw exception::basic(exception::MODULE_IONETWORKSSLCLIENT, CLIENTEX_CONNECTFROM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

			socket = -1;

			throw exception::basic(exception::MODULE_IONETWORKSSLCLIENT, CLIENTEX_CONNECTFROM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}
	} else {
		struct sockaddr_in6 sa;
		sa.sin6_family = AF_INET6;
		sa.sin6_flowinfo = 0;
		sa.sin6_scope_id = 0;
		sa.sin6_port = htons(0);
		inet_pton(AF_INET6, local.data(), &sa.sin6_addr);

		if (::bind(socket, (struct sockaddr *)&sa, sizeof(sa)) == -1)
			throw exception::basic(exception::MODULE_IONETWORKSSLCLIENT, CLIENTEX_CONNECTFROM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		sa.sin6_port = htons(port);
		inet_pton(AF_INET6, host.data(), &sa.sin6_addr);

		if (::connect(socket, (struct sockaddr *)&sa, sizeof(sa)) == -1) {
			if (::close(socket) == -1)
				throw exception::basic(exception::MODULE_IONETWORKSSLCLIENT, CLIENTEX_CONNECTFROM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

			socket = -1;

			throw exception::basic(exception::MODULE_IONETWORKSSLCLIENT, CLIENTEX_CONNECTFROM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}
	}

	connectSsl();

	exchange &sslEx = dynamic_cast<exchange &>(ex);
	sslEx.init(socket, handle, blocked, blockInherited);

	socket = -1;
	handle->handle = NULL;

#ifndef IO_WO_XEXEC
	performPostExec(OPERATION_CONNECTFROM);
#endif
}

//-------------------------------------------------------------------

void
client::connect(const dodoString &path,
				network::exchange         &ex)
{
#ifndef IO_WO_XEXEC
	performPreExec(OPERATION_CONNECT);
#endif

	initSsl();
	makeSocket();

	struct sockaddr_un sa;

	unsigned long size = path.size();

	if (size >= 108)
		throw exception::basic(exception::MODULE_IONETWORKSSLCLIENT, CLIENTEX_CONNECT, exception::ERRNO_LIBDODO, CLIENTEX_LONGPATH, IONETWORKSSLCLIENTEX_LONGPATH_STR, __LINE__, __FILE__);

	strncpy(sa.sun_path, path.data(), size);
	sa.sun_family = AF_UNIX;

	if (::connect(socket, (struct sockaddr *)&sa, path.size() + sizeof(sa.sun_family)) == -1) {
		if (::close(socket) == -1)
			throw exception::basic(exception::MODULE_IONETWORKSSLCLIENT, CLIENTEX_CONNECT, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		socket = -1;

		throw exception::basic(exception::MODULE_IONETWORKSSLCLIENT, CLIENTEX_CONNECT, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	connectSsl();

	exchange &sslEx = dynamic_cast<exchange &>(ex);
	sslEx.init(socket, handle, blocked, blockInherited);

	socket = -1;
	handle->handle = NULL;

#ifndef IO_WO_XEXEC
	performPostExec(OPERATION_CONNECT);
#endif
}

//-------------------------------------------------------------------
#endif

