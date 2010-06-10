/***************************************************************************
 *            string.h
 *
 *  Thu Jun  10 2010
 *  Copyright  2010  Ni@m
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

#ifndef _STRING_H_
#define _STRING_H_ 1

#include <libdodo/directives.h>

namespace dodo {
    /**
     * @class string
     * @brief defines basic string type
     */
    class string {
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
         */
        string(const char *data);

        /**
         * constructor
         */
        string(const char *data,
               unsigned long length);

        /**
         * constructor
         */
        string(char, unsigned long length = 1);

        /**
         * destructor
         */
        ~string();

        /**
         * @return pointer to the data
         */
        const char *data() const;

        /**
         * @return length of the data
         */
        unsigned long size() const;

        /**
         *
         */
        bool empty() const;

        /* /\** */
        /*  * */
        /*  *\/ */
        /* string & append(const string &); */

        /* /\** */
        /*  * */
        /*  *\/ */
        /* string & append(const string &, unsigned long); */

        /* /\** */
        /*  * */
        /*  *\/ */
        /* string & append(unsigned long, char); */

        /* /\** */
        /*  * */
        /*  *\/ */
        /* string& append( const string& str, unsigned long index, unsigned long len ); */

        /**
         *
         */
        string & erase(unsigned long);

        /**
         *
         */
        string & erase(unsigned long, unsigned long);

        /**
         *
         */
        void clear();

        /**
         *
         */
        void resize(unsigned long);

        /* /\** */
        /*  * */
        /*  *\/ */
        /* string & assign(const string &); */

        /* /\** */
        /*  * */
        /*  *\/ */
        /* string & assign(const char *, unsigned long); */

        /* /\** */
        /*  * */
        /*  *\/ */
        /* string & assign(unsigned long, char); */

        /* /\** */
        /*  * */
        /*  *\/ */
        /* string& assign( const string& str, unsigned long index, unsigned long len ); */

        /**
         *
         */
        unsigned long find(const string &) const;

        /* /\** */
        /*  * */
        /*  *\/ */
        /* unsigned long find(char) const; */

        /**
         *
         */
        unsigned long find(const string &, unsigned long) const;

        /* /\** */
        /*  * */
        /*  *\/ */
        /* unsigned long find(char, unsigned long) const; */

        /* /\** */
        /*  * */
        /*  *\/ */
        /* string &replace(unsigned long index1, */
        /*                 unsigned long num1, */
        /*                 const string& str, */
        /*                 unsigned long index2, */
        /*                 unsigned long num2 ); */

        string& replace( unsigned long index, unsigned long num, const string& str );


        string& insert( unsigned long index, const char* str );

        string substr( unsigned long index = 0, unsigned long length = string::npos ) const;

        void reserve( unsigned long size = 0 );

        /**
         *
         */
        string & operator=(const string &data);

        /**
         *
         */
        string & operator+=(const string &);

        /* /\** */
        /*  * */
        /*  *\/ */
        /* string & operator=(char data); */

        /**
         *
         */
        char operator[](unsigned long index) const;

        static unsigned long npos;

      private:

        char *buf; ///< internal buffer to hold string and metadata
        unsigned long len; ///< actual length of the string
    };

    /**
     *
     */
    bool & operator<(const string &, const string &data);

    /**
     *
     */
    bool & operator>(const string &, const string &data);

    /**
     *
     */
    bool & operator==(const string &, const string &data);

    /**
     *
     */
    string & operator+(const string &data, const string &);

    /**
     *
     */
    string & operator+(const string &, const char *data);

    /**
     *
     */
    string & operator+(const char *data, const string &);
};
#endif
