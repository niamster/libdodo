/***************************************************************************
 *            toolsMisc.h
 *
 *  Wed Aug 24 00:42:51 2005
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

#ifndef _TOOLSMISC_H_
#define _TOOLSMISC_H_

#include <libdodo/directives.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/un.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdlib.h>

#include <libdodo/types.h>
#include <libdodo/ioNetwork.h>
#include <libdodo/toolsNetwork.h>
#include <libdodo/toolsString.h>
#include <libdodo/toolsCode.h>
#include <libdodo/toolsMiscEx.h>

namespace dodo
{
	namespace tools
	{
		/**
		 * @typedef escape defines escape function for explode/implode
		 * @param dta defines data to escape
		 */
		typedef dodoString (*escape)(const dodoString &data);

		/**
		 * @enum miscRandomStrengthEnum defines type of randomness
		 */
		enum miscRandomStrengthEnum
		{
			MISC_RANDOMSTRENGTH_DEFAULT,
			MISC_RANDOMSTRENGTH_STRONG
		};

		/**
		 * @class misc provides misc functionality
		 */
		class misc
		{
			public:

				/**
				 * generate random data
				 * @param data defines pointer to store data
				 * @param size defines size of random data in bytes
				 * @param strength defines the ramndomness
				 * @note based on /dev/(u)?random
				 * MISC_RANDOMSTRENGTH_STRONG may block
				 */
				static void random(void *data, unsigned long size, short strength = MISC_RANDOMSTRENGTH_DEFAULT);

				/**
				 * @return random data
				 * @param size defines size of random data in bytes
				 * @param strength defines the ramndomness
				 * @note based on /dev/(u)?random
				 * MISC_RANDOMSTRENGTH_STRONG may block
				 * '\0' is replaced with '*'
				 */
				static dodoString stringRandom(unsigned long size, short strength = MISC_RANDOMSTRENGTH_DEFAULT);

				/**
				 * @return random unsigned long
				 * @param strength defines the ramndomness
				 * @note based on /dev/(u)?random
				 * MISC_RANDOMSTRENGTH_STRONG may block
				 */
				static unsigned long ulRandom(short strength = MISC_RANDOMSTRENGTH_DEFAULT);

				/**
				 * @return random long
				 * @param strength defines the ramndomness
				 * @note based on /dev/(u)?random
				 * MISC_RANDOMSTRENGTH_STRONG may block
				 */
				static long lRandom(short strength = MISC_RANDOMSTRENGTH_DEFAULT);

				/**
				 * @return random unsigned int
				 * @param strength defines the ramndomness
				 * @note based on /dev/(u)?random
				 * MISC_RANDOMSTRENGTH_STRONG may block
				 */
				static unsigned int uiRandom(short strength = MISC_RANDOMSTRENGTH_DEFAULT);

				/**
				 * @return random int
				 * @param strength defines the ramndomness
				 * @note based on /dev/(u)?random
				 * MISC_RANDOMSTRENGTH_STRONG may block
				 */
				static int iRandom(short strength = MISC_RANDOMSTRENGTH_DEFAULT);

				/**
				 * @return random unsigned short
				 * @param strength defines the ramndomness
				 * @note based on /dev/(u)?random
				 * MISC_RANDOMSTRENGTH_STRONG may block
				 */
				static unsigned short usRandom(short strength = MISC_RANDOMSTRENGTH_DEFAULT);

				/**
				 * @return random short
				 * @param strength defines the ramndomness
				 * @note based on /dev/(u)?random
				 * MISC_RANDOMSTRENGTH_STRONG may block
				 */
				static short sRandom(short strength = MISC_RANDOMSTRENGTH_DEFAULT);

				/**
				 * @return random unsigned char
				 * @param strength defines the ramndomness
				 * @note based on /dev/(u)?random
				 * MISC_RANDOMSTRENGTH_STRONG may block
				 */
				static unsigned char ucRandom(short strength = MISC_RANDOMSTRENGTH_DEFAULT);

				/**
				 * @return random char
				 * @param strength defines the ramndomness
				 * @note based on /dev/(u)?random
				 * MISC_RANDOMSTRENGTH_STRONG may block
				 */
				static char cRandom(short strength = MISC_RANDOMSTRENGTH_DEFAULT);

				/**
				 * @return random double
				 * @param strength defines the ramndomness
				 * @note based on /dev/(u)?random
				 * MISC_RANDOMSTRENGTH_STRONG may block
				 */
				static double dRandom(short strength = MISC_RANDOMSTRENGTH_DEFAULT);

				/**
				 * @return true if needle has been found
				 * @param arr defines list to search in
				 * @param needle defines string to search for
				 * @param icase defines case sensitivity
				 */
				static bool isInList(const dodoStringList &arr, const dodoString &needle, bool icase = false);

				/**
				 * @return true if needle has been found
				 * @param arr defines array to search in
				 * @param needle defines string to search for
				 * @param icase defines case sensitivity
				 */
				static bool isInArray(const dodoStringArray &arr, const dodoString &needle, bool icase = false);

				/**
				 * @return array of substrings
				 * @param string defines string to explode
				 * @param separator defines separator string
				 * @param limit defines maximum number of parts to divide string; -1 for unlimit
				 * @note explode will stop when amount of parts exceed limit and the rest of the buffer will be appended to the last piece
				 */
				static dodoStringArray explode(const dodoString &string, const dodoString &separator, int limit = -1);

				/**
				 * @return array of substrings
				 * @param string defines string to explode
				 * @param escapeF defines function which will escape every substring
				 * @param separator defines separator string
				 * @param limit defines maximum number of parts to divide string; -1 for unlimit
				 * @note explode will stop when amount of parts exceed limit and the rest of the buffer will be appended to the last piece
				 */
				static dodoStringArray explode(const dodoString &string, escape escapeF, const dodoString &separator, int limit = -1);

				/**
				 * @return string of substrings, separated with separator
				 * @param fields defines substrings to implode
				 * @param escapeF defines function which will escape every substring
				 * @param separator defines separator string
				 * @param frame defines frame string[frame='; substring => 'substring']
				 * @param limit defines maximum number of parts to divide string; -1 for unlimit
				 */
				static dodoString implode(const dodoStringArray &fields, escape escapeF, const dodoString &separator, const dodoString &frame, int limit = -1);

				/**
				 * @return string of substrings, separated with separator
				 * @param fields defines substrings to implode
				 * @param escapeF defines function which will escape every substring
				 * @param separator defines separator string
				 * @param limit defines maximum number of parts to divide string; -1 for unlimit
				 */
				static dodoString implode(const dodoStringArray &fields, escape escapeF, const dodoString &separator, int limit = -1);

				/**
				 * @return string of substrings, separated with separator
				 * @param fields defines substrings to implode
				 * @param separator defines separator string
				 * @param limit defines maximum number of parts to divide string; -1 for unlimit
				 */
				static dodoString implode(const dodoStringArray &fields, const dodoString &separator, int limit = -1);

				/**
				 * @return string of substrings, separated with separator
				 * @param fields defines substrings to implode
				 * @param separator defines separator string
				 * @param frame defines frame string[frame='; substring => 'substring']
				 * @param limit defines maximum number of parts to divide string; -1 for unlimit
				 */
				static dodoString implode(const dodoStringArray &fields, const dodoString &separator, const dodoString &frame, int limit = -1);

				/**
				 * send mail using sendmail external program
				 * @param to defines mail address[possible multiply separated with coma]
				 * @param subject defines a subject of the letter;for utf should use: `'=?utf-8?B?'.encodeBase64(subject).'?='`
				 * @param message defines a message to send
				 * @param headers defines extra headers
				 * @param path defines path to sendmail
				 */
				static void mail(const dodoString &to, const dodoString &subject, const dodoString &message, const dodoString &headers = __dodostring__, const dodoString &path = "/usr/sbin/sendmail");

				/**
				 * send mail
				 * @param host defines host of smtp server(ip)
				 * @param port defines port of smtp server
				 * @param to defines mail address where to send[possible multiply separated with coma]
				 * @param from defines mail address of sender
				 * @param subject defines a subject of the letter;for utf should use: `'=?utf-8?B?'.encodeBase64(subject).'?='`
				 * @param message defines a message to send
				 * @param login defines a login for auth
				 * @param pass defines a password for auth
				 * @param headers defines extra headers[each must ends with `\r\n`]
				 * @note if login is emty no auth is performed
				 */
				static void mail(const dodoString &host, int port, const dodoString &to, const dodoString &from, const dodoString &subject, const dodoString &message, const dodoString &login = __dodostring__, const dodoString &pass = __dodostring__, const dodoString &headers = __dodostring__);
		};
	};
};
#endif
