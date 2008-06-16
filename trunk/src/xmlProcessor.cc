/***************************************************************************
 *            xmlProcessor.cc
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


#include <libdodo/xmlProcessor.h>

using namespace dodo::xml;

__info::__info(const dodoString &a_version,
			   const dodoString &a_encoding,
			   const dodoString &a_root,
			   int a_compression) : version(a_version),
									encoding(a_encoding),
									root(a_root),
									compression(a_compression)
{
}

//-------------------------------------------------------------------

__info::__info()
{
}

//-------------------------------------------------------------------

__nodeDef::__nodeDef() : chLimit(-1),
						 ignoreChildrenDef(false),
						 ignoreAttributesDef(true)
{
}

//-------------------------------------------------------------------

const dodoString processor::statements[] =
{
	"<",
	":",
	" ",
	"xmlns:",
	"=\"",
	"\" ",
	"/>\r\n",
	">",
	"<![CDATA[",
	"]]>",
	"</",
};

//-------------------------------------------------------------------

processor::processor(processor &xt)
{
}

//-------------------------------------------------------------------

processor::processor() : icaseNames(false)

#ifdef LIBXML2_EXT

						 ,
						 document(NULL)

#endif

{
#ifdef LIBXML2_EXT

	xmlPedanticParserDefault(0);
	xmlInitParser();
	xmlSetStructuredErrorFunc(NULL, processor::errHandler);

#endif
}

//-------------------------------------------------------------------

processor::~processor()
{
#ifdef LIBXML2_EXT

	xmlFreeDoc(document);

	xmlCleanupParser();

#endif
}

//-------------------------------------------------------------------

#ifdef LIBXML2_EXT

bool
processor::isCDATA(xmlNodePtr chNode)
{
	xmlNodePtr xnode = chNode->children;
	while (xnode != NULL)
	{
		if (xnode->type == XML_CDATA_SECTION_NODE)
			return true;

		xnode = xnode->next;
	}

	return false;
}

#endif

//-------------------------------------------------------------------

node
processor::parseFile(const __nodeDef &definition,
					 const dodoString &file)
{
#ifdef LIBXML2_EXT

	xmlFreeDoc(document);

	document = xmlParseFile(file.c_str());
	if (document == NULL)
	{
		xmlErrorPtr error = xmlGetLastError();

		if (error == NULL)
			throw baseEx(ERRMODULE_XMLPROCESSOR, PROCESSOREX_PARSEFILE, ERR_LIBDODO, PROCESSOREX_EMPTYDOCUMENT, XMLPROCESSOREX_EMPTYDOCUMENT_STR, __LINE__, __FILE__, file);
		else
			throw baseEx(ERRMODULE_XMLPROCESSOR, PROCESSOREX_PARSEFILE, ERR_LIBXML2, error->code, error->message, __LINE__, __FILE__, file);
	}

#endif

	return parse(definition);
}

//-------------------------------------------------------------------

node
processor::parseBuffer(const __nodeDef &definition,
					   const dodoString &buffer)
{
#ifdef LIBXML2_EXT

	xmlFreeDoc(document);

	document = xmlParseMemory(buffer.c_str(), buffer.size());
	if (document == NULL)
	{
		xmlErrorPtr error = xmlGetLastError();

		if (error == NULL)
			throw baseEx(ERRMODULE_XMLPROCESSOR, PROCESSOREX_PARSEBUFFER, ERR_LIBDODO, PROCESSOREX_EMPTYDOCUMENT, XMLPROCESSOREX_EMPTYDOCUMENT_STR, __LINE__, __FILE__);
		else
			throw baseEx(ERRMODULE_XMLPROCESSOR, PROCESSOREX_PARSEBUFFER, ERR_LIBXML2, error->code, error->message, __LINE__, __FILE__);
	}

#endif

	return parse(definition);
}

//-------------------------------------------------------------------

node
processor::parse(const __nodeDef &definition)
{
#ifdef LIBXML2_EXT

	xmlNodePtr xnode = xmlDocGetRootElement(document);
	if (xnode == NULL)
	{
		xmlErrorPtr error = xmlGetLastError();

		if (error == NULL)
			throw baseEx(ERRMODULE_XMLPROCESSOR, PROCESSOREX_PARSE, ERR_LIBDODO, PROCESSOREX_NOROOTNODE, XMLPROCESSOREX_NOROOTNODE_STR, __LINE__, __FILE__);
		else
			throw baseEx(ERRMODULE_XMLPROCESSOR, PROCESSOREX_PARSE, ERR_LIBXML2, error->code, error->message, __LINE__, __FILE__);
	}

	xnode = findNode(definition, xnode);

	if (xnode == NULL)
		return node();

	node sample;

	getNodeInfo(xnode, sample);

	getAttributes(definition, xnode, sample.attributes);

	if (xnode->children == NULL)
	{
		sample.empty = true;

		return sample;
	}

	sample.CDATA = isCDATA(xnode);

	if (definition.children.size() > 0)
	{
		dodoMap<dodoString, __nodeDef>::const_iterator i(definition.children.begin()), j(definition.children.end());
		for (; i != j; ++i)
			sample.children.insert(make_pair(i->first, parse(i->second, xnode->children, definition.chLimit)));
	}
	else
	{
		if (definition.ignoreChildrenDef)
		{
			xnode = xnode->children;

			node one;

			dodoArray<node> children;
			dodoArray<node>::iterator i, j;

			while (xnode != NULL)
			{
				if (xnode->type != XML_ELEMENT_NODE)
				{
					xnode = xnode->next;
					continue;
				}

				getNodeInfo(xnode, one);

				getAttributes(xnode, one.attributes);

				one.CDATA = isCDATA(xnode);

				if (xnode->children == NULL)
					one.empty = true;
				else
				{
					children = parse(xnode->children);
					i = children.begin();
					j = children.end();
					for (; i != j; ++i)
						one.children[i->name].push_back(*i);
				}

				sample.children[(char *)xnode->name].push_back(one);

				initNode(one);

				xnode = xnode->next;
			}
		}
	}

	return sample;

#else

	return node();

#endif
}

//-------------------------------------------------------------------

#ifdef LIBXML2_EXT

dodoArray<node>
processor::parse(const __nodeDef &definition,
				 const xmlNodePtr chNode,
				 long chLimit)
{
	xmlNodePtr xnode = chNode, subNode;

	node sample;
	dodoArray<node> sampleArr;

	if (icaseNames)
		cmpFunc = xmlStrcasecmp;
	else
		cmpFunc = xmlStrcmp;

	do
	{
		if (xnode->type != XML_ELEMENT_NODE)
		{
			xnode = xnode->next;

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
			if (xnode->ns == NULL)
			{
				xnode = xnode->next;

				continue;
			}

			if (cmpFunc(xnode->ns->prefix, (xmlChar *)definition.ns.c_str()) != 0)
			{
				xnode = xnode->next;

				continue;
			}
		}

		if (cmpFunc(xnode->name, (xmlChar *)definition.name.c_str()) != 0)
		{
			xnode = xnode->next;

			continue;
		}

		initNode(sample);

		getNodeInfo(xnode, sample);
		getAttributes(definition, xnode, sample.attributes);

		if (xnode->children == NULL)
			sample.empty = true;

		sample.CDATA = isCDATA(xnode);

		if (definition.children.size() > 0)
		{
			dodoMap<dodoString, __nodeDef>::const_iterator i(definition.children.begin()), j(definition.children.end());
			for (; i != j; ++i)
				sample.children.insert(make_pair(i->first, parse(i->second, xnode->children, definition.chLimit)));
		}
		else
		{
			if (definition.ignoreChildrenDef)
			{
				subNode = xnode->children;

				node one;

				dodoArray<node> chldrn;
				dodoArray<node>::iterator i, j;

				while (subNode != NULL)
				{
					if (subNode->type != XML_ELEMENT_NODE)
					{
						subNode = subNode->next;
						continue;
					}

					getNodeInfo(subNode, one);

					getAttributes(subNode, one.attributes);

					one.CDATA = isCDATA(subNode);

					if (xnode->children == NULL)
						one.empty = true;
					else
					{
						chldrn = parse(xnode->children);
						i = chldrn.begin();
						j = chldrn.end();
						for (; i != j; ++i)
							one.children[i->name].push_back(*i);
					}

					sample.children[(char *)subNode->name].push_back(one);

					initNode(one);

					subNode = subNode->next;
				}
			}
		}

		sampleArr.push_back(sample);

		xnode = xnode->next;

	}
	while (xnode != NULL);

	return sampleArr;
}

#endif

//-------------------------------------------------------------------

#ifdef LIBXML2_EXT

void
processor::errHandler(void        *data,
					  xmlErrorPtr error)
{
}

//-------------------------------------------------------------------

void
processor::getAttributes(const __nodeDef &definition,
						 const xmlNodePtr xnode,
						 dodoStringMap &attributes)
{
	attribute = xnode->properties;

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
						xmlChar *xChar = xmlGetProp(xnode, attribute->name);
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
				xmlChar *xChar = xmlGetProp(xnode, (xmlChar *)iAttr->c_str());
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
				xmlChar *xChar = xmlGetProp(xnode, attribute->name);
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
processor::getAttributes(const xmlNodePtr xnode,
						 dodoStringMap &attributes)
{
	attribute = xnode->properties;

	while (attribute != NULL)
	{
		xmlChar *xChar = xmlGetProp(xnode, attribute->name);
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
processor::getNodeInfo(const xmlNodePtr xnode,
					   node &resNode)
{
	if (xnode->ns != NULL)
	{
		resNode.ns = (char *)xnode->ns->prefix;
		resNode.nsHref = (char *)xnode->ns->href;
	}

	if (xnode->nsDef != NULL)
	{
		resNode.nsDef = (char *)xnode->nsDef->prefix;
		resNode.nsDefHref = (char *)xnode->nsDef->href;
	}

	if (xnode->name != NULL)
		resNode.name.assign((char *)xnode->name);

	xmlChar *xChar = xmlNodeListGetString(document, xnode->children, 1);
	if (xChar != NULL)
	{
		resNode.value.assign((char *)xChar);
		xmlFree(xChar);
	}
}

#endif

//-------------------------------------------------------------------

__info
processor::getXMLFileInfo(const dodoString &file)
{
#ifdef LIBXML2_EXT

	document = xmlParseFile(file.c_str());
	if (document == NULL)
	{
		xmlErrorPtr error = xmlGetLastError();

		if (error == NULL)
			throw baseEx(ERRMODULE_XMLPROCESSOR, PROCESSOREX_GETXMLFILEINFO, ERR_LIBDODO, PROCESSOREX_EMPTYDOCUMENT, XMLPROCESSOREX_EMPTYDOCUMENT_STR, __LINE__, __FILE__, file);
		else
			throw baseEx(ERRMODULE_XMLPROCESSOR, PROCESSOREX_GETXMLFILEINFO, ERR_LIBXML2, error->code, error->message, __LINE__, __FILE__, file);
	}

	return __info(document->version != NULL ? (char *)document->version : __dodostring__,
				  document->encoding != NULL ? (char *)document->encoding : __dodostring__,
				  (document->children != NULL && document->children->name != NULL) ? (char *)document->children->name : __dodostring__,
				  document->compression);

#else

	return __info();

#endif
}

//-------------------------------------------------------------------

__info
processor::getXMLBufferInfo(const dodoString &buffer)
{
#ifdef LIBXML2_EXT

	document = xmlParseMemory(buffer.c_str(), buffer.size());
	if (document == NULL)
	{
		xmlErrorPtr error = xmlGetLastError();

		if (error == NULL)
			throw baseEx(ERRMODULE_XMLPROCESSOR, PROCESSOREX_GETXMLBUFFERINFO, ERR_LIBDODO, PROCESSOREX_EMPTYDOCUMENT, XMLPROCESSOREX_EMPTYDOCUMENT_STR, __LINE__, __FILE__);
		else
			throw baseEx(ERRMODULE_XMLPROCESSOR, PROCESSOREX_GETXMLBUFFERINFO, ERR_LIBXML2, error->code, error->message, __LINE__, __FILE__);
	}

	return __info((char *)document->version, (char *)document->encoding, (char *)document->children->name, document->compression);

#else

	return __info();

#endif
}

//-------------------------------------------------------------------

#ifdef LIBXML2_EXT

dodoArray<node>
processor::parse(xmlNodePtr xnode)
{
	dodoArray<node> sample;

	node one;

	dodoArray<node> children;
	dodoArray<node>::iterator i, j;

	while (xnode != NULL)
	{
		if (xnode->type != XML_ELEMENT_NODE)
		{
			xnode = xnode->next;

			continue;
		}

		getNodeInfo(xnode, one);

		getAttributes(xnode, one.attributes);

		one.CDATA = isCDATA(xnode);

		if (xnode->children == NULL)
			one.empty = true;
		else
		{
			children = parse(xnode->children);
			i = children.begin();
			j = children.end();
			for (; i != j; ++i)
				one.children[i->name].push_back(*i);
		}

		sample.push_back(one);

		initNode(one);

		xnode = xnode->next;
	}

	return sample;
}

#endif

//-------------------------------------------------------------------

void
processor::initNode(node &xnode)
{
	xnode.attributes.clear();
	xnode.children.clear();
	xnode.name.clear();
	xnode.ns.clear();
	xnode.nsDef.clear();
	xnode.nsDefHref.clear();
	xnode.nsHref.clear();
	xnode.value.clear();
}

//-------------------------------------------------------------------

node
processor::parseFile(const dodoString &file)
{
#ifdef LIBXML2_EXT

	xmlFreeDoc(document);

	document = xmlParseFile(file.c_str());
	if (document == NULL)
	{
		xmlErrorPtr error = xmlGetLastError();

		if (error == NULL)
			throw baseEx(ERRMODULE_XMLPROCESSOR, PROCESSOREX_PARSEFILE, ERR_LIBDODO, PROCESSOREX_EMPTYDOCUMENT, XMLPROCESSOREX_EMPTYDOCUMENT_STR, __LINE__, __FILE__, file);
		else
			throw baseEx(ERRMODULE_XMLPROCESSOR, PROCESSOREX_PARSEFILE, ERR_LIBXML2, error->code, error->message, __LINE__, __FILE__, file);
	}

	xmlNodePtr xnode = xmlDocGetRootElement(document);
	if (xnode == NULL)
	{
		xmlErrorPtr error = xmlGetLastError();

		if (error == NULL)
			throw baseEx(ERRMODULE_XMLPROCESSOR, PROCESSOREX_PARSEFILE, ERR_LIBDODO, PROCESSOREX_NOROOTNODE, XMLPROCESSOREX_NOROOTNODE_STR, __LINE__, __FILE__, file);
		else
			throw baseEx(ERRMODULE_XMLPROCESSOR, PROCESSOREX_PARSEFILE, ERR_LIBXML2, error->code, error->message, __LINE__, __FILE__, file);
	}

	return *(parse(xnode).begin());

#else

	return node();

#endif
}

//-------------------------------------------------------------------

node
processor::parseBuffer(const dodoString &buffer)
{
#ifdef LIBXML2_EXT

	xmlFreeDoc(document);

	document = xmlParseMemory(buffer.c_str(), buffer.size());
	if (document == NULL)
	{
		xmlErrorPtr error = xmlGetLastError();

		if (error == NULL)
			throw baseEx(ERRMODULE_XMLPROCESSOR, PROCESSOREX_PARSEBUFFER, ERR_LIBDODO, PROCESSOREX_EMPTYDOCUMENT, XMLPROCESSOREX_EMPTYDOCUMENT_STR, __LINE__, __FILE__);
		else
			throw baseEx(ERRMODULE_XMLPROCESSOR, PROCESSOREX_PARSEBUFFER, ERR_LIBXML2, error->code, error->message, __LINE__, __FILE__);
	}

	xmlNodePtr xnode = xmlDocGetRootElement(document);
	if (xnode == NULL)
	{
		xmlErrorPtr error = xmlGetLastError();

		if (error == NULL)
			throw baseEx(ERRMODULE_XMLPROCESSOR, PROCESSOREX_PARSEBUFFER, ERR_LIBDODO, PROCESSOREX_NOROOTNODE, XMLPROCESSOREX_NOROOTNODE_STR, __LINE__, __FILE__);
		else
			throw baseEx(ERRMODULE_XMLPROCESSOR, PROCESSOREX_PARSEBUFFER, ERR_LIBXML2, error->code, error->message, __LINE__, __FILE__);
	}

	return *(parse(xnode).begin());

#else

	return node();

#endif
}

//-------------------------------------------------------------------

void
processor::initNodeDef(__nodeDef &xnode)
{
	xnode.attributes.clear();
	xnode.children.clear();
	xnode.name.clear();
	xnode.ns.clear();
	xnode.chLimit = -1;
}

//-------------------------------------------------------------------

#ifdef LIBXML2_EXT

xmlNodePtr
processor::findNode(const __nodeDef &definition,
					xmlNodePtr xnode)
{
	xmlNodePtr one;
	bool skip;

	if (icaseNames)
		cmpFunc = xmlStrcasecmp;
	else
		cmpFunc = xmlStrcmp;

	while (xnode != NULL)
	{
		if (xnode->type != XML_ELEMENT_NODE)
		{
			xnode = xnode->next;
			continue;
		}

		skip = false;

		if (definition.ns.size() > 0)
		{
			if (xnode->ns == NULL)
				skip = true;
			else
			if (cmpFunc(xnode->ns->prefix, (xmlChar *)definition.ns.c_str()) != 0)
				skip = true;
		}

		if (!skip && xnode->name != NULL)
			if (cmpFunc(xnode->name, (xmlChar *)definition.name.c_str()) == 0)
				return xnode;

		one = findNode(definition, xnode->children);

		if (one != NULL)
			return one;

		xnode = xnode->next;
	}

	return NULL;
}

#endif

//-------------------------------------------------------------------

void
processor::clear()
{
#ifdef LIBXML2_EXT

	xmlFreeDoc(document);
	document = NULL;

#endif
}

//-------------------------------------------------------------------

dodoString
processor::createXML(const node &root,
					 const dodoString &encoding,
					 const dodoString &version) const
{
	if (root.name.empty())
		return __dodostring__;

	dodoString processor = "<?processor version=\"" + version + "\" encoding=\"" + encoding + "\"?>\r\n";

	processor.append(createNode(root));

	return processor;
}

//-------------------------------------------------------------------

dodoString
processor::createNode(const node &xnode) const
{
	if (xnode.name.empty())
		return __dodostring__;

	dodoString data = statements[PROCESSOR_STATEMENT_LT];

	if (!xnode.ns.empty())
	{
		data.append(xnode.ns);
		data.append(statements[PROCESSOR_STATEMENT_COLON]);
	}
	data.append(xnode.name);
	data.append(statements[PROCESSOR_STATEMENT_SPACE]);

	if (!xnode.nsDef.empty())
	{
		data.append(statements[PROCESSOR_STATEMENT_XMLNS]);
		data.append(xnode.nsDef);
		data.append(statements[PROCESSOR_STATEMENT_EQUALDQUOTE]);
		data.append(xnode.nsDefHref);
		data.append(statements[PROCESSOR_STATEMENT_DQUOTESPACE]);
	}

	dodoMap<dodoString, dodoString, dodoMapStringCompare>::const_iterator i = xnode.attributes.begin(), j = xnode.attributes.end();
	for (; i != j; ++i)
	{
		data.append(i->first);
		data.append(statements[PROCESSOR_STATEMENT_EQUALDQUOTE]);
		data.append(i->second);
		data.append(statements[PROCESSOR_STATEMENT_DQUOTESPACE]);
	}

	if (xnode.empty)
	{
		data.append(statements[PROCESSOR_STATEMENT_SLASHGT]);

		return data;
	}

	data.append(statements[PROCESSOR_STATEMENT_GT]);

	if (!xnode.value.empty())
	{
		if (xnode.CDATA)
		{
			data.append(statements[PROCESSOR_STATEMENT_CDATAOPEN]);
			data.append(xnode.value);
			data.append(statements[PROCESSOR_STATEMENT_CDATACLOSE]);
		}
		else
		{
			data.append(xnode.value);
		}
	}

	dodoMap<dodoString, dodoArray<node>, dodoMapStringCompare>::const_iterator o = xnode.children.begin(), p = xnode.children.end();
	dodoArray<node>::const_iterator x, y;
	for (; o != p; ++o)
	{
		x = o->second.begin();
		y = o->second.end();
		for (; x != y; ++x)
			data.append(createNode(*x));
	}

	data.append(statements[PROCESSOR_STATEMENT_LTSLASH]);

	if (!xnode.ns.empty())
	{
		data.append(xnode.ns);
		data.append(statements[PROCESSOR_STATEMENT_COLON]);
	}

	data.append(xnode.name);
	data.append(statements[PROCESSOR_STATEMENT_GT]);

	return data;
}

//-------------------------------------------------------------------

