/***************************************************************************
 *            ioNetworkSslExchange.cc
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

#include <libdodo/ioNetworkSslExchange.h>

#ifdef OPENSSL_EXT

using namespace dodo::io::network::ssl;

#ifndef IONETWORKSSLEXCHANGE_WO_XEXEC

__xexexIoNetworkExchangeCollectedData::__xexexIoNetworkExchangeCollectedData(int &a_operType,
									     void *a_executor) : operType(a_operType),
												 executor(a_executor)
{
}

#endif

//-------------------------------------------------------------------

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

exchange::exchange(exchange &fse)
#ifndef IONETWORKSSLEXCHANGE_WO_XEXEC

	: collectedData(operType,
			(void *) this)

#endif
{
#ifndef IONETWORKSSLEXCHANGE_WO_XEXEC

	execObject = XEXEC_OBJECT_IONETWORKSSLEXCHANGE;
	execObjectData = (void *)&collectedData;

#endif

	socket = fse.socket;
	opened = fse.opened;
	sslHandle = fse.sslHandle;

	fse.opened = false;
	fse.socket = -1;
	fse.sslHandle = NULL;

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

exchange::exchange() : sslHandle(NULL)
#ifndef IONETWORKSSLEXCHANGE_WO_XEXEC

		       ,
		       collectedData(operType,
				     (void *) this)

#endif
{
#ifndef IONETWORKSSLEXCHANGE_WO_XEXEC

	execObject = XEXEC_OBJECT_IONETWORKSSLEXCHANGE;
	execObjectData = (void *)&collectedData;

#endif
}

//-------------------------------------------------------------------

exchange::exchange(__initialAccept &a_init)
#ifndef IONETWORKSSLEXCHANGE_WO_XEXEC

	: collectedData(operType,
			(void *) this)

#endif
{
#ifndef IONETWORKSSLEXCHANGE_WO_XEXEC

	execObject = XEXEC_OBJECT_IONETWORKSSLEXCHANGE;
	execObjectData = (void *)&collectedData;

#endif

	init(a_init);
}

//-------------------------------------------------------------------

exchange::~exchange()
{
	if (opened)
	{
		if (sslHandle != NULL)
		{
			if (SSL_shutdown(sslHandle) == 0)
				SSL_shutdown(sslHandle);

			SSL_free(sslHandle);
		}

		::shutdown(socket, SHUT_RDWR);

		::close(socket);
	}
}

//-------------------------------------------------------------------

void
exchange::init(__initialAccept &a_init)
{
	init(a_init.socket, a_init.sslHandle, a_init.blocked, a_init.blockInherited);

	a_init.socket = -1;
	a_init.sslHandle = NULL;
}

//-------------------------------------------------------------------

void
exchange::_close(int socket,
		 SSL *sslHandle)
{
	options::_close(socket);

	int err = SSL_shutdown(sslHandle);
	if (err < 0)
	{
		unsigned long nerr = ERR_get_error();
		throw baseEx(ERRMODULE_IONETWORKSSLEXCHANGE, EXCHANGEEX__CLOSE, ERR_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
	}
	if (err == 0)
	{
		err = SSL_shutdown(sslHandle);
		if (err < 0)
		{
			unsigned long nerr = ERR_get_error();
			throw baseEx(ERRMODULE_IONETWORKSSLEXCHANGE, EXCHANGEEX__CLOSE, ERR_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
		}
	}
}

//-------------------------------------------------------------------

void
exchange::close()
{
	raceHazardGuard pg(this);

#ifndef IONETWORKSSLEXCHANGE_WO_XEXEC
	operType = EXCHANGE_OPERATION_CLOSE;
	performXExec(preExec);
#endif

	if (!opened)
		return ;

	_close(socket, sslHandle);

	socket = -1;
	sslHandle = NULL;

	opened = false;

#ifndef IONETWORKSSLEXCHANGE_WO_XEXEC
	performXExec(postExec);
#endif
}

//-------------------------------------------------------------------

void
exchange::init(int a_socket,
	       SSL *a_sslHandle,
	       bool a_blocked,
	       bool blockInherited)
{
	raceHazardGuard pg(this);

	if (opened)
	{
		_close(socket, sslHandle);

		socket = -1;
		sslHandle = NULL;

		opened = false;
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

	_close(socket, sslHandle);

	socket = -1;
	sslHandle = NULL;

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
				if ((n = SSL_write(sslHandle, data + sent_received, outSocketBuffer)) <= 0)
				{
					switch (SSL_get_error(sslHandle, n))
					{
						case SSL_ERROR_WANT_READ:
						case SSL_ERROR_WANT_WRITE:
						case SSL_ERROR_WANT_X509_LOOKUP:
							
							n = 0;
							
							break;
							
						default:
						{
							unsigned long nerr = ERR_get_error();							
							throw baseEx(ERRMODULE_IONETWORKSSLEXCHANGE, EXCHANGEEX__WRITE, ERR_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
						}
							
					}
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
				if ((n = SSL_write(sslHandle, data + sent_received, rest)) <= 0)
				{
					switch (SSL_get_error(sslHandle, n))
					{
						case SSL_ERROR_WANT_READ:
						case SSL_ERROR_WANT_WRITE:
						case SSL_ERROR_WANT_X509_LOOKUP:
							
							n = 0;
							
							break;
							
						default:
						{
							unsigned long nerr = ERR_get_error();							
							throw baseEx(ERRMODULE_IONETWORKSSLEXCHANGE, EXCHANGEEX__WRITE, ERR_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
						}
							
					}
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

#ifndef IONETWORKSSLEXCHANGE_WO_XEXEC
	collectedData.buffer.assign(a_buf, outSize);

	operType = EXCHANGE_OPERATION_WRITE;
	performXExec(preExec);

	try
	{
		_write(collectedData.buffer.c_str());
	}
	catch (...)
	{
		collectedData.buffer.clear();

		throw;
	}
#else
	_write(a_buf);
#endif


#ifndef IONETWORKSSLEXCHANGE_WO_XEXEC
	performXExec(postExec);

	collectedData.buffer.clear();
#endif
}

//-------------------------------------------------------------------

void
exchange::writeString(const dodoString &a_buf)
{
	raceHazardGuard pg(this);

#ifndef IONETWORKSSLEXCHANGE_WO_XEXEC
	collectedData.buffer = a_buf;

	operType = EXCHANGE_OPERATION_WRITESTRING;
	performXExec(preExec);

	try
	{
		_write(collectedData.buffer.c_str());
	}
	catch (...)
	{
		collectedData.buffer.clear();

		throw;
	}
#else
	_write(a_buf.c_str());
#endif


#ifndef IONETWORKSSLEXCHANGE_WO_XEXEC
	performXExec(postExec);

	collectedData.buffer.clear();
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
				if ((n = SSL_read(sslHandle, data + sent_received, inSocketBuffer)) <= 0)
				{
					switch (SSL_get_error(sslHandle, n))
					{
						case SSL_ERROR_WANT_READ:
						case SSL_ERROR_WANT_WRITE:
						case SSL_ERROR_WANT_X509_LOOKUP:
							
							n = 0;
							
							break;
							
						default:
						{
							unsigned long nerr = ERR_get_error();							
							throw baseEx(ERRMODULE_IONETWORKSSLEXCHANGE, EXCHANGEEX__READ, ERR_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
						}
							
					}
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
				if ((n = SSL_read(sslHandle, data + sent_received, rest)) <= 0)
				{
					switch (SSL_get_error(sslHandle, n))
					{
						case SSL_ERROR_WANT_READ:
						case SSL_ERROR_WANT_WRITE:
						case SSL_ERROR_WANT_X509_LOOKUP:
							
							n = 0;
							
							break;
							
						default:
						{
							unsigned long nerr = ERR_get_error();							
							throw baseEx(ERRMODULE_IONETWORKSSLEXCHANGE, EXCHANGEEX__READ, ERR_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
						}
							
					}
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

#ifndef IONETWORKSSLEXCHANGE_WO_XEXEC
	operType = EXCHANGE_OPERATION_READ;
	performXExec(preExec);

	collectedData.buffer.reserve(inSize);
#endif

#ifndef IONETWORKSSLEXCHANGE_WO_XEXEC
	try
	{
		_read(a_void);
	}
	catch (...)
	{
		collectedData.buffer.clear();

		throw;
	}
#else
	_read(a_void);
#endif

#ifndef IONETWORKSSLEXCHANGE_WO_XEXEC
	collectedData.buffer.assign(a_void, inSize);

	performXExec(postExec);

	strncpy(a_void, collectedData.buffer.c_str(), collectedData.buffer.size() > inSize ? inSize : collectedData.buffer.size());
	collectedData.buffer.clear();
#endif
}

//-------------------------------------------------------------------

void
exchange::readString(dodoString &a_str)
{
	raceHazardGuard pg(this);

#ifndef IONETWORKSSLEXCHANGE_WO_XEXEC
	operType = EXCHANGE_OPERATION_READSTRING;
	performXExec(preExec);

	collectedData.buffer.reserve(inSize);
#endif

	char *data = new char[inSize];

	try
	{
		_read(data);
	}
	catch (...)
	{
		delete [] data;

#ifndef IONETWORKSSLEXCHANGE_WO_XEXEC
		collectedData.buffer.clear();
#endif

		throw;
	}

#ifndef IONETWORKSSLEXCHANGE_WO_XEXEC
	collectedData.buffer.assign(data, inSize);
	delete [] data;

	performXExec(postExec);

	a_str = collectedData.buffer;
	collectedData.buffer.clear();
#else
	a_str.assign(data, inSize);
	delete [] data;
#endif
}

//-------------------------------------------------------------------

void
exchange::writeStream(const char * const a_buf)
{
	raceHazardGuard pg(this);

	unsigned long _outSize = outSize;

#ifndef IONETWORKSSLEXCHANGE_WO_XEXEC
	collectedData.buffer = a_buf;

	operType = EXCHANGE_OPERATION_WRITESTREAM;
	performXExec(preExec);

	try
	{
		outSize = collectedData.buffer.size();

		_write(collectedData.buffer.c_str());

		outSize = _outSize;
	}
	catch (...)
	{
		outSize = _outSize;

		collectedData.buffer.clear();

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

#ifndef IONETWORKSSLEXCHANGE_WO_XEXEC
	performXExec(postExec);

	collectedData.buffer.clear();
#endif
}

//-------------------------------------------------------------------

void
exchange::writeStreamString(const dodoString &a_buf)
{
	raceHazardGuard pg(this);

	unsigned long _outSize = outSize;

#ifndef IONETWORKSSLEXCHANGE_WO_XEXEC
	collectedData.buffer = a_buf;

	operType = EXCHANGE_OPERATION_WRITESTREAMSTRING;
	performXExec(preExec);

	try
	{
		outSize = collectedData.buffer.size();

		_write(collectedData.buffer.c_str());

		outSize = _outSize;
	}
	catch (...)
	{
		outSize = _outSize;

		collectedData.buffer.clear();

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

#ifndef IONETWORKSSLEXCHANGE_WO_XEXEC
	performXExec(postExec);

	collectedData.buffer.clear();
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
		if ((n = SSL_read(sslHandle, data, inSize)) <= 0)
		{
			switch (SSL_get_error(sslHandle, n))
			{
				case SSL_ERROR_WANT_READ:
				case SSL_ERROR_WANT_WRITE:
				case SSL_ERROR_WANT_X509_LOOKUP:
				case SSL_ERROR_ZERO_RETURN:

					n = 0;
					
					break;
					
				default:
				{
					unsigned long nerr = ERR_get_error();							
					throw baseEx(ERRMODULE_IONETWORKSSLEXCHANGE, EXCHANGEEX__READSTREAM, ERR_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
				}
			}
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

#ifndef IONETWORKSSLEXCHANGE_WO_XEXEC
	operType = EXCHANGE_OPERATION_READSTREAM;
	performXExec(preExec);
#endif

	unsigned long n = _readStream(a_void);
	if (n == inSize)
		a_void[n] = '\0';

#ifndef IONETWORKSSLEXCHANGE_WO_XEXEC
	collectedData.buffer = a_void;

	performXExec(postExec);

	if (collectedData.buffer.size() > inSize)
		collectedData.buffer.resize(inSize);
	strcpy(a_void, collectedData.buffer.c_str());
	collectedData.buffer.clear();
#endif
}

//-------------------------------------------------------------------

void
exchange::readStreamString(dodoString &a_str)
{
	raceHazardGuard pg(this);

	a_str.clear();

#ifndef IONETWORKSSLEXCHANGE_WO_XEXEC
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

#ifndef IONETWORKSSLEXCHANGE_WO_XEXEC
	if (n > 0)
		collectedData.buffer.assign(data, n);

	delete [] data;

	performXExec(postExec);

	a_str = collectedData.buffer;
	collectedData.buffer.clear();
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

#endif

