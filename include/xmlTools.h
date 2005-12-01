/***************************************************************************
 *            xmlTools.h
 *
 *  Tue Nov 29 23:31:55 2005
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
 
#ifndef _XMLTOOLS_H_
#define _XMLTOOLS_H_

#include <directives.h>

#ifdef LIBXML2_EXT

	#include <string>
	
	#include <libxml/xmlmemory.h>
	#include <libxml/parser.h>

	#include <types.h>
	#include <xmlToolsEx.h>
	
	namespace dodo
	{
		
		/**
		 * @struct __node describes xml tree realization
		 */
		struct __node
		{
			std::string name;///< name of the node [[tag]]
			
			std::vector< std::vector<__node> > children;///< vector of children's realisation
			assocArr attributes;///< hash of attributes
			std::string value;///< value of the node
		};
	
		/**
		 * @struct __nodeDef describes xml tree definition
		 */
		struct __nodeDef
		{
			__nodeDef();
			
			std::string name;///< name of the node [[tag]]
			
			std::vector<__nodeDef> children;///< vector of children's definitions
			long chLimit;///< limit of children to search for[-1 for unlimit, default]
			
			stringArr attributes;///< attrributes to take from node
		};
		
		/**
		 * @class xmlTools provides user friendly communication with XML.
		 */
		class xmlTools
		{
			
			public: 
				
				/**
				 * constructor
				 */
				xmlTools();
				
				/**
				 * destructor
				 */
				virtual ~xmlTools();
				
				/**
				 * parces XML using __nodeDef XML explanation
				 * @return parced into __node structure given XML
				 * @param definition describes structure of XML
				 * @param file path XML file to parce
				 * @note the first given definition is as root for XML document, even it isn't really like that in document
				 */
				virtual __node parseFile(const __nodeDef &definition, const std::string &file);
				
			protected:
				
				/**
				 * parces XML using __nodeDef XML explanation using internal built data
				 * @return parced into __node structure given XML
				 * @param definition describes structure of XML
				 */
				virtual __node parse(const __nodeDef &definition);

				/**
				 * parces XML using __nodeDef XML explanation
				 * @return parced into __node structure given XMLnode
				 * @param definition describes structure of XML
				 * @param chNode is XML tree node
				 * @param chLimit is limit of children to search for
				 */				
				virtual std::vector<__node> parse(const __nodeDef &definition, const xmlNodePtr chNode, long chLimit);
				
				xmlDocPtr document;///< XML Document
				xmlNodePtr node;///< XML node
				xmlErrorPtr error;///< libxml2 error buffer
				
				stringArr::const_iterator iAttr,jAttr;///< iterators for attributes[to save memmory and improve performance]
				
				xmlChar *xChar;///< for internal calculations
		};
	}	

#endif


#endif /* _XMLTOOLS_H */
