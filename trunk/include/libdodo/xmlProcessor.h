/***************************************************************************
 *            xmlProcessor.h
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

#ifndef _XMLPROCESSOR_H_
#define _XMLPROCESSOR_H_

#include <libdodo/directives.h>

#ifdef LIBXML2_EXT

#include <libxml/parser.h>
#include <libxml/xmlmemory.h>

#endif

#include <libdodo/xmlProcessorEx.h>
#include <libdodo/xmlNode.h>
#include <libdodo/types.h>

namespace dodo
{
	namespace xml
	{
		/**
		 * @struct __nodeDef defines processor tree definition
		 */
		struct __nodeDef
		{
			/**
			 * constructor
			 */
			__nodeDef();

			dodoString name;                                    ///< name of the node [[tag]]; if empty - for first - gets root, for children - all[but if children do not have in definition own children]

			dodoMap<dodoString, __nodeDef> children;            ///< children definitions

			long chLimit;                                       ///< limit of children to search for[-1 for unlimit, default]
			bool ignoreChildrenDef;                             ///< if true - parse all children tree if no children definition is given; false by default

			dodoStringArray attributes;                         ///< node attrributes; if empty - take all
			bool ignoreAttributesDef;                           ///< if true - parse all attributes if no attributes definition is given; true by default

			dodoString ns;                                      ///< node namespace; if empty parser skips namespace specification
		};

		/**
		 * @struct __info desribes info got about given XML document
		 */
		struct __info
		{
			/**
			 * constructor
			 */
			__info();

			/**
			 * constructor
			 * @note initializes with user values
			 */
			__info(const dodoString &version, const dodoString &encoding, const dodoString &root, int compression);

			dodoString version;     ///< version of XML document
			dodoString encoding;    ///< encoding of XML document
			dodoString root;        ///< name of the root element of XML document

			int compression;        ///< compression of XML document
		};

		/**
		 * @class processor provides XML parsing
		 */
		class processor
		{
			private:

				/**
				 * copy constructor
				 * to prevent copying
				 */
				processor(processor &xt);

			public:

				/**
				 * constructor
				 */
				processor();

				/**
				 * destructor
				 */
				virtual ~processor();

				/**
				 * parse XML file using __nodeDef XML definition
				 * @return parsed XML in node structure
				 * @param definition defines structure of XML
				 * @param file defines path to XML file
				 */
				virtual node parseFile(const __nodeDef &definition, const dodoString &file);

				/**
				 * parse XML buffer using __nodeDef XML definition
				 * @return parsed XML in node structure
				 * @param definition defines structure of XML
				 * @param buffer defines XML buffer
				 */
				virtual node parseBuffer(const __nodeDef &definition, const dodoString &buffer);

				/**
				 * parse XML file
				 * @return parsed XML in node structure
				 * @param file defines path to XML file
				 */
				virtual node parseFile(const dodoString &file);

				/**
				 * parse XML buffer
				 * @return parsed XML in node structure
				 * @param buffer defines XML buffer
				 */
				virtual node parseBuffer(const dodoString &buffer);

				bool icaseNames;    ///< if true ignore case of names[false by default]

				/**
				 * @return XML info
				 * @param file defines path to XML file
				 */
				virtual __info getXMLFileInfo(const dodoString &file);

				/**
				 * @return XML info
				 * @param buffer defines XML buffer
				 */
				virtual __info getXMLBufferInfo(const dodoString &buffer);

				/**
				 * clear parameters of the give node
				 * @param xnode defines node to clean
				 */
				static void initNode(node &xnode);

				/**
				 * clear parameters of the give node definition
				 * @param xnode defines node definition to clean
				 */
				static void initNodeDef(__nodeDef &xnode);

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
				virtual dodoString createXML(const node &root, const dodoString &encoding = "UTF-8", const dodoString &version = "1.0")  const;

				/**
				 * @return XML string
				 * @param xnode defines root node of XML document
				 */
				virtual dodoString createNode(const node &xnode) const;

			protected:

				/**
				 * parse XML using __nodeDef XML definition
				 * @return parsed XML in node structure
				 * @param definition defines structure of XML
				 */
				virtual node parse(const __nodeDef &definition);

	#ifdef LIBXML2_EXT

				/**
				 * @return true if content is in CDATA
				 * @param xnode defines XML tree node
				 */
				bool isCDATA(xmlNodePtr xnode);

				/**
				 * parse XML
				 * @return parsed XML in node structure
				 * @param xnode defines XML tree node
				 */
				virtual dodoArray<node> parse(xmlNodePtr xnode);

				/**
				 * parses XML using __nodeDef XML definition
				 * @return parsed XML nodes in node structures
				 * @param definition defines structure of XML
				 * @param chNode defines XML tree node
				 * @param chLimit defines limit of children
				 */
				virtual dodoArray<node> parse(const __nodeDef &definition, const xmlNodePtr chNode, long chLimit);

				/**
				 * get node attributes
				 * @param xnode defines node content
				 * @param attributes defines buffer for attributes
				 */
				virtual void getAttributes(const xmlNodePtr xnode, dodoStringMap &attributes);

				/**
				 * get node attributes
				 * @param definition defines structure of XML
				 * @param xnode defines node content
				 * @param attributes defines buffer for attributes
				 */
				virtual void getAttributes(const __nodeDef &definition, const xmlNodePtr xnode, dodoStringMap &attributes);

				/**
				 * get node info
				 * @param xnode defines node content
				 * @param sample defines buffer for node
				 */
				virtual void getNodeInfo(const xmlNodePtr xnode, node &sample);

	#endif

			private:

	#ifdef LIBXML2_EXT

				/**
				 * @typedef xcharCmp defines name compare function
				 */
				typedef int (*xcharCmp)(const xmlChar *, const xmlChar *);

				xcharCmp cmpFunc;    ///< name compare function

				/**
				 * find node by definition
				 * @param definition defines structure of XML
				 * @param node defines node content
				 */
				xmlNodePtr findNode(const __nodeDef &definition, xmlNodePtr node);

				xmlDocPtr document;     ///< XML Document
				xmlAttr *attribute;     ///< XML attributes

				/**
				 * catche libprocessor2 warnings
				 * @param data defines user-defined data
				 * @param error defines error descriptor
				 */
				static void errHandler(void *data, xmlErrorPtr error);

	#endif

		};
	};
};

#endif

