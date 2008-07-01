/***************************************************************************
 *            ioFifo.cc
 *
 *  Tue Jul 1 15:31:57 2008
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

#include <libdodo/ioFifo.h>

using namespace dodo::io;

fifo::fifo() : inFifoBuffer(IOFIFO_INSIZE),
				 outFifoBuffer(IOFIFO_OUTSIZE),
				 blocked(true),
				 inHandle(NULL),
				 outHandle(NULL)
{
#ifndef IO_WO_XEXEC

	execObject = XEXEC_OBJECT_IOFIFO;
	execObjectData = (void *)&collectedData;

#endif
}

//-------------------------------------------------------------------

fifo::fifo(fifo &fd) : inFifoBuffer(fd.inFifoBuffer),
					   outFifoBuffer(fd.outFifoBuffer),
				 	   blocked(fd.blocked),
					   inHandle(NULL),
					   outHandle(NULL)
{
#ifndef IO_WO_XEXEC

	execObject = XEXEC_OBJECT_IOFIFO;
	execObjectData = (void *)&collectedData;

#endif

	opened = fd.opened;

	if (opened)
	{
		int oldDesc, newDesc;

		oldDesc = fileno(fd.inHandle);
		if (oldDesc != -1)
		{
			newDesc = dup(oldDesc);
			if (newDesc != -1)
				inHandle = fdopen(newDesc, "r");
		}
		
		oldDesc = fileno(fd.outHandle);
		if (oldDesc != -1)
		{
			newDesc = dup(oldDesc);
			if (newDesc != -1)
				outHandle = fdopen(newDesc, "w");
		}

		if (inHandle == NULL || outHandle == NULL)
			opened = false;
	}
}

//-------------------------------------------------------------------

fifo::~fifo()
{
	if (opened)
	{
		fclose(inHandle);

		fclose(outHandle);
	}
}

//-------------------------------------------------------------------

int
fifo::getInDescriptor() const
{
	raceHazardGuard pg(this);

	if (!opened)
		throw baseEx(ERRMODULE_IOFILE, FIFOEX_GETINDESCRIPTOR, ERR_LIBDODO, FIFOEX_FIFONOTOPENED, IOFIFOEX_FIFONOTOPENED_STR, __LINE__, __FILE__);

	return fileno(inHandle);
}

//-------------------------------------------------------------------

int
fifo::getOutDescriptor() const
{
	raceHazardGuard pg(this);
	
	if (!opened)
		throw baseEx(ERRMODULE_IOFILE, FIFOEX_GETOUTDESCRIPTOR, ERR_LIBDODO, FIFOEX_FIFONOTOPENED, IOFIFOEX_FIFONOTOPENED_STR, __LINE__, __FILE__);


	return fileno(outHandle);
}

//-------------------------------------------------------------------

void
fifo::close()
{
	raceHazardGuard pg(this);

#ifndef IO_WO_XEXEC
	operType = FIFO_OPERATION_CLOSE;
	performXExec(preExec);
#endif
	
	if (opened)
	{
		if (fclose(inHandle) != 0)
			throw baseEx(ERRMODULE_IOFIFO, FIFOEX_CLOSE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		if (fclose(outHandle) != 0)
			throw baseEx(ERRMODULE_IOFIFO, FIFOEX_CLOSE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		opened = false;
	}

#ifndef IO_WO_XEXEC
	performXExec(postExec);
#endif
}

//-------------------------------------------------------------------

void
fifo::open()
{
	raceHazardGuard pg(this);

#ifndef IO_WO_XEXEC
	operType = FIFO_OPERATION_OPEN;
	performXExec(preExec);
#endif
	
	if (opened)
	{
		if (fclose(inHandle) != 0)
			throw baseEx(ERRMODULE_IOFIFO, FIFOEX_OPEN, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		if (fclose(outHandle) != 0)
			throw baseEx(ERRMODULE_IOFIFO, FIFOEX_OPEN, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		opened = false;
	}

	int pipefd[2];

	if (pipe(pipefd) != 0)
		throw baseEx(ERRMODULE_IOFIFO, FIFOEX_OPEN, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	inHandle = fdopen(pipefd[0], "r");
	if (inHandle == NULL)
		throw baseEx(ERRMODULE_IOFIFO, FIFOEX_OPEN, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	outHandle = fdopen(pipefd[1], "w");
	if (outHandle == NULL)
		throw baseEx(ERRMODULE_IOFIFO, FIFOEX_OPEN, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	opened = true;

#ifndef IO_WO_XEXEC
	performXExec(postExec);
#endif
}

//-------------------------------------------------------------------

void
fifo::_read(char * const a_void)
{
	if (!opened)
		throw baseEx(ERRMODULE_IOFILE, FIFOEX__READ, ERR_LIBDODO, FIFOEX_FIFONOTOPENED, IOFIFOEX_FIFONOTOPENED_STR, __LINE__, __FILE__);

	char *data = a_void;

	memset(a_void, '\0', inSize);

	unsigned long iter = inSize / inFifoBuffer;
	unsigned long rest = inSize % inFifoBuffer;

	for (unsigned long i = 0; i < iter; ++i)
	{
		while (true)
		{
			if (fread(data, inFifoBuffer, 1, inHandle) == 0)
			{
				if (feof(inHandle) != 0 || errno == EAGAIN)
					break;

				if (errno == EINTR)
					continue;

				if (ferror(inHandle) != 0)
					throw baseEx(ERRMODULE_IOFIFO, FIFOEX__READ, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			}

			break;
		}

		data += inFifoBuffer;
	}

	if (rest > 0)
	{
		while (true)
		{
			if (fread(data, rest, 1, inHandle) == 0)
			{
				if (feof(inHandle) != 0 || errno == EAGAIN)
					break;

				if (errno == EINTR)
					continue;

				if (ferror(inHandle) != 0)
					throw baseEx(ERRMODULE_IOFIFO, FIFOEX__READ, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			}

			break;
		}
	}
}

//-------------------------------------------------------------------

void
fifo::_write(const char *const buf)
{
	if (!opened)
		throw baseEx(ERRMODULE_IOFILE, FIFOEX__WRITE, ERR_LIBDODO, FIFOEX_FIFONOTOPENED, IOFIFOEX_FIFONOTOPENED_STR, __LINE__, __FILE__);

	char *data = (char *)buf;

	unsigned long iter = outSize / outFifoBuffer;
	unsigned long rest = outSize % outFifoBuffer;

	for (unsigned long i = 0; i < iter; ++i)
	{
		while (true)
		{
			if (fwrite(data, outFifoBuffer, 1, outHandle) == 0)
			{
				if (errno == EINTR)
					continue;

				if (errno == EAGAIN)
					break;

				if (ferror(outHandle) != 0)
					throw baseEx(ERRMODULE_IOFIFO, FIFOEX__WRITE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			}

			break;
		}

		data += outFifoBuffer;
	}

	if (rest > 0)
	{
		while (true)
		{
			if (fwrite(data, 1, rest, outHandle) == 0)
			{
				if (errno == EINTR)
					continue;

				if (errno == EAGAIN)
					break;

				if (ferror(outHandle) != 0)
					throw baseEx(ERRMODULE_IOFIFO, FIFOEX__WRITE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			}

			break;
		}
	}
}

//-------------------------------------------------------------------

void
fifo::flush()
{
	raceHazardGuard pg(this);
	
	if (!opened)
		throw baseEx(ERRMODULE_IOFILE, FIFOEX_FLUSH, ERR_LIBDODO, FIFOEX_FIFONOTOPENED, IOFIFOEX_FIFONOTOPENED_STR, __LINE__, __FILE__);

	if (fflush(outHandle) != 0)
		throw baseEx(ERRMODULE_IOFIFO, FIFOEX_FLUSH, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

network::__peerInfo
fifo::peerInfo()
{
	raceHazardGuard pg(this);
	
	if (!opened)
		throw baseEx(ERRMODULE_IOFILE, FIFOEX_PEERINFO, ERR_LIBDODO, FIFOEX_FIFONOTOPENED, IOFIFOEX_FIFONOTOPENED_STR, __LINE__, __FILE__);

	network::__peerInfo info;

	struct sockaddr sa;

	socklen_t len = sizeof(sockaddr_in6);

	int desc = fileno(inHandle);
	if (desc == -1)
			throw baseEx(ERRMODULE_IOFIFO, FIFOEX_PEERINFO, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	if (::getpeername(desc, &sa, &len) == 1)
	{
		if (errno != ENOTSOCK)
			throw baseEx(ERRMODULE_IOFIFO, FIFOEX_PEERINFO, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
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
fifo::isBlocked()
{
	raceHazardGuard pg(this);

	return blocked;
}


//-------------------------------------------------------------------

void
fifo::block(bool flag)
{
	raceHazardGuard pg(this);

	if (!opened)
		throw baseEx(ERRMODULE_IOFILE, FIFOEX_BLOCK, ERR_LIBDODO, FIFOEX_FIFONOTOPENED, IOFIFOEX_FIFONOTOPENED_STR, __LINE__, __FILE__);

	int block[3] = { O_NONBLOCK, O_NONBLOCK, O_NONBLOCK };

	if (flag)
	{
		block[0] = fcntl(0, F_GETFL);
		if (block[0] == -1)
			throw baseEx(ERRMODULE_IOFIFO, FIFOEX_BLOCK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		block[0] &= ~O_NONBLOCK;

		block[1] = fcntl(1, F_GETFL);
		if (block[1] == -1)
			throw baseEx(ERRMODULE_IOFIFO, FIFOEX_BLOCK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		block[1] &= ~O_NONBLOCK;

		block[2] = fcntl(2, F_GETFL);
		if (block[2] == -1)
			throw baseEx(ERRMODULE_IOFIFO, FIFOEX_BLOCK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		block[2] &= ~O_NONBLOCK;
	}
	
	int inDesc = fileno(inHandle);
	if (inDesc == -1)
			throw baseEx(ERRMODULE_IOFIFO, FIFOEX_BLOCK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	
	int outDesc = fileno(outHandle);
	if (outDesc == -1)
			throw baseEx(ERRMODULE_IOFIFO, FIFOEX_BLOCK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	if (fcntl(inDesc, F_SETFL, block[0]) == 1)
		throw baseEx(ERRMODULE_IOFIFO, FIFOEX_BLOCK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	if (fcntl(outDesc, F_SETFL, block[1]) == 1)
		throw baseEx(ERRMODULE_IOFIFO, FIFOEX_BLOCK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	blocked = flag;
}

//-------------------------------------------------------------------

unsigned long
fifo::_readStream(char * const a_void)
{
	if (!opened)
		throw baseEx(ERRMODULE_IOFILE, FIFOEX__READSTREAM, ERR_LIBDODO, FIFOEX_FIFONOTOPENED, IOFIFOEX_FIFONOTOPENED_STR, __LINE__, __FILE__);

	memset(a_void, '\0', inSize);

	while (true)
	{
		if (fgets(a_void, inSize, inHandle) == NULL)
		{
			if (errno == EINTR)
				continue;

			if (errno == EAGAIN)
				break;

			if (ferror(inHandle) != 0)
				throw baseEx(ERRMODULE_IOFIFO, FIFOEX__READSTREAM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}

		break;
	}

	return strlen(a_void);
}

//-------------------------------------------------------------------

void
fifo::_writeStream(const char * const data)
{
	_write(data);
}

//-------------------------------------------------------------------

