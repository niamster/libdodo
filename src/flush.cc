/***************************************************************************
 *            flush.cc
 *
 *  Tue Oct 11 00:19:57 2005
 *  Copyright  2005  Ni@m
 *  niam.niam@gmail.com
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <flush.h>

using namespace dodo;

dodoBase * const 
flush::getSelf()
{
	return dynamic_cast<dodoBase *>(this);
}

//-------------------------------------------------------------------

flush::flush(): inSize(INSIZE), 
				outSize(OUTSIZE),
				autoOutSize(true),
				opened(false)
{
}

//-------------------------------------------------------------------

flush::~flush()
{
}
