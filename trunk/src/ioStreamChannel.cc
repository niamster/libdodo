/***************************************************************************
 *            ioStreamChannel.cc
 *
 *  Sat Jun 13 12:26:57 2009
 *  Copyright  2009  Ni@m
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

#include <libdodo/ioStreamChannel.h>
#include <libdodo/ioChannel.h>
#include <libdodo/xexec.h>
#include <libdodo/types.h>
#include <libdodo/ioEventInfo.h>
#include <libdodo/pcSyncProtector.h>

using namespace dodo::io::stream;

channel::channel(short protection) : io::channel(protection)
{
}

//-------------------------------------------------------------------

channel::~channel()
{
}

//-------------------------------------------------------------------

dodoString
channel::read() const
{
	pc::sync::protector pg(keeper);

	dodoString a_str;

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

	return a_str;
}

//-------------------------------------------------------------------

dodoString
channel::readStream() const
{
	pc::sync::protector pg(keeper);

	dodoString a_str;

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
	{
		collectedData.buffer.assign(data, n);
	}
	else
	{
		collectedData.buffer.clear();
	}

	delete [] data;

	performXExec(postExec);

	a_str = collectedData.buffer;

	collectedData.buffer.clear();
#else
	if (n > 0)
	{
		a_str.assign(data, n);
	}
	else
	{
		a_str.clear();
	}

	delete [] data;
#endif

	return a_str;
}

//-------------------------------------------------------------------

void
channel::write(const dodoString &a_data) const
{
	pc::sync::protector pg(keeper);

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
channel::writeStream(const dodoString &a_data) const
{
	pc::sync::protector pg(keeper);

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

