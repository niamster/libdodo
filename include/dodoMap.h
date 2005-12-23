/***************************************************************************
 *            dodoMap.h
 *
 *  Mon Dec 20 23:55:19 2005
 *  Copyright  2005  Ni@m
 *  niam.niam@gmail.com
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef _DODOMAP_H_
#define _DODOMAP_H_

#include <directives.h>
#include <types.h>

namespace dodo
{

		/**
		 * @class dodoMap is a duck for std::map<std::string, std::string> but controlling varName
		 * if varName's value is not defined - '_undefined_' will be returned
		 */
		class dodoMap
		{
			public:
			
				/**
				 * constructor
				 */
				dodoMap();
			
				/**
				 * @return value by hash or '_undefined_' if not found
				 * @param varName value of hash that points to the value
				 */
				std::string operator[](const std::string &varName);

				assocArr realArr;///< real array
				
				bool icase;///< whether to react on keys with keys or no; false[react] by default
				
				/**
				 * @return size of the map
				 */
				int size();
				
			protected:
				
				assocArr::iterator i;///< iterator for methodArr(from begin)
				assocArr::iterator j;///< iterator for methodArr(indicates end)
		};
}

#endif /*DODOMAP_H_*/
