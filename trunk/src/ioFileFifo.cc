/***************************************************************************
 *            ioFileFifo.cc
 *
 *  Wed Oct 8 01:44:18 2005
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

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

#include "ioFile.inline"

#include <libdodo/ioFileFifo.h>
#include <libdodo/toolsFilesystem.h>
#include <libdodo/ioFileFifoEx.h>
#include <libdodo/types.h>
#include <libdodo/ioChannel.h>
#include <libdodo/pcSyncProtector.h>

using namespace dodo::io::file;

fifo::fifo(short protection) : inFileFifoBuffer(IOPIPE_INSIZE),
							   outFileFifoBuffer(IOPIPE_OUTSIZE),
							   blocked(true),
							   handle(new io::__file__),
							   channel(protection)
{
#ifndef IO_WO_XEXEC
	collectedData.setExecObject(XEXEC_OBJECT_IOFILEFIFO);
#endif
}

//-------------------------------------------------------------------

fifo::fifo(const dodoString &path,
		   short            mode,
		   short            protection) : inFileFifoBuffer(IOPIPE_INSIZE),
										  outFileFifoBuffer(IOPIPE_OUTSIZE),
										  blocked(true),
										  handle(new io::__file__),
										  path(path),
										  mode(mode),
										  channel(protection)
{
#ifndef IO_WO_XEXEC
	collectedData.setExecObject(XEXEC_OBJECT_IOFILEFIFO);
#endif

	if (path.size() != 0)
	{
		bool exists(false);
		struct stat st;

		if (::lstat(path.c_str(), &st) == -1)
		{
			if (errno != ENOENT)
			{
				delete handle;

				throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX_FIFO, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
			}
		}
		else
		{
			exists = true;
		}

		if (exists && !S_ISFIFO(st.st_mode))
		{
			delete handle;

			throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX_FIFO, exception::ERRNO_LIBDODO, FIFOEX_WRONGFILENAME, IOFILEFIFOEX_WRONGFILENAME_STR, __LINE__, __FILE__, path);
		}

		if (!exists)
		{
			try
			{
				tools::filesystem::mkfifo(path, DEFAULT_FILE_PERM);
			}
			catch (...)
			{
				delete handle;

				throw;
			}
		}

		switch (mode)
		{
			case FIFO_OPENMODE_WRITE:

				handle->file = fopen(path.c_str(), "w");

				break;

			case FIFO_OPENMODE_READ_OPENNONBLOCK:
			{
#ifdef O_LARGEFILE
				int fd = ::open(path.c_str(), O_NONBLOCK | O_RDONLY | O_LARGEFILE);
#else
				int fd = ::open(path.c_str(), O_NONBLOCK | O_RDONLY);
#endif

				int blockFlag = fcntl(fd, F_GETFL);
				if (blockFlag == -1)
				{
					delete handle;

					throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX_FIFO, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
				}

				blockFlag &= ~O_NONBLOCK;

				if (fcntl(fd, F_SETFL, blockFlag) == -1)
				{
					delete handle;

					throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX_FIFO, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
				}

				handle->file = fdopen(fd, "r");

				break;
			}

			case FIFO_OPENMODE_READ:
			default:

				handle->file = fopen(path.c_str(), "r");
		}
	}

	if (handle->file == NULL)
	{
		delete handle;

		throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX_FIFO, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
	}
}

//-------------------------------------------------------------------

fifo::fifo(const fifo &fd) : inFileFifoBuffer(fd.inFileFifoBuffer),
							 outFileFifoBuffer(fd.outFileFifoBuffer),
							 blocked(fd.blocked),
							 mode(fd.mode),
							 path(fd.path),
							 handle(new __file__),
							 channel(fd.protection)
{
#ifndef IO_WO_XEXEC
	collectedData.setExecObject(XEXEC_OBJECT_IOFILEFIFO);
#endif

	inSize = fd.inSize;
	outSize = fd.outSize;

	if (fd.handle->file != NULL)
	{
		int oldDesc, newDesc;

		oldDesc = fileno(fd.handle->file);
		if (oldDesc == -1)
		{
			delete handle;

			throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX_FIFO, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}

		newDesc = dup(oldDesc);
		if (newDesc == -1)
		{
			delete handle;

			throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX_FIFO, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}

		switch (mode)
		{
			case FIFO_OPENMODE_WRITE:

				handle->file = fdopen(newDesc, "w");

				break;

			case FIFO_OPENMODE_READ:
			default:

				handle->file = fdopen(newDesc, "r");
		}

		if (handle->file == NULL)
		{
			delete handle;

			throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX_FIFO, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}
	}
}

//-------------------------------------------------------------------

fifo::~fifo()
{
	if (handle->file != NULL)
	{
		fclose(handle->file);
	}

	delete handle;
}

//-------------------------------------------------------------------

int
fifo::getInDescriptor() const
{
	pc::sync::protector pg(keeper);

	if (handle->file == NULL)
	{
		throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX_GETINDESCRIPTOR, exception::ERRNO_LIBDODO, FIFOEX_NOTOPENED, IOFILEFIFOEX_NOTOPENED_STR, __LINE__, __FILE__, path);
	}

	return fileno(handle->file);
}

//-------------------------------------------------------------------

int
fifo::getOutDescriptor() const
{
	pc::sync::protector pg(keeper);

	if (handle->file == NULL)
	{
		throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX_GETOUTDESCRIPTOR, exception::ERRNO_LIBDODO, FIFOEX_NOTOPENED, IOFILEFIFOEX_NOTOPENED_STR, __LINE__, __FILE__, path);
	}

	return fileno(handle->file);
}

//-------------------------------------------------------------------

void
fifo::clone(const fifo &fd)
{
	pc::sync::protector pg(keeper);

	if (handle->file != NULL)
	{
		if (fclose(handle->file) != 0)
		{
			throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX_CLONE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}

		handle->file = NULL;
	}

	inFileFifoBuffer = fd.inFileFifoBuffer;
	outFileFifoBuffer = fd.outFileFifoBuffer;
	blocked = fd.blocked;
	path = fd.path;
	mode = fd.mode;
	inSize = fd.inSize;
	outSize = fd.outSize;

	if (fd.handle->file != NULL)
	{
		int oldDesc, newDesc;

		oldDesc = fileno(fd.handle->file);
		if (oldDesc == -1)
		{
			throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX_CLONE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}

		newDesc = dup(oldDesc);
		if (newDesc == -1)
		{
			throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX_CLONE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}

		switch (mode)
		{
			case FIFO_OPENMODE_WRITE:

				handle->file = fdopen(newDesc, "w");

				break;

			case FIFO_OPENMODE_READ:
			default:

				handle->file = fdopen(newDesc, "r");
		}

		if (handle->file == NULL)
		{
			throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX_CLONE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}
	}
}

//-------------------------------------------------------------------

void
fifo::close()
{
	pc::sync::protector pg(keeper);

#ifndef IO_WO_XEXEC
	operType = FIFO_OPERATION_CLOSE;
	performXExec(preExec);
#endif

	if (handle->file != NULL)
	{
		if (fclose(handle->file) != 0)
		{
			throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX_CLOSE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
		}

		handle->file = NULL;
	}

#ifndef IO_WO_XEXEC
	performXExec(postExec);
#endif
}

//-------------------------------------------------------------------

void
fifo::open(const dodoString &a_path,
		   short            a_mode)
{
	pc::sync::protector pg(keeper);

#ifndef IO_WO_XEXEC
	operType = FIFO_OPERATION_OPEN;
	performXExec(preExec);
#endif

	path = a_path;
	mode = a_mode;

	if (handle->file != NULL)
	{
		if (fclose(handle->file) != 0)
		{
			throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX_OPEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
		}

		handle->file = NULL;
	}

	if (path.size() == 0)
	{
		throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX_OPEN, exception::ERRNO_LIBDODO, FIFOEX_WRONGFILENAME, IOFILEFIFOEX_WRONGFILENAME_STR, __LINE__, __FILE__, path);
	}
	else
	{
		bool exists(false);
		struct stat st;

		if (::lstat(path.c_str(), &st) == -1)
		{
			if (errno != ENOENT)
			{
				throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX_OPEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
			}
		}
		else
		{
			exists = true;
		}

		if (exists && !S_ISFIFO(st.st_mode))
		{
			throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX_OPEN, exception::ERRNO_LIBDODO, FIFOEX_WRONGFILENAME, IOFILEFIFOEX_WRONGFILENAME_STR, __LINE__, __FILE__, path);
		}

		if (!exists)
		{
			tools::filesystem::mkfifo(path, DEFAULT_FILE_PERM);
		}

		switch (mode)
		{
			case FIFO_OPENMODE_WRITE:

				handle->file = fopen(path.c_str(), "w");

				break;

			case FIFO_OPENMODE_READ_OPENNONBLOCK:
			{
#ifdef O_LARGEFILE
				int fd = ::open(path.c_str(), O_NONBLOCK | O_RDONLY | O_LARGEFILE);
#else
				int fd = ::open(path.c_str(), O_NONBLOCK | O_RDONLY);
#endif

				int blockFlag = fcntl(fd, F_GETFL);
				if (blockFlag == -1)
				{
					throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX_OPEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
				}

				blockFlag &= ~O_NONBLOCK;

				if (fcntl(fd, F_SETFL, blockFlag) == -1)
				{
					throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX_OPEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
				}

				handle->file = fdopen(fd, "r");

				break;
			}

			case FIFO_OPENMODE_READ:
			default:

				handle->file = fopen(path.c_str(), "r");
		}
	}

	if (handle->file == NULL)
	{
		throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX_OPEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
	}

#ifndef IO_WO_XEXEC
	performXExec(postExec);
#endif
}

//-------------------------------------------------------------------

bool
fifo::isBlocked()
{
	pc::sync::protector pg(keeper);

	return blocked;
}

//-------------------------------------------------------------------

void
fifo::block(bool flag)
{
	pc::sync::protector pg(keeper);

	if (handle->file == NULL)
	{
		throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX_BLOCK, exception::ERRNO_LIBDODO, FIFOEX_NOTOPENED, IOFILEFIFOEX_NOTOPENED_STR, __LINE__, __FILE__);
	}

	if (blocked == flag)
	{
		return;
	}

	int blockFlag;

	int desc = fileno(handle->file);
	if (desc == -1)
	{
		throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX_BLOCK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	blockFlag = fcntl(desc, F_GETFL);
	if (blockFlag == -1)
	{
		throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX_BLOCK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	if (flag)
	{
		blockFlag &= ~O_NONBLOCK;
	}
	else
	{
		blockFlag |= O_NONBLOCK;
	}

	if (fcntl(desc, F_SETFL, blockFlag) == -1)
	{
		throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX_BLOCK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	blocked = flag;
}

//-------------------------------------------------------------------

void
fifo::_read(char * const a_data)
{
	if (handle->file == NULL)
	{
		throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX__READ__, exception::ERRNO_LIBDODO, FIFOEX_NOTOPENED, IOFILEFIFOEX_NOTOPENED_STR, __LINE__, __FILE__, path);
	}

	char *data = a_data;

	memset(a_data, '\0', inSize);

	unsigned long iter = inSize / inFileFifoBuffer;
	unsigned long rest = inSize % inFileFifoBuffer;

	unsigned long batch, n;

	for (unsigned long i = 0; i < iter; ++i)
	{
		batch = inFileFifoBuffer;
		while (batch > 0)
		{
			while (true)
			{
				if ((n = fread(data, 1, batch, handle->file)) == 0)
				{
					if (feof(handle->file) != 0 || errno == EAGAIN)
					{
						break;
					}

					if (errno == EINTR)
					{
						continue;
					}

					if (ferror(handle->file) != 0)
					{
						throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX__READ__, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
					}
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
				if ((n = fread(data, 1, batch, handle->file)) == 0)
				{
					if (feof(handle->file) != 0 || errno == EAGAIN)
					{
						break;
					}

					if (errno == EINTR)
					{
						continue;
					}

					if (ferror(handle->file) != 0)
					{
						throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX__READ__, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
					}
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
fifo::_write(const char *const a_data)
{
	if (handle->file == NULL)
	{
		throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX__WRITE__, exception::ERRNO_LIBDODO, FIFOEX_NOTOPENED, IOFILEFIFOEX_NOTOPENED_STR, __LINE__, __FILE__, path);
	}

	const char *data = a_data;

	unsigned long iter = outSize / outFileFifoBuffer;
	unsigned long rest = outSize % outFileFifoBuffer;

	unsigned long batch, n;

	for (unsigned long i = 0; i < iter; ++i)
	{
		batch = outFileFifoBuffer;
		while (batch > 0)
		{
			while (true)
			{
				if ((n = fwrite(data, 1, batch, handle->file)) == 0)
				{
					if (errno == EINTR)
					{
						continue;
					}

					if (errno == EAGAIN)
					{
						break;
					}

					if (ferror(handle->file) != 0)
					{
						throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX__WRITE__, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
					}
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
				if ((n = fwrite(data, 1, batch, handle->file)) == 0)
				{
					if (errno == EINTR)
					{
						continue;
					}

					if (errno == EAGAIN)
					{
						break;
					}

					if (ferror(handle->file) != 0)
					{
						throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX__WRITE__, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
					}
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
fifo::flush()
{
	pc::sync::protector pg(keeper);

	if (handle->file == NULL)
	{
		throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX_FLUSH, exception::ERRNO_LIBDODO, FIFOEX_NOTOPENED, IOFILEFIFOEX_NOTOPENED_STR, __LINE__, __FILE__, path);
	}

	if (fflush(handle->file) != 0)
	{
		throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX_FLUSH, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
	}
}

//-------------------------------------------------------------------

unsigned long
fifo::_readStream(char * const a_data)
{
	if (handle->file == NULL)
	{
		throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX__READSTREAM__, exception::ERRNO_LIBDODO, FIFOEX_NOTOPENED, IOFILEFIFOEX_NOTOPENED_STR, __LINE__, __FILE__, path);
	}

	unsigned long readSize = inSize + 1;

	memset(a_data, '\0', readSize);

	while (true)
	{
		if (fgets(a_data, readSize, handle->file) == NULL)
		{
			if (errno == EINTR)
			{
				continue;
			}

			if (errno == EAGAIN)
			{
				break;
			}

			if (ferror(handle->file) != 0)
			{
				throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX__READSTREAM__, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			}
		}

		break;
	}

	return strlen(a_data);
}

//-------------------------------------------------------------------

void
fifo::_writeStream(const char *const a_data)
{
	unsigned long _outSize = outSize;

	try
	{
		unsigned int bufSize = strlen(a_data);

		if (bufSize < outSize)
		{
			outSize = bufSize;
		}

		_write(a_data);

		while (true)
		{
			if (fputc('\n', handle->file) == EOF)
			{
				if (errno == EINTR)
				{
					continue;
				}

				if (errno == EAGAIN)
				{
					break;
				}

				if (ferror(handle->file) != 0)
				{
					throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX__WRITESTREAM__, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
				}
			}

			break;
		}

		outSize = _outSize;
	}
	catch (...)
	{
		outSize = _outSize;

		throw;
	}
}

//-------------------------------------------------------------------
