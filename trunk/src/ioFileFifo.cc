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

#include <libdodo/ioFileFifo.h>

using namespace dodo::io::file;

fifo::fifo() : handler(NULL)
{
#ifndef IO_WO_XEXEC

	collectedData.executor = (void *)this;
	execObject = XEXEC_OBJECT_IOFILEFIFO;

#endif
}

//-------------------------------------------------------------------

fifo::fifo(const fifo &fd) : mode(fd.mode),
							 path(fd.path),
							 handler(NULL)

{
#ifndef IO_WO_XEXEC

	collectedData.executor = (void *)this;
	execObject = XEXEC_OBJECT_IOFILEFIFO;

#endif

	inSize = fd.inSize;
	outSize = fd.outSize;

	if (fd.opened)
	{
		int oldDesc, newDesc;

		oldDesc = fileno(fd.handler);

		newDesc = dup(oldDesc);

		switch (mode)
		{
			case FIFO_OPENMODE_WRITE:

				handler = fdopen(newDesc, "w");

				break;

			case FIFO_OPENMODE_READ:
			default:

				handler = fdopen(newDesc, "r");
		}

		if (handler != NULL)
			opened = true;
	}
}

//-------------------------------------------------------------------

fifo::~fifo()
{
	if (opened)
		fclose(handler);
}

//-------------------------------------------------------------------

int
fifo::getInDescriptor() const
{
	protector pg(this);

	if (!opened)
		throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX_GETINDESCRIPTOR, exception::ERRNO_LIBDODO, FIFOEX_NOTOPENED, IOFILEFIFOEX_NOTOPENED_STR, __LINE__, __FILE__, path);

	return fileno(handler);
}

//-------------------------------------------------------------------

int
fifo::getOutDescriptor() const
{
	protector pg(this);

	if (!opened)
		throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX_GETOUTDESCRIPTOR, exception::ERRNO_LIBDODO, FIFOEX_NOTOPENED, IOFILEFIFOEX_NOTOPENED_STR, __LINE__, __FILE__, path);

	return fileno(handler);
}

//-------------------------------------------------------------------

void
fifo::clone(const fifo &fd)
{
	protector pg(this);

	if (opened)
	{
		if (fclose(handler) != 0)
			throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX_CLONE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		handler = NULL;

		opened = false;
	}

	path = fd.path;
	mode = fd.mode;
	inSize = fd.inSize;
	outSize = fd.outSize;

	if (fd.opened)
	{
		int oldDesc, newDesc;

		oldDesc = fileno(fd.handler);
		if (oldDesc == -1)
			throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX_CLONE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		newDesc = dup(oldDesc);
		if (newDesc == -1)
			throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX_CLONE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		switch (mode)
		{
			case FIFO_OPENMODE_WRITE:

				handler = fdopen(newDesc, "w");

				break;

			case FIFO_OPENMODE_READ:
			default:

				handler = fdopen(newDesc, "r");
		}

		if (handler == NULL)
			throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX_CLONE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		opened = true;
	}
}

//-------------------------------------------------------------------

void
fifo::close()
{
	protector pg(this);

#ifndef IO_WO_XEXEC
	operType = FIFO_OPERATION_CLOSE;
	performXExec(preExec);
#endif

	if (opened)
	{
		if (fclose(handler) != 0)
			throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX_CLOSE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

		handler = NULL;

		opened = false;
	}

#ifndef IO_WO_XEXEC
	performXExec(postExec);
#endif
}

//-------------------------------------------------------------------

void
fifo::open(const dodoString &a_path,
		   short a_mode)
{
	protector pg(this);

#ifndef IO_WO_XEXEC
	operType = FIFO_OPERATION_OPEN;
	performXExec(preExec);
#endif

	path = a_path;
	mode = a_mode;

	if (opened)
	{
		if (fclose(handler) != 0)
			throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX_OPEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

		opened = false;
	}

	if (path.size() == 0)
		throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX_OPEN, exception::ERRNO_LIBDODO, FIFOEX_WRONGFILENAME, IOFILEFIFOEX_WRONGFILENAME_STR, __LINE__, __FILE__, path);
	else
	{
		bool exists(false);
		struct stat st;

		if (::lstat(path.c_str(), &st) == -1)
		{
			if (errno != ENOENT)
				throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX_OPEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
		}
		else
			exists = true;

		if (exists && !S_ISFIFO(st.st_mode))
			throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX_OPEN, exception::ERRNO_LIBDODO, FIFOEX_WRONGFILENAME, IOFILEFIFOEX_WRONGFILENAME_STR, __LINE__, __FILE__, path);

		if (!exists)
			tools::filesystem::mkfifo(path, DEFAULT_FILE_PERM);

		switch (mode)
		{
			case FIFO_OPENMODE_WRITE:

				handler = fopen(path.c_str(), "w");

				break;

			case FIFO_OPENMODE_READ:
			default:

				handler = fopen(path.c_str(), "r");
		}
	}

	if (handler == NULL)
		throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX_OPEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

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
		throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX__READ, exception::ERRNO_LIBDODO, FIFOEX_NOTOPENED, IOFILEFIFOEX_NOTOPENED_STR, __LINE__, __FILE__, path);

	memset(a_void, '\0', inSize);

	while (true)
	{
		if (fread(a_void, inSize, 1, handler) == 0)
		{
			if (feof(handler) != 0 || errno == EAGAIN)
				break;

			if (errno == EINTR)
				continue;

			if (ferror(handler) != 0)
				throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX__READ, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}

		break;
	}
}

//-------------------------------------------------------------------

void
fifo::_write(const char *const a_buf)
{
	if (!opened)
		throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX__WRITE, exception::ERRNO_LIBDODO, FIFOEX_NOTOPENED, IOFILEFIFOEX_NOTOPENED_STR, __LINE__, __FILE__, path);

	while (true)
	{
		if (fwrite(a_buf, outSize, 1, handler) == 0)
		{
			if (errno == EINTR)
				continue;

			if (errno == EAGAIN)
				break;

			if (ferror(handler) != 0)
				throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX__WRITE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}

		break;
	}
}

//-------------------------------------------------------------------

void
fifo::flush()
{
	protector pg(this);

	if (!opened)
		throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX_FLUSH, exception::ERRNO_LIBDODO, FIFOEX_NOTOPENED, IOFILEFIFOEX_NOTOPENED_STR, __LINE__, __FILE__, path);

	if (fflush(handler) != 0)
		throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX_FLUSH, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
}

//-------------------------------------------------------------------

unsigned long
fifo::_readStream(char * const a_void)
{
	if (!opened)
		throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX__READSTREAM, exception::ERRNO_LIBDODO, FIFOEX_NOTOPENED, IOFILEFIFOEX_NOTOPENED_STR, __LINE__, __FILE__, path);

	memset(a_void, '\0', inSize);

	while (true)
	{
		if (fgets(a_void, inSize, handler) == NULL)
		{
			if (errno == EINTR)
				continue;

			if (errno == EAGAIN)
				break;

			if (ferror(handler) != 0)
				throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX__READSTREAM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}

		break;
	}

	return strlen(a_void);
}

//-------------------------------------------------------------------

void
fifo::_writeStream(const char *const a_buf)
{
	if (!opened)
		throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX__WRITESTREAM, exception::ERRNO_LIBDODO, FIFOEX_NOTOPENED, IOFILEFIFOEX_NOTOPENED_STR, __LINE__, __FILE__, path);

	unsigned long sent_received = 0;

	unsigned long batch = 0, n;

	while (batch < outSize)
	{
		while (true)
		{
			if ((n = fwrite(a_buf + sent_received, 1, outSize, handler)) == 0)
			{
				if (errno == EINTR)
					continue;

				if (ferror(handler) != 0)
					throw exception::basic(exception::ERRMODULE_IOFILEFIFO, FIFOEX__WRITESTREAM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			}

			break;
		}

		if (n == 0)
			break;

		batch += n;
		sent_received += n;
	}
}

//-------------------------------------------------------------------
