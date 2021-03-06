/***************************************************************************
 *            toolsRegexp.h
 *
 *  Thu Sep 20 2005
 *  Copyright  2005  Dmytro Milinevskyy
 *  milinevskyy@gmail.com
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

#ifndef _TOOLSREGEXP_H_
#define _TOOLSREGEXP_H_ 1

#include <libdodo/directives.h>

#include <libdodo/types.h>

namespace dodo {
    namespace tools {
        struct __regexp__;

        /**
         * @class regexp
         * @brief provides REGEXP support
         * @note PCRE is much faster
         * both POSIX and PCRE don't support binary patterns
         * POSIX doesn't support binary match sting
         */
        class regexp {
          private:

            /**
             * copy constructor
             * @note to prevent copying
             */
            regexp(regexp &rt);

          public:

            /**
             * constructor
             */
            regexp();

            /**
             * constructor
             * @param pattern defines regex pattern
             */
            regexp(const dodo::string &pattern);

            /**
             * destructor
             */
            virtual ~regexp();

            /**
             * @return true if matched pattern
             * @param pattern defines regex pattern
             * @param sample defines a match string
             * @param pockets defines array that will be filled with matched substrings in '()'
             * @note first in pocket is a first match
             */
            virtual bool match(const dodo::string &pattern,
                               const dodo::string &sample,
                               dodoStringArray  &pockets = __dodostringarray__);

            /**
             * match with pattern previously given for match/compile method
             * @return true if matched pattern
             * @param sample defines a match string
             * @param pockets defines array that will be filled with matched substrings in '()'
             * @note first in pocket is a first match
             */
            virtual bool match(const dodo::string &sample,
                               dodoStringArray  &pockets = __dodostringarray__);

            /**
             * replace substrings in match string
             * @return string with replaced substrings
             * @param pattern defines regex pattern
             * @param sample defines a match string
             * @param replacements defines array that will be filled with matched substrings in '()'
             * @note if pattern is not matched - the sample will be returned
             */
            virtual dodo::string replace(const dodo::string      &pattern,
                                       const dodo::string      &sample,
                                       const dodoStringArray &replacements);

            /**
             * replace substrings in match string with pattern previously given for match/compile method
             * @return string with replaced substrings
             * @param sample defines a match string
             * @param replacements defines array that will be filled with matched substrings in '()'
             * @note if pattern is not matched - the sample will be returned
             */
            virtual dodo::string replace(const dodo::string      &sample,
                                       const dodoStringArray &replacements);

            /**
             * compile pattern
             * @param pattern defines regex pattern
             */
            virtual void compile(const dodo::string &pattern);

            bool extended;                              ///< if true use extended regext support[true by default]

            bool icase;                                 ///< if true ignore case[false by default]

            bool greedy;                                ///< if true REGEXPs are greedy[true by default]

            bool multiline;                             ///< if true match strings are treated as multiline[false by default]

          protected:

            /**
             * @struct __regex_match__
             * @brief defines begin and end of matched substring
             */
            struct __regex_match__ {
                int begin;
                int end;
            };

            /**
             * generate list of boundaries matched in match string by pattern
             * @return true if match string has matched substrings
             * @param sample defines a match string
             */
            virtual bool boundMatch(const dodo::string &sample);

          private:

            __regexp__ *regex;                          ///< regexp handle

            dodoArray<__regex_match__> boundaries;      ///< list of boundaries matched in match string by pattern
        };
    };
};
#endif
