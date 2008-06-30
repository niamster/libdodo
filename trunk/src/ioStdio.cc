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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/ioStdio.h>

using namespace dodo::io;

stdio::stdio() : inSTDBuffer(IOSTDIO_INSIZE),
				 outSTDBuffer(IOSTDIO_OUTSIZE),
				 err(false),
				 blocked(true),
				 desc(stdout)
{
#ifndef IO_WO_XEXEC

	execObject = XEXEC_OBJECT_IOSTDIO;
	execObjectData = (void *)&collectedData;

#endif
}

//-------------------------------------------------------------------

stdio::stdio(stdio &fd)
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

unsigned long
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

	return strlen(a_void);
}

//-------------------------------------------------------------------

void
stdio::_writeStream(const char * const data)
{
	_write(data);
}

//-------------------------------------------------------------------
