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
channel::read(dodoString &a_str)
{
	protector pg(this);

	unsigned long readSize = inSize + 1;

#ifndef IO_WO_XEXEC
	operType = IO_OPERATION_READSTRING;
	performXExec(preExec);

	collectedData.buffer.reserve(readSize);
#endif

	char *data = new char[readSize];

	try
	{
		_read(data);

		data[inSize] = '\0';
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
channel::readStream(dodoString &a_str)
{
	protector pg(this);

#ifndef IO_WO_XEXEC
	operType = IO_OPERATION_READSTREAMSTRING;
	performXExec(preExec);
#endif

	char *data = new char[inSize + 1];
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
channel::write(const dodoString &a_data)
{
	protector pg(this);

#ifndef IO_WO_XEXEC

	collectedData.buffer.assign(a_data, 0, outSize);

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

	_write(a_data.c_str());

#endif

#ifndef IO_WO_XEXEC
	performXExec(postExec);

	collectedData.buffer.clear();
#endif
}

//-------------------------------------------------------------------

void
channel::writeStream(const dodoString &a_data)
{
	protector pg(this);

#ifndef IO_WO_XEXEC

	collectedData.buffer = a_data;

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

	_writeStream(a_data.c_str());

#endif

#ifndef IO_WO_XEXEC
	performXExec(postExec);

	collectedData.buffer.clear();
#endif
}

//-------------------------------------------------------------------

