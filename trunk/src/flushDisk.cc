/***************************************************************************
 *            flushDisk.cc
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

#include <libdodo/flushDisk.h>

using namespace dodo;

flushDisk::flushDisk(short type,
					 const dodoString &a_path) : over(false),
												 mode(OPENMODE_READ_WRITE),
												 fileType(type),
												 append(false),
												 path(a_path)
{
}

//-------------------------------------------------------------------

flushDisk::flushDisk(flushDisk &fd)
{
}

//-------------------------------------------------------------------

flushDisk::~flushDisk()
{
	if (opened)
		fclose(file);
}

//-------------------------------------------------------------------

int
flushDisk::getInDescriptor() const
{
	if (!opened)
		return -1;

	return fileno(file);
}

//-------------------------------------------------------------------

int
flushDisk::getOutDescriptor() const
{
	if (!opened)
		return -1;

	return fileno(file);
}

//-------------------------------------------------------------------

#ifndef FLUSH_DISK_WO_XEXEC

int
flushDisk::addPostExec(inExec func,
					   void   *data)
{
	return _addPostExec(func, (void *)this, XEXECOBJ_FLUSHDISK, data);
}

//-------------------------------------------------------------------

int
flushDisk::addPreExec(inExec func,
					  void   *data)
{
	return _addPreExec(func, (void *)this, XEXECOBJ_FLUSHDISK, data);
}

//-------------------------------------------------------------------

    #ifdef DL_EXT

int
flushDisk::addPostExec(const dodoString &module,
					   void             *data,
					   void             *toInit)
{
	return _addPostExec(module, (void *)this, XEXECOBJ_FLUSHDISK, data, toInit);
}

//-------------------------------------------------------------------

int
flushDisk::addPreExec(const dodoString &module,
					  void             *data,
					  void             *toInit)
{
	return _addPreExec(module, (void *)this, XEXECOBJ_FLUSHDISK, data, toInit);
}

//-------------------------------------------------------------------

xexecCounts
flushDisk::addExec(const dodoString &module,
				   void             *data,
				   void             *toInit)
{
	return _addExec(module, (void *)this, XEXECOBJ_FLUSHDISK, data, toInit);
}

    #endif

#endif

//-------------------------------------------------------------------

void
flushDisk::close()
{
    #ifndef FLUSH_DISK_WO_XEXEC
	operType = FLUSHDISK_OPER_CLOSE;
    #endif

    #ifndef FLUSH_DISK_WO_XEXEC
	performXExec(preExec);
    #endif

	if (opened)
	{
		if (fclose(file) != 0)
			throw baseEx(ERRMODULE_FLUSHDISK, FLUSHDISK_CLOSE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

        #ifndef FLUSH_DISK_WO_XEXEC
		performXExec(postExec);
        #endif

		opened = false;
	}
}

//-------------------------------------------------------------------

void
flushDisk::open(const dodoString &a_path)
{
    #ifndef FLUSH_DISK_WO_XEXEC
	operType = FLUSHDISK_OPER_OPEN;
	performXExec(preExec);
    #endif

	if (a_path.size() != 0 && a_path == path)
		path = a_path;

	if (opened)
		close();

	if (fileType == FILETYPE_TMP_FILE)
		file = tmpfile();
	else
	{
		if (path.size() == 0)
			throw baseEx(ERRMODULE_FLUSHDISK, FLUSHDISK_OPEN, ERR_LIBDODO, FLUSHDISK_WRONG_FILENAME, FLUSHDISK_WRONG_FILENAME_STR, __LINE__, __FILE__, path);
		else
		{
			struct stat st;
			bool exists(false);

			if (::lstat(path.c_str(), &st) == 1)
			{
				if (errno != ENOENT)
					throw baseEx(ERRMODULE_FLUSHDISK, FLUSHDISK_OPEN, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
			}
			else
				exists = true;

			if (fileType == FILETYPE_FIFO_FILE)
			{
				if (exists && !S_ISFIFO(st.st_mode))
					throw baseEx(ERRMODULE_FLUSHDISK, FLUSHDISK_OPEN, ERR_LIBDODO, FLUSHDISK_WRONG_FILENAME, FLUSHDISK_WRONG_FILENAME_STR, __LINE__, __FILE__, path);
				if (!exists)
					if (mkfifo(path.c_str(), flushDiskTools::getPermission(FILE_PERM)) == -1)
						throw baseEx(ERRMODULE_FLUSHDISK, FLUSHDISK_OPEN, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
			}
			else
			{
				if ((fileType == FILETYPE_REG_FILE || fileType == FILETYPE_TMP_FILE || FILETYPE_CHAR_FILE) && exists && !S_ISREG(st.st_mode) && !S_ISCHR(st.st_mode))
					throw baseEx(ERRMODULE_FLUSHDISK, FLUSHDISK_OPEN, ERR_LIBDODO, FLUSHDISK_WRONG_FILENAME, FLUSHDISK_WRONG_FILENAME_STR, __LINE__, __FILE__, path);
			}

			switch (mode)
			{
				case OPENMODE_READ_WRITE:

					file = fopen(path.c_str(), "r+");
					if (file == NULL)
						file = fopen(path.c_str(), "w+");

					break;

				case OPENMODE_READ_WRITE_TRUNCATE:

					file = fopen(path.c_str(), "w+");

					break;

				case OPENMODE_APPEND:

					file = fopen(path.c_str(), "a+");
					append = true;

					break;

				case OPENMODE_READ_ONLY:
				default:

					file = fopen(path.c_str(), "r");
			}
		}
	}

	if (file == NULL)
		throw baseEx(ERRMODULE_FLUSHDISK, FLUSHDISK_OPEN, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	flushDiskTools::chmod(path, FILE_PERM);

    #ifndef FLUSH_DISK_WO_XEXEC
	performXExec(postExec);
    #endif

	opened = true;
}

//-------------------------------------------------------------------

void
flushDisk::read(char * const a_void,
				unsigned long a_pos)
{
    #ifndef FLUSH_DISK_WO_XEXEC
	operType = FLUSHDISK_OPER_READ;
	performXExec(preExec);
    #endif

	if (fileType == FILETYPE_REG_FILE || fileType == FILETYPE_TMP_FILE)
		if (fseek(file, a_pos * inSize, SEEK_SET) == -1)
			throw baseEx(ERRMODULE_FLUSHDISK, FLUSHDISK_READ, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	memset(a_void, '\0', inSize);

	errno = 0;
	if (fileType == FILETYPE_REG_FILE || fileType == FILETYPE_TMP_FILE)
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

			throw baseEx(ERRMODULE_FLUSHDISK, FLUSHDISK_READ, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
	}

	buffer.assign(a_void, inSize);

    #ifndef FLUSH_DISK_WO_XEXEC
	performXExec(postExec);
    #endif
}

//-------------------------------------------------------------------

void
flushDisk::readString(dodoString &a_str,
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

		throw baseEx(ERRMODULE_FLUSHDISK, FLUSHDISK_READSTRING, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	a_str.assign(data, inSize);
	delete [] data;
}

//-------------------------------------------------------------------

void
flushDisk::writeString(const dodoString &a_buf,
					   unsigned long a_pos)
{
	this->write(a_buf.c_str(), a_pos);
}

//-------------------------------------------------------------------

void
flushDisk::write(const char *const a_buf,
				 unsigned long a_pos)
{
	buffer.assign(a_buf, outSize);

    #ifndef FLUSH_DISK_WO_XEXEC
	operType = FLUSHDISK_OPER_WRITE;
	performXExec(preExec);
    #endif

	if (fileType == FILETYPE_REG_FILE || fileType == FILETYPE_TMP_FILE)
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
					throw baseEx(ERRMODULE_FLUSHDISK, FLUSHDISK_WRITE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
				}

				read_bytes = fread(t_buf, outSize, 1, file);

				delete [] t_buf;

				if (read_bytes != 0)
					throw baseEx(ERRMODULE_FLUSHDISK, FLUSHDISK_WRITE, ERR_LIBDODO, FLUSHDISK_CANNOT_OVEWRITE, FLUSHDISK_CANNOT_OVEWRITE_STR, __LINE__, __FILE__, path);
			}

			if (fseek(file, a_pos, SEEK_SET) == -1)
				throw baseEx(ERRMODULE_FLUSHDISK, FLUSHDISK_WRITE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
		}
		else
		if (fseek(file, 0, SEEK_END) == -1)
			throw baseEx(ERRMODULE_FLUSHDISK, FLUSHDISK_WRITE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
	}

	errno = 0;
	if (fileType == FILETYPE_REG_FILE || fileType == FILETYPE_TMP_FILE)
		fwrite(buffer.c_str(), outSize, 1, file);
	else
        #ifndef FAST
	if (fileType == FILETYPE_FIFO_FILE)
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

			throw baseEx(ERRMODULE_FLUSHDISK, FLUSHDISK_WRITE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
	}

    #ifndef FLUSH_DISK_WO_XEXEC
	performXExec(postExec);
    #endif
}

//-------------------------------------------------------------------

void
flushDisk::erase(unsigned long a_pos)
{
	char *empty = new char[outSize];

	memset(empty, 0, outSize);

	this->write(empty, a_pos);

	delete [] empty;
}

//-------------------------------------------------------------------

void
flushDisk::flush()
{
	if (fflush(file) != 0)
		throw baseEx(ERRMODULE_FLUSHDISK, FLUSHDISK_FLUSH, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
}

//-------------------------------------------------------------------

dodoString
flushDisk::getPath() const
{
	return path;
}

//-------------------------------------------------------------------

void
flushDisk::readStream(char * const a_void,
					  unsigned long a_pos)
{
    #ifndef FLUSH_DISK_WO_XEXEC
	operType = FLUSHDISK_OPER_READSTREAM;
	performXExec(preExec);
    #endif

	if (fileType == FILETYPE_REG_FILE || fileType == FILETYPE_TMP_FILE)
	{
		if (fseek(file, 0, SEEK_SET) == -1)
			throw baseEx(ERRMODULE_FLUSHDISK, FLUSHDISK_READSTREAM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

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

						throw baseEx(ERRMODULE_FLUSHDISK, FLUSHDISK_READSTREAM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
				}

				throw baseEx(ERRMODULE_FLUSHDISK, FLUSHDISK_READSTREAM, ERR_LIBDODO, FLUSHDISK_FILE_IS_SHORTER_THAN_GIVEN_POSITION, FLUSHDISK_FILE_IS_SHORTER_THAN_GIVEN_POSITION_STR, __LINE__, __FILE__, path);
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

				throw baseEx(ERRMODULE_FLUSHDISK, FLUSHDISK_READSTREAM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
		}

	buffer.assign(a_void);

    #ifndef FLUSH_DISK_WO_XEXEC
	performXExec(postExec);
    #endif
}

//-------------------------------------------------------------------

void
flushDisk::readStreamString(dodoString &a_str,
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

		throw baseEx(ERRMODULE_FLUSHDISK, FLUSHDISK_READSTREAMSTRING, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	a_str.assign(data);
	delete [] data;
}

//-------------------------------------------------------------------

void
flushDisk::writeStreamString(const dodoString &a_buf)
{
	return this->writeStream(a_buf.c_str());
}

//-------------------------------------------------------------------

void
flushDisk::writeStream(const char *const a_buf)
{
	buffer.assign(a_buf);

    #ifndef FLUSH_DISK_WO_XEXEC
	operType = FLUSHDISK_OPER_WRITESTREAM;
	performXExec(preExec);
    #endif

	if (fseek(file, 0, SEEK_END) == -1)
		throw baseEx(ERRMODULE_FLUSHDISK, FLUSHDISK_WRITESTREAM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

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

				throw baseEx(ERRMODULE_FLUSHDISK, FLUSHDISK_WRITESTREAM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
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

				throw baseEx(ERRMODULE_FLUSHDISK, FLUSHDISK_WRITESTREAM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
		}

    #ifndef FLUSH_DISK_WO_XEXEC
	performXExec(postExec);
    #endif
}

//-------------------------------------------------------------------