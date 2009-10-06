/***************************************************************************
 *            ioNetworkSslExchange.cc
 *
 *  Tue Jun 10 2008
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
#include <poll.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "ioSsl.inline"

#include <libdodo/ioNetworkSslExchange.h>
#include <libdodo/ioChannel.h>
#include <libdodo/ioStreamChannel.h>
#include <libdodo/ioNetworkSslExchangeEx.h>
#include <libdodo/ioNetworkExchange.h>
#include <libdodo/ioSsl.h>
#include <libdodo/types.h>
#include <libdodo/xexec.h>
#include <libdodo/pcSyncProtector.h>

using namespace dodo::io::network::ssl;

exchange::__init__::__init__() : network::exchange::__init__(),
					   handle(new io::ssl::__connection__)
{
}

//-------------------------------------------------------------------

exchange::__init__::__init__(__init__ &init) : network::exchange::__init__(init),
									 handle(new io::ssl::__connection__)
{
	handle->handle = init.handle->handle;

	init.handle = NULL;
}

//-------------------------------------------------------------------

exchange::__init__::~__init__()
{
	delete handle;
}

//-------------------------------------------------------------------

exchange::exchange(exchange &fse) : stream::channel(fse.protection)
{
}

//-------------------------------------------------------------------

exchange::exchange(short protection) : stream::channel(protection),
									   handle(new io::ssl::__connection__)
{
#ifndef IO_WO_XEXEC
	collectedData.setExecObject(xexec::OBJECT_IONETWORKSSLEXCHANGE);
#endif
}

//-------------------------------------------------------------------

exchange::exchange(__init__ &a_init,
				   short             protection) : stream::channel(protection),
												   handle(new io::ssl::__connection__)
{
#ifndef IO_WO_XEXEC
	collectedData.setExecObject(xexec::OBJECT_IONETWORKSSLEXCHANGE);
#endif

	init(a_init.socket, a_init.handle, a_init.blocked, a_init.blockInherited);

	a_init.socket = -1;
	a_init.handle->handle = NULL;
}

//-------------------------------------------------------------------

exchange::~exchange()
{
	if (handle->handle != NULL) {
		if (SSL_shutdown(handle->handle) == 0)
			SSL_shutdown(handle->handle);

		SSL_free(handle->handle);
	}

	delete handle;
}

//-------------------------------------------------------------------

void
exchange::_close(int                        socket,
				 io::ssl::__connection__ *handle)
{
	int err = SSL_shutdown(handle->handle);
	if (err < 0) {
		unsigned long nerr = ERR_get_error();
		throw exception::basic(exception::MODULE_IONETWORKSSLEXCHANGE, EXCHANGEEX__CLOSE, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
	}
	if (err == 0) {
		err = SSL_shutdown(handle->handle);
		if (err < 0) {
			unsigned long nerr = ERR_get_error();
			throw exception::basic(exception::MODULE_IONETWORKSSLEXCHANGE, EXCHANGEEX__CLOSE, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
		}
	}

	connection::_close(socket);
}

//-------------------------------------------------------------------

void
exchange::close()
{
	pc::sync::protector pg(keeper);

#ifndef IO_WO_XEXEC
	performPreExec(OPERATION_CLOSE);
#endif

	if (socket != -1) {
		_close(socket, handle);

		socket = -1;
		handle->handle = NULL;
	}

#ifndef IO_WO_XEXEC
	performPostExec(OPERATION_CLOSE);
#endif
}

//-------------------------------------------------------------------

void
exchange::init(int                        a_socket,
			   io::ssl::__connection__ *a_handle,
			   bool                       a_blocked,
			   bool                       blockInherited)
{
	pc::sync::protector pg(keeper);

	if (socket != -1) {
		_close(socket, handle);

		socket = -1;
		handle->handle = NULL;
	}

	blocked = a_blocked;
	socket = a_socket;
	handle->handle = a_handle->handle;

	setInBufferSize(inSocketBufferSize);
	setOutBufferSize(outSocketBufferSize);

	setInTimeout(inSocketTimeout);
	setOutTimeout(outSocketTimeout);

	setLingerOption(lingerOpts, lingerSeconds);

	if (!blocked) {
		if (blockInherited)
			block(false);
		else
			block(true);
	} else {
		block(true);
	}
}

//-------------------------------------------------------------------

bool
exchange::isAlive()
{
	pc::sync::protector pg(keeper);

	if (socket == -1)
		return false;

	pollfd fd;
	fd.fd = socket;
	fd.events = POLLOUT;

	if (poll(&fd, 1, -1) > 0)
		if (isSetFlag(fd.revents, POLLOUT))
			return true;

	_close(socket, handle);

	socket = -1;
	handle->handle = NULL;

	return false;
}

//-------------------------------------------------------------------

void
exchange::_write(const char * const a_data) const
{
	if (socket == -1)
		throw exception::basic(exception::MODULE_IONETWORKSSLEXCHANGE, EXCHANGEEX__WRITE, exception::ERRNO_LIBDODO, EXCHANGEEX_NOCONNECTION, IONETWORKSSLEXCHANGEEX_NOCONNECTION_STR, __LINE__, __FILE__);

	unsigned long iter = blockSize / outSocketBufferSize;
	unsigned long rest = blockSize % outSocketBufferSize;

	const char *data = a_data;

	long n;

	for (unsigned long i = 0; i < iter; ++i) {
		while (true) {
			if ((n = SSL_write(handle->handle, data, outSocketBufferSize)) <= 0) {
				switch (SSL_get_error(handle->handle, n)) {
					case SSL_ERROR_WANT_READ:
					case SSL_ERROR_WANT_WRITE:
					case SSL_ERROR_WANT_X509_LOOKUP:

						continue;

					case SSL_ERROR_SYSCALL:

						if (errno == 0)
							continue;


					default:
					{
						unsigned long nerr = ERR_get_error();
						throw exception::basic(exception::MODULE_IONETWORKSSLEXCHANGE, EXCHANGEEX__WRITE, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
					}
				}
			}

			break;
		}

		data += outSocketBufferSize;
	}

	if (rest > 0) {
		while (true) {
			if ((n = SSL_write(handle->handle, data, rest)) <= 0) {
				switch (SSL_get_error(handle->handle, n)) {
					case SSL_ERROR_WANT_READ:
					case SSL_ERROR_WANT_WRITE:
					case SSL_ERROR_WANT_X509_LOOKUP:

						continue;

					case SSL_ERROR_SYSCALL:

						if (errno == 0)
							continue;


					default:
					{
						unsigned long nerr = ERR_get_error();
						throw exception::basic(exception::MODULE_IONETWORKSSLEXCHANGE, EXCHANGEEX__WRITE, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
					}
				}
			}

			break;
		}
	}
}

//-------------------------------------------------------------------

void
exchange::_read(char * const a_data) const
{
	if (socket == -1)
		throw exception::basic(exception::MODULE_IONETWORKSSLEXCHANGE, EXCHANGEEX__READ, exception::ERRNO_LIBDODO, EXCHANGEEX_NOCONNECTION, IONETWORKSSLEXCHANGEEX_NOCONNECTION_STR, __LINE__, __FILE__);

	long iter = blockSize / inSocketBufferSize;
	long rest = blockSize % inSocketBufferSize;

	char *data = a_data;

	long n;

	for (long i = 0; i < iter; ++i) {
		while (true) {
			if ((n = SSL_read(handle->handle, data, inSocketBufferSize)) <= 0) {
				switch (SSL_get_error(handle->handle, n)) {
					case SSL_ERROR_WANT_READ:
					case SSL_ERROR_WANT_WRITE:
					case SSL_ERROR_WANT_X509_LOOKUP:

						continue;

					case SSL_ERROR_SYSCALL:

						if (errno == 0)
							continue;


					default:
					{
						unsigned long nerr = ERR_get_error();
						throw exception::basic(exception::MODULE_IONETWORKSSLEXCHANGE, EXCHANGEEX__READ, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
					}
				}
			}

			break;
		}

		data += inSocketBufferSize;
	}

	if (rest > 0) {
		while (true) {
			if ((n = SSL_read(handle->handle, data, rest)) <= 0) {
				switch (SSL_get_error(handle->handle, n)) {
					case SSL_ERROR_WANT_READ:
					case SSL_ERROR_WANT_WRITE:
					case SSL_ERROR_WANT_X509_LOOKUP:

						continue;

					case SSL_ERROR_SYSCALL:

						if (errno == 0)
							continue;


					default:
					{
						unsigned long nerr = ERR_get_error();
						throw exception::basic(exception::MODULE_IONETWORKSSLEXCHANGE, EXCHANGEEX__READ, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
					}
				}
			}

			if (n < rest)
				continue;

			break;
		}
	}
}

//-------------------------------------------------------------------

unsigned long
exchange::_readString(char * const data) const
{
	if (socket == -1)
		throw exception::basic(exception::MODULE_IONETWORKSSLEXCHANGE, EXCHANGEEX__READSTREAM, exception::ERRNO_LIBDODO, EXCHANGEEX_NOCONNECTION, IONETWORKSSLEXCHANGEEX_NOCONNECTION_STR, __LINE__, __FILE__);

	long n;

	while (true) {
		if ((n = SSL_read(handle->handle, data, blockSize)) <= 0) {
			switch (SSL_get_error(handle->handle, n)) {
				case SSL_ERROR_WANT_READ:
				case SSL_ERROR_WANT_WRITE:
				case SSL_ERROR_WANT_X509_LOOKUP:
				case SSL_ERROR_ZERO_RETURN:

					continue;

				case SSL_ERROR_SYSCALL:

					if (errno == 0)
						continue;


				default:
				{
					unsigned long nerr = ERR_get_error();
					throw exception::basic(exception::MODULE_IONETWORKSSLEXCHANGE, EXCHANGEEX__READSTREAM, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
				}
			}
		}

		break;
	}

	return (unsigned long)n;
}

//-------------------------------------------------------------------
#endif

