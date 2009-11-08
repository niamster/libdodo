/***************************************************************************
 *            toolsMisc.h
 *
 *  Wed Aug 24 2005
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
#define _TOOLSMISC_H_ 1

#include <libdodo/directives.h>

#include <libdodo/types.h>

namespace dodo {
    namespace tools {
        /**
         * @typedef escape
         * @brief defines escape function for split/join
         * @param dta defines data to escape
         */
        typedef dodoString (*escape)(const dodoString &data);

        /**
         * @class misc
         * @brief provides misc functionality
         */
        class misc {
          public:

            /**
             * @enum randomStrengthEnum defines type of randomness
             */
            enum randomStrengthEnum {
                RANDOM_STRENGTH_DEFAULT,
                RANDOM_STRENGTH_STRONG
            };

            /**
             * generate random data
             * @param data defines pointer to store data
             * @param size defines size of random data in bytes
             * @param strength defines the ramndomness
             * @note based on /dev/(u)?random
             * RANDOM_STRENGTH_STRONG may block
             */
            static void random(void          *data,
                               unsigned long size,
                               short         strength = RANDOM_STRENGTH_DEFAULT);

            /**
             * @return random data
             * @param size defines size of random data in bytes
             * @param strength defines the ramndomness
             * @note based on /dev/(u)?random
             * RANDOM_STRENGTH_STRONG may block
             * nulls are replaced with '*'
             */
            static dodoString stringRandom(unsigned long size,
                                           short         strength = RANDOM_STRENGTH_DEFAULT);

            /**
             * @return random unsigned long
             * @param strength defines the ramndomness
             * @note based on /dev/(u)?random
             * RANDOM_STRENGTH_STRONG may block
             */
            static unsigned long ulRandom(short strength = RANDOM_STRENGTH_DEFAULT);

            /**
             * @return random long
             * @param strength defines the ramndomness
             * @note based on /dev/(u)?random
             * RANDOM_STRENGTH_STRONG may block
             */
            static long lRandom(short strength = RANDOM_STRENGTH_DEFAULT);

            /**
             * @return random unsigned int
             * @param strength defines the ramndomness
             * @note based on /dev/(u)?random
             * RANDOM_STRENGTH_STRONG may block
             */
            static unsigned int uiRandom(short strength = RANDOM_STRENGTH_DEFAULT);

            /**
             * @return random int
             * @param strength defines the ramndomness
             * @note based on /dev/(u)?random
             * RANDOM_STRENGTH_STRONG may block
             */
            static int iRandom(short strength = RANDOM_STRENGTH_DEFAULT);

            /**
             * @return random unsigned short
             * @param strength defines the ramndomness
             * @note based on /dev/(u)?random
             * RANDOM_STRENGTH_STRONG may block
             */
            static unsigned short usRandom(short strength = RANDOM_STRENGTH_DEFAULT);

            /**
             * @return random short
             * @param strength defines the ramndomness
             * @note based on /dev/(u)?random
             * RANDOM_STRENGTH_STRONG may block
             */
            static short sRandom(short strength = RANDOM_STRENGTH_DEFAULT);

            /**
             * @return random unsigned char
             * @param strength defines the ramndomness
             * @note based on /dev/(u)?random
             * RANDOM_STRENGTH_STRONG may block
             */
            static unsigned char ucRandom(short strength = RANDOM_STRENGTH_DEFAULT);

            /**
             * @return random char
             * @param strength defines the ramndomness
             * @note based on /dev/(u)?random
             * RANDOM_STRENGTH_STRONG may block
             */
            static char cRandom(short strength = RANDOM_STRENGTH_DEFAULT);

            /**
             * @return random double
             * @param strength defines the ramndomness
             * @note based on /dev/(u)?random
             * RANDOM_STRENGTH_STRONG may block
             */
            static double dRandom(short strength = RANDOM_STRENGTH_DEFAULT);

            /**
             * @return true if needle has been found
             * @param arr defines list to search in
             * @param needle defines string to search for
             * @param icase defines case sensitivity
             */
            static bool isInList(const dodoStringList &arr,
                                 const dodoString     &needle,
                                 bool                 icase = false);

            /**
             * @return true if needle has been found
             * @param arr defines array to search in
             * @param needle defines string to search for
             * @param icase defines case sensitivity
             */
            static bool isInArray(const dodoStringArray &arr,
                                  const dodoString      &needle,
                                  bool                  icase = false);

            /**
             * @return array of substrings
             * @param string defines string to split
             * @param separator defines separator string
             * @param limit defines maximum number of parts to divide string; -1 for unlimit
             * @note split will stop when amount of parts exceed limit and the rest of the buffer will be appended to the last piece
             */
            static dodoStringArray split(const dodoString &string,
                                         const dodoString &separator,
                                         int              limit = -1);

            /**
             * @return string of substrings, separated with separator
             * @param fields defines substrings to join
             * @param separator defines separator string
             * @param limit defines maximum number of parts to divide string; -1 for unlimit
             */
            static dodoString join(const dodoStringArray &fields,
                                   const dodoString      &separator,
                                   int                   limit = -1);
        };
    };
};
#endif
