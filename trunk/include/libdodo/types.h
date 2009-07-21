/***************************************************************************
 *            types.h
 *
 *  Thu Jul  7 00:06:57 2005
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

#ifndef _TYPES_H_
#define _TYPES_H_ 1

#include <libdodo/directives.h>

#include <vector>
#include <list>
#include <deque>
#include <map>
#include <string>

namespace dodo {
	/**
	 * @def addFlag
	 * @brief add bit flag to the statement
	 */
#define addFlag(statement, flag)       (statement) |= (flag)

	/**
	 * @def removeFlag
	 * @brief remove bit flag from the statement
	 */
#define removeFlag(statement, flag)    (statement) &= (~(flag))

	/**
	 * @def isSetFlag
	 * @brief return true if bit flag is set
	 */
#define isSetFlag(statement, flag)     ((statement) & (flag)) != 0

#define dodoString std::string

	/**
	 * @class dodoMapStringCompare
	 * @brief defines compare functor
	 */
	class dodoMapStringCompare {
	  public:

		/**
		 * compares strings
		 * @param first defines first string to compare
		 * @param second defines second string to compare
		 */
		bool operator()(const dodoString &first,
						const dodoString &second);

		/**
		 * compares strings
		 * @param first defines first string to compare
		 * @param second defines second string to compare
		 */
		bool operator()(const dodoString &first,
						const dodoString &second) const;
	};

	/**
	 * @class dodoMapStringCompare
	 * @brief defines compare functor
	 */
	class dodoMapICaseStringCompare {
	  public:

		/**
		 * compares strings
		 * @param first defines first string to compare
		 * @param second defines second string to compare
		 */
		bool operator()(const dodoString &first,
						const dodoString &second);

		/**
		 * compares strings
		 * @param first defines first string to compare
		 * @param second defines second string to compare
		 */
		bool operator()(const dodoString &first,
						const dodoString &second) const;
	};

	/**
	 * @class singleton
	 * @brief defines singleton base class for derived class
	 * @note not threadsafe
	 */
	template<typename T>
	class singleton {
	  public:

		/**
		 * @return logger instance which can be used globally
		 */
		static T &
		getInstance()
		{
			static T t;

			return t;
		}
	};

#define dodoMap std::map

#ifdef USE_DEQUE
#define dodoArray    std::deque
#else
#define dodoArray    std::vector
#endif

	typedef dodoArray<dodoString> dodoStringArray;                                          ///< array of strings

#define dodoList std::list

	typedef dodoList<dodoString> dodoStringList;                                            ///< array of strings

	typedef dodoMap<dodoString, dodoString, dodoMapStringCompare> dodoStringMap;            ///< hash of strings
	typedef dodoMap<dodoString, dodoString, dodoMapICaseStringCompare> dodoICaseStringMap;  ///< hash of strings[case insensitive]

	typedef dodoArray<dodoStringMap> dodoStringMapArray;                                    ///< array of hashes of string

	/**
	 * predifined constants
	 */

	extern dodoString __dodostring__;

	extern dodoStringArray __dodostringarray__;

	extern dodoStringMap __dodostringmap__;

	extern dodoArray<dodoStringArray> __dodostringarrayarray__;

	extern dodoStringMapArray __dodostringmaparray__;
};

#endif
