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
         * @param str defines null-terminated buffer to construct the string
         */
        string(const char *str);

        /**
         * constructor
         * @param data defines buffer to construct the string
         * @param length defines length of the buffer
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
         * @return true if the string is empty
         */
        bool empty() const;

        /**
         * removes symbols in the string
         * @param index defines position to start erasing from
         * @param count defines amount of symbols to erase
         */
        void erase(unsigned long index = POSITION_BEGIN,
                   unsigned long count = POSITION_END);

        /**
         * appends data to the string
         * @param data defines buffer to append to the string
         * @param length defines length of the data
         */
        void append(const char *data,
                    unsigned long length);

        /**
         * assigns data to the string
         * @param data defines buffer to assign to the string
         * @param length defines length of the data
         */
        void assign(const char *data,
                    unsigned long length);

        /**
         * clears the string
         */
        void clear();

        /**
         * adjusts size of the string
         * @param length defines new string length
         */
        void resize(unsigned long length);

        /**
         * @return position of substring in question
         * @param str defines string to search for
         * @param index defines position to start search from
         */
        unsigned long find(const string &str,
                           unsigned long index = POSITION_BEGIN) const;

        /**
         * replaces chunk in the string
         * @return string with replaced chunk
         * @param index defines position to start replacing from
         * @param count defines amount of symbols to replace
         * @param str defines the replacement string
         */
        string & replace(unsigned long index,
                         unsigned long count,
                         const string &str);

        /**
         * inserts string at position
         * @return string with inserted string
         * @param index defines position to start insert from
         * @param str defines the string to insert
         */
        string & insert(unsigned long index,
                        const string &str);

        /**
         * @return substring
         * @param index defines position of substring start
         * @param length defines length of the substring
         */
        string substr(unsigned long index,
                      unsigned long length = POSITION_END) const;

        /**
         * reserves space for the string
         * @param count defines amount of bytes to reserve
         */
        void reserve(unsigned long count);

        /**
         * assign operator
         * @return new string
         * @param str defines the string to assign
         */
        string & operator=(const string &str);

        /**
         * assign operator
         * @return new string
         * @param str defines the string to assign
         */
        string & operator=(const char *str);

        /**
         * assign operator
         * @return new string
         * @param symbol defines the symbol to assign
         */
        string & operator=(char symbol);

        /**
         * concatenation operator
         * @return new string
         * @param str defines the string to concatenate
         */
        string & operator+=(const string &str);

        /**
         * concatenation operator
         * @return new string
         * @param str defines the string to concatenate
         */
        string & operator+=(const char *str);

        /**
         * concatenation operator
         * @return new string
         * @param symbol defines the symbol to concatenate
         */
        string & operator+=(char symbol);

        /**
         * cast operator
         */
        operator const char *() const;

        static unsigned long POSITION_BEGIN; ///< defines start of the string
        static unsigned long POSITION_END; ///< defines end of the string

      private:

        char *buf; ///< buffer to hold data
        unsigned long bufSize; ///< actual size of the buffer
        unsigned long strLen; ///< actual length of the data
    };

    /**
     * less operator
     * @param s1 defines string to compare
     * @param s2 defines string to compare
     */
    bool operator<(const string &s1,
                   const string &s2);

    /**
     * greater operator
     * @param s1 defines string to compare
     * @param s2 defines string to compare
     */
    bool operator>(const string &s1,
                   const string &s2);

    /**
     * equal operator
     * @param s1 defines string to compare
     * @param s2 defines string to compare
     */
    bool operator==(const string &s1,
                    const string &s2);

    /**
     * equal operator
     * @param s1 defines string to compare
     * @param s2 defines string to compare
     */
    bool operator==(const string &s1,
                    const char *s2);

    /**
     * add operator
     * @param s1 defines string to add
     * @param s2 defines string to add
     */
    string operator+(const string &s1,
                     const string &s2);

    /**
     * add operator
     * @param s1 defines string to add
     * @param s2 defines string to add
     */
    string operator+(const string &s1,
                     const char *s2);

    /**
     * add operator
     * @param s1 defines string to add
     * @param s2 defines string to add
     */
    string operator+(const char *s1,
                     const string &s2);
};
#endif
