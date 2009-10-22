/***************************************************************************
 *            dataFormatXmlProcessor.cc
 *
 *  Wed Nov 30 2005
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
#include <libdodo/ioChannel.h>

#ifdef LIBXML2_EXT
namespace dodo {
    namespace data {
        namespace format {
            namespace xml {
                /**
                 * @struct __node__
                 * @brief defines XML node properties
                 */
                struct __node__ {
                    /**
                     * constructor
                     */
                    __node__() : node(NULL)
                    {
                    }

                    /**
                     * constructor
                     * @param node defines node handle to assign
                     */
                    __node__(xmlNodePtr node) : node(node)
                    {
                    }

                    xmlNodePtr node;    ///< represents internal libxml2 node data
                };

                /**
                 * @struct __doc__
                 * @brief defines XML document properties
                 */
                struct __doc__ {
                    /**
                     * constructor
                     */
                    __doc__() : doc(NULL)
                    {
                    }

                    xmlDocPtr doc;      ///< represents internal libxml2 document data
                };
            };
        };
    };
};
#endif

using namespace dodo::data::format::xml;

__info__::__info__(const dodoString &version,
                   const dodoString &encoding,
                   const dodoString &root,
                   int              compression) : version(version),
                                                   encoding(encoding),
                                                   root(root),
                                                   compression(compression)
{
}

//-------------------------------------------------------------------

__info__::__info__()
{
}

//-------------------------------------------------------------------

__definition__::__definition__() : allChildren(true),
                                   allAttributes(true)
{
}

//-------------------------------------------------------------------

__definition__::__definition__(const dodoString &name,
                               const dodoString &ns) : name(name),
                                                       allChildren(true),
                                                       allAttributes(true),
                                                       ns(ns)
{
}

//-------------------------------------------------------------------

const dodoString processor::statements[] = {
    "<",
    ":",
    " ",
    "xmlns:",
    "=\"",
    "\"",
    "/>\r\n",
    ">",
    "<![CDATA[",
    "]]>",
    "</",
};

//-------------------------------------------------------------------

#ifdef LIBXML2_EXT
void
errHandler(void *,
           xmlErrorPtr)
{
}
#endif

//-------------------------------------------------------------------

processor::processor(processor &)
{
}

//-------------------------------------------------------------------

processor::processor() : icaseNames(false)
#ifdef LIBXML2_EXT
                         ,
                         document(new __doc__)
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
    xmlFreeDoc(document->doc);
    xmlCleanupParser();

    delete document;
#endif
}

//-------------------------------------------------------------------

#ifdef LIBXML2_EXT
bool
processor::isCDATA(const __node__ &a_xnode)
{
    xmlNodePtr xnode = a_xnode.node->children;
    while (xnode != NULL) {
        if (xnode->type == XML_CDATA_SECTION_NODE)
            return true;

        xnode = xnode->next;
    }

    return false;
}
#endif

//-------------------------------------------------------------------

node
processor::process(const __definition__ &definition,
                   const io::channel    &io)
{
#ifdef LIBXML2_EXT
    xmlFreeDoc(document->doc);

    dodoString buffer, bufferPart;
    while (true) {
        bufferPart = io.readString();
        if (bufferPart.size() == 0)
            break;

        buffer.append(bufferPart);
    }
    bufferPart.clear();

    document->doc = xmlParseMemory(buffer.data(), buffer.size());
    if (document->doc == NULL) {
        xmlErrorPtr error = xmlGetLastError();

        if (error == NULL)
            throw exception::basic(exception::MODULE_DATAFORMATXMLPROCESSOR, PROCESSOREX_PARSEBUFFER, exception::ERRNO_LIBDODO, PROCESSOREX_EMPTYDOCUMENT, DATAFORMATXMLPROCESSOREX_EMPTYDOCUMENT_STR, __LINE__, __FILE__);
        else
            throw exception::basic(exception::MODULE_DATAFORMATXMLPROCESSOR, PROCESSOREX_PARSEBUFFER, exception::ERRNO_LIBXML2, error->code, error->message, __LINE__, __FILE__);
    }
#endif

    return parse(definition);
}

//-------------------------------------------------------------------

node
processor::parse(const __definition__ &definition)
{
#ifdef LIBXML2_EXT
    __node__ xnode = xmlDocGetRootElement(document->doc);
    if (xnode.node == NULL) {
        xmlErrorPtr error = xmlGetLastError();

        if (error == NULL)
            throw exception::basic(exception::MODULE_DATAFORMATXMLPROCESSOR, PROCESSOREX_PARSE, exception::ERRNO_LIBDODO, PROCESSOREX_NOROOTNODE, DATAFORMATXMLPROCESSOREX_NOROOTNODE_STR, __LINE__, __FILE__);
        else
            throw exception::basic(exception::MODULE_DATAFORMATXMLPROCESSOR, PROCESSOREX_PARSE, exception::ERRNO_LIBXML2, error->code, error->message, __LINE__, __FILE__);
    }

    xnode = findNode(definition, xnode);

    if (xnode.node == NULL)
        return node();

    node sample;
    getNode(xnode, sample);

    getAttributes(definition, xnode, sample.attributes);

    if (xnode.node->children == NULL)
        return sample;

    sample.CDATA = isCDATA(xnode);

    if (definition.allChildren) {
        xnode.node = xnode.node->children;

        node one;

        dodoArray<node> children;
        dodoArray<node>::iterator i, j;

        while (xnode.node != NULL) {
            if (xnode.node->type != XML_ELEMENT_NODE) {
                xnode.node = xnode.node->next;

                continue;
            }

            getNode(xnode, one);

            getAttributes(xnode, one.attributes);

            one.CDATA = isCDATA(xnode);

            if (xnode.node->children != NULL) {
                children = parse(__node__(xnode.node->children));
                i = children.begin();
                j = children.end();
                for (; i != j; ++i)
                    one.nodeChildren[i->name].push_back(*i);
            }

            sample.nodeChildren[(char *)xnode.node->name].push_back(one);

            initNode(one);

            xnode.node = xnode.node->next;
        }
    } else {
        if (definition.children.size() > 0) {
            dodoMap<dodoString, __definition__>::const_iterator i(definition.children.begin()), j(definition.children.end());
            for (; i != j; ++i)
                sample.nodeChildren.insert(make_pair(i->first, parse(i->second, xnode.node->children)));
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
processor::parse(const __definition__ &definition,
                 const __node__       &a_xnode)
{
    xmlNodePtr xnode = a_xnode.node, subNode;

    node sample;
    dodoArray<node> sampleArr;

    cmpFunc = icaseNames ? xmlStrcasecmp : xmlStrcmp;

    do {
        if (xnode->type != XML_ELEMENT_NODE) {
            xnode = xnode->next;

            continue;
        }

        if (definition.ns.size() > 0) {
            if (xnode->ns == NULL) {
                xnode = xnode->next;

                continue;
            }

            if (cmpFunc(xnode->ns->prefix, (unsigned char *)definition.ns.data()) != 0) {
                xnode = xnode->next;

                continue;
            }
        }

        if (cmpFunc(xnode->name, (unsigned char *)definition.name.data()) != 0) {
            xnode = xnode->next;

            continue;
        }

        initNode(sample);

        getNode(xnode, sample);
        getAttributes(definition, xnode, sample.attributes);

        sample.CDATA = isCDATA(xnode);

        if (definition.allChildren) {
            subNode = xnode->children;

            node one;

            dodoArray<node> chldrn;
            dodoArray<node>::iterator i, j;

            while (subNode != NULL) {
                if (subNode->type != XML_ELEMENT_NODE) {
                    subNode = subNode->next;

                    continue;
                }

                getNode(subNode, one);

                getAttributes(subNode, one.attributes);

                one.CDATA = isCDATA(subNode);

                if (subNode->children != NULL) {
                    chldrn = parse(subNode->children);
                    i = chldrn.begin();
                    j = chldrn.end();
                    for (; i != j; ++i)
                        one.nodeChildren[i->name].push_back(*i);
                }

                sample.nodeChildren[(char *)subNode->name].push_back(one);

                initNode(one);

                subNode = subNode->next;
            }
        } else {
            if (definition.children.size() > 0) {
                dodoMap<dodoString, __definition__>::const_iterator i(definition.children.begin()), j(definition.children.end());
                for (; i != j; ++i)
                    sample.nodeChildren.insert(make_pair(i->first, parse(i->second, xnode->children)));
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
processor::getAttributes(const __definition__ &definition,
                         const __node__       &xnode,
                         dodoStringMap        &attributes)
{
    xmlAttrPtr attribute = xnode.node->properties;

    if (definition.allAttributes) {
        while (attribute != NULL) {
            unsigned char *xChar = xmlGetProp(xnode.node, attribute->name);
            if (xChar != NULL) {
                attributes[(char *)attribute->name] = (char *)xChar;
                xmlFree(xChar);
            }

            attribute = attribute->next;
        }
    } else {
        if (definition.attributes.size() > 0) {
            dodoStringArray::const_iterator jAttr = definition.attributes.end();
            if (icaseNames) {
                while (attribute != NULL) {
                    dodoStringArray::const_iterator iAttr = definition.attributes.begin();
                    for (; iAttr != jAttr; ++iAttr) {
                        if (xmlStrcmp(attribute->name, (unsigned char *)iAttr->data()) == 0) {
                            unsigned char *xChar = xmlGetProp(xnode.node, attribute->name);
                            if (xChar != NULL) {
                                attributes[*iAttr] = (char *)xChar;
                                xmlFree(xChar);
                            }
                        }
                    }

                    attribute = attribute->next;
                }
            } else {
                dodoStringArray::const_iterator iAttr = definition.attributes.begin();
                for (; iAttr != jAttr; ++iAttr) {
                    unsigned char *xChar = xmlGetProp(xnode.node, (unsigned char *)iAttr->data());
                    if (xChar != NULL) {
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
processor::getAttributes(const __node__ &xnode,
                         dodoStringMap  &attributes)
{
    xmlAttrPtr attribute = xnode.node->properties;

    while (attribute != NULL) {
        unsigned char *xChar = xmlGetProp(xnode.node, attribute->name);
        if (xChar != NULL) {
            attributes[(char *)attribute->name] = (char *)xChar;
            xmlFree(xChar);
        }

        attribute = attribute->next;
    }
}

//-------------------------------------------------------------------

void
processor::getNode(const __node__ &xnode,
                   node           &resNode)
{
    if (xnode.node->ns != NULL) {
        resNode.ns.prefix = (char *)xnode.node->ns->prefix;
        resNode.ns.href = (char *)xnode.node->ns->href;
    }

    if (xnode.node->nsDef != NULL) {
        resNode.nsDef.prefix = (char *)xnode.node->nsDef->prefix;
        resNode.nsDef.href = (char *)xnode.node->nsDef->href;
    }

    if (xnode.node->name != NULL)
        resNode.name.assign((char *)xnode.node->name);

    unsigned char *xChar = xmlNodeListGetString(document->doc, xnode.node->children, 1);
    if (xChar != NULL) {
        resNode.nodeValue.assign((char *)xChar);
        xmlFree(xChar);
    }
}
#endif

//-------------------------------------------------------------------

__info__
processor::information()
{
    return info;
}

//-------------------------------------------------------------------

#ifdef LIBXML2_EXT
dodoArray<node>
processor::parse(__node__ xnode)
{
    dodoArray<node> sample;

    node one;

    dodoArray<node> children;
    dodoArray<node>::iterator i, j;

    while (xnode.node != NULL) {
        if (xnode.node->type != XML_ELEMENT_NODE) {
            xnode.node = xnode.node->next;

            continue;
        }

        getNode(xnode, one);

        getAttributes(xnode, one.attributes);

        one.CDATA = isCDATA(xnode);

        if (xnode.node->children != NULL) {
            children = parse(__node__(xnode.node->children));
            i = children.begin();
            j = children.end();
            for (; i != j; ++i)
                one.nodeChildren[i->name].push_back(*i);
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
    xnode.nodeChildren.clear();
    xnode.name.clear();
    xnode.ns.prefix.clear();
    xnode.ns.href.clear();
    xnode.nsDef.prefix.clear();
    xnode.nsDef.href.clear();
    xnode.nodeValue.clear();
}

//-------------------------------------------------------------------

node
processor::process(const io::channel &io)
{
#ifdef LIBXML2_EXT
    xmlFreeDoc(document->doc);

    dodoString buffer, bufferPart;
    while (true) {
        bufferPart = io.readString();
        if (bufferPart.size() == 0)
            break;

        buffer.append(bufferPart);
    }
    bufferPart.clear();

    document->doc = xmlParseMemory(buffer.data(), buffer.size());
    if (document->doc == NULL) {
        xmlErrorPtr error = xmlGetLastError();

        if (error == NULL)
            throw exception::basic(exception::MODULE_DATAFORMATXMLPROCESSOR, PROCESSOREX_PARSEBUFFER, exception::ERRNO_LIBDODO, PROCESSOREX_EMPTYDOCUMENT, DATAFORMATXMLPROCESSOREX_EMPTYDOCUMENT_STR, __LINE__, __FILE__);
        else
            throw exception::basic(exception::MODULE_DATAFORMATXMLPROCESSOR, PROCESSOREX_PARSEBUFFER, exception::ERRNO_LIBXML2, error->code, error->message, __LINE__, __FILE__);
    }

    info = __info__(document->doc->version != NULL ? (char *)document->doc->version : __dodostring__,
                    document->doc->encoding != NULL ? (char *)document->doc->encoding : __dodostring__,
                    (document->doc->children != NULL && document->doc->children->name != NULL) ? (char *)document->doc->children->name : __dodostring__,
                    document->doc->compression);


    xmlNodePtr xnode = xmlDocGetRootElement(document->doc);
    if (xnode == NULL) {
        xmlErrorPtr error = xmlGetLastError();

        if (error == NULL)
            throw exception::basic(exception::MODULE_DATAFORMATXMLPROCESSOR, PROCESSOREX_PARSEBUFFER, exception::ERRNO_LIBDODO, PROCESSOREX_NOROOTNODE, DATAFORMATXMLPROCESSOREX_NOROOTNODE_STR, __LINE__, __FILE__);
        else
            throw exception::basic(exception::MODULE_DATAFORMATXMLPROCESSOR, PROCESSOREX_PARSEBUFFER, exception::ERRNO_LIBXML2, error->code, error->message, __LINE__, __FILE__);
    }

    return *(parse(__node__(xnode)).begin());
#else
    return node();
#endif
}

//-------------------------------------------------------------------

#ifdef LIBXML2_EXT
__node__
processor::findNode(const __definition__ &definition,
                    const __node__       &a_xnode)
{
    xmlNodePtr one, xnode = a_xnode.node;
    bool skip;

    cmpFunc = icaseNames ? xmlStrcasecmp : xmlStrcmp;

    while (xnode != NULL) {
        if (xnode->type != XML_ELEMENT_NODE) {
            xnode = xnode->next;

            continue;
        }

        skip = false;

        if (definition.ns.size() > 0) {
            if (xnode->ns == NULL)
                skip = true;
            else if (cmpFunc(xnode->ns->prefix, (unsigned char *)definition.ns.data()) != 0)
                skip = true;
        }

        if (!skip && xnode->name != NULL)
            if (cmpFunc(xnode->name, (unsigned char *)definition.name.data()) == 0)
                return xnode;

        one = findNode(definition, __node__(xnode->children)).node;

        if (one != NULL)
            return one;

        xnode = xnode->next;
    }

    return __node__(NULL);
}
#endif

//-------------------------------------------------------------------

void
processor::clear()
{
#ifdef LIBXML2_EXT
    xmlFreeDoc(document->doc);
    document->doc = NULL;
#endif
}

//-------------------------------------------------------------------

void
processor::make(const node        &root,
                const dodoString  &encoding,
                const dodoString  &version,
                const io::channel &io) const
{
    if (root.name.empty())
        throw exception::basic(exception::MODULE_DATAFORMATXMLPROCESSOR, PROCESSOREX_MAKE, exception::ERRNO_LIBDODO, PROCESSOREX_NONAME, DATAFORMATXMLPROCESSOREX_NONAME_STR, __LINE__, __FILE__);

    io.writeString("<?xml version=\"");
    io.writeString(version);
    io.writeString("\" encoding=\"");
    io.writeString(encoding);
    io.writeString("\"?>\r\n");

    make(root, io);
}

//-------------------------------------------------------------------

void
processor::make(const node        &xnode,
                const io::channel &io) const
{
    if (xnode.name.empty())
        throw exception::basic(exception::MODULE_DATAFORMATXMLPROCESSOR, PROCESSOREX_MAKE, exception::ERRNO_LIBDODO, PROCESSOREX_NONAME, DATAFORMATXMLPROCESSOREX_NONAME_STR, __LINE__, __FILE__);

    io.writeString(statements[STATEMENT_LT]);

    if (!xnode.ns.prefix.empty()) {
        io.writeString(xnode.ns.prefix);
        io.writeString(statements[STATEMENT_COLON]);
    }
    io.writeString(xnode.name);

    if (!xnode.nsDef.prefix.empty()) {
        io.writeString(statements[STATEMENT_SPACE]);
        io.writeString(statements[STATEMENT_XMLNS]);
        io.writeString(xnode.nsDef.prefix);
        io.writeString(statements[STATEMENT_EQUALDQUOTE]);
        io.writeString(xnode.nsDef.href);
        io.writeString(statements[STATEMENT_DQUOTE]);
    }

    dodoMap<dodoString, dodoString, dodoMapStringCompare>::const_iterator i = xnode.attributes.begin(), j = xnode.attributes.end();
    for (; i != j; ++i) {
        io.writeString(statements[STATEMENT_SPACE]);
        io.writeString(i->first);
        io.writeString(statements[STATEMENT_EQUALDQUOTE]);
        io.writeString(i->second);
        io.writeString(statements[STATEMENT_DQUOTE]);
    }

    if (xnode.nodeValue.empty() && xnode.nodeChildren.empty()) {
        io.writeString(statements[STATEMENT_SLASHGT]);

        return;
    }

    io.writeString(statements[STATEMENT_GT]);

    if (!xnode.nodeValue.empty()) {
        if (xnode.CDATA) {
            io.writeString(statements[STATEMENT_CDATAOPEN]);
            io.writeString(xnode.nodeValue);
            io.writeString(statements[STATEMENT_CDATACLOSE]);
        } else
            io.writeString(xnode.nodeValue);
    }

    dodoMap<dodoString, dodoArray<node>, dodoMapStringCompare>::const_iterator o = xnode.nodeChildren.begin(), p = xnode.nodeChildren.end();
    dodoArray<node>::const_iterator x, y;
    for (; o != p; ++o) {
        x = o->second.begin();
        y = o->second.end();
        for (; x != y; ++x)
            make(*x, io);
    }

    io.writeString(statements[STATEMENT_LTSLASH]);

    if (!xnode.ns.prefix.empty()) {
        io.writeString(xnode.ns.prefix);
        io.writeString(statements[STATEMENT_COLON]);
    }

    io.writeString(xnode.name);
    io.writeString(statements[STATEMENT_GT]);
}

//-------------------------------------------------------------------

