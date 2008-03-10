/***************************************************************************
 *            xml.h
 *
 *  Tue Nov 29 23:31:55 2005
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

#ifndef _XML_H_
#define _XML_H_

#include <libdodo/directives.h>

#ifdef LIBXML2_EXT

#include <libxml/parser.h>
#include <libxml/xmlmemory.h>

#include <libdodo/xmlEx.h>
#include <libdodo/types.h>
#include <libdodo/dodoMap.h>

namespace dodo
{

	/**
	 * @struct __xmlNode defines xml tree
	 * @note children[x][y]: [x] defines vector of xml nodes[y] that are realisation of __xmlNodeDef::children[x] definition
	 */
	struct __xmlNode
	{
		/**
		 * constructor
		 */
		__xmlNode();

		dodoString name;                                                                ///< name of the node [[tag]]

		dodoMap<dodoString, dodoArray<__xmlNode>, stringTools::equal> children;         ///< children

		dodoStringMap attributes;                                                       ///< attributes

		dodoString value;                                                               ///< value of the node

		dodoString ns;                                                                  ///< namespace of the node
		dodoString nsHref;                                                              ///< uri "value"
		dodoString nsDef;                                                               ///< namespace
		dodoString nsDefHref;                                                           ///< namespace URI

		bool CDATA;                                                                     ///< true if CDATA
		bool empty;                                                                     ///< true if node doesn't have contents : `<node />`
	};

	/**
	 * @struct __xmlNodeDef defines xml tree definition
	 */
	struct __xmlNodeDef
	{
		/**
		 * constructor
		 */
		__xmlNodeDef();

		dodoString name;                                    ///< name of the node [[tag]]; if empty - for first - gets root, for children - all[but if children do not have in definition own children]

		std::map<dodoString, __xmlNodeDef> children;        ///< children definitions

		long chLimit;                                       ///< limit of children to search for[-1 for unlimit, default]
		bool ignoreChildrenDef;                             ///< if true - parse all children tree if no children definition is given; false by default

		dodoStringArray attributes;                         ///< node attrributes; if empty - take all
		bool ignoreAttributesDef;                           ///< if true - parse all attributes if no attributes definition is given; true by default

		dodoString ns;                                      ///< node namespace; if empty parser skips namespace specification
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
		__xmlInfo(const dodoString &version, const dodoString &encoding, const dodoString &root, int compression);

		dodoString version;     ///< version of XML document
		dodoString encoding;    ///< encoding of XML document
		dodoString root;        ///< name of the root element of XML document

		int compression;        ///< compression of XML document
	};

	/**
	 * @class xml provides XML parsing
	 */
	class xml
	{
		private:

			/**
			 * copy constructor
			 * to prevent copying
			 */
			xml(xml &xt);

		public:

			/**
			 * constructor
			 */
			xml();

			/**
			 * destructor
			 */
			virtual ~xml();

			/**
			 * parse XML file using __xmlNodeDef XML definition
			 * @return parsed XML in __xmlNode structure
			 * @param definition defines structure of XML
			 * @param file defines path to XML file
			 */
			virtual __xmlNode parseFile(const __xmlNodeDef &definition, const dodoString &file);

			/**
			 * parse XML buffer using __xmlNodeDef XML definition
			 * @return parsed XML in __xmlNode structure
			 * @param definition defines structure of XML
			 * @param buffer defines XML buffer
			 */
			virtual __xmlNode parseBuffer(const __xmlNodeDef &definition, const dodoString &buffer);

			/**
			 * parse XML file
			 * @return parsed XML in __xmlNode structure
			 * @param file defines path to XML file
			 */
			virtual __xmlNode parseFile(const dodoString &file);

			/**
			 * parse XML buffer
			 * @return parsed XML in __xmlNode structure
			 * @param buffer defines XML buffer
			 */
			virtual __xmlNode parseBuffer(const dodoString &buffer);

			bool icaseNames;    ///< if true ignore case of names[false by default]

			/**
			 * @return XML info
			 * @param file defines path to XML file
			 */
			virtual __xmlInfo getXMLFileInfo(const dodoString &file);

			/**
			 * @return XML info
			 * @param buffer defines XML buffer
			 */
			virtual __xmlInfo getXMLBufferInfo(const dodoString &buffer);

			/**
			 * clear parameters of the give node
			 * @param node defines node to clean
			 */
			static void initNode(__xmlNode &node);

			/**
			 * clear parameters of the give node definition
			 * @param node defines node definition to clean
			 */
			static void initNodeDef(__xmlNodeDef &node);

			/**
			 * free internal structures
			 */
			virtual void clear();

			/**
			 * @return XML string
			 * @param root defines root node of XML document
			 * @param encoding defines XML encoding
			 * @param version defines XML version
			 */
			virtual dodoString createXML(const __xmlNode &root, const dodoString &encoding = "UTF-8", const dodoString &version = "1.0")  const;

			/**
			 * @return XML string
			 * @param root defines root node of XML document
			 */
			virtual dodoString createNode(const __xmlNode &node) const;

		protected:

			/**
			 * @return true if content is in CDATA
			 * @param node defines XML tree node
			 */
			bool isCDATA(xmlNodePtr node);

			/**
			 * parse XML
			 * @return parsed XML in __xmlNode structure
			 * @param node defines XML tree node
			 */
			virtual dodoArray<__xmlNode> parse(xmlNodePtr node);

			/**
			 * parse XML using __xmlNodeDef XML definition
			 * @return parsed XML in __xmlNode structure
			 * @param definition defines structure of XML
			 */
			virtual __xmlNode parse(const __xmlNodeDef &definition);

			/**
			 * parses XML using __xmlNodeDef XML definition
			 * @return parsed XML nodes in __xmlNode structures
			 * @param definition defines structure of XML
			 * @param chNode defines XML tree node
			 * @param chLimit defines limit of children
			 */
			virtual dodoArray<__xmlNode> parse(const __xmlNodeDef &definition, const xmlNodePtr chNode, long chLimit);

			/**
			 * get node attributes
			 * @param node defines node content
			 * @param attributes defines buffer for attributes
			 */
			virtual void getAttributes(const xmlNodePtr node, dodoStringMap &attributes);

			/**
			 * get node attributes
			 * @param definition defines structure of XML
			 * @param node defines node content
			 * @param attributes defines buffer for attributes
			 */
			virtual void getAttributes(const __xmlNodeDef &definition, const xmlNodePtr node, dodoStringMap &attributes);

			/**
			 * get node info
			 * @param node defines node content
			 * @param sample defines buffer for node
			 */
			virtual void getNodeInfo(const xmlNodePtr node, __xmlNode &sample);

		private:

			/**
			 * @typedef defines name compare function 
			 */
			typedef int (*xcharCmp)(const xmlChar *, const xmlChar *);

			xcharCmp cmpFunc;    ///< name compare function

			/**
			 * find node by definition
			 * @param definition defines structure of XML
			 * @param node defines node content
			 */
			xmlNodePtr findNode(const __xmlNodeDef &definition, xmlNodePtr node);

			xmlDocPtr document;     ///< XML Document
			xmlAttr *attribute;     ///< XML attributes

			/**
			 * catche libxml2 warnings
			 * @param data defines user-defined data
			 * @param error defines error descriptor
			 */
			static void errHandler(void *data, xmlErrorPtr error);
	};
};

#endif

#endif
