/***************************************************************************
 *            string.h
 *
 *  Thu Jun  10 2010
 *  Copyright  2010  Dmytro Milinevskyy
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

#ifndef _STRING_H_
#define _STRING_H_ 1

#include <libdodo/directives.h>

namespace dodo {
    /**
     * @class string
     * @brief defines basic string type
     */
    class string {
        friend bool operator<(const string &s1, const string &s2);
        friend bool operator>(const string &s1, const string &s2);
        friend bool operator==(const string &s1, const string &s2);
        friend bool operator==(const string &s1, const char *s2);
        friend string operator+(const string &s1, const string &s2);
        friend string operator+(const string &s1, const char *s2);
        friend string operator+(const char *s1, const string &s2);

      public:

        /**
         * constructor
         */
        string();

        /**
         * copy constructor
         */
        string(const string &);

        /**
         * constructor
         * @param data defines null-terminated buffer to construct the string
         */
        string(const char *str);

        /**
         * constructor
         * @param data defines buffer to construct the string
         * @param data defines length of the buffer
         */
        string(const char *data,
               unsigned long length);

        /**
         * constructor
         * @param symbol defines character to construct the string
         * @param count defines amount of characters to fill the string
         */
        string(char symbol,
               unsigned long count = 1);

        /**
         * destructor
         */
        ~string();

        /**
         * @return pointer to the string data
         */
        const char *data() const;

        /**
         * @return length of the string
         */
        unsigned long size() const;

        /**
         * @return true if string is empty
         */
        bool empty() const;

        /**
         *
         */
        void erase(unsigned long index = POSITION_BEGIN,
                   unsigned long count = POSITION_END);

        /**
         *
         */
        void clear();

        /**
         *
         */
        void resize(unsigned long length);

        /**
         *
         */
        unsigned long find(const string &str,
                           unsigned long index = POSITION_BEGIN) const;

        /**
         *
         */
        string & replace(unsigned long index,
                         unsigned long num,
                         const string &str);

        /**
         *
         */
        string & insert(unsigned long index,
                        const string &str);

        /**
         *
         */
        string substr(unsigned long index,
                      unsigned long length = POSITION_END) const;

        /**
         *
         */
        void reserve(unsigned long count);

        /**
         *
         */
        string & operator=(const string &str);

        /**
         *
         */
        string & operator=(char);

        /**
         *
         */
        string & operator+=(const string &);

        /**
         *
         */
        string & operator+=(char);

        /**
         *
         */
        char operator[](unsigned long index) const;

        /**
         *
         */
        operator const char *() const;

        enum positionEnum {
            POSITION_BEGIN = 0,
            POSITION_END = -1,
        };

      private:

        char *buf; ///< buffer to hold data
        unsigned long bufSize; ///< actual size of the buffer
        unsigned long strLen; ///< actual length of the data
    };

    /**
     *
     */
    bool operator<(const string &s1,
                   const string &s2);

    /**
     *
     */
    bool operator>(const string &s1,
                   const string &s2);

    /**
     *
     */
    bool operator==(const string &s1,
                    const string &s2);

    /**
     *
     */
    bool operator==(const string &s1,
                    const char *s2);

    /**
     *
     */
    string operator+(const string &s1,
                     const string &s2);

    /**
     *
     */
    string operator+(const string &s1,
                     const char *s2);

    /**
     *
     */
    string operator+(const char *s1,
                     const string &s2);
};
#endif
