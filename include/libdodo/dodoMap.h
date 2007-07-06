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

#include <directives.h>

#include <map>
#include <dodoString.h>

namespace dodo
{
		/**
		 * @class dodoMap is a duck for std::map<dodoString, any_type> but controlling varName
		 * if varName's value is not defined - empty anyType will be returned
		 */
		template <typename valueType, typename keyType=dodoString>
		class dodoMap
		{
						
			public:
			
				/**
				 * copy constructor
				 */
				dodoMap(const dodoMap &dodoM) : icase(dodoM.icase), 
													realArr(dodoM.realArr)
				{
				}
			
				/**
				 * constructor
				 */
				dodoMap(): icase(false)
				{
				}
				
				bool icase;///< whether to react on keys with keys or no; false[react] by default
							
				/**
				 * @return value of hash by varName or empty valueType already added to map, if not found
				 * @param varName value of hash that points to the value
				 */			 
				valueType &
				operator[](const keyType &varName)
				{
					typename std::map<keyType, valueType>::iterator i(realArr.begin()), j(realArr.end());
					
					if (icase)
						cmpFunc = strcasecmp;
					else
						cmpFunc = strcmp;
					
					for (;i!=j;++i)
						if (cmpFunc(varName.c_str(),i->first.c_str()) == 0)
							return i->second;		
					
					std::pair<typename std::map<keyType, valueType>::iterator, bool> res = realArr.insert(make_pair(varName, type));
					
					return res.first->second;				
				}
							
				/**
				 * @return iterator by hash or end of hash if not found
				 * @param varName value of hash that points to the value
				 */			 
				typename std::map<keyType, valueType>::iterator
				find(const keyType &varName)
				{
					if (icase)
					{
						typename std::map<keyType, valueType>::iterator i(realArr.begin()), j(realArr.end());
						
						for (;i!=j;++i)
							if (strcasecmp(varName.c_str(),i->first.c_str()) == 0)
								return i;		
						
						return j;
					}
					else
						return realArr.find(varName);				
				}
							
				/**
				 * @return const_iterator by hash or end of hash if not found
				 * @param varName is value of hash that points to the value
				 */			 
				typename std::map<keyType, valueType>::const_iterator
				find(const keyType &varName) const
				{
					if (icase)
					{
						typename std::map<keyType, valueType>::iterator i(realArr.begin()), j(realArr.end());
						
						for (;i!=j;++i)
							if (strcasecmp(varName.c_str(),i->first.c_str()) == 0)
								return i;		
						
						return j;
					}
					else
						return realArr.find(varName);				
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
					realArr.insert(make_pair(varName, varVal));
				}
				
				/**
				 * return const_iterator that points on the begin of the original array
				 */
				typename std::map<keyType, valueType>::const_iterator
				begin() const
				{
					return realArr.begin();
				}
				
				/**
				 * return const_iterator that points on the begin of the original array
				 */
				typename std::map<keyType, valueType>::const_iterator
				end() const
				{
					return realArr.end();
				}
				
				/**
				 * return iterator that points on the begin of the original array
				 */
				typename std::map<keyType, valueType>::iterator
				begin()
				{
					return realArr.begin();
				}
				
				/**
				 * return iterator that points on the begin of the original array
				 */
				typename std::map<keyType, valueType>::iterator
				end()
				{
					return realArr.end();
				}
				
				/**
				 * @return true if realArr is empty
				 */
				bool
				empty()
				{
					return realArr.empty();
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
				
				/**
				 * @return true if value is set by given key
				 */
				bool
				isset(const keyType &varName)
				{
					typename std::map<keyType, valueType>::iterator i(realArr.begin()), j(realArr.end());
					
					if (icase)
						cmpFunc = strcasecmp;
					else
						cmpFunc = strcmp;
					
					for (;i!=j;++i)
						if (cmpFunc(varName.c_str(),i->first.c_str()) == 0)
							return true;		
					
					return false;				 	
				}
				
                typedef typename std::map<keyType, valueType>::const_iterator const_iterator;
                typedef typename std::map<keyType, valueType>::iterator iterator;

				std::map<keyType, valueType> realArr;///< real array
				
				valueType type;///< copy of typed
								
			private:
				
				int(*cmpFunc)(const char *, const char *);
		};
};

#endif
