/***************************************************************************
 *            ioNetworkSsl.cc
 *
 *  Wed Jun 11 11:10:47 2008
 *  Copyright  2008  Ni@m
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

#include <libdodo/ioNetworkSsl.h>

#ifdef OPENSSL_EXT

using namespace dodo::io::network::ssl;

__openssl_init__::__openssl_init__()
{
	SSL_load_error_strings();
	SSL_library_init();

	char rand[OPENSSL_RAND_BUFSIZE];

	tools::misc::random(rand, OPENSSL_RAND_BUFSIZE);

	RAND_seed(rand, OPENSSL_RAND_BUFSIZE);
}

//-------------------------------------------------------------------

__openssl_init__::~__openssl_init__()
{
	ERR_free_strings();

	RAND_cleanup();
}

//-------------------------------------------------------------------

__openssl_init__ __openssl_init_object__;

//-------------------------------------------------------------------

#endif

