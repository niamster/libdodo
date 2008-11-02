/***************************************************************************
 *            ioChannel.cc
 *
 *  Tue Oct 11 00:19:57 2005
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

#include <libdodo/ioChannel.h>

using namespace dodo::io;

#ifndef IO_WO_XEXEC

__xexecIoChannelCollectedData::__xexecIoChannelCollectedData(xexec *a_executor,
                                                             short execObject) : __xexecCollectedData(a_executor, execObject)
{
}

#endif

//-------------------------------------------------------------------

channel::channel() : inSize(IO_INSIZE),
					 outSize(IO_OUTSIZE),
					 opened(false)

#ifndef IO_WO_XEXEC

					 ,
					 collectedData(this, XEXEC_OBJECT_XEXEC)

#endif
{
}

//-------------------------------------------------------------------

channel::~channel()
{
}

//-------------------------------------------------------------------

void
channel::read(char * const a_void)
{
	protector pg(this);

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

	strncpy(a_void, collectedData.buffer.c_str(), collectedData.buffer.size());

	collectedData.buffer.clear();
#endif
}

//-------------------------------------------------------------------

void
channel::readString(dodoString &a_str)
{
	protector pg(this);

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
		a_str.clear();

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
channel::readStream(char * const a_void)
{
	protector pg(this);

#ifndef IO_WO_XEXEC
	operType = IO_OPERATION_READSTREAM;
	performXExec(preExec);
#endif

	unsigned long n = _readStream(a_void);

#ifndef IO_WO_XEXEC

	if (n > 0)
		collectedData.buffer.assign(a_void, n);
	else
		collectedData.buffer.clear();

	performXExec(postExec);

	strncpy(a_void, collectedData.buffer.c_str(), collectedData.buffer.size());

	collectedData.buffer.clear();

#endif
}

//-------------------------------------------------------------------

void
channel::readStreamString(dodoString &a_str)
{
	protector pg(this);

#ifndef IO_WO_XEXEC
	operType = IO_OPERATION_READSTREAMSTRING;
	performXExec(preExec);
#endif

	char *data = new char[inSize];
	unsigned long n = 0;

	try
	{
		n = _readStream(data);
	}
	catch (...)
	{
		a_str.clear();

		delete [] data;

		throw;
	}

#ifndef IO_WO_XEXEC

	if (n > 0)
		collectedData.buffer.assign(data, n);
	else
		collectedData.buffer.clear();

	delete [] data;

	performXExec(postExec);

	a_str = collectedData.buffer;

	collectedData.buffer.clear();

#else

	if (n > 0)
		a_str.assign(data, n);
	else
		a_str.clear();

	delete [] data;

#endif
}

//-------------------------------------------------------------------

void
channel::writeString(const dodoString &a_buf)
{
	protector pg(this);

#ifndef IO_WO_XEXEC

	collectedData.buffer.assign(a_buf, 0, outSize);

	operType = IO_OPERATION_WRITESTREAM;
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
}

//-------------------------------------------------------------------

void
channel::write(const char *const a_buf)
{
	protector pg(this);

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
channel::writeStreamString(const dodoString &a_buf)
{
	protector pg(this);

#ifndef IO_WO_XEXEC

	collectedData.buffer = a_buf;

	operType = IO_OPERATION_WRITESTREAMSTRING;
	performXExec(preExec);

	try
	{
		_writeStream(collectedData.buffer.c_str());
	}
	catch (...)
	{
		collectedData.buffer.clear();

		throw;
	}

#else

	_writeStream(a_buf.c_str());

#endif

#ifndef IO_WO_XEXEC
	performXExec(postExec);

	collectedData.buffer.clear();
#endif
}

//-------------------------------------------------------------------

void
channel::writeStream(const char *const a_buf)
{
	protector pg(this);

#ifndef IO_WO_XEXEC

	collectedData.buffer.assign(a_buf);

	operType = IO_OPERATION_WRITESTREAM;
	performXExec(preExec);

	try
	{
		_writeStream(collectedData.buffer.c_str());
	}
	catch (...)
	{
		collectedData.buffer.clear();

		throw;
	}

#else

	_writeStream(a_buf);

#endif

#ifndef IO_WO_XEXEC
	performXExec(postExec);

	collectedData.buffer.clear();
#endif
}

//-------------------------------------------------------------------

