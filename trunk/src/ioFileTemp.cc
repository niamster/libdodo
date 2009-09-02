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

#include <libdodo/directives.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#include "ioFile.inline"

#include <libdodo/ioFileTemp.h>
#include <libdodo/ioFileTempEx.h>
#include <libdodo/types.h>
#include <libdodo/ioChannel.h>
#include <libdodo/ioBlockChannel.h>
#include <libdodo/pcSyncProtector.h>

using namespace dodo::io::file;

temp::temp(bool  open,
		   short protection) : block::channel(protection),
							   block(true),
							   append(false),
							   overwrite(false),
							   handle(new io::__file__)
{
#ifndef IO_WO_XEXEC
	collectedData.setExecObject(xexec::OBJECT_IOFILETEMP);
#endif

	if (open) {
		handle->file = tmpfile();
		if (handle->file == NULL) {
			delete handle;

			throw exception::basic(exception::MODULE_IOFILETEMP, TEMPEX_TEMP, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}
	}
}

//-------------------------------------------------------------------

temp::temp(const temp &fd) : block::channel(protection),
							 block(fd.block),
							 append(fd.append),
							 overwrite(fd.overwrite),
							 handle(new io::__file__)

{
#ifndef IO_WO_XEXEC
	collectedData.setExecObject(xexec::OBJECT_IOFILETEMP);
#endif

	pos = fd.pos;
	blockSize = fd.blockSize;
	blockSize = fd.blockSize;

	if (fd.handle->file != NULL) {
		int oldDesc, newDesc;

		oldDesc = fileno(fd.handle->file);
		if (oldDesc == -1) {
			delete handle;

			throw exception::basic(exception::MODULE_IOFILETEMP, TEMPEX_TEMP, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}

		newDesc = dup(oldDesc);
		if (newDesc == -1) {
			delete handle;

			throw exception::basic(exception::MODULE_IOFILETEMP, TEMPEX_TEMP, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}

		handle->file = fdopen(newDesc, "r+");

		if (handle->file == NULL) {
			delete handle;

			throw exception::basic(exception::MODULE_IOFILETEMP, TEMPEX_TEMP, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}
	}
}

//-------------------------------------------------------------------

temp::~temp()
{
	if (handle->file != NULL)
		fclose(handle->file);

	delete handle;
}

//-------------------------------------------------------------------

int
temp::inDescriptor() const
{
	pc::sync::protector pg(keeper);

	if (handle->file == NULL)
		throw exception::basic(exception::MODULE_IOFILETEMP, TEMPEX_INDESCRIPTOR, exception::ERRNO_LIBDODO, TEMPEX_NOTOPENED, IOFILETEMPEX_NOTOPENED_STR, __LINE__, __FILE__);

	return fileno(handle->file);
}

//-------------------------------------------------------------------

int
temp::outDescriptor() const
{
	pc::sync::protector pg(keeper);

	if (handle->file == NULL)
		throw exception::basic(exception::MODULE_IOFILETEMP, TEMPEX_OUTDESCRIPTOR, exception::ERRNO_LIBDODO, TEMPEX_NOTOPENED, IOFILETEMPEX_NOTOPENED_STR, __LINE__, __FILE__);

	return fileno(handle->file);
}

//-------------------------------------------------------------------

void
temp::clone(const temp &fd)
{
	pc::sync::protector pg(keeper);

	if (handle->file != NULL) {
		if (fclose(handle->file) != 0)
			throw exception::basic(exception::MODULE_IOFILETEMP, TEMPEX_CLONE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		handle->file = NULL;
	}

	overwrite = fd.overwrite;
	pos = fd.pos;
	block = fd.block;
	append = fd.append;
	blockSize = fd.blockSize;
	blockSize = fd.blockSize;

	if (fd.handle->file != NULL) {
		int oldDesc, newDesc;

		oldDesc = fileno(fd.handle->file);
		if (oldDesc == -1)
			throw exception::basic(exception::MODULE_IOFILETEMP, TEMPEX_CLONE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		newDesc = dup(oldDesc);
		if (newDesc == -1)
			throw exception::basic(exception::MODULE_IOFILETEMP, TEMPEX_CLONE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		handle->file = fdopen(newDesc, "r+");

		if (handle->file == NULL)
			throw exception::basic(exception::MODULE_IOFILETEMP, TEMPEX_CLONE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}
}

//-------------------------------------------------------------------

void
temp::close()
{
	pc::sync::protector pg(keeper);

#ifndef IO_WO_XEXEC
	operType = OPERATION_CLOSE;
	performPreExec();
#endif

	if (handle->file != NULL) {
		if (fclose(handle->file) != 0)
			throw exception::basic(exception::MODULE_IOFILETEMP, TEMPEX_CLOSE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		handle->file = NULL;
	}

#ifndef IO_WO_XEXEC
	performPostExec();
#endif
}

//-------------------------------------------------------------------

void
temp::open()
{
	pc::sync::protector pg(keeper);

#ifndef IO_WO_XEXEC
	operType = OPERATION_OPEN;
	performPreExec();
#endif

	if (handle->file != NULL) {
		if (fclose(handle->file) != 0)
			throw exception::basic(exception::MODULE_IOFILETEMP, TEMPEX_OPEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		handle->file = NULL;
	}

	handle->file = tmpfile();
	if (handle->file == NULL)
		throw exception::basic(exception::MODULE_IOFILETEMP, TEMPEX_OPEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

#ifndef IO_WO_XEXEC
	performPostExec();
#endif
}

//-------------------------------------------------------------------

void
temp::_read(char * const a_data) const
{
	if (handle->file == NULL)
		throw exception::basic(exception::MODULE_IOFILETEMP, TEMPEX__READ, exception::ERRNO_LIBDODO, TEMPEX_NOTOPENED, IOFILETEMPEX_NOTOPENED_STR, __LINE__, __FILE__);

	unsigned long pos = block ? this->pos * blockSize : this->pos;

	if (fseek(handle->file, pos * blockSize, SEEK_SET) == -1)
		throw exception::basic(exception::MODULE_IOFILETEMP, TEMPEX__READ, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	while (true) {
		if (fread(a_data, blockSize, 1, handle->file) == 0) {
			if (feof(handle->file) != 0 || errno == EAGAIN)
				break;

			if (errno == EINTR)
				continue;

			if (ferror(handle->file) != 0)
				throw exception::basic(exception::MODULE_IOFILETEMP, TEMPEX__READ, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}

		break;
	}
}

//-------------------------------------------------------------------

void
temp::_write(const char *const a_data) const
{
	if (handle->file == NULL)
		throw exception::basic(exception::MODULE_IOFILETEMP, TEMPEX__WRITE, exception::ERRNO_LIBDODO, TEMPEX_NOTOPENED, IOFILETEMPEX_NOTOPENED_STR, __LINE__, __FILE__);

	if (append) {
		if (fseek(handle->file, 0, SEEK_END) == -1)
			throw exception::basic(exception::MODULE_IOFILETEMP, TEMPEX__WRITE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	} else {
		unsigned long pos = block ? this->pos * blockSize : this->pos;
		if (!overwrite) {
			if (fseek(handle->file, pos, SEEK_SET) == -1)
				throw exception::basic(exception::MODULE_IOFILETEMP, TEMPEX__WRITE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

			char *t_buf = new char[blockSize];

			size_t read = fread(t_buf, blockSize, 1, handle->file);

			delete [] t_buf;

			if (read != 0)
				throw exception::basic(exception::MODULE_IOFILETEMP, TEMPEX__WRITE, exception::ERRNO_LIBDODO, TEMPEX_CANNOTOVEWRITE, IOFILETEMPEX_CANNOTOVEWRITE_STR, __LINE__, __FILE__);
		}

		if (fseek(handle->file, pos, SEEK_SET) == -1)
			throw exception::basic(exception::MODULE_IOFILETEMP, TEMPEX__WRITE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	while (true) {
		if (fwrite(a_data, blockSize, 1, handle->file) == 0) {
			if (errno == EINTR)
				continue;

			if (errno == EAGAIN)
				break;

			if (ferror(handle->file) != 0)
				throw exception::basic(exception::MODULE_IOFILETEMP, TEMPEX__WRITE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}

		break;
	}
}

//-------------------------------------------------------------------

void
temp::erase()
{
	pc::sync::protector pg(keeper);

	char *empty = new char[blockSize];

	memset(empty, 0, blockSize);

	bool _overwrite = overwrite;
	overwrite = true;

	try {
		_write(empty);

		overwrite = _overwrite;
	} catch (...) {
		overwrite = _overwrite;

		delete [] empty;

		throw;
	}

	delete [] empty;
}

//-------------------------------------------------------------------

void
temp::flush() const
{
	pc::sync::protector pg(keeper);

	if (handle->file == NULL)
		throw exception::basic(exception::MODULE_IOFILETEMP, TEMPEX_FLUSH, exception::ERRNO_LIBDODO, TEMPEX_NOTOPENED, IOFILETEMPEX_NOTOPENED_STR, __LINE__, __FILE__);

	if (fflush(handle->file) != 0)
		throw exception::basic(exception::MODULE_IOFILETEMP, TEMPEX_FLUSH, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

unsigned long
temp::_readString(char * const a_data) const
{
	if (handle->file == NULL)
		throw exception::basic(exception::MODULE_IOFILETEMP, TEMPEX__READSTREAM, exception::ERRNO_LIBDODO, TEMPEX_NOTOPENED, IOFILETEMPEX_NOTOPENED_STR, __LINE__, __FILE__);

	unsigned long readSize = blockSize + 1;

	if (block) {
		if (fseek(handle->file, 0, SEEK_SET) == -1)
			throw exception::basic(exception::MODULE_IOFILETEMP, TEMPEX__READSTREAM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		for (unsigned long i = 0; i < pos; ++i) {
			if (fgets(a_data, readSize, handle->file) == NULL) {
				switch (errno) {
					case EIO:
					case EINTR:
					case EBADF:
					case EOVERFLOW:
					case ENOMEM:
					case ENXIO:

						throw exception::basic(exception::MODULE_IOFILETEMP, TEMPEX__READSTREAM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
				}

				throw exception::basic(exception::MODULE_IOFILETEMP, TEMPEX__READSTREAM, exception::ERRNO_LIBDODO, TEMPEX_FILEISSHORTERTHANGIVENPOSITION, IOFILETEMPEX_FILEISSHORTERTHANGIVENPOSITION_STR, __LINE__, __FILE__);
			}
		}
	} else if (fseek(handle->file, pos, SEEK_SET) == -1)
		throw exception::basic(exception::MODULE_IOFILETEMP, TEMPEX__READSTREAM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	memset(a_data, '\0', readSize);

	while (true) {
		if (fgets(a_data, readSize, handle->file) == NULL) {
			if (errno == EINTR)
				continue;

			if (errno == EAGAIN)
				break;

			if (ferror(handle->file) != 0)
				throw exception::basic(exception::MODULE_IOFILETEMP, TEMPEX__READSTREAM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}

		break;
	}

	return strlen(a_data);
}

//-------------------------------------------------------------------

void
temp::_writeString(const char *const a_data) const
{
	if (handle->file == NULL)
		throw exception::basic(exception::MODULE_IOFILETEMP, TEMPEX__WRITESTREAM, exception::ERRNO_LIBDODO, TEMPEX_NOTOPENED, IOFILETEMPEX_NOTOPENED_STR, __LINE__, __FILE__);

	if (fseek(handle->file, 0, SEEK_END) == -1)
		throw exception::basic(exception::MODULE_IOFILETEMP, TEMPEX__WRITESTREAM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	unsigned long _blockSize = blockSize;

	unsigned int bufSize = strlen(a_data);

	if (bufSize < _blockSize)
		_blockSize = bufSize;

	while (true) {
		if (fwrite(a_data, _blockSize, 1, handle->file) == 0) {
			if (errno == EINTR)
				continue;

			if (ferror(handle->file) != 0)
				throw exception::basic(exception::MODULE_IOFILETEMP, TEMPEX__WRITESTREAM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}

		break;
	}
}

//-------------------------------------------------------------------

