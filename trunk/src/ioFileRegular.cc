/***************************************************************************
 *            ioFileRegular.cc
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

#include <libdodo/ioFileRegular.h>

using namespace dodo::io::file;

regular::regular() : overwrite(false),
			 pos(0),
			 blockOffset(true),
			 append(false),
			 handler(NULL)
{
#ifndef IO_WO_XEXEC

	collectedData.setExecObject(XEXEC_OBJECT_IOFILEREGULAR);

#endif
}

//-------------------------------------------------------------------

regular::regular(const dodoString &a_path,
      		   short a_mode) : overwrite(false),
			 pos(0),
			 blockOffset(true),
			 append(false),
			 handler(NULL),
			 path(a_path),
			 mode(a_mode)
{
#ifndef IO_WO_XEXEC

	collectedData.setExecObject(XEXEC_OBJECT_IOFILEREGULAR);

#endif

	bool exists(false);

	if (path.size() == 0)
		throw exception::basic(exception::ERRMODULE_IOFILEREGULAR, REGULAREX_REGULAR, exception::ERRNO_LIBDODO, REGULAREX_WRONGFILENAME, IOFILEREGULAREX_WRONGFILENAME_STR, __LINE__, __FILE__, path);
	else
	{
		struct stat st;

		if (::lstat(path.c_str(), &st) == -1)
		{
			if (errno != ENOENT)
				throw exception::basic(exception::ERRMODULE_IOFILEREGULAR, REGULAREX_REGULAR, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
		}
		else
			exists = true;

		if (exists && !S_ISREG(st.st_mode) && !S_ISBLK(st.st_mode))
			throw exception::basic(exception::ERRMODULE_IOFILEREGULAR, REGULAREX_REGULAR, exception::ERRNO_LIBDODO, REGULAREX_WRONGFILENAME, IOFILEREGULAREX_WRONGFILENAME_STR, __LINE__, __FILE__, path);

		switch (mode)
		{
			case REGULAR_OPENMODE_READ_WRITE:

				handler = fopen(path.c_str(), "r+");
				if (handler == NULL)
					handler = fopen(path.c_str(), "w+");

				break;

			case REGULAR_OPENMODE_READ_WRITE_TRUNCATE:

				handler = fopen(path.c_str(), "w+");

				break;

			case REGULAR_OPENMODE_APPEND:

				handler = fopen(path.c_str(), "a");

				break;

			case REGULAR_OPENMODE_READ_ONLY:
			default:

				handler = fopen(path.c_str(), "r");
		}
	}

	if (handler == NULL)
		throw exception::basic(exception::ERRMODULE_IOFILEREGULAR, REGULAREX_REGULAR, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	if (!exists)
		tools::filesystem::chmod(path, DEFAULT_FILE_PERM);
}

//-------------------------------------------------------------------

regular::regular(const regular &fd) : overwrite(fd.overwrite),
							 path(fd.path),
							 pos(fd.pos),
							 blockOffset(fd.blockOffset),
							 append(fd.append),
							 mode(fd.mode),
							 handler(NULL)

{
#ifndef IO_WO_XEXEC

	collectedData.setExecObject(XEXEC_OBJECT_IOFILEREGULAR);

#endif

	inSize = fd.inSize;
	outSize = fd.outSize;

	if (fd.handler != NULL)
	{
		int oldDesc, newDesc;

		oldDesc = fileno(fd.handler);
		if (oldDesc == -1)
			throw exception::basic(exception::ERRMODULE_IOFILEREGULAR, REGULAREX_REGULAR, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		newDesc = dup(oldDesc);
		if (newDesc == -1)
			throw exception::basic(exception::ERRMODULE_IOFILEREGULAR, REGULAREX_REGULAR, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		switch (mode)
		{
			case REGULAR_OPENMODE_READ_WRITE:
			case REGULAR_OPENMODE_READ_WRITE_TRUNCATE:

				handler = fdopen(newDesc, "r+");

				break;

			case REGULAR_OPENMODE_APPEND:

				handler = fdopen(newDesc, "a");

				break;

			case REGULAR_OPENMODE_READ_ONLY:
			default:

				handler = fdopen(newDesc, "r");
		}

		if (handler == NULL)
			throw exception::basic(exception::ERRMODULE_IOFILEREGULAR, REGULAREX_REGULAR, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}
}

//-------------------------------------------------------------------

regular::~regular()
{
	if (handler != NULL)
		fclose(handler);
}

//-------------------------------------------------------------------

int
regular::getInDescriptor() const
{
	protector pg(this);

	if (handler == NULL)
		throw exception::basic(exception::ERRMODULE_IOFILEREGULAR, REGULAREX_GETINDESCRIPTOR, exception::ERRNO_LIBDODO, REGULAREX_NOTOPENED, IOFILEREGULAREX_NOTOPENED_STR, __LINE__, __FILE__, path);

	return fileno(handler);
}

//-------------------------------------------------------------------

int
regular::getOutDescriptor() const
{
	protector pg(this);

	if (handler == NULL)
		throw exception::basic(exception::ERRMODULE_IOFILEREGULAR, REGULAREX_GETOUTDESCRIPTOR, exception::ERRNO_LIBDODO, REGULAREX_NOTOPENED, IOFILEREGULAREX_NOTOPENED_STR, __LINE__, __FILE__, path);

	return fileno(handler);
}

//-------------------------------------------------------------------

void
regular::clone(const regular &fd)
{
	protector pg(this);

	if (handler != NULL)
	{
		if (fclose(handler) != 0)
			throw exception::basic(exception::ERRMODULE_IOFILEREGULAR, REGULAREX_CLONE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		handler = NULL;
	}

	overwrite = fd.overwrite;
	path = fd.path;
	mode = fd.mode;
	pos = fd.pos;
	blockOffset = fd.blockOffset;
	append = fd.append;
	inSize = fd.inSize;
	outSize = fd.outSize;

	if (fd.handler != NULL)
	{
		int oldDesc, newDesc;

		oldDesc = fileno(fd.handler);
		if (oldDesc == -1)
			throw exception::basic(exception::ERRMODULE_IOFILEREGULAR, REGULAREX_CLONE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		newDesc = dup(oldDesc);
		if (newDesc == -1)
			throw exception::basic(exception::ERRMODULE_IOFILEREGULAR, REGULAREX_CLONE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		switch (mode)
		{
			case REGULAR_OPENMODE_READ_WRITE:
			case REGULAR_OPENMODE_READ_WRITE_TRUNCATE:

				handler = fdopen(newDesc, "r+");

				break;

			case REGULAR_OPENMODE_APPEND:

				handler = fdopen(newDesc, "a");

				break;

			case REGULAR_OPENMODE_READ_ONLY:
			default:

				handler = fdopen(newDesc, "r");
		}

		if (handler == NULL)
			throw exception::basic(exception::ERRMODULE_IOFILEREGULAR, REGULAREX_CLONE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}
}

//-------------------------------------------------------------------

void
regular::close()
{
	protector pg(this);

#ifndef IO_WO_XEXEC
	operType = REGULAR_OPERATION_CLOSE;
	performXExec(preExec);
#endif

	if (handler != NULL)
	{
		if (fclose(handler) != 0)
			throw exception::basic(exception::ERRMODULE_IOFILEREGULAR, REGULAREX_CLOSE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

		handler = NULL;
	}

#ifndef IO_WO_XEXEC
	performXExec(postExec);
#endif
}

//-------------------------------------------------------------------

void
regular::open(const dodoString &a_path,
		   short a_mode)
{
	protector pg(this);

#ifndef IO_WO_XEXEC
	operType = REGULAR_OPERATION_OPEN;
	performXExec(preExec);
#endif

	path = a_path;
	mode = a_mode;

	if (handler != NULL)
	{
		if (fclose(handler) != 0)
			throw exception::basic(exception::ERRMODULE_IOFILEREGULAR, REGULAREX_OPEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

		handler = NULL;
	}

	bool exists(false);

	if (path.size() == 0)
		throw exception::basic(exception::ERRMODULE_IOFILEREGULAR, REGULAREX_OPEN, exception::ERRNO_LIBDODO, REGULAREX_WRONGFILENAME, IOFILEREGULAREX_WRONGFILENAME_STR, __LINE__, __FILE__, path);
	else
	{
		struct stat st;

		if (::lstat(path.c_str(), &st) == -1)
		{
			if (errno != ENOENT)
				throw exception::basic(exception::ERRMODULE_IOFILEREGULAR, REGULAREX_OPEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
		}
		else
			exists = true;

		if (exists && !S_ISREG(st.st_mode) && !S_ISBLK(st.st_mode))
			throw exception::basic(exception::ERRMODULE_IOFILEREGULAR, REGULAREX_OPEN, exception::ERRNO_LIBDODO, REGULAREX_WRONGFILENAME, IOFILEREGULAREX_WRONGFILENAME_STR, __LINE__, __FILE__, path);

		switch (mode)
		{
			case REGULAR_OPENMODE_READ_WRITE:

				handler = fopen(path.c_str(), "r+");
				if (handler == NULL)
					handler = fopen(path.c_str(), "w+");

				break;

			case REGULAR_OPENMODE_READ_WRITE_TRUNCATE:

				handler = fopen(path.c_str(), "w+");

				break;

			case REGULAR_OPENMODE_APPEND:

				handler = fopen(path.c_str(), "a");

				break;

			case REGULAR_OPENMODE_READ_ONLY:
			default:

				handler = fopen(path.c_str(), "r");
		}
	}

	if (handler == NULL)
		throw exception::basic(exception::ERRMODULE_IOFILEREGULAR, REGULAREX_OPEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	if (!exists)
		tools::filesystem::chmod(path, DEFAULT_FILE_PERM);

#ifndef IO_WO_XEXEC
	performXExec(postExec);
#endif
}

//-------------------------------------------------------------------

void
regular::_read(char * const a_data)
{
	if (handler == NULL)
		throw exception::basic(exception::ERRMODULE_IOFILEREGULAR, REGULAREX__READ, exception::ERRNO_LIBDODO, REGULAREX_NOTOPENED, IOFILEREGULAREX_NOTOPENED_STR, __LINE__, __FILE__, path);

	unsigned long pos = blockOffset?this->pos * inSize:this->pos;

	if (fseek(handler, pos, SEEK_SET) == -1)
		throw exception::basic(exception::ERRMODULE_IOFILEREGULAR, REGULAREX__READ, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	memset(a_data, '\0', inSize);

	while (true)
	{
		if (fread(a_data, inSize, 1, handler) == 0)
		{
			if (feof(handler) != 0 || errno == EAGAIN)
				break;

			if (errno == EINTR)
				continue;

			if (ferror(handler) != 0)
				throw exception::basic(exception::ERRMODULE_IOFILEREGULAR, REGULAREX__READ, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}

		break;
	}
}

//-------------------------------------------------------------------

void
regular::_write(const char *const a_data)
{
	if (handler == NULL)
		throw exception::basic(exception::ERRMODULE_IOFILEREGULAR, REGULAREX__WRITE, exception::ERRNO_LIBDODO, REGULAREX_NOTOPENED, IOFILEREGULAREX_NOTOPENED_STR, __LINE__, __FILE__, path);

	if (mode != REGULAR_OPENMODE_APPEND)
	{
		if (append)
		{
			if (fseek(handler, 0, SEEK_END) == -1)
				throw exception::basic(exception::ERRMODULE_IOFILEREGULAR, REGULAREX__WRITE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
		}
		else
		{
			unsigned long pos = blockOffset?this->pos * outSize:this->pos;
			if (!overwrite)
			{
				if (fseek(handler, pos, SEEK_SET) == -1)
					throw exception::basic(exception::ERRMODULE_IOFILEREGULAR, REGULAREX__WRITE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

				char *t_buf = new char[outSize];

				size_t read = fread(t_buf, outSize, 1, handler);

				delete [] t_buf;

				if (read != 0)
					throw exception::basic(exception::ERRMODULE_IOFILEREGULAR, REGULAREX__WRITE, exception::ERRNO_LIBDODO, REGULAREX_CANNOTOVEWRITE, IOFILEREGULAREX_CANNOTOVEWRITE_STR, __LINE__, __FILE__, path);
			}

			if (fseek(handler, pos, SEEK_SET) == -1)
				throw exception::basic(exception::ERRMODULE_IOFILEREGULAR, REGULAREX__WRITE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
		}
	}

	while (true)
	{
		if (fwrite(a_data, outSize, 1, handler) == 0)
		{
			if (errno == EINTR)
				continue;

			if (errno == EAGAIN)
				break;

			if (ferror(handler) != 0)
				throw exception::basic(exception::ERRMODULE_IOFILEREGULAR, REGULAREX__WRITE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}

		break;
	}
}

//-------------------------------------------------------------------

void
regular::erase()
{
	protector pg(this);

	char *empty = new char[outSize];

	memset(empty, 0, outSize);

	bool _overwrite = overwrite;
	overwrite = true;

	try
	{
		_write(empty);

		overwrite = _overwrite;
	}
	catch (...)
	{
		overwrite = _overwrite;

		delete [] empty;

		throw;
	}

	delete [] empty;
}

//-------------------------------------------------------------------

void
regular::flush()
{
	protector pg(this);

	if (handler == NULL)
		throw exception::basic(exception::ERRMODULE_IOFILEREGULAR, REGULAREX_FLUSH, exception::ERRNO_LIBDODO, REGULAREX_NOTOPENED, IOFILEREGULAREX_NOTOPENED_STR, __LINE__, __FILE__, path);

	if (fflush(handler) != 0)
		throw exception::basic(exception::ERRMODULE_IOFILEREGULAR, REGULAREX_FLUSH, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
}

//-------------------------------------------------------------------

unsigned long
regular::_readStream(char * const a_data)
{
	if (handler == NULL)
		throw exception::basic(exception::ERRMODULE_IOFILEREGULAR, REGULAREX__READSTREAM, exception::ERRNO_LIBDODO, REGULAREX_NOTOPENED, IOFILEREGULAREX_NOTOPENED_STR, __LINE__, __FILE__, path);

	unsigned long readSize = inSize + 1;

	if (blockOffset)
	{
		if (fseek(handler, 0, SEEK_SET) == -1)
			throw exception::basic(exception::ERRMODULE_IOFILEREGULAR, REGULAREX__READSTREAM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

		for (unsigned long i = 0; i < pos; ++i)
		{
			if (fgets(a_data, readSize, handler) == NULL)
			{
				switch (errno)
				{
					case EIO:
					case EINTR:
					case EBADF:
					case EOVERFLOW:
					case ENOMEM:
					case ENXIO:

						throw exception::basic(exception::ERRMODULE_IOFILEREGULAR, REGULAREX__READSTREAM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
				}

				throw exception::basic(exception::ERRMODULE_IOFILEREGULAR, REGULAREX__READSTREAM, exception::ERRNO_LIBDODO, REGULAREX_FILEISSHORTERTHANGIVENPOSITION, IOFILEREGULAREX_FILEISSHORTERTHANGIVENPOSITION_STR, __LINE__, __FILE__, path);
			}
		}
	}
	else
		if (fseek(handler, pos, SEEK_SET) == -1)
			throw exception::basic(exception::ERRMODULE_IOFILEREGULAR, REGULAREX__READSTREAM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	memset(a_data, '\0', readSize);

	while (true)
	{
		if (fgets(a_data, readSize, handler) == NULL)
		{
			if (errno == EINTR)
				continue;

			if (errno == EAGAIN)
				break;

			if (ferror(handler) != 0)
				throw exception::basic(exception::ERRMODULE_IOFILEREGULAR, REGULAREX__READSTREAM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}

		break;
	}

	return strlen(a_data);
}

//-------------------------------------------------------------------

void
regular::_writeStream(const char *const a_data)
{
	if (handler == NULL)
		throw exception::basic(exception::ERRMODULE_IOFILEREGULAR, REGULAREX__WRITESTREAM, exception::ERRNO_LIBDODO, REGULAREX_NOTOPENED, IOFILEREGULAREX_NOTOPENED_STR, __LINE__, __FILE__, path);

	if (mode != REGULAR_OPENMODE_APPEND)
		if (fseek(handler, 0, SEEK_END) == -1)
			throw exception::basic(exception::ERRMODULE_IOFILEREGULAR, REGULAREX__WRITESTREAM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	unsigned int bufSize = strlen(a_data);

	unsigned long _outSize = outSize;

	if (bufSize < _outSize)
		_outSize = bufSize;

	while (true)
	{
		if (fwrite(a_data, _outSize, 1, handler) == 0)
		{
			if (errno == EINTR)
				continue;

			if (ferror(handler) != 0)
				throw exception::basic(exception::ERRMODULE_IOFILEREGULAR, REGULAREX__WRITESTREAM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}

		break;
	}
}

//-------------------------------------------------------------------

