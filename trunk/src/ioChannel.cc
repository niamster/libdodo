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

#include <libdodo/directives.h>

#include <libdodo/ioChannel.h>
#include <libdodo/xexec.h>
#include <libdodo/types.h>
#include <libdodo/pcSyncProcessSection.h>
#include <libdodo/pcSyncThreadSection.h>
#include <libdodo/pcSyncProtector.h>

using namespace dodo::io;

#ifndef IO_WO_XEXEC
__xexecIoChannelCollectedData__::__xexecIoChannelCollectedData__(xexec *a_executor,
															 short execObject) : __xexecCollectedData__(a_executor, execObject)
{
}
#endif

//-------------------------------------------------------------------

channel::channel(short protection) : inSize(IO_INSIZE),
									 outSize(IO_OUTSIZE),
									 keeper(NULL),
									 protection(protection)
#ifndef IO_WO_XEXEC
									 ,
									 collectedData(this, XEXEC_OBJECT_XEXEC)
#endif
{
	if (protection == CHANNEL_PROTECTION_THREAD)
	{
		keeper = new pc::sync::thread::section;
	}
	else
	{
		if (protection == CHANNEL_PROTECTION_PROCESS)
		{
			keeper = new pc::sync::process::section(0);
		}
	}
}

//-------------------------------------------------------------------

channel::~channel()
{
	delete keeper;
}

//-------------------------------------------------------------------
