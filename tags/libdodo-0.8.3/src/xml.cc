/***************************************************************************
 *            xml.cc
 *
 *  Wed Nov 30 22:02:16 2005
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


#include <libdodo/xml.h>

#ifdef LIBXML2_EXT

using namespace dodo;

__xmlInfo::__xmlInfo(const dodoString &a_version,
					 const dodoString &a_encoding,
					 const dodoString &a_root,
					 int a_compression) : version(a_version),
										  encoding(a_encoding),
										  root(a_root),
										  compression(a_compression)
{
}

//-------------------------------------------------------------------

__xmlInfo::__xmlInfo()
{
}

//-------------------------------------------------------------------

__xmlNodeDef::__xmlNodeDef() : chLimit(-1),
							   ignoreChildrenDef(false),
							   ignoreAttributesDef(true)
{
}

//-------------------------------------------------------------------

__xmlNode::__xmlNode() : CDATA(false),
						 empty(false)
{
}

//-------------------------------------------------------------------

xml::xml(xml &xt)
{
}

//-------------------------------------------------------------------

xml::xml() : icaseNames(false),
			 document(NULL)
{
	xmlPedanticParserDefault(0);
	xmlInitParser();
	xmlSetStructuredErrorFunc(NULL, xml::errHandler);
}

//-------------------------------------------------------------------

xml::~xml()
{
	xmlFreeDoc(document);

	xmlCleanupParser();
}

//-------------------------------------------------------------------

__xmlNode
xml::reParse(const __xmlNodeDef &definition)
{
	if (document == NULL)
		throw baseEx(ERRMODULE_LIBXML2, XMLEX_REPARCE, ERR_LIBDODO, XMLEX_NOTPARCEDBEFORE, XMLEX_NOTPARCEDBEFORE_STR, __LINE__, __FILE__);

	return parse(definition);
}

//-------------------------------------------------------------------

bool
xml::isCDATA(xmlNodePtr chNode)
{
	xmlNodePtr node = chNode->children;
	while (node != NULL)
	{
		if (node->type == XML_CDATA_SECTION_NODE)
			return true;

		node = node->next;
	}

	return false;
}

//-------------------------------------------------------------------

__xmlNode
xml::parseFile(const __xmlNodeDef &definition,
			   const dodoString &file)
{
	xmlFreeDoc(document);

	document = xmlParseFile(file.c_str());
	if (document == NULL)
	{
		xmlErrorPtr error = xmlGetLastError();

		throw baseEx(ERRMODULE_LIBXML2, XMLEX_PARCEFILE, ERR_LIBXML2, error->code, error->message, __LINE__, __FILE__, file);
	}

	return parse(definition);
}

//-------------------------------------------------------------------

__xmlNode
xml::parseBuffer(const __xmlNodeDef &definition,
				 const dodoString &buffer)
{
	xmlFreeDoc(document);

	document = xmlParseMemory(buffer.c_str(), buffer.size());
	if (document == NULL)
	{
		xmlErrorPtr error = xmlGetLastError();

		throw baseEx(ERRMODULE_LIBXML2, XMLEX_PARCEBUFFER, ERR_LIBXML2, error->code, error->message, __LINE__, __FILE__);
	}

	return parse(definition);
}

//-------------------------------------------------------------------

void
xml::parseFileInt(const dodoString &file)
{
	xmlFreeDoc(document);

	document = xmlParseFile(file.c_str());
	if (document == NULL)
	{
		xmlErrorPtr error = xmlGetLastError();

		throw baseEx(ERRMODULE_LIBXML2, XMLEX_PARCEFILEINT, ERR_LIBXML2, error->code, error->message, __LINE__, __FILE__, file);
	}
}

//-------------------------------------------------------------------

void
xml::parseBufferInt(const dodoString &buffer)
{
	xmlFreeDoc(document);

	document = xmlParseMemory(buffer.c_str(), buffer.size());
	if (document == NULL)
	{
		xmlErrorPtr error = xmlGetLastError();

		throw baseEx(ERRMODULE_LIBXML2, XMLEX_PARCEBUFFERINT, ERR_LIBXML2, error->code, error->message, __LINE__, __FILE__);
	}
}

//-------------------------------------------------------------------

__xmlNode
xml::parse(const __xmlNodeDef &definition)
{
	xmlNodePtr node = xmlDocGetRootElement(document);
	if (node == NULL)
	{
		xmlErrorPtr error = xmlGetLastError();

		throw baseEx(ERRMODULE_LIBXML2, XMLEX_PARCE, ERR_LIBXML2, error->code, error->message, __LINE__, __FILE__);
	}

	__xmlNode sample;

	node = findNode(definition, node);

	if (node == NULL)
		return sample;

	getNodeInfo(node, sample);

	getAttributes(definition, node, sample.attributes);

	if (node->children == NULL)
	{
		sample.empty = true;
		return sample;
	}

	sample.CDATA = isCDATA(node);

	if (definition.children.size() > 0)
	{
		std::map<dodoString, __xmlNodeDef>::const_iterator i(definition.children.begin()), j(definition.children.end());
		for (; i != j; ++i)
			sample.children.insert(i->first, parse(i->second, node->children, definition.chLimit));
	}
	else
	{
		if (definition.ignoreChildrenDef)
		{
			node = node->children;

			__xmlNode one;

			while (node != NULL)
			{
				if (node->type != XML_ELEMENT_NODE)
				{
					node = node->next;
					continue;
				}

				getNodeInfo(node, one);

				getAttributes(node, one.attributes);

				if (node->children == NULL)
					one.empty = true;

				one.CDATA = isCDATA(node);

				one.children.insert((char *)node->name, parse(node->children));

				sample.children[(char *)node->name].push_back(one);

				initNode(one);

				node = node->next;
			}
		}
	}

	return sample;
}

//-------------------------------------------------------------------

dodoArray<__xmlNode>
xml::parse(const __xmlNodeDef &definition,
		   const xmlNodePtr chNode,
		   long chLimit)
{
	xmlNodePtr node = chNode, subNode;

	__xmlNode sample, one;
	dodoArray<__xmlNode> sampleArr;

	if (icaseNames)
		cmpFunc = xmlStrcasecmp;
	else
		cmpFunc = xmlStrcmp;

	do
	{
		if (node->type != XML_ELEMENT_NODE)
		{
			node = node->next;
			continue;
		}

		if (chLimit != -1)
		{
			if (chLimit <= 0)
				return sampleArr;

			--chLimit;
		}

		if (definition.ns.size() > 0)
		{
			if (node->ns == NULL)
			{
				node = node->next;
				continue;
			}

			if (cmpFunc(node->ns->prefix, (xmlChar *)definition.ns.c_str()) != 0)
			{
				node = node->next;
				continue;
			}
		}

		if (cmpFunc(node->name, (xmlChar *)definition.name.c_str()) != 0)
		{
			node = node->next;
			continue;
		}

		initNode(sample);

		getNodeInfo(node, sample);
		getAttributes(definition, node, sample.attributes);

		if (node->children == NULL)
			sample.empty = true;

		sample.CDATA = isCDATA(node);

		if (definition.children.size() > 0)
		{
			std::map<dodoString, __xmlNodeDef>::const_iterator i(definition.children.begin()), j(definition.children.end());
			for (; i != j; ++i)
				sample.children.insert(i->first, parse(i->second, node->children, definition.chLimit));

		}
		else
		{
			if (definition.ignoreChildrenDef)
			{
				subNode = node->children;

				while (subNode != NULL)
				{
					if (subNode->type != XML_ELEMENT_NODE)
					{
						subNode = subNode->next;
						continue;
					}

					getNodeInfo(subNode, one);

					getAttributes(subNode, one.attributes);

					if (subNode->children == NULL)
						one.empty = true;

					one.CDATA = isCDATA(subNode);

					one.children.insert((char *)subNode->name, parse(subNode->children));

					sample.children[(char *)subNode->name].push_back(one);

					initNode(one);

					subNode = subNode->next;
				}
			}
		}

		sampleArr.push_back(sample);

		node = node->next;

	}
	while (node != NULL);

	return sampleArr;
}

//-------------------------------------------------------------------

void
xml::errHandler(void        *data,
				xmlErrorPtr error)
{
}

//-------------------------------------------------------------------

void
xml::getAttributes(const __xmlNodeDef &definition,
				   const xmlNodePtr node,
				   dodoStringMap &attributes)
{
	attribute = node->properties;

	if (definition.attributes.size() > 0)
	{
		dodoStringArray::const_iterator jAttr = definition.attributes.end();
		if (icaseNames)
		{
			while (attribute != NULL)
			{
				dodoStringArray::const_iterator iAttr = definition.attributes.begin();
				for (; iAttr != jAttr; ++iAttr)
				{
					if (xmlStrcmp(attribute->name, (xmlChar *)iAttr->c_str()) == 0)
					{
						xmlChar *xChar = xmlGetProp(node, attribute->name);
						if (xChar != NULL)
						{
							attributes[*iAttr] = (char *)xChar;
							xmlFree(xChar);
						}
					}
				}

				attribute = attribute->next;
			}
		}
		else
		{
			dodoStringArray::const_iterator iAttr = definition.attributes.begin();
			for (; iAttr != jAttr; ++iAttr)
			{
				xmlChar *xChar = xmlGetProp(node, (xmlChar *)iAttr->c_str());
				if (xChar != NULL)
				{
					attributes[*iAttr] = (char *)xChar;
					xmlFree(xChar);
				}
			}
		}
	}
	else
	{
		if (definition.ignoreAttributesDef)
		{
			while (attribute != NULL)
			{
				xmlChar *xChar = xmlGetProp(node, attribute->name);
				if (xChar != NULL)
				{
					attributes[(char *)attribute->name] = (char *)xChar;
					xmlFree(xChar);
				}

				attribute = attribute->next;
			}
		}
	}
}

//-------------------------------------------------------------------

void
xml::getAttributes(const xmlNodePtr node,
				   dodoStringMap &attributes)
{
	attribute = node->properties;

	while (attribute != NULL)
	{
		xmlChar *xChar = xmlGetProp(node, attribute->name);
		if (xChar != NULL)
		{
			attributes[(char *)attribute->name] = (char *)xChar;
			xmlFree(xChar);
		}

		attribute = attribute->next;
	}
}

//-------------------------------------------------------------------

void
xml::getNodeInfo(const xmlNodePtr node,
				 __xmlNode &resNode)
{
	if (node->ns != NULL)
	{
		resNode.ns = (char *)node->ns->prefix;
		resNode.nsHref = (char *)node->ns->href;
	}

	if (node->nsDef != NULL)
	{
		resNode.nsDef = (char *)node->nsDef->prefix;
		resNode.nsDefHref = (char *)node->nsDef->href;
	}

	if (node->name != NULL)
		resNode.name.assign((char *)node->name);

	xmlChar *xChar = xmlNodeListGetString(document, node->children, 1);
	if (xChar != NULL)
	{
		resNode.value.assign((char *)xChar);
		xmlFree(xChar);
	}
}

//-------------------------------------------------------------------

__xmlInfo
xml::getXMLFileInfo(const dodoString &file)
{
	document = xmlParseFile(file.c_str());
	if (document == NULL)
	{
		xmlErrorPtr error = xmlGetLastError();

		throw baseEx(ERRMODULE_LIBXML2, XMLEX_GETXMLFILEINFO, ERR_LIBXML2, error->code, error->message, __LINE__, __FILE__, file);
	}

	return __xmlInfo(document->version != NULL ? (char *)document->version : __dodostring__,
					 document->encoding != NULL ? (char *)document->encoding : __dodostring__,
					 (document->children != NULL && document->children->name != NULL) ? (char *)document->children->name : __dodostring__,
					 document->compression);
}

//-------------------------------------------------------------------

__xmlInfo
xml::getXMLBufferInfo(const dodoString &buffer)
{
	document = xmlParseMemory(buffer.c_str(), buffer.size());
	if (document == NULL)
	{
		xmlErrorPtr error = xmlGetLastError();

		throw baseEx(ERRMODULE_LIBXML2, XMLEX_GETXMLBUFFERINFO, ERR_LIBXML2, error->code, error->message, __LINE__, __FILE__);
	}

	return __xmlInfo((char *)document->version, (char *)document->encoding, (char *)document->children->name, document->compression);
}

//-------------------------------------------------------------------

dodoArray<__xmlNode>
xml::parse(xmlNodePtr node)
{
	dodoArray<__xmlNode> sample;

	__xmlNode one;

	while (node != NULL)
	{
		if (node->type != XML_ELEMENT_NODE)
		{
			node = node->next;
			continue;
		}

		getNodeInfo(node, one);

		getAttributes(node, one.attributes);

		if (node->children == NULL)
			one.empty = true;

		one.CDATA = isCDATA(node);

		one.children.insert((char *)node->name, parse(node->children));

		sample.push_back(one);

		initNode(one);

		node = node->next;
	}

	return sample;
}

//-------------------------------------------------------------------

void
xml::initNode(__xmlNode &node)
{
	node.attributes.clear();
	node.children.clear();
	node.name.clear();
	node.ns.clear();
	node.nsDef.clear();
	node.nsDefHref.clear();
	node.nsHref.clear();
	node.value.clear();
}

//-------------------------------------------------------------------

__xmlNode
xml::parseFile(const dodoString &file)
{
	xmlFreeDoc(document);

	document = xmlParseFile(file.c_str());
	if (document == NULL)
	{
		xmlErrorPtr error = xmlGetLastError();

		throw baseEx(ERRMODULE_LIBXML2, XMLEX_PARCEFILE, ERR_LIBXML2, error->code, error->message, __LINE__, __FILE__, file);
	}

	xmlNodePtr node = xmlDocGetRootElement(document);
	if (node == NULL)
	{
		xmlErrorPtr error = xmlGetLastError();

		throw baseEx(ERRMODULE_LIBXML2, XMLEX_PARCEFILE, ERR_LIBXML2, error->code, error->message, __LINE__, __FILE__, file);
	}

	__xmlNode sample = *(parse(node).begin());

	return sample;
}

//-------------------------------------------------------------------

__xmlNode
xml::parseBuffer(const dodoString &buffer)
{
	xmlFreeDoc(document);

	document = xmlParseMemory(buffer.c_str(), buffer.size());
	if (document == NULL)
	{
		xmlErrorPtr error = xmlGetLastError();

		throw baseEx(ERRMODULE_LIBXML2, XMLEX_PARCEBUFFER, ERR_LIBXML2, error->code, error->message, __LINE__, __FILE__);
	}

	xmlNodePtr node = xmlDocGetRootElement(document);
	if (node == NULL)
	{
		xmlErrorPtr error = xmlGetLastError();

		throw baseEx(ERRMODULE_LIBXML2, XMLEX_PARCEBUFFER, ERR_LIBXML2, error->code, error->message, __LINE__, __FILE__);
	}

	__xmlNode sample = *(parse(node).begin());

	return sample;
}

//-------------------------------------------------------------------

void
xml::initNodeDef(__xmlNodeDef &node)
{
	node.attributes.clear();
	node.children.clear();
	node.name.clear();
	node.ns.clear();
	node.chLimit = -1;
}

//-------------------------------------------------------------------

xmlNodePtr
xml::findNode(const __xmlNodeDef &definition,
			  xmlNodePtr node)
{
	xmlNodePtr one;
	bool skip;

	if (icaseNames)
		cmpFunc = xmlStrcasecmp;
	else
		cmpFunc = xmlStrcmp;

	while (node != NULL)
	{
		if (node->type != XML_ELEMENT_NODE)
		{
			node = node->next;
			continue;
		}

		skip = false;

		if (definition.ns.size() > 0)
		{
			if (node->ns == NULL)
				skip = true;
			else
			if (cmpFunc(node->ns->prefix, (xmlChar *)definition.ns.c_str()) != 0)
				skip = true;
		}

		if (!skip && node->name != NULL)
			if (cmpFunc(node->name, (xmlChar *)definition.name.c_str()) == 0)
				return node;

		one = findNode(definition, node->children);

		if (one != NULL)
			return one;

		node = node->next;
	}

	return NULL;
}

//-------------------------------------------------------------------

void
xml::clear()
{
	xmlFreeDoc(document);
	document = NULL;
}

//-------------------------------------------------------------------

dodoString
xml::createXML(const __xmlNode &root,
			   const dodoString &encoding,
			   const dodoString &version) const
{
	if (root.name.empty())
		return __dodostring__;

	dodoString xml = "<?xml version=\"" + version + "\" encoding=\"" + encoding + "\"?>\r\n";

	xml.append(createNode(root));

	return xml;
}

//-------------------------------------------------------------------

dodoString
xml::createNode(const __xmlNode &node) const
{
	if (node.name.empty())
		return __dodostring__;

	dodoString xml = "<";

	if (!node.ns.empty())
	{
		xml.append(node.ns);
		xml.append(":");
	}
	xml.append(node.name);
	xml.append(" ");

	if (!node.nsDef.empty())
	{
		xml.append("xmlns:");
		xml.append(node.nsDef);
		xml.append("=\"");
		xml.append(node.nsDefHref);
		xml.append("\" ");
	}

	dodoMap<dodoString, dodoString, stringTools::equal>::const_iterator i = node.attributes.begin(), j = node.attributes.end();
	for (; i != j; ++i)
	{
		xml.append(i->first);
		xml.append("=\"");
		xml.append(i->second);
		xml.append("\" ");
	}

	if (node.empty)
	{
		xml.append("/>\r\n");
		return xml;
	}

	xml.append(">");

	if (!node.value.empty())
	{
		if (node.CDATA)
		{
			xml.append("<![CDATA[");
			xml.append(node.value);
			xml.append("]]>");
		}
		else
		{
			xml.append(node.value);
		}
	}

	dodoMap<dodoString, dodoArray<__xmlNode>, stringTools::equal>::const_iterator o = node.children.begin(), p = node.children.end();
	dodoArray<__xmlNode>::const_iterator x, y;
	for (; o != p; ++o)
	{
		x = o->second.begin();
		y = o->second.end();
		for (; x != y; ++x)
			xml.append(createNode(*x));
	}

	xml.append("</");

	if (!node.ns.empty())
	{
		xml.append(node.ns);
		xml.append(":");
	}

	xml.append(node.name);
	xml.append(">\r\n");

	return xml;
}

#endif

//-------------------------------------------------------------------

