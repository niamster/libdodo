/***************************************************************************
 *            dodoMap.h
 *
 *  Mon Dec 20 23:55:19 2005
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

#ifndef _DODOMAP_H_
#define _DODOMAP_H_

#include <libdodo/directives.h>

#include <map>

#include <libdodo/dodoString.h>

namespace dodo
{
	/**
	 * @class dodoMapFunc defines functions for dodoMap
	 */
	template <typename keyType>
	class dodoMapFunc
	{
		public:

			/**
			 * @typedef type of pointer on function to compare two values
			 */
			typedef bool (*cmpFunc)(const keyType &, const keyType &);

			/**
			 * @typedef type of pointer on function to generate hash for keyType
			 */
			typedef unsigned long (*hashFunc)(const keyType &);
	};

	/**
	 * @class dodoMap is a wrapper for std::map<keyType, valueType>
	 * TODO: apply hash function
	 */
	template <typename keyType,
			  typename valueType,
			  typename dodoMapFunc<keyType>::cmpFunc cmpFunc>
	class dodoMap
	{

		public:

			/**
			 * copy constructor
			 */
			dodoMap(const dodoMap &dodoM) : contents(dodoM.contents)
			{
			}

			/**
			 * constructor
			 */
			dodoMap(std::map<keyType, valueType> a_contents) : contents(a_contents)
			{
			}

			/**
			 * constructor
			 */
			dodoMap()
			{
			}

			/**
			 * @return value of hash by varName or empty valueType already added to map, if not found
			 * @param varName value of hash that points to the value
			 */
			valueType &
			operator[](const keyType &varName)
			{
				typename std::map<keyType, valueType>::iterator i(contents.begin()), j(contents.end());

				for (; i != j; ++i)
					if (cmpFunc(varName, i->first))
						return i->second;

				std::pair < typename std::map<keyType, valueType>::iterator, bool > res = contents.insert(make_pair(varName, valueType()));

				return res.first->second;
			}

			/**
			 * @return value of hash by varName or empty valueType already added to map, if not found
			 * @param varName value of hash that points to the value
			 */
			const valueType &
			operator[](const keyType &varName) const
			{
				typename std::map<keyType, valueType>::iterator i(contents.begin()), j(contents.end());

				for (; i != j; ++i)
					if (cmpFunc(varName, i->first))
						return i->second;

				std::pair < typename std::map<keyType, valueType>::iterator, bool > res = contents.insert(make_pair(varName, valueType()));

				return res.first->second;
			}

			/**
			 * @return iterator by hash or end of hash if not found
			 * @param varName value of hash that points to the value
			 */
			typename std::map<keyType, valueType>::iterator
			find(const keyType &varName)
			{
				typename std::map<keyType, valueType>::iterator i(contents.begin()), j(contents.end());

				for (; i != j; ++i)
					if (cmpFunc(varName, i->first))
						return i;

				return j;
			}

			/**
			 * @return const_iterator by hash or end of hash if not found
			 * @param varName is value of hash that points to the value
			 */
			typename std::map<keyType, valueType>::const_iterator
			find(const keyType &varName) const
			{
				typename std::map<keyType, valueType>::iterator i(contents.begin()), j(contents.end());

				for (; i != j; ++i)
					if (cmpFunc(varName, i->first))
						return i;

				return j;
			}

			/**
			 * removes element with given key
			 * @param varName is value of hash that points to the value
			 */
			void
			erase(const keyType &varName)
			{
				contents.erase(varName);
			}

			/**
			 * insert into hash
			 * @param varName is value of hash that points to the value
			 * @param varVal is value of hash by varName
			 */
			void
			insert(const keyType &varName,
				   const valueType &varVal)
			{
				contents.insert(make_pair(varName, varVal));
			}

			/**
			 * return const_iterator that points on the begin of the original array
			 */
			typename std::map<keyType, valueType>::const_iterator
			begin() const
			{
				return contents.begin();
			}

			/**
			 * return const_iterator that points on the begin of the original array
			 */
			typename std::map<keyType, valueType>::const_iterator
			end() const
			{
				return contents.end();
			}

			/**
			 * return iterator that points on the begin of the original array
			 */
			typename std::map<keyType, valueType>::iterator
			begin()
			{
				return contents.begin();
			}

			/**
			 * return iterator that points on the begin of the original array
			 */
			typename std::map<keyType, valueType>::iterator
			end()
			{
				return contents.end();
			}

			/**
			 * @return true if contents is empty
			 */
			bool
			empty()
			{
				return contents.empty();
			}

			/**
			 * @return size of the map
			 */
			int
			size()
			{
				return contents.size();
			}

			/**
			 * clears contents
			 */
			void
			clear()
			{
				contents.clear();
			}

			operator const std::map < keyType, valueType> & ()
			{
				return contents;
			}

			/**
			 * @return true if value is set by given key
			 */
			bool
			isset(const keyType &varName)
			{
				typename std::map<keyType, valueType>::iterator i(contents.begin()), j(contents.end());

				for (; i != j; ++i)
					if (cmpFunc(varName, i->first))
						return true;

				return false;
			}

			typedef typename std::map<keyType, valueType>::const_iterator const_iterator;       ///< const iterator
			typedef typename std::map<keyType, valueType>::iterator iterator;                   ///< iterator 

		private:

			std::map<keyType, valueType> contents;                                              ///< real array
	};
};

#endif
