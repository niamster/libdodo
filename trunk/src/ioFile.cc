/***************************************************************************
 *            ioFile.cc
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

#include <libdodo/ioFile.h>

using namespace dodo::io;

#ifndef IO_WO_XEXEC

__xexecIoFileCollectedData::__xexecIoFileCollectedData(int &a_operType,
													   void *a_executor) : operType(a_operType),
																		   executor(a_executor)
{
}

#endif

//-------------------------------------------------------------------

file::file(const dodoString &a_path,
		   short a_fileType,
		   short mode) : over(false),
						 append(false),
						 path(a_path),
						 fileType(a_fileType),
						 pos(0)
#ifndef IO_WO_XEXEC

						 ,
						 collectedData(operType,
									   (void *) this)

#endif
{
#ifndef IO_WO_XEXEC

	execObject = XEXEC_OBJECT_IOFILE;
	execObjectData = (void *)&collectedData;

#endif

	if (path != __dodostring__)
		try
		{
			open(path, fileType, mode);
		}
		catch (...)
		{

		}
}

//-------------------------------------------------------------------

file::file(file &fd)
#ifndef IO_WO_XEXEC

	: collectedData(operType,
					(void *) this)

#endif
{
}

//-------------------------------------------------------------------

file::~file()
{
	if (opened)
		fclose(handler);
}

//-------------------------------------------------------------------

int
file::getInDescriptor() const
{
	raceHazardGuard pg(this);

	if (!opened)
		return -1;

	return fileno(handler);
}

//-------------------------------------------------------------------

int
file::getOutDescriptor() const
{
	raceHazardGuard pg(this);

	if (!opened)
		return -1;

	return fileno(handler);
}

//-------------------------------------------------------------------

void
file::close()
{
	raceHazardGuard pg(this);

#ifndef IO_WO_XEXEC
	operType = FILE_OPERATION_CLOSE;
	performXExec(preExec);
#endif

	if (opened)
	{
		if (fclose(handler) != 0)
			throw baseEx(ERRMODULE_IOFILE, FILEEX_CLOSE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

		opened = false;
	}

#ifndef IO_WO_XEXEC
	performXExec(postExec);
#endif
}

//-------------------------------------------------------------------

void
file::open(const dodoString &a_path,
		   short a_fileType,
		   short mode)
{
	raceHazardGuard pg(this);

#ifndef IO_WO_XEXEC
	operType = FILE_OPERATION_OPEN;
	performXExec(preExec);
#endif

	path = a_path;
	fileType = a_fileType;

	if (opened)
	{
		if (fclose(handler) != 0)
			throw baseEx(ERRMODULE_IOFILE, FILEEX_OPEN, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

		opened = false;
	}

	if (fileType == FILE_FILETYPE_TMP_FILE)
		handler = tmpfile();
	else
	{
		if (path.size() == 0)
			throw baseEx(ERRMODULE_IOFILE, FILEEX_OPEN, ERR_LIBDODO, FILEEX_WRONGFILENAME, IOFILEEX_WRONGFILENAME_STR, __LINE__, __FILE__, path);
		else
		{
			struct stat st;
			bool exists(false);

			if (::lstat(path.c_str(), &st) == -1)
			{
				if (errno != ENOENT)
					throw baseEx(ERRMODULE_IOFILE, FILEEX_OPEN, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
			}
			else
				exists = true;

			if (fileType == FILE_FILETYPE_FIFO_FILE)
			{
				if (exists && !S_ISFIFO(st.st_mode))
					throw baseEx(ERRMODULE_IOFILE, FILEEX_OPEN, ERR_LIBDODO, FILEEX_WRONGFILENAME, IOFILEEX_WRONGFILENAME_STR, __LINE__, __FILE__, path);
				if (!exists)
					tools::filesystem::mkfifo(path, DEFAULT_FILE_PERM);
			}
			else
			{
				if ((fileType == FILE_FILETYPE_REG_FILE || fileType == FILE_FILETYPE_TMP_FILE || fileType ==  FILE_FILETYPE_CHAR_FILE) && exists && !S_ISREG(st.st_mode) && !S_ISCHR(st.st_mode))
					throw baseEx(ERRMODULE_IOFILE, FILEEX_OPEN, ERR_LIBDODO, FILEEX_WRONGFILENAME, IOFILEEX_WRONGFILENAME_STR, __LINE__, __FILE__, path);
			}

			switch (mode)
			{
				case FILE_OPENMODE_READ_WRITE:

					handler = fopen(path.c_str(), "r+");
					if (handler == NULL)
						handler = fopen(path.c_str(), "w+");

					break;

				case FILE_OPENMODE_READ_WRITE_TRUNCATE:

					handler = fopen(path.c_str(), "w+");

					break;

				case FILE_OPENMODE_APPEND:

					handler = fopen(path.c_str(), "a+");
					append = true;

					break;

				case FILE_OPENMODE_READ_ONLY:
				default:

					handler = fopen(path.c_str(), "r");
			}
		}
	}

	if (handler == NULL)
		throw baseEx(ERRMODULE_IOFILE, FILEEX_OPEN, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	tools::filesystem::chmod(path, DEFAULT_FILE_PERM);

#ifndef IO_WO_XEXEC
	performXExec(postExec);
#endif

	opened = true;
}

//-------------------------------------------------------------------

void
file::_read(char * const a_void)
{
	if (fileType == FILE_FILETYPE_REG_FILE || fileType == FILE_FILETYPE_TMP_FILE)
		if (fseek(handler, pos * inSize, SEEK_SET) == -1)
			throw baseEx(ERRMODULE_IOFILE, FILEEX__READ, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

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
				throw baseEx(ERRMODULE_IOFILE, FILEEX__READ, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}

		break;
	}
}

//-------------------------------------------------------------------

void
file::read(char * const a_void)
{
	raceHazardGuard pg(this);

#ifndef IO_WO_XEXEC
	operType = IO_OPERATION_READ;
	performXExec(preExec);

	collectedData.buffer.reserve(inSize);
#endif

#ifndef IO_WO_XEXEC

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

#ifndef IO_WO_XEXEC
	collectedData.buffer.assign(a_void, inSize);

	performXExec(postExec);

	strncpy(a_void, collectedData.buffer.c_str(), collectedData.buffer.size() > inSize ? inSize : collectedData.buffer.size());
	collectedData.buffer.clear();
#endif
}

//-------------------------------------------------------------------

void
file::readString(dodoString &a_str)
{
	raceHazardGuard pg(this);

#ifndef IO_WO_XEXEC
	operType = IO_OPERATION_READSTRING;
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

#ifndef IO_WO_XEXEC
		collectedData.buffer.clear();
#endif

		throw;
	}

#ifndef IO_WO_XEXEC

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
file::writeString(const dodoString &a_buf)
{
	raceHazardGuard pg(this);

#ifndef IO_WO_XEXEC

	collectedData.buffer = a_buf;

	operType = IO_OPERATION_WRITESTRING;
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


#ifndef IO_WO_XEXEC
	performXExec(postExec);

	collectedData.buffer.clear();
#endif

	this->write(a_buf.c_str());
}

//-------------------------------------------------------------------

void
file::_write(const char *const a_buf)
{
	unsigned long pos = this->pos;

	if (fileType == FILE_FILETYPE_REG_FILE || fileType == FILE_FILETYPE_TMP_FILE)
	{
		pos *= outSize;

		if (!append)
		{
			if (!over)
			{
				size_t read = 0;
				char *t_buf = new char[outSize];

				if (fseek(handler, pos, SEEK_SET) == -1)
				{
					delete [] t_buf;

					throw baseEx(ERRMODULE_IOFILE, FILEEX__WRITE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
				}

				read = fread(t_buf, outSize, 1, handler);

				delete [] t_buf;

				if (read != 0)
					throw baseEx(ERRMODULE_IOFILE, FILEEX__WRITE, ERR_LIBDODO, FILEEX_CANNOTOVEWRITE, IOFILEEX_CANNOTOVEWRITE_STR, __LINE__, __FILE__, path);
			}

			if (fseek(handler, pos, SEEK_SET) == -1)
				throw baseEx(ERRMODULE_IOFILE, FILEEX__WRITE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
		}
		else
		if (fseek(handler, 0, SEEK_END) == -1)
			throw baseEx(ERRMODULE_IOFILE, FILEEX__WRITE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
	}

	while (true)
	{
		if (fwrite(a_buf, outSize, 1, handler) == 0)
		{
			if (errno == EINTR)
				continue;

			if (errno == EAGAIN)
				break;

			if (ferror(handler) != 0)
				throw baseEx(ERRMODULE_IOFILE, FILEEX__WRITE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}

		break;
	}
}

//-------------------------------------------------------------------

void
file::write(const char *const a_buf)
{
	raceHazardGuard pg(this);

#ifndef IO_WO_XEXEC

	collectedData.buffer.assign(a_buf, outSize);

	operType = IO_OPERATION_WRITE;
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


#ifndef IO_WO_XEXEC
	performXExec(postExec);

	collectedData.buffer.clear();
#endif
}

//-------------------------------------------------------------------

void
file::erase()
{
	raceHazardGuard pg(this);

	char *empty = new char[outSize];

	memset(empty, 0, outSize);

	_write(empty);

	delete [] empty;
}

//-------------------------------------------------------------------

void
file::flush()
{
	raceHazardGuard pg(this);

	if (fflush(handler) != 0)
		throw baseEx(ERRMODULE_IOFILE, FILEEX_FLUSH, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
}

//-------------------------------------------------------------------

dodoString
file::getPath() const
{
	raceHazardGuard pg(this);

	return path;
}

//-------------------------------------------------------------------

void
file::_readStream(char * const a_void)
{
	unsigned long pos = this->pos;

	if (fileType == FILE_FILETYPE_REG_FILE || fileType == FILE_FILETYPE_TMP_FILE)
	{
		if (fseek(handler, 0, SEEK_SET) == -1)
			throw baseEx(ERRMODULE_IOFILE, FILEEX__READSTREAM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

		for (unsigned long i(0); i < pos; ++i)
		{
			if (fgets(a_void, inSize, handler) == NULL)
			{
				switch (errno)
				{
					case EIO:
					case EINTR:
					case EBADF:
					case EOVERFLOW:
					case ENOMEM:
					case ENXIO:

						throw baseEx(ERRMODULE_IOFILE, FILEEX__READSTREAM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
				}

				throw baseEx(ERRMODULE_IOFILE, FILEEX__READSTREAM, ERR_LIBDODO, FILEEX_FILEISSHORTERTHANGIVENPOSITION, IOFILEEX_FILEISSHORTERTHANGIVENPOSITION_STR, __LINE__, __FILE__, path);
			}
		}
	}

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
				throw baseEx(ERRMODULE_IOFILE, FILEEX__READSTREAM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}

		break;
	}
}

//-------------------------------------------------------------------

void
file::readStream(char * const a_void)
{
	raceHazardGuard pg(this);

#ifndef IO_WO_XEXEC
	operType = IO_OPERATION_READSTREAM;
	performXExec(preExec);
#endif

	_readStream(a_void);

#ifndef IO_WO_XEXEC

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
file::readStreamString(dodoString &a_str)
{
	raceHazardGuard pg(this);

#ifndef IO_WO_XEXEC
	operType = IO_OPERATION_READSTREAMSTRING;
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

#ifndef IO_WO_XEXEC

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
file::_writeStream(const char *const a_buf)
{
	if (fseek(handler, 0, SEEK_END) == -1)
		throw baseEx(ERRMODULE_IOFILE, FILEEX__WRITESTREAM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

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
					throw baseEx(ERRMODULE_IOFILE, FILEEX__WRITESTREAM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			}

			break;
		}

		if (n == 0)
			break;

		batch += n;
		sent_received += n;
	}
}

void
file::writeStreamString(const dodoString &a_buf)
{
	raceHazardGuard pg(this);

	unsigned long _outSize = outSize;

#ifndef IO_WO_XEXEC

	collectedData.buffer = a_buf;

	operType = IO_OPERATION_WRITESTREAMSTRING;
	performXExec(preExec);

	try
	{
		outSize = collectedData.buffer.size();

		_writeStream(collectedData.buffer.c_str());

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

		_writeStream(a_buf.c_str());

		outSize = _outSize;
	}
	catch (...)
	{
		outSize = _outSize;

		throw;
	}

#endif

#ifndef IO_WO_XEXEC
	performXExec(postExec);

	collectedData.buffer.clear();
#endif
}

//-------------------------------------------------------------------

void
file::writeStream(const char *const a_buf)
{
	raceHazardGuard pg(this);

	unsigned long _outSize = outSize;

#ifndef IO_WO_XEXEC

	collectedData.buffer = a_buf;

	operType = IO_OPERATION_WRITESTREAM;
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

#ifndef IO_WO_XEXEC
	performXExec(postExec);

	collectedData.buffer.clear();
#endif
}

//-------------------------------------------------------------------

short
file::getFileType() const
{
	raceHazardGuard pg(this);

	return fileType;
}

//-------------------------------------------------------------------
