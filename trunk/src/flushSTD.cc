/***************************************************************************
 *            flushSTD.cc
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

#include <libdodo/flushSTD.h>

using namespace dodo;

flushSTD::flushSTD() : inSTDBuffer(STD_INSIZE),
					   outSTDBuffer(STD_OUTSIZE),
					   err(false),
					   blocked(true)
{
}

//-------------------------------------------------------------------

flushSTD::flushSTD(flushSTD &fd)
{
}

//-------------------------------------------------------------------

flushSTD::~flushSTD()
{
}

//-------------------------------------------------------------------

int
flushSTD::getInDescriptor() const
{
	return fileno(stdin);
}

//-------------------------------------------------------------------

int
flushSTD::getOutDescriptor() const
{
	if (err)
		return fileno(stderr);
	else
		return fileno(stdout);
}

//-------------------------------------------------------------------

#ifndef FLUSH_STD_WO_XEXEC

int
flushSTD::addPostExec(inExec func,
					  void   *data)
{
	return _addPostExec(func, (void *)this, XEXECOBJ_FLUSHSTD, data);
}

//-------------------------------------------------------------------

int
flushSTD::addPreExec(inExec func,
					 void   *data)
{
	return _addPreExec(func, (void *)this, XEXECOBJ_FLUSHSTD, data);
}

//-------------------------------------------------------------------

    #ifdef DL_EXT

int
flushSTD::addPostExec(const dodoString &module,
					  void             *data,
					  void             *toInit)
{
	return _addPostExec(module, (void *)this, XEXECOBJ_FLUSHSTD, data, toInit);
}

//-------------------------------------------------------------------

xexecCounts
flushSTD::addExec(const dodoString &module,
				  void             *data,
				  void             *toInit)
{
	return _addExec(module, (void *)this, XEXECOBJ_FLUSHSTD, data, toInit);
}

//-------------------------------------------------------------------

int
flushSTD::addPreExec(const dodoString &module,
					 void             *data,
					 void             *toInit)
{
	return _addPreExec(module, (void *)this, XEXECOBJ_FLUSHSTD, data, toInit);
}

    #endif

#endif

//-------------------------------------------------------------------

void
flushSTD::read(char * const a_void)
{
    #ifndef FLUSH_STD_WO_XEXEC
	operType = FLUSHSTD_OPER_READ;
	performXExec(preExec);
    #endif

	memset(a_void, '\0', inSize);

	unsigned long iter = inSize / inSTDBuffer;
	unsigned long rest = inSize % inSTDBuffer;

	unsigned long sent_received = 0;

	for (unsigned long i = 0; i < iter; ++i)
	{
		while (true)
		{
			if (fread(a_void + sent_received, inSTDBuffer, 1, stdin) == 0)
			{
				if (errno == EINTR)
					continue;

				if (ferror(stdin) != 0)
					throw baseEx(ERRMODULE_FLUSHSTD, FLUSHSTD_READ, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			}

			break;
		}

		sent_received += inSTDBuffer;
	}

	if (rest > 0)
	{
		while (true)
		{
			if (fread(a_void + sent_received, rest, 1, stdin) == 0)
			{
				if (errno == EINTR)
					continue;

				if (ferror(stdin) != 0)
					throw baseEx(ERRMODULE_FLUSHSTD, FLUSHSTD_READ, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			}

			break;
		}
	}

	buffer.assign(a_void, inSize);

    #ifndef FLUSH_STD_WO_XEXEC
	performXExec(postExec);
    #endif
}

//-------------------------------------------------------------------

void
flushSTD::readString(dodoString &a_str)
{
	char *data = new char[inSize + 1];

	try
	{

		this->read(data);
	}
	catch (...)
	{
		a_str.assign(data, inSize);
		delete [] data;

		throw baseEx(ERRMODULE_FLUSHSTD, FLUSHSTD_READSTRING, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	a_str.assign(data, inSize);
	delete [] data;
}

//-------------------------------------------------------------------

void
flushSTD::writeString(const dodoString &a_buf)
{
	this->write(a_buf.c_str());
}

//-------------------------------------------------------------------

void
flushSTD::write(const char *const aa_buf)
{
	buffer.assign(aa_buf, outSize);

    #ifndef FLUSH_STD_WO_XEXEC
	operType = FLUSHSTD_OPER_WRITE;
	performXExec(preExec);
    #endif

	unsigned long iter = outSize / outSTDBuffer;
	unsigned long rest = outSize % outSTDBuffer;

	unsigned long sent_received = 0;

	desc = stdout;
	if (err)
		desc = stderr;

	for (unsigned long i = 0; i < iter; ++i)
	{
		while (true)
		{
			if (fwrite(buffer.c_str() + sent_received, outSTDBuffer, 1, desc) == 0)
			{
				if (errno == EINTR)
					continue;

				if (ferror(desc) != 0)
					throw baseEx(ERRMODULE_FLUSHSTD, FLUSHSTD_WRITE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			}

			break;
		}

		sent_received += outSTDBuffer;
	}

	if (rest > 0)
	{
		while (true)
		{
			if (fwrite(buffer.c_str() + sent_received, rest, 1, desc) == 0)
			{
				if (errno == EINTR)
					continue;

				if (ferror(desc) != 0)
					throw baseEx(ERRMODULE_FLUSHSTD, FLUSHSTD_WRITE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			}

			break;
		}
	}

    #ifndef FLUSH_STD_WO_XEXEC
	performXExec(postExec);
    #endif
}

//-------------------------------------------------------------------

void
flushSTD::flush()
{
	desc = stdout;
	if (err)
		desc = stderr;

	if (fflush(desc) != 0)
		throw baseEx(ERRMODULE_FLUSHSTD, FLUSHSTD_FLUSH, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

__connInfo
flushSTD::inputterInfo()
{
	__connInfo info;

	struct sockaddr sa;

	socklen_t len = sizeof(sockaddr_in6);

	if (::getpeername(1, &sa, &len) == 1)
	{
		if (errno != ENOTSOCK)
			throw baseEx(ERRMODULE_FLUSHSTD, FLUSHSTD_INPUTTERINFO, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
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
flushSTD::isBlocked() const
{
	return blocked;
}

//-------------------------------------------------------------------

void
flushSTD::block(bool flag)
{
	int block[3] = { O_NONBLOCK, O_NONBLOCK, O_NONBLOCK };

	if (flag)
	{
		block[0] = fcntl(0, F_GETFL);
		if (block[0] == -1)
			throw baseEx(ERRMODULE_FLUSHSTD, FLUSHSTD_BLOCK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		block[0] &= ~O_NONBLOCK;

		block[1] = fcntl(1, F_GETFL);
		if (block[1] == -1)
			throw baseEx(ERRMODULE_FLUSHSTD, FLUSHSTD_BLOCK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		block[1] &= ~O_NONBLOCK;

		block[2] = fcntl(2, F_GETFL);
		if (block[2] == -1)
			throw baseEx(ERRMODULE_FLUSHSTD, FLUSHSTD_BLOCK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		block[2] &= ~O_NONBLOCK;
	}

	if (fcntl(0, F_SETFL, block[0]) == 1)
		throw baseEx(ERRMODULE_FLUSHSTD, FLUSHSTD_BLOCK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	if (fcntl(1, F_SETFL, block[1]) == 1)
		throw baseEx(ERRMODULE_FLUSHSTD, FLUSHSTD_BLOCK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	if (fcntl(2, F_SETFL, block[2]) == 1)
		throw baseEx(ERRMODULE_FLUSHSTD, FLUSHSTD_BLOCK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	blocked = flag;
}

//-------------------------------------------------------------------

void
flushSTD::readStream(char * const a_void)
{
    #ifndef FLUSH_STD_WO_XEXEC
	operType = FLUSHSTD_OPER_READSTREAM;
	performXExec(preExec);
    #endif

	memset(a_void, '\0', inSTDBuffer);

	while (true)
	{
		if (fgets(a_void, inSTDBuffer + 1, stdin) == NULL)
		{
			if (errno == EINTR)
				continue;

			if (ferror(stdin) != 0)
				throw baseEx(ERRMODULE_FLUSHSTD, FLUSHSTD_READSTREAM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}

		break;
	}

	buffer.assign(a_void);

    #ifndef FLUSH_STD_WO_XEXEC
	performXExec(postExec);
    #endif
}

//-------------------------------------------------------------------

void
flushSTD::readStreamString(dodoString &a_str)
{
	char *data = new char[inSTDBuffer + 1];

	try
	{

		this->readStream(data);
	}
	catch (...)
	{
		a_str.assign(data);
		delete [] data;

		throw baseEx(ERRMODULE_FLUSHSTD, FLUSHSTD_READSTREAMSTRING, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	a_str.assign(data);
	delete [] data;
}

//-------------------------------------------------------------------

void
flushSTD::writeStreamString(const dodoString &a_buf)
{
	this->writeStream(a_buf.c_str());
}

//-------------------------------------------------------------------

void
flushSTD::writeStream(const char *const aa_buf)
{
	buffer.assign(aa_buf);

    #ifndef FLUSH_STD_WO_XEXEC
	operType = FLUSHSTD_OPER_WRITESTREAM;
	performXExec(preExec);
    #endif

	desc = stdout;
	if (err)
		desc = stderr;

	buffer.append(1, '\n');

	unsigned long outSize = strlen(aa_buf);

	unsigned long iter = outSize / outSTDBuffer;
	unsigned long rest = outSize % outSTDBuffer;

	unsigned long sent_received = 0;

	desc = stdout;
	if (err)
		desc = stderr;

	char *buff = new char[outSTDBuffer + 1];

	for (unsigned long i = 0; i < iter; ++i)
	{
		strncpy(buff, buffer.c_str() + sent_received, outSTDBuffer);
		buff[outSTDBuffer] = '\0';

		while (true)
		{
			if (fputs(buff, desc) == 0)
			{
				if (errno == EINTR)
					continue;

				if (ferror(desc) != 0)
				{
					delete [] buff;

					throw baseEx(ERRMODULE_FLUSHSTD, FLUSHSTD_WRITESTREAM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
				}
			}

			break;
		}

		sent_received += outSTDBuffer;
	}

	if (rest > 0)
	{
		strncpy(buff, buffer.c_str() + sent_received, rest);
		buff[rest] = '\0';

		while (true)
		{
			if (fputs(buff, desc) == 0)
			{
				if (errno == EINTR)
					continue;

				if (ferror(desc) != 0)
				{
					delete [] buff;

					throw baseEx(ERRMODULE_FLUSHSTD, FLUSHSTD_WRITESTREAM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
				}
			}

			break;
		}
	}

	delete [] buff;

    #ifndef FLUSH_STD_WO_XEXEC
	performXExec(postExec);
    #endif
}

//-------------------------------------------------------------------
