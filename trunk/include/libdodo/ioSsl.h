/***************************************************************************
 *            ioSsl.h
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

#ifndef _IOSSL_H_
#define _IOSSL_H_ 1

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
		namespace ssl
		{
			enum keyTypeEnum
			{
				KEYTYPE_PKEY,
				KEYTYPE_RSA,
			};

			/**
			 * @struct __certificates
			 * @brief defines SSL options
			 * @note key, certificate or CA should be in PEM format
			 * allowed ciphers you can list by `openssl ciphers -v` command
			 * ciphers should be separated with ':'
			 * some of them:
			 * RC4-MD5
			 * RC4-SHA
			 * AES256-SHA
			 * DES-CBC3-SHA
			 * DES-CBC3-MD5
			 * DHE-RSA-AES128-SHA
			 * DHE-DSS-AES128-SHA
			 * AES128-SHA
			 * DES-CBC-SHA
			 * DES-CBC-MD5
			 */
			struct __certificates
			{
				/**
				 * constructor
				 */
				__certificates();

				dodoString key;         ///< pathname to the key file
				dodoString keyPassword; ///< password for the key file
				short      keyType;     ///< type of the key[see keyTypeEnum]
				dodoString cert;        ///< pathname to the certificate file
				dodoString ca;          ///< pathname to the certificate authority file
				dodoString caPath;      ///< pathname to a directory that contains trusted SSL CA certificates in PEM format or to a file of CA certificates in PEM format
				dodoString cipher;      ///< allowed SSL ciphers
			};

			/**
			 * @class __openssl_init__
			 * @brief initializes openssl evironment
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
				 * add entropy for randomness
				 */
				void addEntropy();
			};

			extern __openssl_init__ __openssl_init_object__;
		};
	};
};


#endif

#endif
