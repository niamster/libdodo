/***************************************************************************
 *            toolsLibrary.h
 *
 *  Thu Mar 13 2008
 *  Copyright  2008  Dmytro Milinevskyy
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

#ifndef _TOOLSLIBRARY_H_
#define _TOOLSLIBRARY_H_ 1

#include <libdodo/directives.h>

#ifdef DL_EXT
#include <libdodo/types.h>

namespace dodo {
    namespace tools {
        /**
         * @class library
         * @brief provides handling object from static and dynamic libraries
         */
        class library {
          public:

            /**
             * consructor
             */
            library();

            /**
             * consructor
             * @param path defines path to the library[if not in ldconfig db] or library name
             */
            library(const dodo::string &path);

            /**
             * destructor
             */
            virtual ~library();

            /**
             * open library
             * @param path defines path to the library[if not in ldconfig db] or library name
             */
            virtual void open(const dodo::string &path);

            /**
             * close library
             * @note function exported from the library will be inaccessible
             */
            virtual void close();

            /**
             * get function from the library
             * @param name defines function name
             */
            virtual void *function(const dodo::string &name);

            /**
             * get function from the library
             * @param name defines function name
             */
            virtual void *operator[](const dodo::string &name);

          protected:

            void *handle; ///< handle to library
        };
    };
};
#endif
#endif
