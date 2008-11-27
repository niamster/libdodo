/***************************************************************************
 *            ioString.cc
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

#include <libdodo/ioString.h>

using namespace dodo::io;

string::string() : pos(0),
					blockOffset(false),
					 append(false)
{
#ifndef IO_WO_XEXEC

	collectedData.setExecObject(XEXEC_OBJECT_IOSTRING);

#endif
}

//-------------------------------------------------------------------

string::string(const string &fd) : pos(fd.pos),
								blockOffset(fd.blockOffset),
								 append(fd.append),
								 buffer(fd.buffer)
{
#ifndef IO_WO_XEXEC

	collectedData.setExecObject(XEXEC_OBJECT_IOFILEREGULAR);

#endif

	inSize = fd.inSize;
	outSize = fd.outSize;
}

//-------------------------------------------------------------------

string::~string()
{
}

//-------------------------------------------------------------------

int
string::getInDescriptor() const
{
	throw exception::basic(exception::ERRMODULE_IOSTRING, STRINGEX_GETINDESCRIPTOR, exception::ERRNO_LIBDODO, STRINGEX_CANTBEUSEDWITHIOEVENT, IOSTRINGEX_CANTBEUSEDWITHIOEVENT_STR, __LINE__, __FILE__);

	return -1;
}

//-------------------------------------------------------------------

int
string::getOutDescriptor() const
{
	throw exception::basic(exception::ERRMODULE_IOSTRING, STRINGEX_GETOUTDESCRIPTOR, exception::ERRNO_LIBDODO, STRINGEX_CANTBEUSEDWITHIOEVENT, IOSTRINGEX_CANTBEUSEDWITHIOEVENT_STR, __LINE__, __FILE__);

	return -1;
}

//-------------------------------------------------------------------

void
string::flush()
{
}

//-------------------------------------------------------------------

string::operator const dodoString &()
{
	return buffer;
}

//-------------------------------------------------------------------

string::operator const char *()
{
	return buffer.data();
}

//-------------------------------------------------------------------

void
string::clone(const string &fd)
{
	protector pg(this);

	pos = fd.pos;
	blockOffset = fd.blockOffset;
	append = fd.append;
	inSize = fd.inSize;
	outSize = fd.outSize;

	buffer = fd.buffer;
}

//-------------------------------------------------------------------

void
string::_read(char * const a_data)
{
	unsigned long pos = blockOffset?this->pos * inSize:this->pos;

	if ((pos + inSize) > buffer.size())
		throw exception::basic(exception::ERRMODULE_IOSTRING, STRINGEX__READ, exception::ERRNO_LIBDODO, STRINGEX_OUTOFBOUNDS, IOSTRINGEX_OUTOFBOUNDS_STR, __LINE__, __FILE__);

	memset(a_data, '\0', inSize);

	buffer.copy(a_data, inSize, pos);
}

//-------------------------------------------------------------------

void
string::_write(const char *const a_data)
{
	if (append)
		buffer.append(a_data, outSize);
	else
	{
		unsigned long pos = blockOffset?this->pos * outSize:this->pos;

		unsigned long shift = pos + outSize;
		if (shift > buffer.size())
			buffer.resize(shift, '\0');

		buffer.replace(pos, outSize, a_data);
	}
}

//-------------------------------------------------------------------

void
string::erase()
{
	protector pg(this);

	unsigned long pos = blockOffset?this->pos * outSize:this->pos;

	unsigned long shift = outSize + pos;
	if (shift > buffer.size())
		buffer.resize(shift, '\0');

	buffer.replace(pos, outSize, 1, '\0');
}

//-------------------------------------------------------------------

unsigned long
string::_readStream(char * const a_data)
{
	memset(a_data, '\0', inSize);

	unsigned long length = buffer.size();
	unsigned long read = 0;

	const char *data = buffer.data();

	if (blockOffset)
	{
		unsigned long block = 0;
		unsigned long index = 0;
		for (;index<length;++index)
		{
			if (data[index] == '\n' || data[index] == '\0')
				++block;

			if (block == pos)
			{
				++index;

				for (unsigned long i=index;i<length && read<inSize;++i,++read)
				{
					a_data[read] = data[i];

					if (data[i] == '\n' || data[i] == '\0')
						break;
				}

				break;
			}
		}
	}
	else
	{
		for (unsigned long i=pos;i<length && read<inSize;++i,++read)
		{
			a_data[read] = data[i];

			if (data[i] == '\n' || data[i] == '\0')
				break;
		}
	}

	return read;
}

//-------------------------------------------------------------------

void
string::_writeStream(const char *const a_data)
{
	unsigned long _outSize = outSize;
	unsigned int bufSize = strlen(a_data);

	if (bufSize < _outSize)
		_outSize = bufSize;

	//if (append)
		buffer.append(a_data, _outSize);
	//else
	{
		//!blockOffset ||
		/*unsigned long pos = blockOffset?this->pos * outSize:this->pos;

		unsigned long shift = pos + outSize;
		if (shift > buffer.size())
			buffer.resize(shift, '\0');

		buffer.replace(pos, outSize, a_data);*/
	}

	/*unsigned long length = buffer.size();
	unsigned long read = 0;

	const char *data = buffer.data();

	if (blockOffset)
	{
		unsigned long block = 0;
		unsigned long index = 0;
		for (;index<length;++index)
		{
			if (data[index] == '\n' || data[index] == '\0')
				++block;

			if (block == pos)
			{
				++index;

				for (unsigned long i=index;i<length && read<inSize;++i,++read)
				{
					a_data[read] = data[i];

					if (data[i] == '\n' || data[i] == '\0')
						break;
				}

				break;
			}
		}
	}
	else
	{
		for (unsigned long i=pos;i<length && read<inSize;++i,++read)
		{
			a_data[read] = data[i];

			if (data[i] == '\n' || data[i] == '\0')
				break;
		}
	}*/
}

//-------------------------------------------------------------------

