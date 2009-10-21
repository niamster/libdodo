/***************************************************************************
 *            graphicsColor.cc
 *
 *  Thu Nov 23 2007
 *  Copyright  2007  Ni@m
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

#ifdef IMAGEMAGICK_EXT
#include <libdodo/graphicsColor.h>

using namespace dodo::graphics;

__color__ color::red = {
    65535, 0, 0, 0
};

//-------------------------------------------------------------------

__color__ color::green = {
    0, 65535, 0, 0
};

//-------------------------------------------------------------------

__color__ color::blue = {
    0, 0, 65535, 0
};

//-------------------------------------------------------------------

__color__ color::white = {
    65535, 65535, 65535, 0
};

//-------------------------------------------------------------------

__color__ color::black = {
    0, 0, 0, 0
};

//-------------------------------------------------------------------

__color__ color::transparent = {
    65535, 65535, 65535, 65535
};
#endif

//-------------------------------------------------------------------
