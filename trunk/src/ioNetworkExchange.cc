/***************************************************************************
 *            ioNetworkExchange.cc
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/directives.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <poll.h>
#include <errno.h>
#include <string.h>

#include <libdodo/ioNetworkExchange.h>
#include <libdodo/ioNetworkConnection.h>
#include <libdodo/ioChannel.h>
#include <libdodo/ioNetworkExchangeEx.h>
#include <libdodo/types.h>
#include <libdodo/pcSyncProtector.h>

using namespace dodo::io::network;

__initialAccept__::__initialAccept__() : socket(-1)
{
}

//-------------------------------------------------------------------

__initialAccept__::__initialAccept__(__initialAccept__ &init) : socket(init.socket)
{
	init.socket = -1;
}

//-------------------------------------------------------------------

exchange::exchange(exchange &fse) : channel(fse.protection)
{
#ifndef IO_WO_XEXEC
	collectedData.setExecObject(XEXEC_OBJECT_IONETWORKEXCHANGE);
#endif

	inSize = fse.inSize;
	outSize = fse.outSize;
	socketOpts = fse.socketOpts;
	inTimeout = fse.inTimeout;
	outTimeout = fse.outTimeout;
	inSocketBuffer = fse.inSocketBuffer;
	outSocketBuffer = fse.outSocketBuffer;
	lingerOpts = fse.lingerOpts;
	lingerSeconds = fse.lingerSeconds;
	blocked = fse.blocked;
	socket = fse.socket;

	fse.socket = -1;
}

//-------------------------------------------------------------------

exchange::exchange(short protection) : channel(protection)
{
#ifndef IO_WO_XEXEC
	collectedData.setExecObject(XEXEC_OBJECT_IONETWORKEXCHANGE);
#endif
}

//-------------------------------------------------------------------

exchange::exchange(__initialAccept__ &a_init,
				   short           protection) : channel(protection)
{
#ifndef IO_WO_XEXEC
	collectedData.setExecObject(XEXEC_OBJECT_IONETWORKEXCHANGE);
#endif

	init(a_init);
}

//-------------------------------------------------------------------

exchange::~exchange()
{
	if (socket != -1)
	{
		::shutdown(socket, SHUT_RDWR);

		::close(socket);
	}
}

//-------------------------------------------------------------------

void
exchange::init(__initialAccept__ &a_init)
{
	init(a_init.socket, a_init.blocked, a_init.blockInherited);

	a_init.socket = -1;
}

//-------------------------------------------------------------------

void
exchange::close()
{
	pc::sync::protector pg(keeper);

#ifndef IO_WO_XEXEC
	operType = EXCHANGE_OPERATION_CLOSE;
	performXExec(preExec);
#endif

	if (socket != -1)
	{
		_close(socket);

		socket = -1;
	}

#ifndef IO_WO_XEXEC
	performXExec(postExec);
#endif
}

//-------------------------------------------------------------------

void
exchange::init(int  a_socket,
			   bool a_blocked,
			   bool blockInherited)
{
	pc::sync::protector pg(keeper);

	if (socket != -1)
	{
		_close(socket);

		socket = -1;
	}

	blocked = a_blocked;
	socket = a_socket;

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

bool
exchange::isAlive()
{
	pc::sync::protector pg(keeper);

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

	_close(socket);

	socket = -1;

	return false;
}

//-------------------------------------------------------------------

void
exchange::_write(const char * const a_data)
{
	if (socket == -1)
	{
		throw exception::basic(exception::ERRMODULE_IONETWORKEXCHANGE, EXCHANGEEX__WRITE, exception::ERRNO_LIBDODO, EXCHANGEEX_NOCONNECTION, IONETWORKEXCHANGEEX_NOCONNECTION_STR, __LINE__, __FILE__);
	}

	unsigned long iter = outSize / outSocketBuffer;
	unsigned long rest = outSize % outSocketBuffer;

	const char *data = a_data;

	unsigned long batch, n;

	for (unsigned long i = 0; i < iter; ++i)
	{
		batch = outSocketBuffer;
		while (batch > 0)
		{
			while (true)
			{
				if ((n = ::send(socket, data, batch, 0)) == -1)
				{
					if (errno == EINTR)
					{
						continue;
					}

					if (errno == EAGAIN)
					{
						break;
					}

					throw exception::basic(exception::ERRMODULE_IONETWORKEXCHANGE, EXCHANGEEX__WRITE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
				}

				break;
			}

			batch -= n;
			data += n;
		}
	}

	if (rest > 0)
	{
		batch = rest;
		while (batch > 0)
		{
			while (true)
			{
				if ((n = ::send(socket, data, batch, 0)) == -1)
				{
					if (errno == EINTR)
					{
						continue;
					}

					if (errno == EAGAIN)
					{
						break;
					}

					throw exception::basic(exception::ERRMODULE_IONETWORKEXCHANGE, EXCHANGEEX__WRITE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
				}

				break;
			}

			batch -= n;
			data += n;
		}
	}
}

//-------------------------------------------------------------------

void
exchange::_read(char * const a_data)
{
	if (socket == -1)
	{
		throw exception::basic(exception::ERRMODULE_IONETWORKEXCHANGE, EXCHANGEEX__READ, exception::ERRNO_LIBDODO, EXCHANGEEX_NOCONNECTION, IONETWORKEXCHANGEEX_NOCONNECTION_STR, __LINE__, __FILE__);
	}

	memset(a_data, '\0', inSize);

	unsigned long iter = inSize / inSocketBuffer;
	unsigned long rest = inSize % inSocketBuffer;

	char *data = a_data;

	unsigned long batch, n;

	for (unsigned long i = 0; i < iter; ++i)
	{
		batch = inSocketBuffer;
		while (batch > 0)
		{
			while (true)
			{
				if ((n = ::recv(socket, data, batch, 0)) == -1)
				{
					if (errno == EINTR)
					{
						continue;
					}

					if (errno == EAGAIN)
					{
						break;
					}

					throw exception::basic(exception::ERRMODULE_IONETWORKEXCHANGE, EXCHANGEEX__READ, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
				}

				break;
			}

			if (n == 0)
			{
				break;
			}

			batch -= n;
			data += n;
		}
	}

	if (rest > 0)
	{
		batch = rest;
		while (batch > 0)
		{
			while (true)
			{
				if ((n = ::recv(socket, data, batch, 0)) == -1)
				{
					if (errno == EINTR)
					{
						continue;
					}

					if (errno == EAGAIN)
					{
						break;
					}

					throw exception::basic(exception::ERRMODULE_IONETWORKEXCHANGE, EXCHANGEEX__READ, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
				}

				break;
			}

			if (n == 0)
			{
				break;
			}

			batch -= n;
			data += n;
		}
	}
}

//-------------------------------------------------------------------

void
exchange::_writeStream(const char * const data)
{
	unsigned long _outSize = outSize;

	try
	{
		unsigned int bufSize = strlen(data) + 1;

		if (bufSize < outSize)
		{
			outSize = bufSize;
		}

		_write(data);

		outSize = _outSize;
	}
	catch (...)
	{
		outSize = _outSize;

		throw;
	}
}

//-------------------------------------------------------------------

unsigned long
exchange::_readStream(char * const data)
{
	if (socket == -1)
	{
		throw exception::basic(exception::ERRMODULE_IONETWORKEXCHANGE, EXCHANGEEX__READSTREAM, exception::ERRNO_LIBDODO, EXCHANGEEX_NOCONNECTION, IONETWORKEXCHANGEEX_NOCONNECTION_STR, __LINE__, __FILE__);
	}

	memset(data, '\0', inSize);

	unsigned long n = 0;

	while (true)
	{
		if ((n = ::recv(socket, data, inSize, 0)) == -1)
		{
			if (errno == EINTR)
			{
				continue;
			}

			if (errno == EAGAIN)
			{
				break;
			}

			throw exception::basic(exception::ERRMODULE_IONETWORKEXCHANGE, EXCHANGEEX__READSTREAM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}

		break;
	}

	return n;
}

//-------------------------------------------------------------------

void
exchange::flush()
{
}

//-------------------------------------------------------------------

int
exchange::getInDescriptor() const
{
	pc::sync::protector pg(keeper);

	return socket;
}

//-------------------------------------------------------------------

int
exchange::getOutDescriptor() const
{
	pc::sync::protector pg(keeper);

	return socket;
}

//-------------------------------------------------------------------

