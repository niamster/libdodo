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
#include <map>
#include <string>

namespace dodo
{
		/**
		 * @class dodoMap is a duck for std::map<std::string, any_type> but controlling varName
		 * if varName's value is not defined - empty anyType will be returned
		 */
		template <typename anyType>
		class __dodoMap
		{
						
			public:
			
				/**
				 * copy constructor
				 */
				__dodoMap(const __dodoMap &dodoM) : icase(dodoM.icase), realArr(dodoM.realArr)
				{
				}
			
				/**
				 * constructor
				 */
				__dodoMap(): icase(false)
				{
				}
				
				bool icase;///< whether to react on keys with keys or no; false[react] by default
							
				/**
				 * @return value by hash or empty anyType if not found
				 * @param varName value of hash that points to the value
				 */			 
				anyType
				operator[](const std::string &varName)
				{
					i = realArr.begin();
					j = realArr.end();
					
					if (icase)
						cmpFunc = strcasecmp;
					else
						cmpFunc = strcmp;
					
					for (;i!=j;++i)
						if (cmpFunc(varName.c_str(),i->first.c_str()) == 0)
							return i->second;		
					
					return type;				
				}
				
				/**
				 * return iterator that points on the begin of the original array
				 */
				typename std::map<std::string, anyType>::iterator
				begin()
				{
					return realArr.begin();
				}
				
				/**
				 * return iterator that points on the begin of the original array
				 */
				typename std::map<std::string, anyType>::iterator
				end()
				{
					return realArr.end();
				}
												
				/**
				 * @return size of the map
				 */
				int 
				size()
				{
					return realArr.size();
				}
				
				/**
				 * clears realArr
				 */
				void
				clear()
				{
					realArr.clear();
				}
				
				std::map<std::string, anyType> realArr;///< real array
				
				anyType type;
								
			private:
				
				typedef int(*charCmp)(const char *, const char *);
				
				charCmp cmpFunc;
				
				typename std::map<std::string, anyType>::iterator i;///< iterator for realArr(from begin)
				typename std::map<std::string, anyType>::iterator j;///< iterator for realArr(indicates end)
		};
};

#endif /*DODOMAP_H_*/
