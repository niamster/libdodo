/***************************************************************************
 *            ioStdio.cc
 *
 *  Tue Nov 15 21:19:57 2005
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

#include <libdodo/ioStdio.h>

using namespace dodo::io;

stdio::stdio()
{
#ifndef IO_WO_XEXEC

	collectedData.setExecObject(XEXEC_OBJECT_IOSTDIO);

#endif

	inHandle = stdin;
	outHandle = stdout;

	opened = true;
}

//-------------------------------------------------------------------

stdio::stdio(stdio &fd)
{
}

//-------------------------------------------------------------------

stdio::~stdio()
{
	opened = false;
}

//-------------------------------------------------------------------

void
stdio::open()
{
}

//-------------------------------------------------------------------

void
stdio::close()
{
}

//-------------------------------------------------------------------

void
stdio::redirectToStderr(bool toStderr)
{
	if (err == toStderr)
		return;

	err = toStderr;

	if (err)
		outHandle = stderr;
	else
		outHandle = stdout;
}

//-------------------------------------------------------------------

bool
stdio::isRedirectedToStderr()
{
	return err;
}

//-------------------------------------------------------------------

void
stdio::_writeStream(const char * const data)
{
	unsigned long _outSize = outSize;

	try
	{
		unsigned int bufSize = strlen(data);

		if (bufSize < outSize)
			outSize = bufSize;

		_write(data);

		outSize = _outSize;
	}
	catch (...)
	{
		outSize = _outSize;

		throw;
	}
}

//-------------------------------------------------------------------
