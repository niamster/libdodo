/***************************************************************************
 *            ioStdio.cc
 *
 *  Tue Nov 15 21:19:57 2005
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

#include <libdodo/ioStdio.h>

using namespace dodo::io;

#ifndef IOSTDIO_WO_XEXEC

__xexexIoStdioCollectedData::__xexexIoStdioCollectedData(int &a_operType,
														 void *a_executor) : operType(a_operType),
																			 executor(a_executor)
{
}

#endif

//-------------------------------------------------------------------

stdio::stdio() : inSTDBuffer(STD_INSIZE),
				 outSTDBuffer(STD_OUTSIZE),
				 err(false),
				 blocked(true),
				 desc(stdout)

#ifndef IOSTDIO_WO_XEXEC

				 ,
				 collectedData(operType,
							   (void *) this)

#endif
{
}

//-------------------------------------------------------------------

stdio::stdio(stdio &fd)
#ifndef IOSTDIO_WO_XEXEC

	: collectedData(operType,
					(void *) this)

#endif
{
}

//-------------------------------------------------------------------

stdio::~stdio()
{
}

//-------------------------------------------------------------------

int
stdio::getInDescriptor() const
{
	raceHazardGuard pg(this);

	return fileno(stdin);
}

//-------------------------------------------------------------------

int
stdio::getOutDescriptor() const
{
	raceHazardGuard pg(this);

	if (err)
		return fileno(stderr);
	else
		return fileno(stdout);
}

//-------------------------------------------------------------------

#ifndef IOSTDIO_WO_XEXEC

int
stdio::addPostExec(inExec func,
				   void   *data)
{
	return _addPostExec(func, (void *)&collectedData, XEXEC_OBJECT_IOSTDIO, data);
}

//-------------------------------------------------------------------

int
stdio::addPreExec(inExec func,
				  void   *data)
{
	return _addPreExec(func, (void *)&collectedData, XEXEC_OBJECT_IOSTDIO, data);
}

//-------------------------------------------------------------------

#ifdef DL_EXT

int
stdio::addPostExec(const dodoString &module,
				   void             *data,
				   void             *toInit)
{
	return _addPostExec(module, (void *)&collectedData, XEXEC_OBJECT_IOSTDIO, data, toInit);
}

//-------------------------------------------------------------------

dodo::__xexecCounts
stdio::addExec(const dodoString &module,
			   void             *data,
			   void             *toInit)
{
	return _addExec(module, (void *)&collectedData, XEXEC_OBJECT_IOSTDIO, data, toInit);
}

//-------------------------------------------------------------------

int
stdio::addPreExec(const dodoString &module,
				  void             *data,
				  void             *toInit)
{
	return _addPreExec(module, (void *)&collectedData, XEXEC_OBJECT_IOSTDIO, data, toInit);
}

#endif

#endif

//-------------------------------------------------------------------

void
stdio::_read(char * const a_void)
{
	char *data = a_void;

	memset(a_void, '\0', inSize);

	unsigned long iter = inSize / inSTDBuffer;
	unsigned long rest = inSize % inSTDBuffer;

	for (unsigned long i = 0; i < iter; ++i)
	{
		while (true)
		{
			if (fread(data, inSTDBuffer, 1, stdin) == 0)
			{
				if (feof(stdin) != 0 || errno == EAGAIN)
					break;
			
				if (errno == EINTR)
					continue;

				if (ferror(stdin) != 0)
					throw baseEx(ERRMODULE_IOSTDIO, STDIOEX__READ, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			}

			break;
		}

		data += inSTDBuffer;
	}

	if (rest > 0)
	{
		while (true)
		{
			if (fread(data, rest, 1, stdin) == 0)
			{
				if (feof(stdin) != 0 || errno == EAGAIN)
					break;
			
				if (errno == EINTR)
					continue;

				if (ferror(stdin) != 0)
					throw baseEx(ERRMODULE_IOSTDIO, STDIOEX__READ, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			}

			break;
		}
	}
}

//-------------------------------------------------------------------

void
stdio::read(char * const a_void)
{
	raceHazardGuard pg(this);

#ifndef IOSTDIO_WO_XEXEC
	operType = STDIO_OPERATION_READ;
	performXExec(preExec);

	collectedData.buffer.reserve(inSize);
#endif

#ifndef IOSTDIO_WO_XEXEC
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

#ifndef IOSTDIO_WO_XEXEC
	collectedData.buffer.assign(a_void, inSize);

	performXExec(postExec);

	strncpy(a_void, collectedData.buffer.c_str(), collectedData.buffer.size() > inSize ? inSize : collectedData.buffer.size());
	collectedData.buffer.clear();
#endif
}

//-------------------------------------------------------------------

void
stdio::readString(dodoString &a_str)
{
	raceHazardGuard pg(this);

#ifndef IOSTDIO_WO_XEXEC
	operType = STDIO_OPERATION_READSTRING;
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

#ifndef IOSTDIO_WO_XEXEC
		collectedData.buffer.clear();
#endif

		throw;
	}

#ifndef IOSTDIO_WO_XEXEC
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
stdio::writeString(const dodoString &a_buf)
{
	raceHazardGuard pg(this);

#ifndef IOSTDIO_WO_XEXEC
	collectedData.buffer = a_buf;

	operType = STDIO_OPERATION_WRITESTRING;
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


#ifndef IOSTDIO_WO_XEXEC
	performXExec(postExec);

	collectedData.buffer.clear();
#endif
}

//-------------------------------------------------------------------

void
stdio::write(const char *const a_buf)
{
	raceHazardGuard pg(this);

#ifndef IOSTDIO_WO_XEXEC
	collectedData.buffer.assign(a_buf, outSize);

	operType = STDIO_OPERATION_WRITE;
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


#ifndef IOSTDIO_WO_XEXEC
	performXExec(postExec);

	collectedData.buffer.clear();
#endif
}

//-------------------------------------------------------------------

void
stdio::_write(const char *const buf)
{
	char *data = (char *)buf;

	unsigned long iter = outSize / outSTDBuffer;
	unsigned long rest = outSize % outSTDBuffer;

	for (unsigned long i = 0; i < iter; ++i)
	{
		while (true)
		{
			if (fwrite(data, outSTDBuffer, 1, desc) == 0)
			{
				if (errno == EINTR)
					continue;

				if (errno == EAGAIN)
					break;

				if (ferror(desc) != 0)
					throw baseEx(ERRMODULE_IOSTDIO, STDIOEX__WRITE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			}

			break;
		}

		data += outSTDBuffer;
	}

	if (rest > 0)
	{
		while (true)
		{
			if (fwrite(data, 1, rest, desc) == 0)
			{
				if (errno == EINTR)
					continue;

				if (errno == EAGAIN)
					break;

				if (ferror(desc) != 0)
					throw baseEx(ERRMODULE_IOSTDIO, STDIOEX__WRITE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			}

			break;
		}
	}
}

//-------------------------------------------------------------------

void
stdio::flush()
{
	raceHazardGuard pg(this);

	if (fflush(desc) != 0)
		throw baseEx(ERRMODULE_IOSTDIO, STDIOEX_FLUSH, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

network::__connInfo
stdio::inputterInfo()
{
	raceHazardGuard pg(this);

	network::__connInfo info;

	struct sockaddr sa;

	socklen_t len = sizeof(sockaddr_in6);

	if (::getpeername(1, &sa, &len) == 1)
	{
		if (errno != ENOTSOCK)
			throw baseEx(ERRMODULE_IOSTDIO, STDIOEX_INPUTTERINFO, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		else
			return info;
	}

	switch (len)
	{
		case sizeof(sockaddr_in):

		{
			char temp[15];
			sockaddr_in *sa4;
			sa4 = (sockaddr_in *)&sa;
			if (inet_ntop(AF_INET, &(sa4->sin_addr), temp, 15) != NULL)
				info.host.assign(temp);
			info.port = ntohs(sa4->sin_port);

			return info;
		}

		case sizeof(sockaddr_in6):

		{
			char temp[INET6_ADDRSTRLEN];
			sockaddr_in6 *sa6;
			sa6 = (sockaddr_in6 *)&sa6;
			if (inet_ntop(AF_INET6, &(sa6->sin6_addr), temp, INET6_ADDRSTRLEN) != NULL)
				info.host.assign(temp);
			info.port = ntohs(sa6->sin6_port);

			return info;
		}

		default:

			return info;
	}
}

//-------------------------------------------------------------------

bool
stdio::isBlocked()
{
	raceHazardGuard pg(this);

	return blocked;
}

//-------------------------------------------------------------------

void
stdio::redirectToSTDErr(bool toSTDErr)
{
	err = toSTDErr;

	if (err)
		desc = stderr;
	else
		desc = stdout;
}

//-------------------------------------------------------------------

bool
stdio::isRedirectedToSTDErr()
{
	return err;
}

//-------------------------------------------------------------------

void
stdio::block(bool flag)
{
	raceHazardGuard pg(this);

	int block[3] = { O_NONBLOCK, O_NONBLOCK, O_NONBLOCK };

	if (flag)
	{
		block[0] = fcntl(0, F_GETFL);
		if (block[0] == -1)
			throw baseEx(ERRMODULE_IOSTDIO, STDIOEX_BLOCK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		block[0] &= ~O_NONBLOCK;

		block[1] = fcntl(1, F_GETFL);
		if (block[1] == -1)
			throw baseEx(ERRMODULE_IOSTDIO, STDIOEX_BLOCK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		block[1] &= ~O_NONBLOCK;

		block[2] = fcntl(2, F_GETFL);
		if (block[2] == -1)
			throw baseEx(ERRMODULE_IOSTDIO, STDIOEX_BLOCK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		block[2] &= ~O_NONBLOCK;
	}

	if (fcntl(0, F_SETFL, block[0]) == 1)
		throw baseEx(ERRMODULE_IOSTDIO, STDIOEX_BLOCK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	if (fcntl(1, F_SETFL, block[1]) == 1)
		throw baseEx(ERRMODULE_IOSTDIO, STDIOEX_BLOCK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	if (fcntl(2, F_SETFL, block[2]) == 1)
		throw baseEx(ERRMODULE_IOSTDIO, STDIOEX_BLOCK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	blocked = flag;
}

//-------------------------------------------------------------------

void
stdio::_readStream(char * const a_void)
{
	memset(a_void, '\0', inSize);

	while (true)
	{
		if (fgets(a_void, inSize, stdin) == NULL)
		{
			if (errno == EINTR)
				continue;

			if (errno == EAGAIN)
				break;

			if (ferror(stdin) != 0)
				throw baseEx(ERRMODULE_IOSTDIO, STDIOEX__READSTREAM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}

		break;
	}
}

//-------------------------------------------------------------------

void
stdio::readStream(char * const a_void)
{
	raceHazardGuard pg(this);

#ifndef IOSTDIO_WO_XEXEC
	operType = STDIO_OPERATION_READSTREAM;
	performXExec(preExec);
#endif

	_readStream(a_void);

#ifndef IOSTDIO_WO_XEXEC
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
stdio::readStreamString(dodoString &a_str)
{
	raceHazardGuard pg(this);

#ifndef IOSTDIO_WO_XEXEC
	operType = STDIO_OPERATION_READSTREAMSTRING;
	performXExec(preExec);
#endif

	char *data = new char[inSize];

	try
	{
		_readStream(data);
	}
	catch (...)
	{
		delete [] data;

		throw;
	}

#ifndef IOSTDIO_WO_XEXEC
	collectedData.buffer = data;
	delete [] data;

	performXExec(postExec);

	a_str = collectedData.buffer;
	collectedData.buffer.clear();
#else
	a_str = data;
	delete [] data;
#endif
}

//-------------------------------------------------------------------

void
stdio::writeStreamString(const dodoString &a_buf)
{
	raceHazardGuard pg(this);

	unsigned long _outSize = outSize;

#ifndef IOSTDIO_WO_XEXEC
	collectedData.buffer = a_buf;

	operType = STDIO_OPERATION_WRITESTREAMSTRING;
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

#ifndef IOSTDIO_WO_XEXEC
	performXExec(postExec);

	collectedData.buffer.clear();
#endif
}

//-------------------------------------------------------------------

void
stdio::writeStream(const char *const a_buf)
{
	raceHazardGuard pg(this);

	unsigned long _outSize = outSize;

#ifndef IOSTDIO_WO_XEXEC
	collectedData.buffer = a_buf;

	operType = STDIO_OPERATION_WRITESTREAM;
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

#ifndef IOSTDIO_WO_XEXEC
	performXExec(postExec);

	collectedData.buffer.clear();
#endif
}

//-------------------------------------------------------------------
