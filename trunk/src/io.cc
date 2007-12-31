/***************************************************************************
 *            io.cc
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

#include <libdodo/io.h>

using namespace dodo;

__xexexIoCollectedData::__xexexIoCollectedData(dodoString &a_buffer,
											   int &a_operType,
											   void *a_executor) : buffer(a_buffer),
																   operType(a_operType),
																   executor(a_executor)
{
}

//-------------------------------------------------------------------

io::io() : inSize(INSIZE),
		   outSize(OUTSIZE),
		   opened(false),
		   collectedData(buffer,
						 operType,
						 (void *) this)
{
}

//-------------------------------------------------------------------

io::~io()
{
}
