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
							 fileType(a_fileType),
							 pos(0)
{
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
	guard pg(this);

	if (!opened)
		return -1;

	return fileno(file);
}

//-------------------------------------------------------------------

int
ioDisk::getOutDescriptor() const
{
	guard pg(this);

	if (!opened)
		return -1;

	return fileno(file);
}

//-------------------------------------------------------------------

#ifndef IODISK_WO_XEXEC

int
ioDisk::addPostExec(inExec func,
					void   *data)
{
	return _addPostExec(func, (void *)&collectedData, XEXEC_OBJECT_IODISK, data);
}

//-------------------------------------------------------------------

int
ioDisk::addPreExec(inExec func,
				   void   *data)
{
	return _addPreExec(func, (void *)&collectedData, XEXEC_OBJECT_IODISK, data);
}

//-------------------------------------------------------------------

#ifdef DL_EXT

int
ioDisk::addPostExec(const dodoString &module,
					void             *data,
					void             *toInit)
{
	return _addPostExec(module, (void *)&collectedData, XEXEC_OBJECT_IODISK, data, toInit);
}

//-------------------------------------------------------------------

int
ioDisk::addPreExec(const dodoString &module,
				   void             *data,
				   void             *toInit)
{
	return _addPreExec(module, (void *)&collectedData, XEXEC_OBJECT_IODISK, data, toInit);
}

//-------------------------------------------------------------------

__xexecCounts
ioDisk::addExec(const dodoString &module,
				void             *data,
				void             *toInit)
{
	return _addExec(module, (void *)&collectedData, XEXEC_OBJECT_IODISK, data, toInit);
}

#endif

#endif

//-------------------------------------------------------------------

void
ioDisk::close()
{
	guard pg(this);

#ifndef IODISK_WO_XEXEC
	operType = IODISK_OPERATION_CLOSE;
#endif

#ifndef IODISK_WO_XEXEC
	performXExec(preExec);
#endif

	if (opened)
	{
		if (fclose(file) != 0)
			throw baseEx(ERRMODULE_IODISK, IODISKEX_CLOSE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

		opened = false;
	}

#ifndef IODISK_WO_XEXEC
	performXExec(postExec);
#endif
}

//-------------------------------------------------------------------

void
ioDisk::open(const dodoString &a_path,
			 short a_fileType,
			 short mode)
{
	guard pg(this);

#ifndef IODISK_WO_XEXEC
	operType = IODISK_OPERATION_OPEN;
	performXExec(preExec);
#endif

	path = a_path;
	fileType = a_fileType;

	if (opened)
	{
		if (fclose(file) != 0)
			throw baseEx(ERRMODULE_IODISK, IODISKEX_OPEN, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

		opened = false;
	}

	if (fileType == IODISK_FILETYPE_TMP_FILE)
		file = tmpfile();
	else
	{
		if (path.size() == 0)
			throw baseEx(ERRMODULE_IODISK, IODISKEX_OPEN, ERR_LIBDODO, IODISKEX_WRONGFILENAME, IODISKEX_WRONGFILENAME_STR, __LINE__, __FILE__, path);
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
					throw baseEx(ERRMODULE_IODISK, IODISKEX_OPEN, ERR_LIBDODO, IODISKEX_WRONGFILENAME, IODISKEX_WRONGFILENAME_STR, __LINE__, __FILE__, path);
				if (!exists)
					ioDiskTools::mkfifo(path, DEFAULT_FILE_PERM);
			}
			else
			{
				if ((fileType == IODISK_FILETYPE_REG_FILE || fileType == IODISK_FILETYPE_TMP_FILE || fileType ==  IODISK_FILETYPE_CHAR_FILE) && exists && !S_ISREG(st.st_mode) && !S_ISCHR(st.st_mode))
					throw baseEx(ERRMODULE_IODISK, IODISKEX_OPEN, ERR_LIBDODO, IODISKEX_WRONGFILENAME, IODISKEX_WRONGFILENAME_STR, __LINE__, __FILE__, path);
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

	ioDiskTools::chmod(path, DEFAULT_FILE_PERM);

#ifndef IODISK_WO_XEXEC
	performXExec(postExec);
#endif

	opened = true;
}

//-------------------------------------------------------------------

void
ioDisk::_read(char * const a_void)
{
	if (fileType == IODISK_FILETYPE_REG_FILE || fileType == IODISK_FILETYPE_TMP_FILE)
		if (fseek(file, pos * inSize, SEEK_SET) == -1)
			throw baseEx(ERRMODULE_IODISK, IODISKEX__READ, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	memset(a_void, '\0', inSize);

	unsigned long sent_received = 0;

	unsigned long batch = 0, n;

	while (batch < inSize)
	{
		while (true)
		{
			if ((n = fread(a_void + sent_received, 1, inSize, file)) == 0)
			{
				if (errno == EINTR)
					continue;

				if (ferror(file) != 0)
					throw baseEx(ERRMODULE_IODISK, IODISKEX__READ, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
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

void
ioDisk::read(char * const a_void)
{
	guard pg(this);

#ifndef IODISK_WO_XEXEC
	operType = IODISK_OPERATION_READ;
	performXExec(preExec);

	buffer.reserve(inSize);
#endif

#ifndef IODISK_WO_XEXEC
	try
	{
		_read(a_void);
	}
	catch (...)
	{
		buffer.clear();

		throw;
	}
#else
	_read(a_void, a_pos);
#endif

#ifndef IODISK_WO_XEXEC
	buffer.assign(a_void, inSize);

	performXExec(postExec);

	strncpy(a_void, buffer.c_str(), buffer.size() > inSize ? inSize : buffer.size());
	buffer.clear();
#endif
}

//-------------------------------------------------------------------

void
ioDisk::readString(dodoString &a_str)
{
	guard pg(this);

#ifndef IODISK_WO_XEXEC
	operType = IODISK_OPERATION_READSTRING;
	performXExec(preExec);

	buffer.reserve(inSize);
#endif

	char *data = new char[inSize];

	try
	{
		_read(data);
	}
	catch (...)
	{
		delete [] data;

#ifndef IODISK_WO_XEXEC
		buffer.clear();
#endif

		throw;
	}

#ifndef IODISK_WO_XEXEC
	buffer.assign(data, inSize);
	delete [] data;

	performXExec(postExec);

	a_str = buffer;
	buffer.clear();
#else
	a_str.assign(data, inSize);
	delete [] data;
#endif
}

//-------------------------------------------------------------------

void
ioDisk::writeString(const dodoString &a_buf)
{
	guard pg(this);

#ifndef IODISK_WO_XEXEC
	buffer = a_buf;

	operType = IODISK_OPERATION_WRITESTRING;
	performXExec(preExec);

	try
	{
		_write(buffer.c_str());
	}
	catch (...)
	{
		buffer.clear();

		throw;
	}
#else
	_write(a_buf.c_str());
#endif


#ifndef IODISK_WO_XEXEC
	performXExec(postExec);

	buffer.clear();
#endif

	this->write(a_buf.c_str());
}

//-------------------------------------------------------------------

void
ioDisk::_write(const char *const a_buf)
{
	unsigned long pos = this->pos;
	
	if (fileType == IODISK_FILETYPE_REG_FILE || fileType == IODISK_FILETYPE_TMP_FILE)
	{
		pos *= outSize;

		if (!append)
		{
			if (!over)
			{
				size_t read_bytes(0);
				char *t_buf = new char[outSize];

				if (fseek(file, pos, SEEK_SET) == -1)
				{
					delete [] t_buf;

					throw baseEx(ERRMODULE_IODISK, IODISKEX__WRITE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
				}

				read_bytes = fread(t_buf, outSize, 1, file);

				delete [] t_buf;

				if (read_bytes != 0)
					throw baseEx(ERRMODULE_IODISK, IODISKEX__WRITE, ERR_LIBDODO, IODISKEX_CANNOTOVEWRITE, IODISKEX_CANNOTOVEWRITE_STR, __LINE__, __FILE__, path);
			}

			if (fseek(file, pos, SEEK_SET) == -1)
				throw baseEx(ERRMODULE_IODISK, IODISKEX__WRITE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
		}
		else
		if (fseek(file, 0, SEEK_END) == -1)
			throw baseEx(ERRMODULE_IODISK, IODISKEX__WRITE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
	}

	unsigned long sent_received = 0;

	unsigned long batch = 0, n;

	while (batch < outSize)
	{
		while (true)
		{
			if ((n = fwrite(a_buf + sent_received, 1, outSize, file)) == 0)
			{
				if (errno == EINTR)
					continue;

				if (ferror(file) != 0)
					throw baseEx(ERRMODULE_IODISK, IODISKEX__WRITE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
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

void
ioDisk::write(const char *const a_buf)
{
	guard pg(this);

#ifndef IODISK_WO_XEXEC
	buffer.assign(a_buf, outSize);

	operType = IODISK_OPERATION_WRITE;
	performXExec(preExec);

	try
	{
		_write(buffer.c_str());
	}
	catch (...)
	{
		buffer.clear();

		throw;
	}
#else
	_write(a_buf);
#endif


#ifndef IODISK_WO_XEXEC
	performXExec(postExec);

	buffer.clear();
#endif
}

//-------------------------------------------------------------------

void
ioDisk::erase()
{
	guard pg(this);

	char *empty = new char[outSize];

	memset(empty, 0, outSize);

	_write(empty);

	delete [] empty;
}

//-------------------------------------------------------------------

void
ioDisk::flush()
{
	guard pg(this);

	if (fflush(file) != 0)
		throw baseEx(ERRMODULE_IODISK, IODISKEX_FLUSH, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
}

//-------------------------------------------------------------------

dodoString
ioDisk::getPath() const
{
	guard pg(this);

	return path;
}

//-------------------------------------------------------------------

void
ioDisk::_readStream(char * const a_void)
{
	unsigned long pos = this->pos;
	
	if (fileType == IODISK_FILETYPE_REG_FILE || fileType == IODISK_FILETYPE_TMP_FILE)
	{
		if (fseek(file, 0, SEEK_SET) == -1)
			throw baseEx(ERRMODULE_IODISK, IODISKEX__READSTREAM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

		for (unsigned long i(0); i < pos; ++i)
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

						throw baseEx(ERRMODULE_IODISK, IODISKEX__READSTREAM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
				}

				throw baseEx(ERRMODULE_IODISK, IODISKEX__READSTREAM, ERR_LIBDODO, IODISKEX_FILEISSHORTERTHANGIVENPOSITION, IODISKEX_FILEISSHORTERTHANGIVENPOSITION_STR, __LINE__, __FILE__, path);
			}
		}
	}

	memset(a_void, '\0', inSize);

	while (true)
	{
		if (fgets(a_void, inSize, file) == NULL)
		{
			if (errno == EINTR)
				continue;

			if (ferror(file) != 0)
				throw baseEx(ERRMODULE_IODISK, IODISKEX__READSTREAM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}

		break;
	}
}

//-------------------------------------------------------------------

void
ioDisk::readStream(char * const a_void)
{
	guard pg(this);

#ifndef IODISK_WO_XEXEC
	operType = IODISK_OPERATION_READSTREAM;
	performXExec(preExec);
#endif

	_readStream(a_void);

#ifndef IODISK_WO_XEXEC
	buffer = a_void;

	performXExec(postExec);

	if (buffer.size() > inSize)
		buffer.resize(inSize);
	strcpy(a_void, buffer.c_str());
	buffer.clear();
#endif
}

//-------------------------------------------------------------------

void
ioDisk::readStreamString(dodoString &a_str)
{
	guard pg(this);

#ifndef IODISK_WO_XEXEC
	operType = IODISK_OPERATION_READSTREAMSTRING;
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

#ifndef IODISK_WO_XEXEC
	buffer = data;
	delete [] data;

	performXExec(postExec);

	a_str = buffer;
	buffer.clear();
#else
	a_str = data;
	delete [] data;
#endif
}

//-------------------------------------------------------------------

void
ioDisk::_writeStream(const char *const a_buf)
{
	if (fseek(file, 0, SEEK_END) == -1)
		throw baseEx(ERRMODULE_IODISK, IODISKEX__WRITESTREAM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
	
	unsigned long sent_received = 0;

	unsigned long batch = 0, n;

	while (batch < outSize)
	{
		while (true)
		{
			if ((n = fwrite(a_buf + sent_received, 1, outSize, file)) == 0)
			{
				if (errno == EINTR)
					continue;

				if (ferror(file) != 0)
					throw baseEx(ERRMODULE_IODISK, IODISKEX__WRITESTREAM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
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
ioDisk::writeStreamString(const dodoString &a_buf)
{
	guard pg(this);

	unsigned long _outSize = outSize;

#ifndef IODISK_WO_XEXEC
	buffer = a_buf;

	operType = IODISK_OPERATION_WRITESTREAMSTRING;
	performXExec(preExec);

	try
	{
		outSize = buffer.size();

		_writeStream(buffer.c_str());

		outSize = _outSize;
	}
	catch (...)
	{
		outSize = _outSize;

		buffer.clear();

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

#ifndef IODISK_WO_XEXEC
	performXExec(postExec);

	buffer.clear();
#endif
}

//-------------------------------------------------------------------

void
ioDisk::writeStream(const char *const a_buf)
{
	guard pg(this);

	unsigned long _outSize = outSize;

#ifndef IODISK_WO_XEXEC
	buffer = a_buf;

	operType = IODISK_OPERATION_WRITESTREAM;
	performXExec(preExec);

	try
	{
		outSize = buffer.size();

		_write(buffer.c_str());

		outSize = _outSize;
	}
	catch (...)
	{
		outSize = _outSize;

		buffer.clear();

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

#ifndef IODISK_WO_XEXEC
	performXExec(postExec);

	buffer.clear();
#endif
}

//-------------------------------------------------------------------

short
ioDisk::getFileType() const
{
	guard pg(this);

	return fileType;
}

//-------------------------------------------------------------------
