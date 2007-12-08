/***************************************************************************
 *            ioDisk.cc
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

#include <libdodo/ioDisk.h>

using namespace dodo;

ioDisk::ioDisk(const dodoString &a_path,
					short a_fileType,
					 short mode) : over(false),
									 append(false),
									 path(a_path),
									 fileType(a_fileType)
{
	if (path != __dodostring__)
		try
		{
			open(path, fileType, mode);
		}
		catch(...)
		{
			
		}
}

//-------------------------------------------------------------------

ioDisk::ioDisk(ioDisk &fd)
{
}

//-------------------------------------------------------------------

ioDisk::~ioDisk()
{
	if (opened)
		fclose(file);
}

//-------------------------------------------------------------------

int
ioDisk::getInDescriptor() const
{
	if (!opened)
		return -1;

	return fileno(file);
}

//-------------------------------------------------------------------

int
ioDisk::getOutDescriptor() const
{
	if (!opened)
		return -1;

	return fileno(file);
}

//-------------------------------------------------------------------

#ifndef IO_DISK_WO_XEXEC

int
ioDisk::addPostExec(inExec func,
					   void   *data)
{
	return _addPostExec(func, (void *)this, XEXECOBJ_IODISK, data);
}

//-------------------------------------------------------------------

int
ioDisk::addPreExec(inExec func,
					  void   *data)
{
	return _addPreExec(func, (void *)this, XEXECOBJ_IODISK, data);
}

//-------------------------------------------------------------------

	#ifdef DL_EXT

int
ioDisk::addPostExec(const dodoString &module,
					   void             *data,
					   void             *toInit)
{
	return _addPostExec(module, (void *)this, XEXECOBJ_IODISK, data, toInit);
}

//-------------------------------------------------------------------

int
ioDisk::addPreExec(const dodoString &module,
					  void             *data,
					  void             *toInit)
{
	return _addPreExec(module, (void *)this, XEXECOBJ_IODISK, data, toInit);
}

//-------------------------------------------------------------------

xexecCounts
ioDisk::addExec(const dodoString &module,
				   void             *data,
				   void             *toInit)
{
	return _addExec(module, (void *)this, XEXECOBJ_IODISK, data, toInit);
}

	#endif

#endif

//-------------------------------------------------------------------

void
ioDisk::close()
{
	#ifndef IO_DISK_WO_XEXEC
	operType = IODISK_OPER_CLOSE;
	#endif

	#ifndef IO_DISK_WO_XEXEC
	performXExec(preExec);
	#endif

	if (opened)
	{
		if (fclose(file) != 0)
			throw baseEx(ERRMODULE_IODISK, IODISKEX_CLOSE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

		#ifndef IO_DISK_WO_XEXEC
		performXExec(postExec);
		#endif

		opened = false;
	}
}

//-------------------------------------------------------------------

void
ioDisk::open(const dodoString &a_path,
			short a_fileType,
			short mode)
{
	#ifndef IO_DISK_WO_XEXEC
	operType = IODISK_OPER_OPEN;
	performXExec(preExec);
	#endif

	path = a_path;
	fileType = a_fileType;

	if (opened)
		close();

	if (fileType == IODISK_FILETYPE_TMP_FILE)
		file = tmpfile();
	else
	{
		if (path.size() == 0)
			throw baseEx(ERRMODULE_IODISK, IODISKEX_OPEN, ERR_LIBDODO, IODISKEX_WRONG_FILENAME, IODISKEX_WRONG_FILENAME_STR, __LINE__, __FILE__, path);
		else
		{
			struct stat st;
			bool exists(false);

			if (::lstat(path.c_str(), &st) == -1)
			{
				if (errno != ENOENT)
					throw baseEx(ERRMODULE_IODISK, IODISKEX_OPEN, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
			}
			else
				exists = true;

			if (fileType == IODISK_FILETYPE_FIFO_FILE)
			{
				if (exists && !S_ISFIFO(st.st_mode))
					throw baseEx(ERRMODULE_IODISK, IODISKEX_OPEN, ERR_LIBDODO, IODISKEX_WRONG_FILENAME, IODISKEX_WRONG_FILENAME_STR, __LINE__, __FILE__, path);
				if (!exists)
					if (mkfifo(path.c_str(), ioDiskTools::getPermission(FILE_PERM)) == -1)
						throw baseEx(ERRMODULE_IODISK, IODISKEX_OPEN, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
			}
			else
			{
				if ((fileType == IODISK_FILETYPE_REG_FILE || fileType == IODISK_FILETYPE_TMP_FILE || fileType ==  IODISK_FILETYPE_CHAR_FILE) && exists && !S_ISREG(st.st_mode) && !S_ISCHR(st.st_mode))
					throw baseEx(ERRMODULE_IODISK, IODISKEX_OPEN, ERR_LIBDODO, IODISKEX_WRONG_FILENAME, IODISKEX_WRONG_FILENAME_STR, __LINE__, __FILE__, path);
			}

			switch (mode)
			{
				case IODISK_OPENMODE_READ_WRITE:

					file = fopen(path.c_str(), "r+");
					if (file == NULL)
						file = fopen(path.c_str(), "w+");

					break;

				case IODISK_OPENMODE_READ_WRITE_TRUNCATE:

					file = fopen(path.c_str(), "w+");

					break;

				case IODISK_OPENMODE_APPEND:

					file = fopen(path.c_str(), "a+");
					append = true;

					break;

				case IODISK_OPENMODE_READ_ONLY:
				default:

					file = fopen(path.c_str(), "r");
			}
		}
	}

	if (file == NULL)
		throw baseEx(ERRMODULE_IODISK, IODISKEX_OPEN, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	ioDiskTools::chmod(path, FILE_PERM);

	#ifndef IO_DISK_WO_XEXEC
	performXExec(postExec);
	#endif

	opened = true;
}

//-------------------------------------------------------------------

void
ioDisk::read(char * const a_void,
				unsigned long a_pos)
{
	#ifndef IO_DISK_WO_XEXEC
	operType = IODISK_OPER_READ;
	performXExec(preExec);
	#endif

	if (fileType == IODISK_FILETYPE_REG_FILE || fileType == IODISK_FILETYPE_TMP_FILE)
		if (fseek(file, a_pos * inSize, SEEK_SET) == -1)
			throw baseEx(ERRMODULE_IODISK, IODISKEX_READ, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	memset(a_void, '\0', inSize);

	errno = 0;
	if (fileType == IODISK_FILETYPE_REG_FILE || fileType == IODISK_FILETYPE_TMP_FILE)
		fread(a_void, inSize, 1, file);
	else
		#ifndef FAST
	if (fileType == FIFO_FILE)
		#endif
		fgets(a_void, inSize + 1, file);

	switch (errno)
	{
		case EIO:
		case EINTR:
		case EBADF:
		case EOVERFLOW:
		case ENOMEM:
		case ENXIO:

			throw baseEx(ERRMODULE_IODISK, IODISKEX_READ, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
	}

	buffer.assign(a_void, inSize);

	#ifndef IO_DISK_WO_XEXEC
	performXExec(postExec);
	#endif
}

//-------------------------------------------------------------------

void
ioDisk::readString(dodoString &a_str,
					  unsigned long a_pos)
{
	char *data = new char[inSize + 1];

	try
	{

		this->read(data, a_pos);

	}
	catch (...)
	{
		a_str.assign(data, inSize);
		delete [] data;

		throw baseEx(ERRMODULE_IODISK, IODISKEX_READSTRING, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	a_str.assign(data, inSize);
	delete [] data;
}

//-------------------------------------------------------------------

void
ioDisk::writeString(const dodoString &a_buf,
					   unsigned long a_pos)
{
	this->write(a_buf.c_str(), a_pos);
}

//-------------------------------------------------------------------

void
ioDisk::write(const char *const a_buf,
				 unsigned long a_pos)
{
	buffer.assign(a_buf, outSize);

	#ifndef IO_DISK_WO_XEXEC
	operType = IODISK_OPER_WRITE;
	performXExec(preExec);
	#endif

	if (fileType == IODISK_FILETYPE_REG_FILE || fileType == IODISK_FILETYPE_TMP_FILE)
	{
		a_pos *= outSize;

		if (!append)
		{
			if (!over)
			{
				size_t read_bytes(0);
				char *t_buf = new char[outSize];

				if (fseek(file, a_pos, SEEK_SET) == -1)
				{
					delete [] t_buf;
					throw baseEx(ERRMODULE_IODISK, IODISKEX_WRITE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
				}

				read_bytes = fread(t_buf, outSize, 1, file);

				delete [] t_buf;

				if (read_bytes != 0)
					throw baseEx(ERRMODULE_IODISK, IODISKEX_WRITE, ERR_LIBDODO, IODISKEX_CANNOT_OVEWRITE, IODISKEX_CANNOT_OVEWRITE_STR, __LINE__, __FILE__, path);
			}

			if (fseek(file, a_pos, SEEK_SET) == -1)
				throw baseEx(ERRMODULE_IODISK, IODISKEX_WRITE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
		}
		else
		if (fseek(file, 0, SEEK_END) == -1)
			throw baseEx(ERRMODULE_IODISK, IODISKEX_WRITE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
	}

	errno = 0;
	if (fileType == IODISK_FILETYPE_REG_FILE || fileType == IODISK_FILETYPE_TMP_FILE)
		fwrite(buffer.c_str(), outSize, 1, file);
	else
		#ifndef FAST
	if (fileType == IODISK_FILETYPE_FIFO_FILE)
		#endif
		fputs(buffer.c_str(), file);

	switch (errno)
	{
		case EFBIG:
		case EIO:
		case EINTR:
		case EBADF:
		case EOVERFLOW:
		case ENOSPC:
		case EPIPE:
		case ENOMEM:
		case ENXIO:

			throw baseEx(ERRMODULE_IODISK, IODISKEX_WRITE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
	}

	#ifndef IO_DISK_WO_XEXEC
	performXExec(postExec);
	#endif
}

//-------------------------------------------------------------------

void
ioDisk::erase(unsigned long a_pos)
{
	char *empty = new char[outSize];

	memset(empty, 0, outSize);

	this->write(empty, a_pos);

	delete [] empty;
}

//-------------------------------------------------------------------

void
ioDisk::flush()
{
	if (fflush(file) != 0)
		throw baseEx(ERRMODULE_IODISK, IODISKEX_IO, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
}

//-------------------------------------------------------------------

dodoString
ioDisk::getPath() const
{
	return path;
}

//-------------------------------------------------------------------

void
ioDisk::readStream(char * const a_void,
					  unsigned long a_pos)
{
	#ifndef IO_DISK_WO_XEXEC
	operType = IODISK_OPER_READSTREAM;
	performXExec(preExec);
	#endif

	if (fileType == IODISK_FILETYPE_REG_FILE || fileType == IODISK_FILETYPE_TMP_FILE)
	{
		if (fseek(file, 0, SEEK_SET) == -1)
			throw baseEx(ERRMODULE_IODISK, IODISKEX_READSTREAM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

		for (unsigned long i(0); i < a_pos; ++i)
		{
			if (fgets(a_void, inSize, file) == NULL)
			{
				switch (errno)
				{
					case EIO:
					case EINTR:
					case EBADF:
					case EOVERFLOW:
					case ENOMEM:
					case ENXIO:

						throw baseEx(ERRMODULE_IODISK, IODISKEX_READSTREAM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
				}

				throw baseEx(ERRMODULE_IODISK, IODISKEX_READSTREAM, ERR_LIBDODO, IODISKEX_FILE_IS_SHORTER_THAN_GIVEN_POSITION, IODISKEX_FILE_IS_SHORTER_THAN_GIVEN_POSITION_STR, __LINE__, __FILE__, path);
			}
		}
	}

	memset(a_void, '\0', inSize);

	if (fgets(a_void, inSize, file) == NULL)
		switch (errno)
		{
			case EIO:
			case EINTR:
			case EBADF:
			case EOVERFLOW:
			case ENOMEM:
			case ENXIO:

				throw baseEx(ERRMODULE_IODISK, IODISKEX_READSTREAM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
		}

	buffer.assign(a_void);

	#ifndef IO_DISK_WO_XEXEC
	performXExec(postExec);
	#endif
}

//-------------------------------------------------------------------

void
ioDisk::readStreamString(dodoString &a_str,
							unsigned long a_pos)
{
	char *data = new char[inSize + 1];

	try
	{

		this->readStream(data, a_pos);
	}
	catch (...)
	{
		a_str.assign(data);
		delete [] data;

		throw baseEx(ERRMODULE_IODISK, IODISKEX_READSTREAMSTRING, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	a_str.assign(data);
	delete [] data;
}

//-------------------------------------------------------------------

void
ioDisk::writeStreamString(const dodoString &a_buf)
{
	return this->writeStream(a_buf.c_str());
}

//-------------------------------------------------------------------

void
ioDisk::writeStream(const char *const a_buf)
{
	buffer.assign(a_buf);

	#ifndef IO_DISK_WO_XEXEC
	operType = IODISK_OPER_WRITESTREAM;
	performXExec(preExec);
	#endif

	if (fseek(file, 0, SEEK_END) == -1)
		throw baseEx(ERRMODULE_IODISK, IODISKEX_WRITESTREAM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	if (buffer.size() > outSize)
		buffer.resize(outSize);

	if (fputs(buffer.c_str(), file) < 0)
		switch (errno)
		{
			case EFBIG:
			case EIO:
			case EINTR:
			case EBADF:
			case EOVERFLOW:
			case ENOSPC:
			case EPIPE:
			case ENOMEM:
			case ENXIO:

				throw baseEx(ERRMODULE_IODISK, IODISKEX_WRITESTREAM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
		}

	if (fputc('\n', file) < 0)
		switch (errno)
		{
			case EFBIG:
			case EIO:
			case EINTR:
			case EBADF:
			case EOVERFLOW:
			case ENOSPC:
			case EPIPE:
			case ENOMEM:
			case ENXIO:

				throw baseEx(ERRMODULE_IODISK, IODISKEX_WRITESTREAM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
		}

	#ifndef IO_DISK_WO_XEXEC
	performXExec(postExec);
	#endif
}

//-------------------------------------------------------------------

short 
ioDisk::getFileType() const
{
	return fileType;
}

//-------------------------------------------------------------------