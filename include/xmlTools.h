/***************************************************************************
 *            xmlTools.h
 *
 *  Tue Nov 29 23:31:55 2005
 *  Copyright  2005  User
 *  Email
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
 
#ifndef _XMLTOOLS_H_
#define _XMLTOOLS_H_

#include <directives.h>

#ifdef LIBXML2_EXT

	#include <types.h>
		
	namespace dodo
	{
	
		/**
		 * @struct __node describes xml tree realization
		 */
		struct __node
		{
			std::string name;///< name of the node [[tag]]
			
			std::vector<__node> children;///< vector of children's realisation
			assocArr attributes;///< hash of attributes
			std::string value;///< value of the node
		};
	
		/**
		 * @struct __nodeDef describes xml tree definition
		 */
		struct __nodeDef
		{
			std::string name;///< name of the node [[tag]]
			
			std::vector<__nodeDef> children;///< vector of children's definitions
			unsigned long chLimit;///< limit of children to search for
			
			stringArr attributes;///< attrributes to take from node
		};
		
		/**
		 * @class xmlTools provides user friendly communication with XML.
		 */
		class xmlTools
		{
		};
	}	

#endif


#endif /* _XMLTOOLS_H */