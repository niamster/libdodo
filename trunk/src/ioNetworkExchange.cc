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

#include <libdodo/ioNetworkExchange.h>

using namespace dodo::io::network;

#ifndef IONETWORKEXCHANGE_WO_XEXEC

__xexexIoNetworkExchangeCollectedData::__xexexIoNetworkExchangeCollectedData(dodoString &a_buffer,
											   int &a_operType,
											   void *a_executor) : buffer(a_buffer),
																   operType(a_operType),
																   executor(a_executor)
{
}

#endif

//-------------------------------------------------------------------

__initialAccept::__initialAccept() : socket(-1),
									 family(-1),
									 type(-1)
{
}

//-------------------------------------------------------------------

__initialAccept::__initialAccept(__initialAccept &init) : socket(init.socket),
														  family(init.family),
														  type(init.type)
{
	init.socket = -1;
}

//-------------------------------------------------------------------

exchange::exchange(exchange &fse)

#ifndef IONETWORKEXCHANGE_WO_XEXEC

		: collectedData(buffer,
					 operType,
					 (void *) this)
			 
#endif
			 
{
	socket = fse.socket;
	opened = fse.opened;

	fse.opened = false;
	fse.socket = -1;

	family = fse.family;
	type = fse.type;

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

#ifndef IONETWORKEXCHANGE_WO_XEXEC

		: collectedData(buffer,
					 operType,
					 (void *) this)
			 
#endif
			 
{
}

//-------------------------------------------------------------------

exchange::exchange(__initialAccept &a_init)

#ifndef IONETWORKEXCHANGE_WO_XEXEC

		: collectedData(buffer,
					 operType,
					 (void *) this)
			 
#endif
			 
{
	init(a_init.socket, a_init.blockInherited);
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
	family = a_init.family;
	type = a_init.type;

	blocked = a_init.blocked;

	init(a_init.socket, a_init.blockInherited);
}

//-------------------------------------------------------------------

void
exchange::close()
{
	raceHazardGuard pg(this);

#ifndef IONETWORKEXCHANGE_WO_XEXEC
	operType = EXCHANGE_OPERATION_CLOSE;
	performXExec(preExec);
#endif

	if (!opened)
		return ;

	options::_close(socket);

	socket = -1;

	opened = false;

#ifndef IONETWORKEXCHANGE_WO_XEXEC
	performXExec(postExec);
#endif
}

//-------------------------------------------------------------------

void
exchange::init(int a_socket,
					   bool blockInherited)
{
	raceHazardGuard pg(this);

	if (opened)
	{
		options::_close(socket);

		socket = -1;

		opened = false;
	}

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
	
	options::_close(socket);

	socket = -1;
	opened = false;

	return false;
}


//-------------------------------------------------------------------

void
exchange::_write(const char * const data)
{
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
exchange::write(const char * const a_buf)
{
	raceHazardGuard pg(this);

#ifndef IONETWORKEXCHANGE_WO_XEXEC
	buffer.assign(a_buf, outSize);

	operType = EXCHANGE_OPERATION_WRITE;
	performXExec(preExec);

	try
	{
		_write(buffer.c_str());
	}
	catch (...)
	{
		buffer.clear();

		throw;
	}
#else
	_write(a_buf);
#endif


#ifndef IONETWORKEXCHANGE_WO_XEXEC
	performXExec(postExec);

	buffer.clear();
#endif
}

//-------------------------------------------------------------------

void
exchange::writeString(const dodoString &a_buf)
{
	raceHazardGuard pg(this);

#ifndef IONETWORKEXCHANGE_WO_XEXEC
	buffer = a_buf;

	operType = EXCHANGE_OPERATION_WRITESTRING;
	performXExec(preExec);

	try
	{
		_write(buffer.c_str());
	}
	catch (...)
	{
		buffer.clear();

		throw;
	}
#else
	_write(a_buf.c_str());
#endif


#ifndef IONETWORKEXCHANGE_WO_XEXEC
	performXExec(postExec);

	buffer.clear();
#endif
}

//-------------------------------------------------------------------

void
exchange::_read(char * const data)
{
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
exchange::read(char * const a_void)
{
	raceHazardGuard pg(this);

#ifndef IONETWORKEXCHANGE_WO_XEXEC
	operType = EXCHANGE_OPERATION_READ;
	performXExec(preExec);

	buffer.reserve(inSize);
#endif

#ifndef IONETWORKEXCHANGE_WO_XEXEC
	try
	{
		_read(a_void);
	}
	catch (...)
	{
		buffer.clear();

		throw;
	}
#else
	_read(a_void);
#endif

#ifndef IONETWORKEXCHANGE_WO_XEXEC
	buffer.assign(a_void, inSize);

	performXExec(postExec);

	strncpy(a_void, buffer.c_str(), buffer.size() > inSize ? inSize : buffer.size());
	buffer.clear();
#endif
}

//-------------------------------------------------------------------

void
exchange::readString(dodoString &a_str)
{
	raceHazardGuard pg(this);

#ifndef IONETWORKEXCHANGE_WO_XEXEC
	operType = EXCHANGE_OPERATION_READSTRING;
	performXExec(preExec);

	buffer.reserve(inSize);
#endif

	char *data = new char[inSize];

	try
	{
		_read(data);
	}
	catch (...)
	{
		delete [] data;

#ifndef IONETWORKEXCHANGE_WO_XEXEC
		buffer.clear();
#endif

		throw;
	}

#ifndef IONETWORKEXCHANGE_WO_XEXEC
	buffer.assign(data, inSize);
	delete [] data;

	performXExec(postExec);

	a_str = buffer;
	buffer.clear();
#else
	a_str.assign(data, inSize);
	delete [] data;
#endif
}

#ifndef IONETWORKEXCHANGE_WO_XEXEC

//-------------------------------------------------------------------

int
exchange::addPostExec(inExec func,
							  void   *data)
{
	return _addPostExec(func, (void *)&collectedData, XEXEC_OBJECT_IONETWORKEXCHANGE, data);
}

//-------------------------------------------------------------------

int
exchange::addPreExec(inExec func,
							 void   *data)
{
	return _addPreExec(func, (void *)&collectedData, XEXEC_OBJECT_IONETWORKEXCHANGE, data);
}

//-------------------------------------------------------------------

#ifdef DL_EXT

int
exchange::addPostExec(const dodoString &module,
							  void             *data,
							  void             *toInit)
{
	return _addPostExec(module, (void *)&collectedData, XEXEC_OBJECT_IONETWORKEXCHANGE, data, toInit);
}

//-------------------------------------------------------------------

int
exchange::addPreExec(const dodoString &module,
							 void             *data,
							 void             *toInit)
{
	return _addPreExec(module, (void *)&collectedData, XEXEC_OBJECT_IONETWORKEXCHANGE, data, toInit);
}

//-------------------------------------------------------------------

dodo::__xexecCounts
exchange::addExec(const dodoString &module,
						  void             *data,
						  void             *toInit)
{
	return _addExec(module, (void *)&collectedData, XEXEC_OBJECT_IONETWORKEXCHANGE, data, toInit);
}

#endif

#endif

//-------------------------------------------------------------------

void
exchange::writeStream(const char * const a_buf)
{
	raceHazardGuard pg(this);

	unsigned long _outSize = outSize;

#ifndef IONETWORKEXCHANGE_WO_XEXEC
	buffer = a_buf;

	operType = EXCHANGE_OPERATION_WRITESTREAM;
	performXExec(preExec);

	try
	{
		outSize = buffer.size();

		_write(buffer.c_str());

		outSize = _outSize;
	}
	catch (...)
	{
		outSize = _outSize;

		buffer.clear();

		throw;
	}
#else
	try
	{
		outSize = strlen(a_buf);

		_write(a_buf);

		outSize = _outSize;
	}
	catch (...)
	{
		outSize = _outSize;

		throw;
	}
#endif

#ifndef IONETWORKEXCHANGE_WO_XEXEC
	performXExec(postExec);

	buffer.clear();
#endif
}

//-------------------------------------------------------------------

void
exchange::writeStreamString(const dodoString &a_buf)
{
	raceHazardGuard pg(this);

	unsigned long _outSize = outSize;

#ifndef IONETWORKEXCHANGE_WO_XEXEC
	buffer = a_buf;

	operType = EXCHANGE_OPERATION_WRITESTREAMSTRING;
	performXExec(preExec);

	try
	{
		outSize = buffer.size();

		_write(buffer.c_str());

		outSize = _outSize;
	}
	catch (...)
	{
		outSize = _outSize;

		buffer.clear();

		throw;
	}
#else
	try
	{
		outSize = a_buf.size();

		_write(a_buf.c_str());

		outSize = _outSize;
	}
	catch (...)
	{
		outSize = _outSize;

		throw;
	}
#endif

#ifndef IONETWORKEXCHANGE_WO_XEXEC
	performXExec(postExec);

	buffer.clear();
#endif
}

//-------------------------------------------------------------------

unsigned long
exchange::_readStream(char * const data)
{
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

void
exchange::readStream(char * const a_void)
{
	raceHazardGuard pg(this);

#ifndef IONETWORKEXCHANGE_WO_XEXEC
	operType = EXCHANGE_OPERATION_READSTREAM;
	performXExec(preExec);
#endif

	unsigned long n = _readStream(a_void);
	if (n == inSize)
		a_void[n] = '\0';

#ifndef IONETWORKEXCHANGE_WO_XEXEC
	buffer = a_void;

	performXExec(postExec);

	if (buffer.size() > inSize)
		buffer.resize(inSize);
	strcpy(a_void, buffer.c_str());
	buffer.clear();
#endif
}

//-------------------------------------------------------------------

void
exchange::readStreamString(dodoString &a_str)
{
	raceHazardGuard pg(this);

	a_str.clear();
	
#ifndef IONETWORKEXCHANGE_WO_XEXEC
	operType = EXCHANGE_OPERATION_READSTREAMSTRING;
	performXExec(preExec);
#endif

	char *data = new char[inSize];
	unsigned long n = 0;
	
	try
	{
		n = _readStream(data);
	}
	catch (...)
	{
		delete [] data;

		throw;
	}

#ifndef IONETWORKEXCHANGE_WO_XEXEC
	if (n > 0)
		buffer.assign(data, n);
	
	delete [] data;

	performXExec(postExec);

	a_str = buffer;
	buffer.clear();
#else
	if (n > 0)
		a_str.assign(data, n);
	
	delete [] data;
#endif
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

	copy->family = family;
	copy->type = type;

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

