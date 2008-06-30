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

#include <libdodo/ioNetworkExchange.h>

using namespace dodo::io::network;

__initialAccept::__initialAccept() : socket(-1)
{
}

//-------------------------------------------------------------------

__initialAccept::__initialAccept(__initialAccept &init) : socket(init.socket)
{
	init.socket = -1;
}

//-------------------------------------------------------------------

exchange::exchange(exchange &fse)
{
#ifndef IO_WO_XEXEC

	execObject = XEXEC_OBJECT_IONETWORKEXCHANGE;
	execObjectData = (void *)&collectedData;

#endif
	socket = fse.socket;
	opened = fse.opened;

	fse.opened = false;
	fse.socket = -1;

	socketOpts = fse.socketOpts;
	inTimeout = fse.inTimeout;
	outTimeout = fse.outTimeout;
	inSocketBuffer = fse.inSocketBuffer;
	outSocketBuffer = fse.outSocketBuffer;
	lingerOpts = fse.lingerOpts;
	lingerSeconds = fse.lingerSeconds;
	blocked = fse.blocked;
}

//-------------------------------------------------------------------

exchange::exchange()
{
#ifndef IO_WO_XEXEC

	execObject = XEXEC_OBJECT_IONETWORKEXCHANGE;
	execObjectData = (void *)&collectedData;

#endif
}

//-------------------------------------------------------------------

exchange::exchange(__initialAccept &a_init)
{
#ifndef IO_WO_XEXEC

	execObject = XEXEC_OBJECT_IONETWORKEXCHANGE;
	execObjectData = (void *)&collectedData;

#endif

	init(a_init);
}

//-------------------------------------------------------------------

exchange::~exchange()
{
	if (opened)
	{
		::shutdown(socket, SHUT_RDWR);

		::close(socket);
	}
}

//-------------------------------------------------------------------

void
exchange::init(__initialAccept &a_init)
{
	init(a_init.socket, a_init.blocked, a_init.blockInherited);

	a_init.socket = -1;
}

//-------------------------------------------------------------------

void
exchange::close()
{
	raceHazardGuard pg(this);

#ifndef IO_WO_XEXEC
	operType = EXCHANGE_OPERATION_CLOSE;
	performXExec(preExec);
#endif

	if (!opened)
		return ;

	_close(socket);

	socket = -1;

	opened = false;

#ifndef IO_WO_XEXEC
	performXExec(postExec);
#endif
}

//-------------------------------------------------------------------

void
exchange::init(int a_socket,
			   bool a_blocked,
			   bool blockInherited)
{
	raceHazardGuard pg(this);

	if (opened)
	{
		_close(socket);

		socket = -1;

		opened = false;
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
			block(false);
		else
			block(true);
	}
	else
		block(true);

	opened = true;
}

//-------------------------------------------------------------------

bool
exchange::isAlive()
{
	raceHazardGuard pg(this);

	if (!opened)
		return false;

	pollfd fd;
	fd.fd = socket;
	fd.events = POLLOUT;

	if (poll(&fd, 1, -1) > 0)
		if (isSetFlag(fd.revents, POLLOUT))
			return true;

	_close(socket);

	socket = -1;
	opened = false;

	return false;
}


//-------------------------------------------------------------------

void
exchange::_write(const char * const data)
{
	if (!opened)
		throw baseEx(ERRMODULE_IONETWORKEXCHANGE, EXCHANGEEX__WRITE, ERR_LIBDODO, EXCHANGEEX_NOCONNECTION, IONETWORKEXCHANGEEX_NOCONNECTION_STR, __LINE__, __FILE__);

	unsigned long iter = outSize / outSocketBuffer;
	unsigned long rest = outSize % outSocketBuffer;

	unsigned long sent_received = 0;

	unsigned long batch, n;

	for (unsigned long i = 0; i < iter; ++i)
	{
		batch = 0;
		while (batch < outSocketBuffer)
		{
			while (true)
			{
				if ((n = ::send(socket, data + sent_received, outSocketBuffer, 0)) == -1)
				{
					if (errno == EINTR)
						continue;

					if (errno == EAGAIN)
						break;

					throw baseEx(ERRMODULE_IONETWORKEXCHANGE, EXCHANGEEX__WRITE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
				}

				break;
			}

			batch += n;
			sent_received += n;
		}
	}

	if (rest > 0)
	{
		batch = 0;
		while (batch < rest)
		{
			while (true)
			{
				if ((n = ::send(socket, data + sent_received, rest, 0)) == -1)
				{
					if (errno == EINTR)
						continue;

					if (errno == EAGAIN)
						break;

					throw baseEx(ERRMODULE_IONETWORKEXCHANGE, EXCHANGEEX__WRITE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
				}

				break;
			}

			batch += n;
			sent_received += n;
		}
	}
}

//-------------------------------------------------------------------

void
exchange::_read(char * const data)
{
	if (!opened)
		throw baseEx(ERRMODULE_IONETWORKEXCHANGE, EXCHANGEEX__READ, ERR_LIBDODO, EXCHANGEEX_NOCONNECTION, IONETWORKEXCHANGEEX_NOCONNECTION_STR, __LINE__, __FILE__);

	memset(data, '\0', inSize);

	unsigned long iter = inSize / inSocketBuffer;
	unsigned long rest = inSize % inSocketBuffer;

	unsigned long sent_received = 0;

	unsigned long batch, n;

	for (unsigned long i = 0; i < iter; ++i)
	{
		batch = 0;
		while (batch < inSocketBuffer)
		{
			while (true)
			{
				if ((n = ::recv(socket, data + sent_received, inSocketBuffer, 0)) == -1)
				{
					if (errno == EINTR)
						continue;

					if (errno == EAGAIN)
						break;

					throw baseEx(ERRMODULE_IONETWORKEXCHANGE, EXCHANGEEX__READ, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
				}

				break;
			}

			if (n == 0)
				break;

			batch += n;
			sent_received += n;
		}
	}

	if (rest > 0)
	{
		batch = 0;
		while (batch < rest)
		{
			while (true)
			{
				if ((n = ::recv(socket, data + sent_received, rest, 0)) == -1)
				{
					if (errno == EINTR)
						continue;

					if (errno == EAGAIN)
						break;

					throw baseEx(ERRMODULE_IONETWORKEXCHANGE, EXCHANGEEX__READ, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
				}

				break;
			}

			if (n == 0)
				break;

			batch += n;
			sent_received += n;
		}
	}
}

//-------------------------------------------------------------------

void
exchange::_writeStream(const char * const data)
{
	_write(data);
}

//-------------------------------------------------------------------

unsigned long
exchange::_readStream(char * const data)
{
	if (!opened)
		throw baseEx(ERRMODULE_IONETWORKEXCHANGE, EXCHANGEEX__READSTREAM, ERR_LIBDODO, EXCHANGEEX_NOCONNECTION, IONETWORKEXCHANGEEX_NOCONNECTION_STR, __LINE__, __FILE__);

	memset(data, '\0', inSize);

	unsigned long n = 0;

	while (true)
	{
		if ((n = ::recv(socket, data, inSize, 0)) == -1)
		{
			if (errno == EINTR)
				continue;

			if (errno == EAGAIN)
				break;

			throw baseEx(ERRMODULE_IONETWORKEXCHANGE, EXCHANGEEX__READSTREAM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}

		break;
	}

	return n;
}

//-------------------------------------------------------------------

exchange *
exchange::createCopy()
{
	raceHazardGuard pg(this);

	exchange *copy = new exchange;

	copy->socket = socket;
	copy->opened = opened;

	opened = false;
	socket = -1;

	copy->socketOpts = socketOpts;
	copy->inTimeout = inTimeout;
	copy->outTimeout = outTimeout;
	copy->inSocketBuffer = inSocketBuffer;
	copy->outSocketBuffer = outSocketBuffer;
	copy->lingerOpts = lingerOpts;
	copy->lingerSeconds = lingerSeconds;
	copy->blocked = blocked;

	return copy;
}

//-------------------------------------------------------------------

void
exchange::deleteCopy(exchange *copy)
{
	delete copy;
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
	raceHazardGuard pg(this);

	return socket;
}

//-------------------------------------------------------------------

int
exchange::getOutDescriptor() const
{
	raceHazardGuard pg(this);

	return socket;
}

//-------------------------------------------------------------------

