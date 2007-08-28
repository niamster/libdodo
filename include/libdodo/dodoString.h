/***************************************************************************
 *            dodoString.h
 *
 *  Fri Nov 10 07:07:19 2006
 *  Copyright  2006  Ni@m
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

#ifndef _DODOSTRING_H_
#define _DODOSTRING_H_

#include <directives.h>

#include <string>

namespace dodo
{
	/**
	 * @class dodoString extends standart std::string class
	 */
	class dodoString: public std::string
	{
		public:
			
			/**
			 * constructor
			 */
			dodoString();

			/**
			 * constructor
			 */
			dodoString(const std::string &str);

			/**
			 * constructor
			 */
			dodoString(const char *str);

			/**
			 * copy constructor
			 */
			dodoString(const dodoString &str);

			/**
			 * constructor
			 */
			dodoString(const char *str, size_type length);

			/**
			 * constructor
			 */
			dodoString(const std::string &str, size_type index, size_type length);

			/**
			 * constructor
			 */
			dodoString(size_type length, const char &ch);
			
			/**
			 * constructor
			 */
			template<typename inputIterator>
			dodoString(inputIterator start, inputIterator end);
			
			/**
			 * @return true if strings are equal
			 * @param first string to compare
			 * @param second string to compare
			 */
			static bool equal(const dodoString &first, const dodoString &second);

			/**
			 * @return true if strings are equal
			 * @param first string to compare
			 * @param second string to compare
			 * @note it ignores case
			 */
			static bool iequal(const dodoString &first, const dodoString &second);
	};
};

#endif
