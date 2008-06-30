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

file::file(const dodoString &a_path,
		   short a_fileType,
		   short mode) : over(false),
						 append(false),
						 path(a_path),
						 fileType(a_fileType),
						 pos(0)
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
file::_write(const char *const a_buf)
{

	if (fileType == FILE_FILETYPE_REG_FILE || fileType == FILE_FILETYPE_TMP_FILE)
	{
		if (!append)
		{
			unsigned long pos = this->pos * outSize;
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

unsigned long
file::_readStream(char * const a_void)
{
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

	return strlen(a_void);
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

//-------------------------------------------------------------------

short
file::getFileType() const
{
	raceHazardGuard pg(this);

	return fileType;
}

//-------------------------------------------------------------------
