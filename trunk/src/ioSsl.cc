/***************************************************************************
 *            ioSsl.cc
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/directives.h>

#ifdef OPENSSL_EXT
#include <sys/stat.h>
#include <sys/time.h>
#include <openssl/ssl.h>
#include <openssl/rand.h>
#include <openssl/err.h>

#include "ioSsl.inline"

#include <libdodo/ioSsl.h>

namespace dodo
{
	namespace io
	{
		namespace ssl
		{
			__openssl_init__ __openssl_init_object__;
		};
	};
};

//-------------------------------------------------------------------

using namespace dodo::io::ssl;

__certificates::__certificates() : keyType(-1)
{
}

//-------------------------------------------------------------------

__openssl_init__::__openssl_init__()
{
	SSL_load_error_strings();
	SSL_library_init();

	struct stat randstat;

	if (stat("/dev/random", &randstat) == -1 && stat("/dev/random", &randstat) == -1)
	{
		char buf[4];
		struct timeval tv;

		for (int i = 0; i < 10000; ++i)
		{
			if (RAND_status() == 1)
			{
				break;
			}

			gettimeofday(&tv, NULL);

			buf[0] = tv.tv_usec & 0xF;
			buf[2] = (tv.tv_usec & 0xF0) >> 4;
			buf[3] = (tv.tv_usec & 0xF00) >> 8;
			buf[1] = (tv.tv_usec & 0xF000) >> 12;

			RAND_add(buf, sizeof(buf), 0.1);
		}
	}
}

//-------------------------------------------------------------------

__openssl_init__::~__openssl_init__()
{
	ERR_free_strings();

	RAND_cleanup();
}

//-------------------------------------------------------------------

void
__openssl_init__::addEntropy()
{
	struct stat randstat;

	if (stat("/dev/random", &randstat) == -1 && stat("/dev/random", &randstat) == -1)
	{
		char buf[4];
		struct timeval tv;

		for (int i = 0; i < 10000; ++i)
		{
			if (RAND_status() == 1)
			{
				break;
			}

			gettimeofday(&tv, NULL);

			buf[0] = tv.tv_usec & 0xF;
			buf[2] = (tv.tv_usec & 0xF0) >> 4;
			buf[3] = (tv.tv_usec & 0xF00) >> 8;
			buf[1] = (tv.tv_usec & 0xF000) >> 12;

			RAND_add(buf, sizeof(buf), 0.1);
		}
	}
}

//-------------------------------------------------------------------
#endif

