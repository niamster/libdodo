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
	 * @struct __xmlNode describes xml tree realization
	 * @note children[x][y] =>> [x] describes vector of xml nodes[y] that are realisation of __xmlNodeDef::children[x] definition
	 */
	struct __xmlNode
	{
		/**
		 * constructor
		 */
		__xmlNode();

		dodoString name;                                                            ///< name of the node [[tag]]

		dodoMap<dodoString, dodoArray<__xmlNode>, stringTools::equal> children;      ///< vector of children's realisation;

		dodoStringMap attributes;                                                   ///< hash of attributes

		dodoString value;                                                           ///< value of the node

		dodoString ns;                                                              ///< namespace of the node
		dodoString nsHref;                                                          ///< uri "value" <URI>
		dodoString nsDef;                                                           ///< namespace that this node defines
		dodoString nsDefHref;                                                       ///< namespace's value <URI> that this node defines

		bool CDATA;                                                                 ///< true if CDATA
		bool empty;                                                                 ///< true if node doesn't have content : `<node />`
	};

	/**
	 * @struct __xmlNodeDef describes xml tree definition
	 */
	struct __xmlNodeDef
	{
		/**
		 * constructor
		 */
		__xmlNodeDef();

		dodoString name;                                ///< name of the node [[tag]]; if empty - for first - gets root, for children - all[but if children do not have in definition own  children]

		std::map<dodoString, __xmlNodeDef> children;    ///< vector of children's definitions

		long chLimit;                                   ///< limit of children to search for[-1 for unlimit, default]
		bool ignoreChildrenDef;                         ///< if true - parse all children tree if no children difenition; false by default

		dodoStringArr attributes;                       ///< attrributes to take from node; if empty - take all
		bool ignoreAttributesDef;                       ///< if true - parse all attributes if no attributes difenition; true by default

		dodoString ns;                                  ///< namespace of the node; if empty - skips
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
	 * @class xml provides user friendly communication with XML.
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
		 * parses XML from file if you want to use reParse wo calling parseFile/parseBuffer
		 * @param file contains XML to parse
		 */
		virtual void
		parseFileInt(const dodoString &file);

		/**
		 * parses XML from buffer if you want to use reParse wo calling parseFile/parseBuffer
		 * @param buffer contains XML to parcse
		 */
		virtual void
		parseBufferInt(const dodoString &buffer);

		/**
		 * parses XML using __xmlNodeDef XML explanation from file
		 * @return parsed into __xmlNode structure given XML
		 * @param definition describes structure of XML
		 * @param file path XML file to parse
		 * @note the first given definition is as root for XML document, even it isn't really like that in document
		 */
		virtual __xmlNode parseFile(const __xmlNodeDef &definition, const dodoString &file);

		/**
		 * parses XML using __xmlNodeDef XML explanation from buffer
		 * @return parsed into __xmlNode structure given XML
		 * @param definition describes structure of XML
		 * @param buffer contains XML to parse
		 * @note the first given definition is as root for XML document, even it isn't really like that in document
		 */
		virtual __xmlNode parseBuffer(const __xmlNodeDef &definition, const dodoString &buffer);

		/**
		 * parses XML using __xmlNodeDef XML explanation from file
		 * @param definition describes structure of XML
		 * @param file path XML file to parse
		 * @note the first given definition is as root for XML document, even it isn't really like that in document
		 */
		virtual __xmlNode parseFile(const dodoString &file);

		/**
		 * parses XML using __xmlNodeDef XML explanation from buffer
		 * @return parsed into __xmlNode structure given XML
		 * @param buffer contains XML to parse
		 * @note the first given definition is as root for XML document, even it isn't really like that in document
		 */
		virtual __xmlNode parseBuffer(const dodoString &buffer);

		/**
		 * parses XML using __xmlNodeDef XML explanation with xml you have once parsed; faster
		 * @return parsed into __xmlNode structure given XML
		 * @param definition describes structure of XML
		 * @note the first given definition is as root for XML document, even it isn't really like that in document
		 */
		virtual __xmlNode reParse(const __xmlNodeDef &definition);

		bool icaseNames;        ///< whether to check nodes names and attributes' names with(out) case matching; with case(false) by default

		/**
		 * @return got info about XML from file
		 * @param file path XML file to parse
		 */
		virtual __xmlInfo getXMLFileInfo(const dodoString &file);

		/**
		 * @return got info about XML from buffer
		 * @param buffer contains XML to parse
		 */
		virtual __xmlInfo getXMLBufferInfo(const dodoString &buffer);

		/**
		 * clears params of the give node
		 */
		static void initNode(__xmlNode &node);

		/**
		 * clears params of the give node definition
		 */
		static void initNodeDef(__xmlNodeDef &node);

		/**
		 * frees internal structures
		 */
		virtual void clear();

		/**
		 * @return XML string
		 * @param root defines root node of XML document
		 * @param encoding declears encoding of XML
		 * @param version declears version of XML
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
		 * @param node is XML tree node
		 */
		bool isCDATA(xmlNodePtr node);

		/**
		 * parses whole XML
		 * @return parsed into __xmlNode structure given XML
		 * @param node is XML tree node
		 */
		virtual dodoArray<__xmlNode> parse(xmlNodePtr node);

		/**
		 * parses XML using __xmlNodeDef XML explanation using internal built data
		 * @return parsed into __xmlNode structure given XML
		 * @param definition describes structure of XML
		 */
		virtual __xmlNode parse(const __xmlNodeDef &definition);

		/**
		 * parses XML using __xmlNodeDef XML explanation
		 * @return parsed into __xmlNode structure given XMLnode
		 * @param definition describes structure of XML
		 * @param chNode is XML tree node
		 * @param chLimit is limit of children to search for
		 */
		virtual dodoArray<__xmlNode> parse(const __xmlNodeDef &definition, const xmlNodePtr chNode, long chLimit);

		/**
		 * gets attributes from node
		 * @param node describes the node content
		 * @param attributes describes array of got attributes
		 */
		virtual void getAttributes(const xmlNodePtr node, dodoStringMap &attributes);

		/**
		 * gets attributes from node
		 * @param definition describes definitions of the node
		 * @param node describes the node content
		 * @param attributes describes array of got attributes
		 */
		virtual void getAttributes(const __xmlNodeDef &definition, const xmlNodePtr node, dodoStringMap &attributes);

		/**
		 * get diff info from node
		 * @param node describes the node content
		 * @param sample describes node that contains result data
		 */
		virtual void getNodeInfo(const xmlNodePtr node, __xmlNode &sample);

		private:

		typedef int (*xcharCmp)(const xmlChar *, const xmlChar *);

		xcharCmp cmpFunc;        ///< function to compare

		xmlNodePtr findNode(const __xmlNodeDef &definition, xmlNodePtr node);

		xmlDocPtr document;         ///< XML Document
		xmlAttr *attribute;         ///< XML attributes

		/**
		 * catches libxml2 warnings
		 * @param data is user-defined data
		 * @param error is error descriptor
		 */
		static void errHandler(void *data, xmlErrorPtr error);
	};
};

#endif

#endif
