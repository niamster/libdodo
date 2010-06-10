/***************************************************************************
 *            toolsString.h
 *
 *  Sun Oct 30 2007
 *  Copyright  2007  Ni@m
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

#ifndef _TOOLSSTRING_H_
#define _TOOLSSTRING_H_ 1

#include <libdodo/directives.h>

#include <libdodo/types.h>

namespace dodo {
    namespace tools {
        /**
         * @class string
         * @brief provides various string tools
         */
        class string {
          public:

            /**
             * replace substrings in string
             * @param needle defines substring which to replace
             * @param replacement defines string which to replace with
             * @param data defines string where to replace substrings
             */
            static void replace(const dodo::string &needle,
                                const dodo::string &replacement,
                                dodo::string       &data);

            /**
             * replace substrings in string
             * @param needle defines substring which to replace
             * @param replacement defines string which to replace with
             * @param data defines string where to replace substrings
             */
            static void replace(const dodoStringArray &needle,
                                const dodoStringArray &replacement,
                                dodo::string            &data);

            /**
             * @return true if strings are equal
             * @param first defines string to compare
             * @param second defines string for compare
             */
            static bool equal(const dodo::string &first,
                              const dodo::string &second);

            /**
             * @return true if strings are equal
             * @param first defines string to compare
             * @param second defines string for compare
             * @note string case is ignored
             */
            static bool iequal(const dodo::string &first,
                               const dodo::string &second);

            /**
             * @return true in needle is found
             * @param str defines string where to look for the needle
             * @param needle defines string which to look for
             * @param icase defines case sensitivity
             */
            static bool contains(const dodo::string &str,
                                 const dodo::string &needle,
                                 bool             icase = false);

            /**
             * @return position of needle if it is found
             * @param str defines string where to look for the needle
             * @param needle defines string which to look for
             * @param icase defines case sensitivity
             */
            static unsigned long find(const dodo::string &str,
                                      const dodo::string &needle,
                                      bool             icase = false);

            /**
             * @return position of needle if it is found
             * @param str defines string where to look for the needle
             * @param needle defines string which to look for
             * @param position defines start position for search
             * @param icase defines case sensitivity
             */
            static unsigned long find(const dodo::string &str,
                                      const dodo::string &needle,
                                      unsigned long    position,
                                      bool             icase = false);

            /**
             * @return formated string
             * @param format defines format of the result string
             * @note format according to the printf
             */
            static dodo::string format(const dodo::string &format,
                                     ...);

            /**
             * remove symbols from the end and from the begin of the string
             * @return trimmed string
             * @param data defines string to trim
             * @param symbol defines symbol to remove
             */
            static dodo::string trim(const dodo::string &data,
                                   const char       symbol = ' ');

            /**
             * remove symbols from the end and from the begin of the string
             * @return trimmed string
             * @param data defines string to trim
             * @param symbols defines symbols to remove
             * @param symCount is amount of symbols in @param symbols
             */
            static dodo::string trim(const dodo::string &data,
                                   const char       symbols[],
                                   int              symCount);

            /**
             * remove symbols from the end of the string
             * @return trimmed string
             * @param data defines string to trim
             * @param symbols defines symbols to remove
             * @param symCount is amount of symbols in @param symbols
             */
            static dodo::string rTrim(const dodo::string &data,
                                    const char       symbols[],
                                    int              symCount);

            /**
             * remove symbols from the begin of the string
             * @return trimmed string
             * @param data defines string to trim
             * @param symbols defines symbols to remove
             * @param symCount is amount of symbols in @param symbols
             */
            static dodo::string lTrim(const dodo::string &data,
                                    const char       symbols[],
                                    int              symCount);

            /**
             * remove symbols from the end of the string
             * @return trimmed string
             * @param data defines string to trim
             * @param symbol defines symbol to remove
             */
            static dodo::string rTrim(const dodo::string &data,
                                    char             symbol = ' ');

            /**
             * remove symbols from the begin of the string
             * @return trimmed string
             * @param data defines string to trim
             * @param symbol defines symbol to remove
             */
            static dodo::string lTrim(const dodo::string &data,
                                    char             symbol = ' ');

            /**
             * @return converted long to string
             * @param number defines number to convert
             */
            static dodo::string lToString(long number);

            /**
             * @return converted unsigned long to string
             * @param number defines number to convert
             */
            static dodo::string ulToString(unsigned long number);

            /**
             * @return converted int to string
             * @param number defines number to convert
             */
            static dodo::string iToString(int number);

            /**
             * @return converted unsigned int to string
             * @param number defines number to convert
             */
            static dodo::string uiToString(unsigned int number);

            /**
             * @return converted short to string
             * @param number defines number to convert
             */
            static dodo::string sToString(short number);

            /**
             * @return converted unsigned short to string
             * @param number defines number to convert
             */
            static dodo::string usToString(unsigned short number);

            /**
             * @return converted float to string
             * @param number defines number to convert
             */
            static dodo::string fToString(float number);

            /**
             * @return converted double to string
             * @param number defines number to convert
             */
            static dodo::string dToString(double number);

            /**
             * @return string converted to unsigned long
             */
            static unsigned long stringToUL(const dodo::string &data);

            /**
             * @return string converted to long
             */
            static long stringToL(const dodo::string &data);

            /**
             * @return string converted to int
             */
            static int stringToI(const dodo::string &data);

            /**
             * @return string converted to unsigned int
             */
            static unsigned int stringToUI(const dodo::string &data);

            /**
             * @return string converted to short
             */
            static short stringToS(const dodo::string &data);

            /**
             * @return string converted to unsigned short
             */
            static unsigned short stringToUS(const dodo::string &data);

            /**
             * @return string converted to double
             */
            static double stringToD(const dodo::string &data);

            /**
             * @return string converted to float
             */
            static float stringToF(const dodo::string &data);
        };
    };
};
#endif
