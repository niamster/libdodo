/***************************************************************************
 *            dataFormatXmlProcessor.cc
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/directives.h>

#ifdef LIBXML2_EXT
#include <libxml/parser.h>
#include <libxml/xmlmemory.h>
#endif

#include <libdodo/dataFormatXmlProcessor.h>
#include <libdodo/dataFormatXmlNode.h>
#include <libdodo/dataFormatXmlProcessorEx.h>

#ifdef LIBXML2_EXT
namespace dodo
{
	namespace data
	{
		namespace format
		{
			namespace xml
			{
				/**
				 * @struct __node
				 * @brief defines XML node properties
				 */
				struct __node
				{
					/**
					 * constructor
					 */
					__node()
					{
					}

					/**
					 * constructor
					 */
					__node(xmlNodePtr node) : node(node)
					{
					}

					xmlNodePtr node; ///< represents internal libxml2 node data
				};

				/**
				 * @struct __doc
				 * @brief defines XML document properties
				 */
				struct __doc
				{
					xmlDocPtr document; ///< represents internal libxml2 document data
				};
			};
		};
	};
};
#endif

using namespace dodo::data::format::xml;

__info::__info(const dodoString &version,
			   const dodoString &encoding,
			   const dodoString &root,
			   int              compression) : version(version),
											   encoding(encoding),
											   root(root),
											   compression(compression)
{
}

//-------------------------------------------------------------------

__info::__info()
{
}

//-------------------------------------------------------------------

__nodeDef::__nodeDef() : allChildren(true),
						 allAttributes(true)
{
}

//-------------------------------------------------------------------

__nodeDef::__nodeDef(const dodoString &name,
					 const dodoString &ns) : allChildren(true),
											 allAttributes(true),
											 name(name),
											 ns(ns)
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

#ifdef LIBXML2_EXT
void
errHandler(void        *data,
		   xmlErrorPtr error)
{
}
#endif

//-------------------------------------------------------------------


processor::processor(processor &xt)
{
}

//-------------------------------------------------------------------

processor::processor() : icaseNames(false)
#ifdef LIBXML2_EXT
						 ,
						 document(new __doc)
#endif
{
#ifdef LIBXML2_EXT
	xmlPedanticParserDefault(0);
	xmlInitParser();
	xmlSetStructuredErrorFunc(NULL, errHandler);
#endif
}

//-------------------------------------------------------------------

processor::~processor()
{
#ifdef LIBXML2_EXT
	xmlFreeDoc(document->document);
	xmlCleanupParser();

	delete document;
#endif
}

//-------------------------------------------------------------------

#ifdef LIBXML2_EXT
bool
processor::isCDATA(const __node &a_xnode)
{
	xmlNodePtr xnode = a_xnode.node->children;
	while (xnode != NULL)
	{
		if (xnode->type == XML_CDATA_SECTION_NODE)
		{
			return true;
		}

		xnode = xnode->next;
	}

	return false;
}
#endif

//-------------------------------------------------------------------

node
processor::processFile(const __nodeDef  &definition,
					   const dodoString &file)
{
#ifdef LIBXML2_EXT
	xmlFreeDoc(document->document);

	document->document = xmlParseFile(file.c_str());
	if (document->document == NULL)
	{
		xmlErrorPtr error = xmlGetLastError();

		if (error == NULL)
		{
			throw exception::basic(exception::ERRMODULE_DATAFORMATXMLPROCESSOR, PROCESSOREX_PARSEFILE, exception::ERRNO_LIBDODO, PROCESSOREX_EMPTYDOCUMENT, DATAFORMATXMLPROCESSOREX_EMPTYDOCUMENT_STR, __LINE__, __FILE__, file);
		}
		else
		{
			throw exception::basic(exception::ERRMODULE_DATAFORMATXMLPROCESSOR, PROCESSOREX_PARSEFILE, exception::ERRNO_LIBXML2, error->code, error->message, __LINE__, __FILE__, file);
		}
	}
#endif

	return parse(definition);
}

//-------------------------------------------------------------------

node
processor::processString(const __nodeDef  &definition,
						 const dodoString &buffer)
{
#ifdef LIBXML2_EXT
	xmlFreeDoc(document->document);

	document->document = xmlParseMemory(buffer.c_str(), buffer.size());
	if (document->document == NULL)
	{
		xmlErrorPtr error = xmlGetLastError();

		if (error == NULL)
		{
			throw exception::basic(exception::ERRMODULE_DATAFORMATXMLPROCESSOR, PROCESSOREX_PARSEBUFFER, exception::ERRNO_LIBDODO, PROCESSOREX_EMPTYDOCUMENT, DATAFORMATXMLPROCESSOREX_EMPTYDOCUMENT_STR, __LINE__, __FILE__);
		}
		else
		{
			throw exception::basic(exception::ERRMODULE_DATAFORMATXMLPROCESSOR, PROCESSOREX_PARSEBUFFER, exception::ERRNO_LIBXML2, error->code, error->message, __LINE__, __FILE__);
		}
	}
#endif

	return parse(definition);
}

//-------------------------------------------------------------------

node
processor::parse(const __nodeDef &definition)
{
#ifdef LIBXML2_EXT
	__node xnode = xmlDocGetRootElement(document->document);
	if (xnode.node == NULL)
	{
		xmlErrorPtr error = xmlGetLastError();

		if (error == NULL)
		{
			throw exception::basic(exception::ERRMODULE_DATAFORMATXMLPROCESSOR, PROCESSOREX_PARSE, exception::ERRNO_LIBDODO, PROCESSOREX_NOROOTNODE, DATAFORMATXMLPROCESSOREX_NOROOTNODE_STR, __LINE__, __FILE__);
		}
		else
		{
			throw exception::basic(exception::ERRMODULE_DATAFORMATXMLPROCESSOR, PROCESSOREX_PARSE, exception::ERRNO_LIBXML2, error->code, error->message, __LINE__, __FILE__);
		}
	}

	xnode = findNode(definition, xnode);

	if (xnode.node == NULL)
	{
		return node();
	}

	node sample;
	getNodeInfo(xnode, sample);

	getAttributes(definition, xnode, sample.attributes);

	if (xnode.node->children == NULL)
	{
		return sample;
	}

	sample.CDATA = isCDATA(xnode);

	if (definition.allChildren)
	{
		xnode.node = xnode.node->children;

		node one;

		dodoArray<node> children;
		dodoArray<node>::iterator i, j;

		while (xnode.node != NULL)
		{
			if (xnode.node->type != XML_ELEMENT_NODE)
			{
				xnode.node = xnode.node->next;

				continue;
			}

			getNodeInfo(xnode, one);

			getAttributes(xnode, one.attributes);

			one.CDATA = isCDATA(xnode);

			if (xnode.node->children != NULL)
			{
				children = parse(__node(xnode.node->children));
				i = children.begin();
				j = children.end();
				for (; i != j; ++i)
				{
					one.children[i->name].push_back(*i);
				}
			}

			sample.children[(char *)xnode.node->name].push_back(one);

			initNode(one);

			xnode.node = xnode.node->next;
		}
	}
	else
	{
		if (definition.children.size() > 0)
		{
			dodoMap<dodoString, __nodeDef>::const_iterator i(definition.children.begin()), j(definition.children.end());
			for (; i != j; ++i)
			{
				sample.children.insert(make_pair(i->first, parse(i->second, xnode.node->children)));
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
				 const __node	 &a_xnode)
{
	xmlNodePtr xnode = a_xnode.node, subNode;

	node sample;
	dodoArray<node> sampleArr;

	cmpFunc = icaseNames?xmlStrcasecmp:xmlStrcmp;

	do
	{
		if (xnode->type != XML_ELEMENT_NODE)
		{
			xnode = xnode->next;

			continue;
		}

		if (definition.ns.size() > 0)
		{
			if (xnode->ns == NULL)
			{
				xnode = xnode->next;

				continue;
			}

			if (cmpFunc(xnode->ns->prefix, (unsigned char *)definition.ns.c_str()) != 0)
			{
				xnode = xnode->next;

				continue;
			}
		}

		if (cmpFunc(xnode->name, (unsigned char *)definition.name.c_str()) != 0)
		{
			xnode = xnode->next;

			continue;
		}

		initNode(sample);

		getNodeInfo(xnode, sample);
		getAttributes(definition, xnode, sample.attributes);

		sample.CDATA = isCDATA(xnode);

		if (definition.allChildren)
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

				if (subNode->children != NULL)
				{
					chldrn = parse(subNode->children);
					i = chldrn.begin();
					j = chldrn.end();
					for (; i != j; ++i)
					{
						one.children[i->name].push_back(*i);
					}
				}

				sample.children[(char *)subNode->name].push_back(one);

				initNode(one);

				subNode = subNode->next;
			}
		}
		else
		{
			if (definition.children.size() > 0)
			{
				dodoMap<dodoString, __nodeDef>::const_iterator i(definition.children.begin()), j(definition.children.end());
				for (; i != j; ++i)
				{
					sample.children.insert(make_pair(i->first, parse(i->second, xnode->children)));
				}
			}
		}

		sampleArr.push_back(sample);

		xnode = xnode->next;
	} while (xnode != NULL);

	return sampleArr;
}
#endif

//-------------------------------------------------------------------

#ifdef LIBXML2_EXT
void
processor::getAttributes(const __nodeDef  &definition,
						 const __node &xnode,
						 dodoStringMap    &attributes)
{
	xmlAttrPtr attribute = xnode.node->properties;

	if (definition.allAttributes)
	{
		while (attribute != NULL)
		{
			unsigned char *xChar = xmlGetProp(xnode.node, attribute->name);
			if (xChar != NULL)
			{
				attributes[(char *)attribute->name] = (char *)xChar;
				xmlFree(xChar);
			}

			attribute = attribute->next;
		}
	}
	else
	{
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
						if (xmlStrcmp(attribute->name, (unsigned char *)iAttr->c_str()) == 0)
						{
							unsigned char *xChar = xmlGetProp(xnode.node, attribute->name);
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
					unsigned char *xChar = xmlGetProp(xnode.node, (unsigned char *)iAttr->c_str());
					if (xChar != NULL)
					{
						attributes[*iAttr] = (char *)xChar;
						xmlFree(xChar);
					}
				}
			}
		}
	}
}

//-------------------------------------------------------------------

void
processor::getAttributes(const __node &xnode,
						 dodoStringMap    &attributes)
{
	xmlAttrPtr attribute = xnode.node->properties;

	while (attribute != NULL)
	{
		unsigned char *xChar = xmlGetProp(xnode.node, attribute->name);
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
processor::getNodeInfo(const __node &xnode,
					   node			&resNode)
{
	if (xnode.node->ns != NULL)
	{
		resNode.ns.prefix = (char *)xnode.node->ns->prefix;
		resNode.ns.href = (char *)xnode.node->ns->href;
	}

	if (xnode.node->nsDef != NULL)
	{
		resNode.nsDef.prefix = (char *)xnode.node->nsDef->prefix;
		resNode.nsDef.href = (char *)xnode.node->nsDef->href;
	}

	if (xnode.node->name != NULL)
	{
		resNode.name.assign((char *)xnode.node->name);
	}

	unsigned char *xChar = xmlNodeListGetString(document->document, xnode.node->children, 1);
	if (xChar != NULL)
	{
		resNode.value.assign((char *)xChar);
		xmlFree(xChar);
	}
}
#endif

//-------------------------------------------------------------------

__info
processor::getFileInfo(const dodoString &file)
{
#ifdef LIBXML2_EXT
	xmlDocPtr document = xmlParseFile(file.c_str());
	if (document == NULL)
	{
		xmlErrorPtr error = xmlGetLastError();

		if (error == NULL)
		{
			throw exception::basic(exception::ERRMODULE_DATAFORMATXMLPROCESSOR, PROCESSOREX_GETFILEINFO, exception::ERRNO_LIBDODO, PROCESSOREX_EMPTYDOCUMENT, DATAFORMATXMLPROCESSOREX_EMPTYDOCUMENT_STR, __LINE__, __FILE__, file);
		}
		else
		{
			throw exception::basic(exception::ERRMODULE_DATAFORMATXMLPROCESSOR, PROCESSOREX_GETFILEINFO, exception::ERRNO_LIBXML2, error->code, error->message, __LINE__, __FILE__, file);
		}
	}

	__info i(document->version != NULL ? (char *)document->version : __dodostring__,
			 document->encoding != NULL ? (char *)document->encoding : __dodostring__,
			 (document->children != NULL && document->children->name != NULL) ? (char *)document->children->name : __dodostring__,
			 document->compression);

	xmlFreeDoc(document);

	return i;
#else
	return __info();
#endif
}

//-------------------------------------------------------------------

__info
processor::getBufferInfo(const dodoString &buffer)
{
#ifdef LIBXML2_EXT
	xmlDocPtr document = xmlParseMemory(buffer.c_str(), buffer.size());
	if (document == NULL)
	{
		xmlErrorPtr error = xmlGetLastError();

		if (error == NULL)
		{
			throw exception::basic(exception::ERRMODULE_DATAFORMATXMLPROCESSOR, PROCESSOREX_GETBUFFERINFO, exception::ERRNO_LIBDODO, PROCESSOREX_EMPTYDOCUMENT, DATAFORMATXMLPROCESSOREX_EMPTYDOCUMENT_STR, __LINE__, __FILE__);
		}
		else
		{
			throw exception::basic(exception::ERRMODULE_DATAFORMATXMLPROCESSOR, PROCESSOREX_GETBUFFERINFO, exception::ERRNO_LIBXML2, error->code, error->message, __LINE__, __FILE__);
		}
	}

	__info i(document->version != NULL ? (char *)document->version : __dodostring__,
			 document->encoding != NULL ? (char *)document->encoding : __dodostring__,
			 (document->children != NULL && document->children->name != NULL) ? (char *)document->children->name : __dodostring__,
			 document->compression);

	xmlFreeDoc(document);

	return i;
#else
	return __info();
#endif
}

//-------------------------------------------------------------------

#ifdef LIBXML2_EXT
dodoArray<node>
processor::parse(__node xnode)
{
	dodoArray<node> sample;

	node one;

	dodoArray<node> children;
	dodoArray<node>::iterator i, j;

	while (xnode.node != NULL)
	{
		if (xnode.node->type != XML_ELEMENT_NODE)
		{
			xnode.node = xnode.node->next;

			continue;
		}

		getNodeInfo(xnode, one);

		getAttributes(xnode, one.attributes);

		one.CDATA = isCDATA(xnode);

		if (xnode.node->children != NULL)
		{
			children = parse(__node(xnode.node->children));
			i = children.begin();
			j = children.end();
			for (; i != j; ++i)
			{
				one.children[i->name].push_back(*i);
			}
		}

		sample.push_back(one);

		initNode(one);

		xnode.node = xnode.node->next;
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
	xnode.ns.prefix.clear();
	xnode.ns.href.clear();
	xnode.nsDef.prefix.clear();
	xnode.nsDef.href.clear();
	xnode.value.clear();
}

//-------------------------------------------------------------------

node
processor::processFile(const dodoString &file)
{
#ifdef LIBXML2_EXT
	xmlFreeDoc(document->document);

	document->document = xmlParseFile(file.c_str());
	if (document->document == NULL)
	{
		xmlErrorPtr error = xmlGetLastError();

		if (error == NULL)
		{
			throw exception::basic(exception::ERRMODULE_DATAFORMATXMLPROCESSOR, PROCESSOREX_PARSEFILE, exception::ERRNO_LIBDODO, PROCESSOREX_EMPTYDOCUMENT, DATAFORMATXMLPROCESSOREX_EMPTYDOCUMENT_STR, __LINE__, __FILE__, file);
		}
		else
		{
			throw exception::basic(exception::ERRMODULE_DATAFORMATXMLPROCESSOR, PROCESSOREX_PARSEFILE, exception::ERRNO_LIBXML2, error->code, error->message, __LINE__, __FILE__, file);
		}
	}

	xmlNodePtr xnode = xmlDocGetRootElement(document->document);
	if (xnode == NULL)
	{
		xmlErrorPtr error = xmlGetLastError();

		if (error == NULL)
		{
			throw exception::basic(exception::ERRMODULE_DATAFORMATXMLPROCESSOR, PROCESSOREX_PARSEFILE, exception::ERRNO_LIBDODO, PROCESSOREX_NOROOTNODE, DATAFORMATXMLPROCESSOREX_NOROOTNODE_STR, __LINE__, __FILE__, file);
		}
		else
		{
			throw exception::basic(exception::ERRMODULE_DATAFORMATXMLPROCESSOR, PROCESSOREX_PARSEFILE, exception::ERRNO_LIBXML2, error->code, error->message, __LINE__, __FILE__, file);
		}
	}

	return *(parse(__node(xnode)).begin());
#else
	return node();
#endif
}

//-------------------------------------------------------------------

node
processor::processString(const dodoString &buffer)
{
#ifdef LIBXML2_EXT
	xmlFreeDoc(document->document);

	document->document = xmlParseMemory(buffer.c_str(), buffer.size());
	if (document->document == NULL)
	{
		xmlErrorPtr error = xmlGetLastError();

		if (error == NULL)
		{
			throw exception::basic(exception::ERRMODULE_DATAFORMATXMLPROCESSOR, PROCESSOREX_PARSEBUFFER, exception::ERRNO_LIBDODO, PROCESSOREX_EMPTYDOCUMENT, DATAFORMATXMLPROCESSOREX_EMPTYDOCUMENT_STR, __LINE__, __FILE__);
		}
		else
		{
			throw exception::basic(exception::ERRMODULE_DATAFORMATXMLPROCESSOR, PROCESSOREX_PARSEBUFFER, exception::ERRNO_LIBXML2, error->code, error->message, __LINE__, __FILE__);
		}
	}

	xmlNodePtr xnode = xmlDocGetRootElement(document->document);
	if (xnode == NULL)
	{
		xmlErrorPtr error = xmlGetLastError();

		if (error == NULL)
		{
			throw exception::basic(exception::ERRMODULE_DATAFORMATXMLPROCESSOR, PROCESSOREX_PARSEBUFFER, exception::ERRNO_LIBDODO, PROCESSOREX_NOROOTNODE, DATAFORMATXMLPROCESSOREX_NOROOTNODE_STR, __LINE__, __FILE__);
		}
		else
		{
			throw exception::basic(exception::ERRMODULE_DATAFORMATXMLPROCESSOR, PROCESSOREX_PARSEBUFFER, exception::ERRNO_LIBXML2, error->code, error->message, __LINE__, __FILE__);
		}
	}

	return *(parse(__node(xnode)).begin());
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
}

//-------------------------------------------------------------------

#ifdef LIBXML2_EXT
__node
processor::findNode(const __nodeDef &definition,
					const __node	&a_xnode)
{
	xmlNodePtr one, xnode = a_xnode.node;
	bool skip;

	cmpFunc = icaseNames?xmlStrcasecmp:xmlStrcmp;

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
			{
				skip = true;
			}
			else if (cmpFunc(xnode->ns->prefix, (unsigned char *)definition.ns.c_str()) != 0)
			{
				skip = true;
			}
		}

		if (!skip && xnode->name != NULL)
		{
			if (cmpFunc(xnode->name, (unsigned char *)definition.name.c_str()) == 0)
			{
				return xnode;
			}
		}

		one = findNode(definition, __node(xnode->children)).node;

		if (one != NULL)
		{
			return one;
		}

		xnode = xnode->next;
	}

	return __node(NULL);
}
#endif

//-------------------------------------------------------------------

void
processor::clear()
{
#ifdef LIBXML2_EXT
	xmlFreeDoc(document->document);
	document->document = NULL;
#endif
}

//-------------------------------------------------------------------

dodoString
processor::make(const node       &root,
				const dodoString &encoding,
				const dodoString &version) const
{
	if (root.name.empty())
	{
		throw exception::basic(exception::ERRMODULE_DATAFORMATXMLPROCESSOR, PROCESSOREX_MAKE, exception::ERRNO_LIBDODO, PROCESSOREX_NONAME, DATAFORMATXMLPROCESSOREX_NONAME_STR, __LINE__, __FILE__);
	}

	dodoString processor = "<?xml version=\"" + version + "\" encoding=\"" + encoding + "\"?>\r\n";

	processor.append(make(root));

	return processor;
}

//-------------------------------------------------------------------

dodoString
processor::make(const node &xnode) const
{
	if (xnode.name.empty())
	{
		throw exception::basic(exception::ERRMODULE_DATAFORMATXMLPROCESSOR, PROCESSOREX_MAKE, exception::ERRNO_LIBDODO, PROCESSOREX_NONAME, DATAFORMATXMLPROCESSOREX_NONAME_STR, __LINE__, __FILE__);
	}

	dodoString data = statements[PROCESSOR_STATEMENT_LT];

	if (!xnode.ns.prefix.empty())
	{
		data.append(xnode.ns.prefix);
		data.append(statements[PROCESSOR_STATEMENT_COLON]);
	}
	data.append(xnode.name);
	data.append(statements[PROCESSOR_STATEMENT_SPACE]);

	if (!xnode.nsDef.prefix.empty())
	{
		data.append(statements[PROCESSOR_STATEMENT_XMLNS]);
		data.append(xnode.nsDef.prefix);
		data.append(statements[PROCESSOR_STATEMENT_EQUALDQUOTE]);
		data.append(xnode.nsDef.href);
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

	if (xnode.value.empty() && xnode.children.empty())
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
		{
			data.append(make(*x));
		}
	}

	data.append(statements[PROCESSOR_STATEMENT_LTSLASH]);

	if (!xnode.ns.prefix.empty())
	{
		data.append(xnode.ns.prefix);
		data.append(statements[PROCESSOR_STATEMENT_COLON]);
	}

	data.append(xnode.name);
	data.append(statements[PROCESSOR_STATEMENT_GT]);

	return data;
}

//-------------------------------------------------------------------

