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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
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
				 * @struct __certificates defines SSL options
				 */
				struct __certificates
				{
					/**
					 * constructor
					 */
					__certificates();

					/**
					 * constructor
					 * @param key defines pathname to the key file
					 * @param cert defines pathname to the certificate file
					 * @param ca defines pathname to the certificate authority file
					 * @param capath defines pathname to the directory that contains trusted SSL CA certificates in pem format
					 * @param cipher defines allowed SSL ciphers
					 */
					__certificates(const dodoString &key, const dodoString &cert = __dodostring__, const dodoString &ca = __dodostring__, const dodoString &capath = __dodostring__, const dodoString &cipher = __dodostring__);

					dodoString key;                     ///< pathname to the key file
					dodoString cert;                    ///< pathname to the certificate file
					dodoString ca;                      ///< pathname to the certificate authority file
					dodoString capath;                  ///< pathname to a directory that contains trusted SSL CA certificates in pem format
					dodoString cipher;                  ///< allowed SSL ciphers
				};
				
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
