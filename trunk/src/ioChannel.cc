/***************************************************************************
 *            ioChannel.cc
 *
 *  Tue Oct 11 2005
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
#include <libdodo/pcSyncProcess.h>
#include <libdodo/pcSyncThread.h>
#include <libdodo/pcSyncStack.h>

using namespace dodo::io;

#ifndef IO_WO_XEXEC
channel::__collected_data__::__collected_data__(xexec *a_executor,
                                                short execObject) : xexec::__collected_data__(a_executor, execObject)
{
}
#endif

//-------------------------------------------------------------------

channel::channel(short protection) : blockSize(IO_BLOCKSIZE),
                                     keeper(NULL),
                                     protection(protection)
#ifndef IO_WO_XEXEC
                                     ,
                                     collectedData(this, xexec::OBJECT_XEXEC)
#endif
{
    if (protection == channel::PROTECTION_THREAD)
        keeper = new pc::sync::thread;
    else if (protection == channel::PROTECTION_PROCESS)
        keeper = new pc::sync::process(0);
}

//-------------------------------------------------------------------

channel::~channel()
{
    delete keeper;
}

//-------------------------------------------------------------------
