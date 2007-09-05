/***************************************************************************
 *            xmlTools.cc
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


#include <xmlTools.h>

#ifdef LIBXML2_EXT
	
	using namespace dodo;
 
	__xmlInfo::__xmlInfo(const dodoString &a_version, 
		 				const dodoString &a_encoding, 
	 					const dodoString &a_root, 
	 					int a_compression): version(a_version),
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
 
	__xmlNodeDef::__xmlNodeDef(): chLimit(-1),
							ignoreChildrenDef(false),
							ignoreAttributesDef(true)
	{
	}
	
	//-------------------------------------------------------------------
	
	__xmlNode::__xmlNode(): CDATA(false),
					empty(false)	
	{
	}
	
	//-------------------------------------------------------------------

	xmlTools::xmlTools(xmlTools &xt)
	{
	}

	//-------------------------------------------------------------------
	  
	xmlTools::xmlTools() : icaseNames(false), 
							document(NULL)
	{
		xmlPedanticParserDefault(0);
		xmlInitParser();
		xmlSetStructuredErrorFunc(NULL, xmlTools::errHandler);
	}
	 
	//-------------------------------------------------------------------
	 
	xmlTools::~xmlTools()
	{
		xmlFreeDoc(document);
		
		xmlCleanupParser();
	}

	//-------------------------------------------------------------------
	
	__xmlNode 
	xmlTools::reParse(const __xmlNodeDef &definition)
	{
		if (document == NULL)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_LIBXML2, XMLTOOLS_REPARCE, ERR_LIBDODO, XMLTOOLS_NOT_PARCED_BEFORE, XMLTOOLS_NOT_PARCED_BEFORE_STR,__LINE__,__FILE__);
			#else
				return __xmlNode();
			#endif
		
		return parse(definition);		
	}
	
	//-------------------------------------------------------------------
	
	bool 
	xmlTools::isCDATA(xmlNodePtr chNode)
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
	xmlTools::parseFile(const __xmlNodeDef &definition, 
						const dodoString &file)
	{
		xmlFreeDoc(document);
		
		document = xmlParseFile(file.c_str());
		if (document == NULL)
			#ifndef NO_EX
			{
				xmlErrorPtr error = xmlGetLastError();
				throw baseEx(ERRMODULE_LIBXML2, XMLTOOLS_PARCEFILE, ERR_LIBXML2, error->code, error->message,__LINE__,__FILE__, file);
			}
			#else
				return __xmlNode();
			#endif
		
		return parse(definition);
	}
	 
	//-------------------------------------------------------------------
	
	__xmlNode 
	xmlTools::parseBuffer(const __xmlNodeDef &definition, 
						const dodoString &buffer)
	{
		xmlFreeDoc(document);
		
		document = xmlParseMemory(buffer.c_str(), buffer.size());
		if (document == NULL)
			#ifndef NO_EX
			{
				xmlErrorPtr error = xmlGetLastError();
				throw baseEx(ERRMODULE_LIBXML2, XMLTOOLS_PARCEBUFFER, ERR_LIBXML2, error->code, error->message,__LINE__,__FILE__);
			}
			#else
				return __xmlNode();
			#endif
		
		return parse(definition);
	}
	
	//-------------------------------------------------------------------
	
	#ifndef NO_EX
		void 
	#else
		bool 
	#endif	
	xmlTools::parseFileInt(const dodoString &file)
	{
		xmlFreeDoc(document);
		
		document = xmlParseFile(file.c_str());
		if (document == NULL)
			#ifndef NO_EX
			{
				xmlErrorPtr error = xmlGetLastError();
				throw baseEx(ERRMODULE_LIBXML2, XMLTOOLS_PARCEFILEINT, ERR_LIBXML2, error->code, error->message,__LINE__,__FILE__, file);
			}
			#else
				return false;
			#endif
		
		#ifdef NO_EX
			return true;
		#endif
	}
	 
	//-------------------------------------------------------------------
	
	#ifndef NO_EX
		void 
	#else
		bool 
	#endif		
	xmlTools::parseBufferInt(const dodoString &buffer)
	{
		xmlFreeDoc(document);
		
		document = xmlParseMemory(buffer.c_str(), buffer.size());
		if (document == NULL)
			#ifndef NO_EX
			{
				xmlErrorPtr error = xmlGetLastError();
				throw baseEx(ERRMODULE_LIBXML2, XMLTOOLS_PARCEBUFFERINT, ERR_LIBXML2, error->code, error->message,__LINE__,__FILE__);
			}
			#else
				return false;
			#endif
		
		#ifdef NO_EX
			return true;
		#endif
	}
		
	//-------------------------------------------------------------------

	__xmlNode 
	xmlTools::parse(const __xmlNodeDef &definition)
	{
		xmlNodePtr node = xmlDocGetRootElement(document);
		if (node == NULL)
			#ifndef NO_EX
			{
				xmlErrorPtr error = xmlGetLastError();
				throw baseEx(ERRMODULE_LIBXML2, XMLTOOLS_PARCE, ERR_LIBXML2, error->code, error->message,__LINE__,__FILE__);
			}
			#else
				return __xmlNode();
			#endif
		
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
			for (;i!=j;++i)
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
	xmlTools::parse(const __xmlNodeDef &definition, 
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
				if (chLimit<=0)
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
	
				if (cmpFunc(node->ns->prefix,(xmlChar *)definition.ns.c_str()) != 0)
				{
					node = node->next;
					continue;
				}
			}
				
			if (cmpFunc(node->name,(xmlChar *)definition.name.c_str()) != 0)
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
				for (;i!=j;++i)
					sample.children.insert(i->first, parse(i->second, node->children, definition.chLimit));
				
			}
			else
			{
				if (definition.ignoreChildrenDef)
				{
					subNode = node->children;
					
					while (subNode!=NULL)
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
	xmlTools::errHandler(void *data, 
						xmlErrorPtr error)
	{
	}

	//-------------------------------------------------------------------
	
	void 
	xmlTools::getAttributes(const __xmlNodeDef &definition, 
						const xmlNodePtr node,
						dodoStringMap &attributes)
	{
		attribute = node->properties;
		
		if (definition.attributes.size() > 0)
		{
			dodoStringArr::const_iterator jAttr = definition.attributes.end();
			if (icaseNames)
			{
				while (attribute != NULL)
				{
					dodoStringArr::const_iterator iAttr = definition.attributes.begin();
					for (;iAttr!=jAttr;++iAttr)
					{
						if (xmlStrcmp(attribute->name,(xmlChar *)iAttr->c_str()) == 0)
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
				dodoStringArr::const_iterator iAttr = definition.attributes.begin();
				for (;iAttr!=jAttr;++iAttr)
				{
					xmlChar *xChar = xmlGetProp(node,(xmlChar *)iAttr->c_str());
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
					if (xChar!=NULL)
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
	xmlTools::getAttributes(const xmlNodePtr node, 
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
	xmlTools::getNodeInfo(const xmlNodePtr node, 
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
	xmlTools::getXMLFileInfo(const dodoString &file)
	{
		document = xmlParseFile(file.c_str());
		if (document == NULL)
			#ifndef NO_EX
			{
				xmlErrorPtr error = xmlGetLastError();
				throw baseEx(ERRMODULE_LIBXML2, XMLTOOLS_GETXMLFILEINFO, ERR_LIBXML2, error->code, error->message,__LINE__,__FILE__, file);
			}
			#else
				return __xmlInfo();
			#endif		
			
		return __xmlInfo(document->version!=NULL?(char *)document->version:__dodostring__,
		document->encoding != NULL?(char *)document->encoding:__dodostring__,
		(document->children != NULL && document->children->name != NULL)?(char *)document->children->name:__dodostring__,
		document->compression);
	}

	//-------------------------------------------------------------------
	
	__xmlInfo 
	xmlTools::getXMLBufferInfo(const dodoString &buffer)
	{
		document = xmlParseMemory(buffer.c_str(), buffer.size());
		if (document == NULL)
			#ifndef NO_EX
			{
				xmlErrorPtr error = xmlGetLastError();
				throw baseEx(ERRMODULE_LIBXML2, XMLTOOLS_GETXMLBUFFERINFO, ERR_LIBXML2, error->code, error->message,__LINE__,__FILE__);
			}
			#else
				return __xmlInfo();
			#endif		
			
		return __xmlInfo((char *)document->version,(char *)document->encoding,(char *)document->children->name, document->compression);
	}

	//-------------------------------------------------------------------	
	
	dodoArray<__xmlNode>
	xmlTools::parse(xmlNodePtr node)
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
	xmlTools::initNode(__xmlNode &node)
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
	xmlTools::parseFile(const dodoString &file)
	{
		xmlFreeDoc(document);
		
		document = xmlParseFile(file.c_str());
		if (document == NULL)
			#ifndef NO_EX
			{
				xmlErrorPtr error = xmlGetLastError();
				throw baseEx(ERRMODULE_LIBXML2, XMLTOOLS_PARCEFILE, ERR_LIBXML2, error->code, error->message,__LINE__,__FILE__, file);
			}
			#else
				return __xmlNode();
			#endif
			
		xmlNodePtr node = xmlDocGetRootElement(document);
		if (node == NULL)
			#ifndef NO_EX
			{
				xmlErrorPtr error = xmlGetLastError();
				throw baseEx(ERRMODULE_LIBXML2, XMLTOOLS_PARCEFILE, ERR_LIBXML2, error->code, error->message,__LINE__,__FILE__, file);
			}
			#else
				return __xmlNode();
			#endif
					
		__xmlNode sample = *(parse(node).begin());
		
		return sample;
	}
	 
	//-------------------------------------------------------------------
	
	__xmlNode 
	xmlTools::parseBuffer(const dodoString &buffer)
	{
		xmlFreeDoc(document);
		
		document = xmlParseMemory(buffer.c_str(), buffer.size());
		if (document == NULL)
			#ifndef NO_EX
			{
				xmlErrorPtr error = xmlGetLastError();
				throw baseEx(ERRMODULE_LIBXML2, XMLTOOLS_PARCEBUFFER, ERR_LIBXML2, error->code, error->message,__LINE__,__FILE__);
			}
			#else
				return __xmlNode();
			#endif
			
		xmlNodePtr node = xmlDocGetRootElement(document);
		if (node == NULL)
			#ifndef NO_EX
			{
				xmlErrorPtr error = xmlGetLastError();
				throw baseEx(ERRMODULE_LIBXML2, XMLTOOLS_PARCEBUFFER, ERR_LIBXML2, error->code, error->message,__LINE__,__FILE__);
			}
			#else
				return __xmlNode();
			#endif
					
		__xmlNode sample = *(parse(node).begin());
		
		return sample;
	}
	
	//-------------------------------------------------------------------

	void 
	xmlTools::initNodeDef(__xmlNodeDef &node)
	{
		node.attributes.clear();
		node.children.clear();
		node.name.clear();
		node.ns.clear();
		node.chLimit = -1;
	}

	//-------------------------------------------------------------------

	xmlNodePtr
	xmlTools::findNode(const __xmlNodeDef &definition,
						xmlNodePtr node)
	{
		xmlNodePtr one;
		bool skip;

		if (icaseNames)
			cmpFunc = xmlStrcasecmp;
		else
			cmpFunc = xmlStrcmp;
					
		while (node!=NULL)
		{
			if (node->type != XML_ELEMENT_NODE)
			{
				node = node->next;
				continue;
			}

			skip = false;

			if (definition.ns.size()>0)
			{
				if (node->ns == NULL)
					skip = true;
				else
					if (cmpFunc(node->ns->prefix,(xmlChar *)definition.ns.c_str()) != 0)
						skip = true;
			}
			
			if (!skip && node->name != NULL)
				if (cmpFunc(node->name,(xmlChar *)definition.name.c_str()) == 0)
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
	xmlTools::clear()
	{
		xmlFreeDoc(document);
		document = NULL;
	}

	//-------------------------------------------------------------------
	
	dodoString 
	xmlTools::createXML(const __xmlNode &root, 
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
	xmlTools::createNode(const __xmlNode &node) const
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
		
		dodoMap<dodoString, dodoString, dodoString::equal>::const_iterator i = node.attributes.begin(), j = node.attributes.end(); 
		for (;i!=j;++i)
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
		
		dodoMap<dodoString, dodoArray<__xmlNode>, dodoString::equal>::const_iterator o = node.children.begin(), p = node.children.end();
		dodoArray<__xmlNode>::const_iterator x, y;
		for (;o!=p;++o)
		{
			x = o->second.begin();
			y = o->second.end();
			for (;x!=y;++x)
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

	//-------------------------------------------------------------------
	
#endif
