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

memory::memory(short protection) : block::channel(protection)
{
#ifndef IO_WO_XEXEC
	collectedData.setExecObject(XEXEC_OBJECT_IOMEMORY);
#endif
}

//-------------------------------------------------------------------

memory::memory(const memory &fd) : buffer(fd.buffer),
								   block::channel(fd.protection)
{
#ifndef IO_WO_XEXEC
	collectedData.setExecObject(XEXEC_OBJECT_IOFILEREGULAR);
#endif

	block = fd.block;
	append = fd.append;
	pos = fd.pos;
	inSize = fd.inSize;
	outSize = fd.outSize;
}

//-------------------------------------------------------------------

memory::memory(const dodoString &data,
			   short            protection) : buffer(data),
											  block::channel(protection)
{
#ifndef IO_WO_XEXEC
	collectedData.setExecObject(XEXEC_OBJECT_IOFILEREGULAR);
#endif
}

//-------------------------------------------------------------------

memory::~memory()
{
}

//-------------------------------------------------------------------

int
memory::getInDescriptor() const
{
	throw exception::basic(exception::ERRMODULE_IOMEMORY, MEMORYEX_GETINDESCRIPTOR, exception::ERRNO_LIBDODO, MEMORYEX_CANTBEUSEDWITHIOEVENT, IOMEMORYEX_CANTBEUSEDWITHIOEVENT_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

int
memory::getOutDescriptor() const
{
	throw exception::basic(exception::ERRMODULE_IOMEMORY, MEMORYEX_GETOUTDESCRIPTOR, exception::ERRNO_LIBDODO, MEMORYEX_CANTBEUSEDWITHIOEVENT, IOMEMORYEX_CANTBEUSEDWITHIOEVENT_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
memory::flush()
{
}

//-------------------------------------------------------------------

void
memory::clear()
{
	buffer.clear();
}

//-------------------------------------------------------------------

memory::operator const dodoString
& ()
{
	return buffer;
}

//-------------------------------------------------------------------

memory::operator const char
*()
{
	return buffer.data();
}

//-------------------------------------------------------------------

void
memory::clone(const memory &fd)
{
	pc::sync::protector pg(keeper);

	pos = fd.pos;
	block = fd.block;
	append = fd.append;
	inSize = fd.inSize;
	outSize = fd.outSize;

	buffer = fd.buffer;
}

//-------------------------------------------------------------------

void
memory::_read(char * const a_data) const
{
	unsigned long pos = block ? this->pos * inSize : this->pos;

	if ((pos + inSize) > buffer.size())
	{
		throw exception::basic(exception::ERRMODULE_IOMEMORY, MEMORYEX__READ, exception::ERRNO_LIBDODO, MEMORYEX_OUTOFBOUNDS, IOMEMORYEX_OUTOFBOUNDS_STR, __LINE__, __FILE__);
	}

	memset(a_data, '\0', inSize);

	buffer.copy(a_data, inSize, pos);
}

//-------------------------------------------------------------------

void
memory::_write(const char *const a_data)
{
	if (append)
	{
		buffer.append(a_data, outSize);
	}
	else
	{
		unsigned long pos = block ? this->pos * outSize : this->pos;

		unsigned long shift = pos + outSize;
		if (shift > buffer.size())
		{
			buffer.resize(shift, '\0');
		}

		buffer.replace(pos, outSize, a_data);
	}
}

//-------------------------------------------------------------------

void
memory::erase()
{
	pc::sync::protector pg(keeper);

	unsigned long pos = block ? this->pos * outSize : this->pos;

	unsigned long shift = outSize + pos;
	if (shift > buffer.size())
	{
		buffer.resize(shift, '\0');
	}

	buffer.replace(pos, outSize, 1, '\0');
}

//-------------------------------------------------------------------

unsigned long
memory::_readStream(char * const a_data) const
{
	unsigned long readSize = inSize + 1;

	memset(a_data, '\0', readSize);

	unsigned long length = buffer.size();
	unsigned long read = 0;

	const char *data = buffer.data();

	if (block)
	{
		unsigned long block = 0;
		unsigned long index = 0;
		for (; index < length; ++index)
		{
			if (data[index] == '\n' || data[index] == '\0')
			{
				++block;
			}

			if (block == pos)
			{
				++index;

				for (unsigned long i = index; i < length && read < readSize; ++i)
				{
					a_data[read] = data[i];

					++read;

					if (data[i] == '\n' || data[i] == '\0')
					{
						break;
					}
				}

				break;
			}
		}
	}
	else
	{
		for (unsigned long i = pos; i < length && read < readSize; ++i)
		{
			a_data[read] = data[i];

			++read;

			if (data[i] == '\n' || data[i] == '\0')
			{
				break;
			}
		}
	}

	return read;
}

//-------------------------------------------------------------------

void
memory::_writeStream(const char *const a_data)
{
	unsigned long _outSize = outSize;
	unsigned int bufSize = strlen(a_data);

	if (bufSize < _outSize)
	{
		_outSize = bufSize;
	}

	buffer.append(a_data, _outSize);
}

//-------------------------------------------------------------------

