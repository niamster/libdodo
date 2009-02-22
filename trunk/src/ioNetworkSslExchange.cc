/***************************************************************************
 *            ioNetworkSslExchange.cc
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

#include <libdodo/ioNetworkSslExchange.h>

#ifdef OPENSSL_EXT

using namespace dodo::io::network::ssl;

__initialAccept::__initialAccept() : socket(-1),
									 sslHandle(NULL)
{
}

//-------------------------------------------------------------------

__initialAccept::__initialAccept(__initialAccept &init) : socket(init.socket),
														  sslHandle(init.sslHandle)
{
	init.socket = -1;
	init.sslHandle = NULL;
}

//-------------------------------------------------------------------

exchange::exchange(exchange &fse) : network::exchange(fse)
{
#ifndef IO_WO_XEXEC

	collectedData.setExecObject(XEXEC_OBJECT_IONETWORKSSLEXCHANGE);

#endif

	sslHandle = fse.sslHandle;

	fse.sslHandle = NULL;
}

//-------------------------------------------------------------------

exchange::exchange() : sslHandle(NULL)
{
#ifndef IO_WO_XEXEC

	collectedData.setExecObject(XEXEC_OBJECT_IONETWORKSSLEXCHANGE);

#endif
}

//-------------------------------------------------------------------

exchange::exchange(__initialAccept &a_init)
{
#ifndef IO_WO_XEXEC

	collectedData.setExecObject(XEXEC_OBJECT_IONETWORKSSLEXCHANGE);

#endif

	init(a_init);
}

//-------------------------------------------------------------------

exchange::~exchange()
{
	if (sslHandle != NULL)
	{
		if (SSL_shutdown(sslHandle) == 0)
		{
			SSL_shutdown(sslHandle);
		}

		SSL_free(sslHandle);
	}
}

//-------------------------------------------------------------------

void exchange::init(__initialAccept &a_init)
{
	init(a_init.socket, a_init.sslHandle, a_init.blocked, a_init.blockInherited);

	a_init.socket = -1;
	a_init.sslHandle = NULL;
}

//-------------------------------------------------------------------

void exchange::_close(int socket,
					  SSL *sslHandle)
{
	int err = SSL_shutdown(sslHandle);
	if (err < 0)
	{
		unsigned long nerr = ERR_get_error();
		throw exception::basic(exception::ERRMODULE_IONETWORKSSLEXCHANGE, EXCHANGEEX__CLOSE, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
	}
	if (err == 0)
	{
		err = SSL_shutdown(sslHandle);
		if (err < 0)
		{
			unsigned long nerr = ERR_get_error();
			throw exception::basic(exception::ERRMODULE_IONETWORKSSLEXCHANGE, EXCHANGEEX__CLOSE, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
		}
	}

	connection::_close(socket);
}

//-------------------------------------------------------------------

void exchange::close()
{
	protector pg(this);

#ifndef IO_WO_XEXEC
	operType = EXCHANGE_OPERATION_CLOSE;
	performXExec(preExec);
#endif

	if (socket != -1)
	{
		_close(socket, sslHandle);

		socket = -1;
		sslHandle = NULL;
	}

#ifndef IO_WO_XEXEC
	performXExec(postExec);
#endif
}

//-------------------------------------------------------------------

void exchange::init(int  a_socket,
					SSL  *a_sslHandle,
					bool a_blocked,
					bool blockInherited)
{
	protector pg(this);

	if (socket != -1)
	{
		_close(socket, sslHandle);

		socket = -1;
		sslHandle = NULL;
	}

	blocked = a_blocked;
	socket = a_socket;
	sslHandle = a_sslHandle;

	setInBufferSize(inSocketBuffer);
	setOutBufferSize(outSocketBuffer);

	setInTimeout(inTimeout);
	setOutTimeout(outTimeout);

	setLingerOption(lingerOpts, lingerSeconds);

	if (!blocked)
	{
		if (blockInherited)
		{
			block(false);
		}
		else
		{
			block(true);
		}
	}
	else
	{
		block(true);
	}
}

//-------------------------------------------------------------------

bool exchange::isAlive()
{
	protector pg(this);

	if (socket == -1)
	{
		return false;
	}

	pollfd fd;
	fd.fd = socket;
	fd.events = POLLOUT;

	if (poll(&fd, 1, -1) > 0)
	{
		if (isSetFlag(fd.revents, POLLOUT))
		{
			return true;
		}
	}

	_close(socket, sslHandle);

	socket = -1;
	sslHandle = NULL;

	return false;
}

//-------------------------------------------------------------------

void exchange::_write(const char * const a_data)
{
	if (socket == -1)
	{
		throw exception::basic(exception::ERRMODULE_IONETWORKSSLEXCHANGE, EXCHANGEEX__WRITE, exception::ERRNO_LIBDODO, EXCHANGEEX_NOCONNECTION, IONETWORKSSLEXCHANGEEX_NOCONNECTION_STR, __LINE__, __FILE__);
	}

	unsigned long iter = outSize / outSocketBuffer;
	unsigned long rest = outSize % outSocketBuffer;

	const char *data = a_data;

	long n;

	for (unsigned long i = 0; i < iter; ++i)
	{
		while (true)
		{
			if ((n = SSL_write(sslHandle, data, outSocketBuffer)) <= 0)
			{
				switch (SSL_get_error(sslHandle, n))
				{
					case SSL_ERROR_WANT_READ:
					case SSL_ERROR_WANT_WRITE:
					case SSL_ERROR_WANT_X509_LOOKUP:

						continue;

					case SSL_ERROR_SYSCALL:

						if (errno == 0)
						{
							continue;
						}

					default:
					{
						unsigned long nerr = ERR_get_error();
						throw exception::basic(exception::ERRMODULE_IONETWORKSSLEXCHANGE, EXCHANGEEX__WRITE, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
					}
				}
			}

			break;
		}

		data += outSocketBuffer;
	}

	if (rest > 0)
	{
		while (true)
		{
			if ((n = SSL_write(sslHandle, data, rest)) <= 0)
			{
				switch (SSL_get_error(sslHandle, n))
				{
					case SSL_ERROR_WANT_READ:
					case SSL_ERROR_WANT_WRITE:
					case SSL_ERROR_WANT_X509_LOOKUP:

						continue;

					case SSL_ERROR_SYSCALL:

						if (errno == 0)
						{
							continue;
						}

					default:
					{
						unsigned long nerr = ERR_get_error();
						throw exception::basic(exception::ERRMODULE_IONETWORKSSLEXCHANGE, EXCHANGEEX__WRITE, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
					}
				}
			}

			break;
		}
	}
}

//-------------------------------------------------------------------

void exchange::_read(char * const a_data)
{
	if (socket == -1)
	{
		throw exception::basic(exception::ERRMODULE_IONETWORKSSLEXCHANGE, EXCHANGEEX__READ, exception::ERRNO_LIBDODO, EXCHANGEEX_NOCONNECTION, IONETWORKSSLEXCHANGEEX_NOCONNECTION_STR, __LINE__, __FILE__);
	}

	memset(a_data, '\0', inSize);

	unsigned long iter = inSize / inSocketBuffer;
	unsigned long rest = inSize % inSocketBuffer;

	char *data = a_data;

	long n;

	for (unsigned long i = 0; i < iter; ++i)
	{
		while (true)
		{
			if ((n = SSL_read(sslHandle, data, inSocketBuffer)) <= 0)
			{
				switch (SSL_get_error(sslHandle, n))
				{
					case SSL_ERROR_WANT_READ:
					case SSL_ERROR_WANT_WRITE:
					case SSL_ERROR_WANT_X509_LOOKUP:

						continue;

					case SSL_ERROR_SYSCALL:

						if (errno == 0)
						{
							continue;
						}

					default:
					{
						unsigned long nerr = ERR_get_error();
						throw exception::basic(exception::ERRMODULE_IONETWORKSSLEXCHANGE, EXCHANGEEX__READ, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
					}
				}
			}

			break;
		}

		data += inSocketBuffer;
	}

	if (rest > 0)
	{
		while (true)
		{
			if ((n = SSL_read(sslHandle, data, rest)) <= 0)
			{
				switch (SSL_get_error(sslHandle, n))
				{
					case SSL_ERROR_WANT_READ:
					case SSL_ERROR_WANT_WRITE:
					case SSL_ERROR_WANT_X509_LOOKUP:

						continue;

					case SSL_ERROR_SYSCALL:

						if (errno == 0)
						{
							continue;
						}

					default:
					{
						unsigned long nerr = ERR_get_error();
						throw exception::basic(exception::ERRMODULE_IONETWORKSSLEXCHANGE, EXCHANGEEX__READ, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
					}
				}
			}

			break;
		}
	}
}

//-------------------------------------------------------------------

unsigned long exchange::_readStream(char * const data)
{
	if (socket == -1)
	{
		throw exception::basic(exception::ERRMODULE_IONETWORKSSLEXCHANGE, EXCHANGEEX__READSTREAM, exception::ERRNO_LIBDODO, EXCHANGEEX_NOCONNECTION, IONETWORKSSLEXCHANGEEX_NOCONNECTION_STR, __LINE__, __FILE__);
	}

	memset(data, '\0', inSize);

	long n;

	while (true)
	{
		if ((n = SSL_read(sslHandle, data, inSize)) <= 0)
		{
			switch (SSL_get_error(sslHandle, n))
			{
				case SSL_ERROR_WANT_READ:
				case SSL_ERROR_WANT_WRITE:
				case SSL_ERROR_WANT_X509_LOOKUP:
				case SSL_ERROR_ZERO_RETURN:

					continue;

				case SSL_ERROR_SYSCALL:

					if (errno == 0)
					{
						continue;
					}

				default:
				{
					unsigned long nerr = ERR_get_error();
					throw exception::basic(exception::ERRMODULE_IONETWORKSSLEXCHANGE, EXCHANGEEX__READSTREAM, exception::ERRNO_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
				}
			}
		}

		break;
	}

	return (unsigned long)n;
}

//-------------------------------------------------------------------

#endif

