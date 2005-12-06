/***************************************************************************
 *            xmlTools.cc
 *
 *  Wed Nov 30 22:02:16 2005
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


#include <xmlTools.h>

#ifdef LIBXML2_EXT
	
	using namespace dodo;
 
	__xmlInfo::__xmlInfo(const std::string &a_version, 
		 				const std::string &a_encoding, 
	 					const std::string &a_root, 
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
 
	__nodeDef::__nodeDef(): chLimit(-1)
	{
	}
	
	//-------------------------------------------------------------------
	  
	xmlTools::xmlTools() : icaseNames(false)
	{
		xmlInitParser();
		xmlSetStructuredErrorFunc(NULL, xmlTools::errHandler);
	}
	 
	//-------------------------------------------------------------------
	 
	xmlTools::~xmlTools()
	{
		xmlCleanupParser();
	}
	 
	//-------------------------------------------------------------------
	
	__node 
	xmlTools::parseFile(const __nodeDef &definition, 
						const std::string &file)
	{
		document = xmlParseFile(file.c_str());
		if (document == NULL)
			#ifndef NO_EX
			{
				error = xmlGetLastError();
				throw baseEx(ERRMODULE_LIBXML2,XMLTOOLS_PARCEFILE,ERR_LIBXML2,error->code,error->message,__LINE__,__FILE__);
			}
			#else
				return __node();
			#endif
		
		__node sample = parse(definition);
		
		xmlFreeDoc(document);
		
		return sample;
	}
	 
	//-------------------------------------------------------------------
	
	__node 
	xmlTools::parseBuffer(const __nodeDef &definition, 
						const std::string &buffer)
	{
		document = xmlParseMemory(buffer.c_str(),buffer.size());
		if (document == NULL)
			#ifndef NO_EX
			{
				error = xmlGetLastError();
				throw baseEx(ERRMODULE_LIBXML2,XMLTOOLS_PARCEBUFFER,ERR_LIBXML2,error->code,error->message,__LINE__,__FILE__);
			}
			#else
				return __node();
			#endif
		
		__node sample = parse(definition);
		
		xmlFreeDoc(document);
		
		return sample;
	}
	
	//-------------------------------------------------------------------

	__node 
	xmlTools::parse(const __nodeDef &definition)
	{
		xmlNodePtr node = xmlDocGetRootElement(document);
		if (node == NULL)
			#ifndef NO_EX
			{
				error = xmlGetLastError();
				throw baseEx(ERRMODULE_LIBXML2,XMLTOOLS_PARCE,ERR_LIBXML2,error->code,error->message,__LINE__,__FILE__);
			}
			#else
				return __node();
			#endif
		
		__node sample;
		
		do
		{			
			if (definition.ns.size()>0)
			{
				if (node->ns==NULL)
				{
					node = node->next;
					continue;
				}
				
				if (icaseNames)
					result = xmlStrcasecmp(node->ns->prefix,(xmlChar *)definition.ns.c_str());
				else
					result = xmlStrcmp(node->ns->prefix,(xmlChar *)definition.ns.c_str());
	
				if (result!=0)
				{
					node = node->next;
					continue;
				}
			}
			
			result = 0;
			if (definition.name.size()>0)
			{			
				if (icaseNames)
					result = xmlStrcasecmp(node->name,(xmlChar *)definition.name.c_str());
				else
					result = xmlStrcmp(node->name,(xmlChar *)definition.name.c_str());
			}
			else
				if (node->type != XML_ELEMENT_NODE)
				{
					node = node->next;
					continue;
				}
			
			if (result==0)
			{
				getNodeInfo(node,sample);
				
				getAttributes(definition,node,sample.attributes);
				
				std::vector<__nodeDef>::const_iterator i(definition.children.begin()),j(definition.children.end());
				for (;i!=j;++i)
						sample.children.push_back(parse(*i,node->children,definition.chLimit));
				break;
			}
			
			node = node->next;
		}
		while (node!=NULL);
		
		return sample;
	}

	//-------------------------------------------------------------------

	std::vector<__node>
	xmlTools::parse(const __nodeDef &definition, 
					const xmlNodePtr chNode, 
					long chLimit)
	{
		if (chNode == NULL)
			return std::vector<__node>();
		
		register xmlNodePtr node = chNode;
		
		__node sample;
		std::vector<__node> sampleArr;

		do
		{
			if (chLimit!=-1)
			{
				if (chLimit<=0)
					return sampleArr;
				
				--chLimit;
			}
			
			if (definition.ns.size()>0)
			{
				if (node->ns==NULL)
				{
					node = node->next;
					continue;
				}
				
				if (icaseNames)
					result = xmlStrcasecmp(node->ns->prefix,(xmlChar *)definition.ns.c_str());
				else
					result = xmlStrcmp(node->ns->prefix,(xmlChar *)definition.ns.c_str());
	
				if (result!=0)
				{
					node = node->next;
					continue;
				}
			}
			
			result = 0;
			if (definition.name.size()>0)
			{
				if (icaseNames)
					result = xmlStrcasecmp(node->name,(xmlChar *)definition.name.c_str());
				else
					result = xmlStrcmp(node->name,(xmlChar *)definition.name.c_str());
			}
			else
				if (node->type != XML_ELEMENT_NODE)
				{
					node = node->next;
					continue;
				}
			
			if (result!=0)
			{
				node = node->next;
				continue;		
			}
		
			getNodeInfo(node,sample);

			sample.attributes.clear();
			getAttributes(definition,node,sample.attributes);
	
			std::vector<__nodeDef>::const_iterator i(definition.children.begin()),j(definition.children.end());
			
			for (;i!=j;++i)
				sample.children.push_back(parse(*i,node->children,definition.chLimit));
			
			sampleArr.push_back(sample);
			
			node = node->next;
		}
		while (node!=NULL);
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
	xmlTools::getAttributes(const __nodeDef &definition, 
						const xmlNodePtr node,
						assocArr &attributes)
	{
		attribute = node->properties;
		jAttr = definition.attributes.end();
		
		while (attribute!=NULL)
		{
			if (definition.attributes.size()>0)
			{
				iAttr = definition.attributes.begin();
				for (;iAttr!=jAttr;++iAttr)
				{
					if (icaseNames)
						result = xmlStrcasecmp(attribute->name,(xmlChar *)iAttr->c_str());
					else
						result = xmlStrcmp(attribute->name,(xmlChar *)iAttr->c_str());
					
					if (result==0)
					{
						xChar = xmlGetProp(node,attribute->name);
						if (xChar!=NULL)
						{
							attributes[iAttr->c_str()] = (char *)xChar;
							xmlFree(xChar);
						}
					}
                }
			}
			else
			{
				xChar = xmlGetProp(node,attribute->name);
				if (xChar!=NULL)
				{
					attributes[(char *)attribute->name] = (char *)xChar;
					xmlFree(xChar);
				}						
			}
			
			attribute = attribute->next;
		}		
	}

	//-------------------------------------------------------------------

	void 
	xmlTools::getAttributes(const xmlNodePtr node, 
							assocArr &attributes)
	{
		attribute = node->properties;
		
		while (attribute!=NULL)
		{
			xChar = xmlGetProp(node,attribute->name);
			if (xChar!=NULL)
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
							__node &resNode)
	{
		if (node->ns!=NULL)
		{
			resNode.ns = (char *)node->ns->prefix;
			resNode.nsHref = (char *)node->ns->href;
		}
		
		if (node->nsDef!=NULL)
		{
			resNode.nsDef = (char *)node->nsDef->prefix;
			resNode.nsDefHref = (char *)node->nsDef->href;
		}

		if (node->name!=NULL)
			resNode.name.assign((char *)node->name);
		
		xChar = xmlNodeListGetString(document,node->children,1);
		if (xChar!=NULL)
		{
			resNode.value.assign((char *)xChar);
			xmlFree(xChar);
		}		
	}
	
	//-------------------------------------------------------------------
	
	__xmlInfo 
	xmlTools::getXMLFileInfo(const std::string &file)
	{
		document = xmlParseFile(file.c_str());
		if (document == NULL)
			#ifndef NO_EX
			{
				error = xmlGetLastError();
				throw baseEx(ERRMODULE_LIBXML2,XMLTOOLS_GETXMLFILEINFO,ERR_LIBXML2,error->code,error->message,__LINE__,__FILE__);
			}
			#else
				return __xmlInfo();
			#endif		
			
		return __xmlInfo((char *)document->version,(char *)document->encoding,(char *)document->children->name,document->compression);
	}

	//-------------------------------------------------------------------
	
	__xmlInfo 
	xmlTools::getXMLBufferInfo(const std::string &buffer)
	{
		document = xmlParseMemory(buffer.c_str(),buffer.size());
		if (document == NULL)
			#ifndef NO_EX
			{
				error = xmlGetLastError();
				throw baseEx(ERRMODULE_LIBXML2,XMLTOOLS_GETXMLBUFFERINFO,ERR_LIBXML2,error->code,error->message,__LINE__,__FILE__);
			}
			#else
				return __xmlInfo();
			#endif		
			
		return __xmlInfo((char *)document->version,(char *)document->encoding,(char *)document->children->name,document->compression);
	}

	//-------------------------------------------------------------------	
	
	std::vector<__node> 
	xmlTools::parse(xmlNodePtr node)
	{
		std::vector<__node> sample;
		__node one;
		
		while (node!=NULL)
		{
			if (node->type != XML_ELEMENT_NODE)
			{
				node = node->next;
				continue;
			}
										
			getNodeInfo(node,one);
			
			getAttributes(node,one.attributes);
			
			one.children.push_back(parse(node->children));
			
			sample.push_back(one);
			
			initNode(one);
			
			node = node->next;
		}
		
		return sample;		
	}

	//-------------------------------------------------------------------	
		
	void 
	xmlTools::initNode(__node &node)
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
	
	__node 
	xmlTools::parseFile(const std::string &file)
	{
		document = xmlParseFile(file.c_str());
		if (document == NULL)
			#ifndef NO_EX
			{
				error = xmlGetLastError();
				throw baseEx(ERRMODULE_LIBXML2,XMLTOOLS_PARCEFILE,ERR_LIBXML2,error->code,error->message,__LINE__,__FILE__);
			}
			#else
				return __node();
			#endif
			
		xmlNodePtr node = xmlDocGetRootElement(document);
		if (node == NULL)
			#ifndef NO_EX
			{
				error = xmlGetLastError();
				throw baseEx(ERRMODULE_LIBXML2,XMLTOOLS_PARCEFILE,ERR_LIBXML2,error->code,error->message,__LINE__,__FILE__);
			}
			#else
				return __node();
			#endif
					
		__node sample = parse(node)[0];
		
		xmlFreeDoc(document);
		
		return sample;
	}
	 
	//-------------------------------------------------------------------
	
	__node 
	xmlTools::parseBuffer(const std::string &buffer)
	{
		document = xmlParseMemory(buffer.c_str(),buffer.size());
		if (document == NULL)
			#ifndef NO_EX
			{
				error = xmlGetLastError();
				throw baseEx(ERRMODULE_LIBXML2,XMLTOOLS_PARCEBUFFER,ERR_LIBXML2,error->code,error->message,__LINE__,__FILE__);
			}
			#else
				return __node();
			#endif
			
		xmlNodePtr node = xmlDocGetRootElement(document);
		if (node == NULL)
			#ifndef NO_EX
			{
				error = xmlGetLastError();
				throw baseEx(ERRMODULE_LIBXML2,XMLTOOLS_PARCEBUFFER,ERR_LIBXML2,error->code,error->message,__LINE__,__FILE__);
			}
			#else
				return __node();
			#endif
					
		__node sample = parse(node)[0];
		
		xmlFreeDoc(document);
		
		return sample;
	}
	
	//-------------------------------------------------------------------

	
#endif
