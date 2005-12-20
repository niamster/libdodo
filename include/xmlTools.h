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
	#include <dodoMap.h>	

	#include <xmlToolsEx.h>
	
	namespace dodo
	{
		
		/**
		 * @struct __node describes xml tree realization
		 * @note children[x][y] =>> [x] describes vector of xml nodes[y] that are realisation of __nodeDef::children[x] definition
		 */
		struct __node
		{
			std::string name;///< name of the node [[tag]]
			
			std::vector< std::vector<__node> > children;///< vector of children's realisation;
			
			dodoMap attributes;///< hash of attributes
			
			std::string value;///< value of the node
			
			std::string ns;///< namespace of the node
			std::string nsHref;///< uri "value" <URI>
			std::string nsDef;///< namespace that this node defines
			std::string nsDefHref;///< namespace's value <URI> that this node defines
		};
	
		/**
		 * @struct __nodeDef describes xml tree definition
		 */
		struct __nodeDef
		{
			/**
			 * constructor
			 */
			__nodeDef();
			
			std::string name;///< name of the node [[tag]]; if empty - for first - gets root, for children - all[but if children do not have in definition own  children]
			
			std::vector<__nodeDef> children;///< vector of children's definitions
			long chLimit;///< limit of children to search for[-1 for unlimit, default]
			
			stringArr attributes;///< attrributes to take from node; if empty - take all
			
			std::string ns;///< namespace of the node; if empty - skips
		};
		
		/**
		 * @struct __xmlInfo desribes info got about given XML document
		 */
		struct __xmlInfo
		{
			/**
			 * constructor
			 */
			__xmlInfo();
			
			/**
			 * constructor
			 * @note initializes with user values
			 */
			__xmlInfo(const std::string &version, const std::string &encoding, const std::string &root, int compression);
			 
			std::string version;///< version of XML document
			std::string encoding;///< encoding of XML document
			std::string root;///< name of the root element of XML document
			
			int compression;///< compression of XML document
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
				 * parses XML from file if you want to use reParse wo calling parseFile/parseBuffer
				 * @param file contains XML to parse
				 */
				#ifndef NO_EX
					virtual void 
				#else
					virtual bool 
				#endif						 
								parseFileInt(const std::string &file);
				
				/**
				 * parses XML from buffer if you want to use reParse wo calling parseFile/parseBuffer
				 * @param buffer contains XML to parcse
				 */
				#ifndef NO_EX
					virtual void 
				#else
					virtual bool 
				#endif						 
								parseBufferInt(const std::string &buffer);
								
				/**
				 * parses XML using __nodeDef XML explanation from file
				 * @return parsed into __node structure given XML
				 * @param definition describes structure of XML
				 * @param file path XML file to parse
				 * @note the first given definition is as root for XML document, even it isn't really like that in document
				 */
				virtual __node parseFile(const __nodeDef &definition, const std::string &file);
				
				/**
				 * parses XML using __nodeDef XML explanation from buffer
				 * @return parsed into __node structure given XML
				 * @param definition describes structure of XML
				 * @param buffer contains XML to parse
				 * @note the first given definition is as root for XML document, even it isn't really like that in document
				 */
				virtual __node parseBuffer(const __nodeDef &definition, const std::string &buffer);
				
				/**
				 * parses XML using __nodeDef XML explanation from file
				 * @param definition describes structure of XML
				 * @param file path XML file to parse
				 * @note the first given definition is as root for XML document, even it isn't really like that in document
				 */
				virtual __node parseFile(const std::string &file);
				
				/**
				 * parses XML using __nodeDef XML explanation from buffer
				 * @return parsed into __node structure given XML
				 * @param buffer contains XML to parse
				 * @note the first given definition is as root for XML document, even it isn't really like that in document
				 */
				virtual __node parseBuffer(const std::string &buffer);

				/**
				 * parses XML using __nodeDef XML explanation with xml you have once parsed; faster
				 * @return parsed into __node structure given XML
				 * @param definition describes structure of XML
				 * @note the first given definition is as root for XML document, even it isn't really like that in document
				 */
				virtual __node reParse(const __nodeDef &definition);
											
				bool icaseNames;///< whether to check nodes names and attributes' names with(out) case matching; with case(false) by default
				
				/**
				 * @return got info about XML from file
				 * @param file path XML file to parse
				 */
				virtual __xmlInfo getXMLFileInfo(const std::string &file);
				
				/**
				 * @return got info about XML from buffer
				 * @param buffer contains XML to parse
				 */
				virtual __xmlInfo getXMLBufferInfo(const std::string &buffer);
						
				/**
				 * clears params of the give node
				 */
				virtual void initNode(__node &node);
												
			protected:
				
				/**
				 * parses whole XML
				 * @return parsed into __node structure given XML
				 * @param node is XML tree node
				 */
				virtual std::vector<__node> parse(xmlNodePtr node);
				
				/**
				 * parses XML using __nodeDef XML explanation using internal built data
				 * @return parsed into __node structure given XML
				 * @param definition describes structure of XML
				 */
				virtual __node parse(const __nodeDef &definition);

				/**
				 * parses XML using __nodeDef XML explanation
				 * @return parsed into __node structure given XMLnode
				 * @param definition describes structure of XML
				 * @param chNode is XML tree node
				 * @param chLimit is limit of children to search for
				 */				
				virtual std::vector<__node> parse(const __nodeDef &definition, const xmlNodePtr chNode, long chLimit);
				
				/**
				 * gets attributes from node
				 * @param node describes the node content
				 * @param attributes describes array of got attributes
				 */
				virtual void getAttributes(const xmlNodePtr node, assocArr &attributes);
				
				/**
				 * gets attributes from node
				 * @param definition describes definitions of the node
				 * @param node describes the node content
				 * @param attributes describes array of got attributes
				 */
				virtual void getAttributes(const __nodeDef &definition, const xmlNodePtr node, assocArr &attributes);
				
				/**
				 * get diff info from node
				 * @param node describes the node content
				 * @param sample describes node that contains result data
				 */
				virtual void getNodeInfo(const xmlNodePtr node, __node &sample);
				
				/**
				 * @note share vars
				 */
				xmlDocPtr document;///< XML Document
				xmlErrorPtr error;///< libxml2 error buffer
				xmlAttr *attribute;///< XML attributes
				stringArr::const_iterator iAttr,jAttr;///< for internal calculations; iterators for attributes
				xmlChar *xChar;///< for internal calculations; to store results
				int result;///< to set result from different operations; to store results
				
				/**
				 * catches libxml2 warnings
				 * @param data is user-defined data
				 * @param error is error descriptor
				 */
				static void errHandler(void *data, xmlErrorPtr error);
				
		};
	}	

#endif


#endif /* _XMLTOOLS_H */
