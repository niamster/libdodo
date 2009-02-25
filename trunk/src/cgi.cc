/***************************************************************************
 *            cgi.cc
 *
 *  Sat Sep  18 17:18:19 2005
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

#include <libdodo/cgi.h>

#include <libdodo/directives.h>

#include <libdodo/types.h>

using namespace dodo::cgi;

__cgiFile::__cgiFile() : size(0),
						 error(CGI_POSTFILEERR_NO_FILE)
{
}

//-------------------------------------------------------------------

__cgiCookie::__cgiCookie(const dodoString &a_name,
						 const dodoString &a_value,
						 const dodoString &a_expires,
						 const dodoString &a_path,
						 const dodoString &a_domain,
						 bool             a_secure) : name(a_name),
													  value(a_value),
													  expires(a_expires),
													  path(a_path),
													  domain(a_domain),
													  secure(a_secure)
{
}

//-------------------------------------------------------------------

__cgiCookie::__cgiCookie() : secure(false)
{
}

//-------------------------------------------------------------------

__cgiCookie::__cgiCookie(bool a_secure) : secure(a_secure)
{
}

//-------------------------------------------------------------------
