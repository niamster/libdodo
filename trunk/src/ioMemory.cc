/***************************************************************************
 *            ioMemory.cc
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

#include <string.h>

#include <libdodo/ioMemory.h>
#include <libdodo/types.h>
#include <libdodo/ioChannel.h>
#include <libdodo/ioBlockChannel.h>
#include <libdodo/ioMemoryEx.h>
#include <libdodo/pcSyncProtector.h>

using namespace dodo::io;

memory::memory(char          *a_data,
			   unsigned long size,
			   short         flags,
			   short         protection) : block::channel(protection),
										   size(size),
										   flags(flags),
										   nullEnd(false)
{
#ifndef IO_WO_XEXEC
	collectedData.setExecObject(xexec::OBJECT_IOMEMORY);
#endif

	if (flags & FLAGS_NORMAL) {
		data = new char[size];
		memcpy(data, a_data, size);
	} else {
		if (flags & FLAGS_EXTERN)
			data = a_data;
		else
			throw exception::basic(exception::MODULE_IOMEMORY, MEMORYEX_MEMORY, exception::ERRNO_LIBDODO, MEMORYEX_WRONGFLAGS, IOMEMORYEX_WRONGFLAGS_STR, __LINE__, __FILE__);
	}
}

//-------------------------------------------------------------------

memory::memory(short protection) : block::channel(protection),
								   data(NULL),
								   size(0),
								   flags(FLAGS_NORMAL),
								   nullEnd(false)
{
#ifndef IO_WO_XEXEC
	collectedData.setExecObject(xexec::OBJECT_IOMEMORY);
#endif
}

//-------------------------------------------------------------------

memory::memory(const memory &fd) : block::channel(fd.protection),
								   size(fd.size),
								   flags(fd.flags),
								   nullEnd(fd.nullEnd)
{
#ifndef IO_WO_XEXEC
	collectedData.setExecObject(xexec::OBJECT_IOFILEREGULAR);
#endif

	if (flags & FLAGS_NORMAL) {
		if (nullEnd) {
			data = new char[size + 1];
			memcpy(data, fd.data, size + 1);
		} else {
			data = new char[size + 1];
			memcpy(data, fd.data, size + 1);
		}
	} else if (flags & FLAGS_EXTERN) {
		data = fd.data;
	}

	block = fd.block;
	append = fd.append;
	pos = fd.pos;
	blockSize = fd.blockSize;
}

//-------------------------------------------------------------------

memory::memory(const dodoString &buffer,
			   short            protection) : block::channel(protection),
											  size(buffer.size()),
											  flags(FLAGS_NORMAL),
											  nullEnd(false)
{
#ifndef IO_WO_XEXEC
	collectedData.setExecObject(xexec::OBJECT_IOFILEREGULAR);
#endif

	data = new char[size];
	memcpy(data, buffer.data(), size);
}

//-------------------------------------------------------------------

memory::~memory()
{
	if (!(flags & FLAGS_EXTERN))
		delete [] data;
}

//-------------------------------------------------------------------

int
memory::inDescriptor() const
{
	throw exception::basic(exception::MODULE_IOMEMORY, MEMORYEX_INDESCRIPTOR, exception::ERRNO_LIBDODO, MEMORYEX_CANTBEUSEDWITHIOEVENT, IOMEMORYEX_CANTBEUSEDWITHIOEVENT_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

int
memory::outDescriptor() const
{
	throw exception::basic(exception::MODULE_IOMEMORY, MEMORYEX_OUTDESCRIPTOR, exception::ERRNO_LIBDODO, MEMORYEX_CANTBEUSEDWITHIOEVENT, IOMEMORYEX_CANTBEUSEDWITHIOEVENT_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
memory::flush() const
{
}

//-------------------------------------------------------------------

void
memory::clear()
{
	if (!(flags & FLAGS_EXTERN)) {
		delete [] data;
		data = NULL;

		size = 0;
	}
}

//-------------------------------------------------------------------

memory::operator const char *()
{
	return data;
}

//-------------------------------------------------------------------

void
memory::clone(const memory &fd)
{
	pc::sync::protector pg(keeper);

	pos = fd.pos;
	block = fd.block;
	append = fd.append;
	blockSize = fd.blockSize;
	flags = fd.flags;
	size = fd.size;

	if (flags == FLAGS_NORMAL) {
		data = new char[size];
		memcpy(data, fd.data, size);
	} else if (flags == FLAGS_EXTERN)
		data = fd.data;

}

//-------------------------------------------------------------------

void
memory::_read(char * const a_data) const
{
	unsigned long pos = block ? this->pos * blockSize : this->pos;

	if ((pos + blockSize) > size)
		throw exception::basic(exception::MODULE_IOMEMORY, MEMORYEX__READ, exception::ERRNO_LIBDODO, MEMORYEX_OUTOFBOUNDS, IOMEMORYEX_OUTOFBOUNDS_STR, __LINE__, __FILE__);

	memcpy(a_data, data + pos, blockSize);
}

//-------------------------------------------------------------------

void
memory::_write(const char *const a_data) const
{
	if (append) {
		if (flags & FLAGS_FIXED_LENGTH)
			throw exception::basic(exception::MODULE_IOMEMORY, MEMORYEX__WRITE, exception::ERRNO_LIBDODO, MEMORYEX_APPENDTOFIXED, IOMEMORYEX_APPENDTOFIXED_STR, __LINE__, __FILE__);
		else {
			char *newData = new char[size + blockSize];
			memcpy(newData, data, size);
			memcpy(newData + size, a_data, blockSize);
			size += blockSize;
			delete [] data;
			data = newData;
		}
	} else {
		unsigned long pos = block ? this->pos * blockSize : this->pos;

		unsigned long shift = pos + blockSize;
		if (shift > size) {
			if (flags & FLAGS_FIXED_LENGTH)
				throw exception::basic(exception::MODULE_IOMEMORY, MEMORYEX__WRITE, exception::ERRNO_LIBDODO, MEMORYEX_EXTENDFIXED, IOMEMORYEX_EXTENDFIXED_STR, __LINE__, __FILE__);
			else {
				shift -= size;
				char *newData = new char[size + shift];
				memcpy(newData, data, size);
				memset(newData + size, 0x0, shift);
				size += shift;
				delete [] data;
				data = newData;
			}
		}

		memcpy(data + pos, a_data, blockSize);
	}
}

//-------------------------------------------------------------------

void
memory::erase()
{
	pc::sync::protector pg(keeper);

	unsigned long pos = block ? this->pos * blockSize : this->pos;

	unsigned long shift = pos + blockSize;
	if (shift > size) {
		if (flags & FLAGS_FIXED_LENGTH)
			throw exception::basic(exception::MODULE_IOMEMORY, MEMORYEX_ERASE, exception::ERRNO_LIBDODO, MEMORYEX_EXTENDFIXED, IOMEMORYEX_EXTENDFIXED_STR, __LINE__, __FILE__);
		else {
			shift -= size;
			char *newData = new char[size + shift];
			memcpy(newData, data, size);
			memset(newData + size, 0x0, shift);
			size += shift;
			delete [] data;
			data = newData;
		}
	}

	memset(data + pos, 0x0, blockSize);
}

//-------------------------------------------------------------------

unsigned long
memory::_readString(char * const a_data) const
{
	unsigned long readSize = blockSize + 1;

	memset(a_data, '\0', readSize);

	unsigned long read = 0;

	if (block) {
		unsigned long block = 0;
		unsigned long index = 0;
		for (; index < size; ++index) {
			if (data[index] == '\n' || data[index] == '\0')
				++block;

			if (block == pos) {
				++index;

				for (unsigned long i = index; i < size && read < readSize; ++i) {
					a_data[read] = data[i];

					++read;

					if (data[i] == '\n' || data[i] == '\0')
						break;
				}

				break;
			}
		}
	} else {
		for (unsigned long i = pos; i < size && read < readSize; ++i) {
			a_data[read] = data[i];

			++read;

			if (data[i] == '\n' || data[i] == '\0')
				break;
		}
	}

	return read;
}

//-------------------------------------------------------------------

void
memory::_writeString(const char *const a_data) const
{
	if (flags & FLAGS_FIXED_LENGTH)
		throw exception::basic(exception::MODULE_IOMEMORY, MEMORYEX__WRITESTREAM, exception::ERRNO_LIBDODO, MEMORYEX_EXTENDFIXED, IOMEMORYEX_EXTENDFIXED_STR, __LINE__, __FILE__);

	unsigned long _blockSize = strnlen(a_data, blockSize);

	char *newData = new char[size + _blockSize + 1];
	memcpy(newData, data, size);
	memcpy(newData + size, a_data, _blockSize);
	size += _blockSize;
	newData[size] = '\0';
	delete [] data;
	data = newData;
	nullEnd = true;
}

//-------------------------------------------------------------------

