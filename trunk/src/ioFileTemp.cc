/***************************************************************************
 *            ioFileTemp.cc
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

#include <libdodo/ioFileTemp.h>

using namespace dodo::io::file;

temp::temp() : overwrite(false),
						 pos(0),
						 blockOffset(true),
						 append(false),
						 handler(NULL)
{
#ifndef IO_WO_XEXEC

	collectedData.setExecObject(XEXEC_OBJECT_IOFILETEMP);

#endif
}

//-------------------------------------------------------------------

temp::temp(const temp &fd) : overwrite(fd.overwrite),
							 pos(fd.pos),
							 blockOffset(fd.blockOffset),
							 append(fd.append),
							 handler(NULL)

{
#ifndef IO_WO_XEXEC

	collectedData.setExecObject(XEXEC_OBJECT_IOFILETEMP);

#endif

	inSize = fd.inSize;
	outSize = fd.outSize;

	if (fd.opened)
	{
		int oldDesc, newDesc;

		oldDesc = fileno(fd.handler);

		newDesc = dup(oldDesc);

		handler = fdopen(newDesc, "r+");

		if (handler != NULL)
			opened = true;
	}
}

//-------------------------------------------------------------------

temp::~temp()
{
	if (opened)
		fclose(handler);
}

//-------------------------------------------------------------------

int
temp::getInDescriptor() const
{
	protector pg(this);

	if (!opened)
		throw exception::basic(exception::ERRMODULE_IOFILETEMP, TEMPEX_GETINDESCRIPTOR, exception::ERRNO_LIBDODO, TEMPEX_NOTOPENED, IOFILETEMPEX_NOTOPENED_STR, __LINE__, __FILE__);

	return fileno(handler);
}

//-------------------------------------------------------------------

int
temp::getOutDescriptor() const
{
	protector pg(this);

	if (!opened)
		throw exception::basic(exception::ERRMODULE_IOFILETEMP, TEMPEX_GETOUTDESCRIPTOR, exception::ERRNO_LIBDODO, TEMPEX_NOTOPENED, IOFILETEMPEX_NOTOPENED_STR, __LINE__, __FILE__);

	return fileno(handler);
}

//-------------------------------------------------------------------

void
temp::clone(const temp &fd)
{
	protector pg(this);

	if (opened)
	{
		if (fclose(handler) != 0)
			throw exception::basic(exception::ERRMODULE_IOFILETEMP, TEMPEX_CLONE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		handler = NULL;

		opened = false;
	}

	overwrite = fd.overwrite;
	pos = fd.pos;
	blockOffset = fd.blockOffset;
	append = fd.append;
	inSize = fd.inSize;
	outSize = fd.outSize;

	if (fd.opened)
	{
		int oldDesc, newDesc;

		oldDesc = fileno(fd.handler);
		if (oldDesc == -1)
			throw exception::basic(exception::ERRMODULE_IOFILETEMP, TEMPEX_CLONE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		newDesc = dup(oldDesc);
		if (newDesc == -1)
			throw exception::basic(exception::ERRMODULE_IOFILETEMP, TEMPEX_CLONE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		handler = fdopen(newDesc, "r+");

		if (handler == NULL)
			throw exception::basic(exception::ERRMODULE_IOFILETEMP, TEMPEX_CLONE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		opened = true;
	}
}

//-------------------------------------------------------------------

void
temp::close()
{
	protector pg(this);

#ifndef IO_WO_XEXEC
	operType = TEMP_OPERATION_CLOSE;
	performXExec(preExec);
#endif

	if (opened)
	{
		if (fclose(handler) != 0)
			throw exception::basic(exception::ERRMODULE_IOFILETEMP, TEMPEX_CLOSE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		handler = NULL;

		opened = false;
	}

#ifndef IO_WO_XEXEC
	performXExec(postExec);
#endif
}

//-------------------------------------------------------------------

void
temp::open()
{
	protector pg(this);

#ifndef IO_WO_XEXEC
	operType = TEMP_OPERATION_OPEN;
	performXExec(preExec);
#endif

	if (opened)
	{
		if (fclose(handler) != 0)
			throw exception::basic(exception::ERRMODULE_IOFILETEMP, TEMPEX_OPEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		opened = false;
	}

	handler = tmpfile();
	if (handler == NULL)
		throw exception::basic(exception::ERRMODULE_IOFILETEMP, TEMPEX_OPEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	opened = true;

#ifndef IO_WO_XEXEC
	performXExec(postExec);
#endif
}

//-------------------------------------------------------------------

void
temp::_read(char * const a_data)
{
	if (!opened)
		throw exception::basic(exception::ERRMODULE_IOFILETEMP, TEMPEX__READ, exception::ERRNO_LIBDODO, TEMPEX_NOTOPENED, IOFILETEMPEX_NOTOPENED_STR, __LINE__, __FILE__);

	unsigned long pos = blockOffset?this->pos * inSize:this->pos;

	if (fseek(handler, pos * inSize, SEEK_SET) == -1)
		throw exception::basic(exception::ERRMODULE_IOFILETEMP, TEMPEX__READ, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

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
				throw exception::basic(exception::ERRMODULE_IOFILETEMP, TEMPEX__READ, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}

		break;
	}
}

//-------------------------------------------------------------------

void
temp::_write(const char *const a_data)
{
	if (!opened)
		throw exception::basic(exception::ERRMODULE_IOFILETEMP, TEMPEX__WRITE, exception::ERRNO_LIBDODO, TEMPEX_NOTOPENED, IOFILETEMPEX_NOTOPENED_STR, __LINE__, __FILE__);

	if (append)
	{
		if (fseek(handler, 0, SEEK_END) == -1)
			throw exception::basic(exception::ERRMODULE_IOFILETEMP, TEMPEX__WRITE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}
	else
	{
		unsigned long pos = blockOffset?this->pos * outSize:this->pos;
		if (!overwrite)
		{
			if (fseek(handler, pos, SEEK_SET) == -1)
				throw exception::basic(exception::ERRMODULE_IOFILETEMP, TEMPEX__WRITE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

			char *t_buf = new char[outSize];

			size_t read = fread(t_buf, outSize, 1, handler);

			delete [] t_buf;

			if (read != 0)
				throw exception::basic(exception::ERRMODULE_IOFILETEMP, TEMPEX__WRITE, exception::ERRNO_LIBDODO, TEMPEX_CANNOTOVEWRITE, IOFILETEMPEX_CANNOTOVEWRITE_STR, __LINE__, __FILE__);
		}

		if (fseek(handler, pos, SEEK_SET) == -1)
			throw exception::basic(exception::ERRMODULE_IOFILETEMP, TEMPEX__WRITE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
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
				throw exception::basic(exception::ERRMODULE_IOFILETEMP, TEMPEX__WRITE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}

		break;
	}
}

//-------------------------------------------------------------------

void
temp::erase()
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
temp::flush()
{
	protector pg(this);

	if (!opened)
		throw exception::basic(exception::ERRMODULE_IOFILETEMP, TEMPEX_FLUSH, exception::ERRNO_LIBDODO, TEMPEX_NOTOPENED, IOFILETEMPEX_NOTOPENED_STR, __LINE__, __FILE__);

	if (fflush(handler) != 0)
		throw exception::basic(exception::ERRMODULE_IOFILETEMP, TEMPEX_FLUSH, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

unsigned long
temp::_readStream(char * const a_data)
{
	if (!opened)
		throw exception::basic(exception::ERRMODULE_IOFILETEMP, TEMPEX__READSTREAM, exception::ERRNO_LIBDODO, TEMPEX_NOTOPENED, IOFILETEMPEX_NOTOPENED_STR, __LINE__, __FILE__);

	if (blockOffset)
	{
		if (fseek(handler, 0, SEEK_SET) == -1)
			throw exception::basic(exception::ERRMODULE_IOFILETEMP, TEMPEX__READSTREAM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		for (unsigned long i = 0; i < pos; ++i)
		{
			if (fgets(a_data, inSize, handler) == NULL)
			{
				switch (errno)
				{
					case EIO:
					case EINTR:
					case EBADF:
					case EOVERFLOW:
					case ENOMEM:
					case ENXIO:

						throw exception::basic(exception::ERRMODULE_IOFILETEMP, TEMPEX__READSTREAM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
				}

				throw exception::basic(exception::ERRMODULE_IOFILETEMP, TEMPEX__READSTREAM, exception::ERRNO_LIBDODO, TEMPEX_FILEISSHORTERTHANGIVENPOSITION, IOFILETEMPEX_FILEISSHORTERTHANGIVENPOSITION_STR, __LINE__, __FILE__);
			}
		}
	}
	else
		if (fseek(handler, pos, SEEK_SET) == -1)
			throw exception::basic(exception::ERRMODULE_IOFILETEMP, TEMPEX__READSTREAM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	memset(a_data, '\0', inSize);

	while (true)
	{
		if (fgets(a_data, inSize, handler) == NULL)
		{
			if (errno == EINTR)
				continue;

			if (errno == EAGAIN)
				break;

			if (ferror(handler) != 0)
				throw exception::basic(exception::ERRMODULE_IOFILETEMP, TEMPEX__READSTREAM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}

		break;
	}

	return strlen(a_data);
}

//-------------------------------------------------------------------

void
temp::_writeStream(const char *const a_data)
{
	if (!opened)
		throw exception::basic(exception::ERRMODULE_IOFILETEMP, TEMPEX__WRITESTREAM, exception::ERRNO_LIBDODO, TEMPEX_NOTOPENED, IOFILETEMPEX_NOTOPENED_STR, __LINE__, __FILE__);

	//if (append)
	{
		if (fseek(handler, 0, SEEK_END) == -1)
			throw exception::basic(exception::ERRMODULE_IOFILETEMP, TEMPEX__WRITESTREAM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}
	/*else//!blockOffset ||
		if (fseek(handler, pos, SEEK_END) == -1)
			throw exception::basic(exception::ERRMODULE_IOFILETEMP, TEMPEX__WRITESTREAM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);*/

	unsigned long _outSize = outSize;

	unsigned int bufSize = strlen(a_data);

	if (bufSize < _outSize)
		_outSize = bufSize;

	while (true)
	{
		if (fwrite(a_data, _outSize, 1, handler) == 0)
		{
			if (errno == EINTR)
				continue;

			if (ferror(handler) != 0)
				throw exception::basic(exception::ERRMODULE_IOFILETEMP, TEMPEX__WRITESTREAM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}

		break;
	}
}

//-------------------------------------------------------------------

