/***************************************************************************
 *            dbConnector.cc
 *
 *  Sun Jan  15 19:45:19 2005
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

#include <libdodo/dbConnector.h>

using namespace dodo::db;

__connectionInfo::__connectionInfo(const dodoString &a_db,
								   const dodoString &a_host,
								   const dodoString &a_user,
								   const dodoString &a_password,
								   const dodoString &a_path,
								   int a_port) : db(a_db),
												 host(a_host),
												 user(a_user),
												 password(a_password),
												 path(a_path),
												 port(a_port)
{
}

//-------------------------------------------------------------------

__connectionInfo::__connectionInfo()
{
}

//-------------------------------------------------------------------

__tuples::__tuples(dodoArray<dodoStringArray> a_rows, dodoStringArray a_fields) : rows(a_rows),
																				  fields(a_fields)
{
}

//-------------------------------------------------------------------

__tuples::__tuples()
{

}

//-------------------------------------------------------------------

connector::connector() : connected(false),
						 reconnect(true)
{
}

//-------------------------------------------------------------------

connector::~connector()
{
}

//-------------------------------------------------------------------

