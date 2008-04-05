/***************************************************************************
 *            ioSTD.cc
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

#include <libdodo/ioSTD.h>

using namespace dodo;

ioSTD::ioSTD() : inSTDBuffer(STD_INSIZE),
				 outSTDBuffer(STD_OUTSIZE),
				 err(false),
				 blocked(true),
				 desc(stdout)
{
}

//-------------------------------------------------------------------

ioSTD::ioSTD(ioSTD &fd)
{
}

//-------------------------------------------------------------------

ioSTD::~ioSTD()
{
}

//-------------------------------------------------------------------

int
ioSTD::getInDescriptor() const
{
	systemRaceHazardGuard pg(this);

	return fileno(stdin);
}

//-------------------------------------------------------------------

int
ioSTD::getOutDescriptor() const
{
	systemRaceHazardGuard pg(this);

	if (err)
		return fileno(stderr);
	else
		return fileno(stdout);
}

//-------------------------------------------------------------------

#ifndef IOSTD_WO_XEXEC

int
ioSTD::addPostExec(inExec func,
				   void   *data)
{
	return _addPostExec(func, (void *)&collectedData, XEXEC_OBJECT_IOSTD, data);
}

//-------------------------------------------------------------------

int
ioSTD::addPreExec(inExec func,
				  void   *data)
{
	return _addPreExec(func, (void *)&collectedData, XEXEC_OBJECT_IOSTD, data);
}

//-------------------------------------------------------------------

#ifdef DL_EXT

int
ioSTD::addPostExec(const dodoString &module,
				   void             *data,
				   void             *toInit)
{
	return _addPostExec(module, (void *)&collectedData, XEXEC_OBJECT_IOSTD, data, toInit);
}

//-------------------------------------------------------------------

__xexecCounts
ioSTD::addExec(const dodoString &module,
			   void             *data,
			   void             *toInit)
{
	return _addExec(module, (void *)&collectedData, XEXEC_OBJECT_IOSTD, data, toInit);
}

//-------------------------------------------------------------------

int
ioSTD::addPreExec(const dodoString &module,
				  void             *data,
				  void             *toInit)
{
	return _addPreExec(module, (void *)&collectedData, XEXEC_OBJECT_IOSTD, data, toInit);
}

#endif

#endif

//-------------------------------------------------------------------

void
ioSTD::_read(char * const a_void)
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
				if (errno == EINTR)
					continue;

				if (errno == EAGAIN)
					break;

				if (ferror(stdin) != 0)
					throw baseEx(ERRMODULE_IOSTD, IOSTDEX__READ, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
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
				if (errno == EINTR)
					continue;

				if (errno == EAGAIN)
					break;

				if (ferror(stdin) != 0)
					throw baseEx(ERRMODULE_IOSTD, IOSTDEX__READ, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			}

			break;
		}
	}
}

//-------------------------------------------------------------------

void
ioSTD::read(char * const a_void)
{
	systemRaceHazardGuard pg(this);

#ifndef IOSTD_WO_XEXEC
	operType = IOSTD_OPERATION_READ;
	performXExec(preExec);

	buffer.reserve(inSize);
#endif

#ifndef IOSTD_WO_XEXEC
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

#ifndef IOSTD_WO_XEXEC
	buffer.assign(a_void, inSize);

	performXExec(postExec);

	strncpy(a_void, buffer.c_str(), buffer.size() > inSize ? inSize : buffer.size());
	buffer.clear();
#endif
}

//-------------------------------------------------------------------

void
ioSTD::readString(dodoString &a_str)
{
	systemRaceHazardGuard pg(this);

#ifndef IOSTD_WO_XEXEC
	operType = IOSTD_OPERATION_READSTRING;
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

#ifndef IOSTD_WO_XEXEC
		buffer.clear();
#endif

		throw;
	}

#ifndef IOSTD_WO_XEXEC
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

//-------------------------------------------------------------------

void
ioSTD::writeString(const dodoString &a_buf)
{
	systemRaceHazardGuard pg(this);

#ifndef IOSTD_WO_XEXEC
	buffer = a_buf;

	operType = IOSTD_OPERATION_WRITESTRING;
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


#ifndef IOSTD_WO_XEXEC
	performXExec(postExec);

	buffer.clear();
#endif
}

//-------------------------------------------------------------------

void
ioSTD::write(const char *const a_buf)
{
	systemRaceHazardGuard pg(this);

#ifndef IOSTD_WO_XEXEC
	buffer.assign(a_buf, outSize);

	operType = IOSTD_OPERATION_WRITE;
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


#ifndef IOSTD_WO_XEXEC
	performXExec(postExec);

	buffer.clear();
#endif
}

//-------------------------------------------------------------------

void
ioSTD::_write(const char *const buf)
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
					throw baseEx(ERRMODULE_IOSTD, IOSTDEX__WRITE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
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
					throw baseEx(ERRMODULE_IOSTD, IOSTDEX__WRITE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			}

			break;
		}
	}
}

//-------------------------------------------------------------------

void
ioSTD::flush()
{
	systemRaceHazardGuard pg(this);

	if (fflush(desc) != 0)
		throw baseEx(ERRMODULE_IOSTD, IOSTDEX_FLUSH, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

__connInfo
ioSTD::inputterInfo()
{
	systemRaceHazardGuard pg(this);

	__connInfo info;

	struct sockaddr sa;

	socklen_t len = sizeof(sockaddr_in6);

	if (::getpeername(1, &sa, &len) == 1)
	{
		if (errno != ENOTSOCK)
			throw baseEx(ERRMODULE_IOSTD, IOSTDEX_INPUTTERINFO, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
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
ioSTD::isBlocked()
{
	systemRaceHazardGuard pg(this);

	return blocked;
}

//-------------------------------------------------------------------

void
ioSTD::redirectToSTDErr(bool toSTDErr)
{
	err = toSTDErr;

	if (err)
		desc = stderr;
	else
		desc = stdout;
}

//-------------------------------------------------------------------

bool
ioSTD::isRedirectedToSTDErr()
{
	return err;
}

//-------------------------------------------------------------------

void
ioSTD::block(bool flag)
{
	systemRaceHazardGuard pg(this);

	int block[3] = { O_NONBLOCK, O_NONBLOCK, O_NONBLOCK };

	if (flag)
	{
		block[0] = fcntl(0, F_GETFL);
		if (block[0] == -1)
			throw baseEx(ERRMODULE_IOSTD, IOSTDEX_BLOCK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		block[0] &= ~O_NONBLOCK;

		block[1] = fcntl(1, F_GETFL);
		if (block[1] == -1)
			throw baseEx(ERRMODULE_IOSTD, IOSTDEX_BLOCK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		block[1] &= ~O_NONBLOCK;

		block[2] = fcntl(2, F_GETFL);
		if (block[2] == -1)
			throw baseEx(ERRMODULE_IOSTD, IOSTDEX_BLOCK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		block[2] &= ~O_NONBLOCK;
	}

	if (fcntl(0, F_SETFL, block[0]) == 1)
		throw baseEx(ERRMODULE_IOSTD, IOSTDEX_BLOCK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	if (fcntl(1, F_SETFL, block[1]) == 1)
		throw baseEx(ERRMODULE_IOSTD, IOSTDEX_BLOCK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	if (fcntl(2, F_SETFL, block[2]) == 1)
		throw baseEx(ERRMODULE_IOSTD, IOSTDEX_BLOCK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	blocked = flag;
}

//-------------------------------------------------------------------

void
ioSTD::_readStream(char * const a_void)
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
				throw baseEx(ERRMODULE_IOSTD, IOSTDEX__READSTREAM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}

		break;
	}
}

//-------------------------------------------------------------------

void
ioSTD::readStream(char * const a_void)
{
	systemRaceHazardGuard pg(this);

#ifndef IOSTD_WO_XEXEC
	operType = IOSTD_OPERATION_READSTREAM;
	performXExec(preExec);
#endif

	_readStream(a_void);

#ifndef IOSTD_WO_XEXEC
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
ioSTD::readStreamString(dodoString &a_str)
{
	systemRaceHazardGuard pg(this);

#ifndef IOSTD_WO_XEXEC
	operType = IOSTD_OPERATION_READSTREAMSTRING;
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

#ifndef IOSTD_WO_XEXEC
	buffer = data;
	delete [] data;

	performXExec(postExec);

	a_str = buffer;
	buffer.clear();
#else
	a_str = data;
	delete [] data;
#endif
}

//-------------------------------------------------------------------

void
ioSTD::writeStreamString(const dodoString &a_buf)
{
	systemRaceHazardGuard pg(this);

	unsigned long _outSize = outSize;

#ifndef IOSTD_WO_XEXEC
	buffer = a_buf;

	operType = IOSTD_OPERATION_WRITESTREAMSTRING;
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

#ifndef IOSTD_WO_XEXEC
	performXExec(postExec);

	buffer.clear();
#endif
}

//-------------------------------------------------------------------

void
ioSTD::writeStream(const char *const a_buf)
{
	systemRaceHazardGuard pg(this);

	unsigned long _outSize = outSize;

#ifndef IOSTD_WO_XEXEC
	buffer = a_buf;

	operType = IOSTD_OPERATION_WRITESTREAM;
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

#ifndef IOSTD_WO_XEXEC
	performXExec(postExec);

	buffer.clear();
#endif
}

//-------------------------------------------------------------------
