/***************************************************************************
 *            ioNetworkSsl.h
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

#ifndef _IONETWORKSSL_H_
#define _IONETWORKSSL_H_

#include <libdodo/directives.h>

#ifdef OPENSSL_EXT

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/time.h>

#include <libdodo/toolsMisc.h>

namespace dodo
{
	namespace io
	{
		namespace network
		{
			namespace ssl
			{
				/**
				 * @class __openssl_init__ initializes openssl evironment
				 */
				class __openssl_init__
				{
					public:

						/**
						 * contructor
						 */
						__openssl_init__();

						/**
						 * destructor
						 */
						~__openssl_init__();

						/**
						 *
						 */
						void addEntropy();
				};

				extern __openssl_init__ __openssl_init_object__;
			};
		};
	};
};


#endif

#endif
