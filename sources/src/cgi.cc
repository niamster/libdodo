/***************************************************************************
 *            cgi.cc
 *
 *  Sat Sep  18 2005
 *  Copyright  2005  Dmytro Milinevskyy
 *  milinevskyy@gmail.com
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

#include <libdodo/cgi.h>
#include <libdodo/types.h>

using namespace dodo::cgi;

file::file() : size(0),
               error(FILE_ERROR_NONE)
{
}

//-------------------------------------------------------------------

cookie::cookie(const dodo::string &name,
               const dodo::string &value,
               const dodo::string &expires,
               const dodo::string &path,
               const dodo::string &domain,
               bool             secure) : name(name),
                                          value(value),
                                          expires(expires),
                                          path(path),
                                          domain(domain),
                                          secure(secure)
{
}

//-------------------------------------------------------------------

cookie::cookie() : secure(false)
{
}

//-------------------------------------------------------------------
