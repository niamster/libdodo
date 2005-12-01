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
 
	__nodeDef::__nodeDef(): chLimit(-1)
	{
	}
	
	//-------------------------------------------------------------------
	  
	xmlTools::xmlTools()
	{
	}
	 
	//-------------------------------------------------------------------
	 
	xmlTools::~xmlTools()
	{
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
		
		__node node = parse(definition);
		
		xmlFreeDoc(document);
		
		return node;
	}

	//-------------------------------------------------------------------

	__node 
	xmlTools::parse(const __nodeDef &definition)
	{
		register xmlNodePtr chNode = xmlDocGetRootElement(document);
		if (chNode == NULL)
			#ifndef NO_EX
			{
				error = xmlGetLastError();
				throw baseEx(ERRMODULE_LIBXML2,XMLTOOLS_PARCE,ERR_LIBXML2,error->code,error->message,__LINE__,__FILE__);
			}
			#else
				return __node();
			#endif
		
		__node node;
		
		if (xmlStrcmp(chNode->name,(xmlChar *)definition.name.c_str())==0)	
		{
			node.name.assign((char *)chNode->name);
			xChar = xmlNodeListGetString(document,chNode->children,1);
			if (xChar!=NULL)
			{
				node.value.assign((char *)xChar);
				xmlFree(xChar);
			}			
			
			iAttr = definition.attributes.begin();
			jAttr = definition.attributes.end();
			for (;iAttr!=jAttr;++iAttr)
			{
				xChar = xmlGetProp(chNode,(xmlChar *)iAttr->c_str());
				if (xChar!=NULL)
				{
					node.attributes[*iAttr] = (char *)xChar;
					xmlFree(xChar);
				}
			}
			
			std::vector<__nodeDef>::const_iterator i(definition.children.begin()),j(definition.children.end());
			for (;i!=j;++i)
					node.children.push_back(parse(*i,chNode->children,definition.chLimit));
		}
		else
		{
			chNode = chNode->children;
			while (chNode!=NULL)
			{
				if (xmlStrcmp(chNode->name,(xmlChar *)definition.name.c_str())==0)	
				{
					node.name.assign((char *)chNode->name);
					xChar = xmlNodeListGetString(document,chNode->children,1);
					if (xChar!=NULL)
					{
						node.value.assign((char *)xChar);
						xmlFree(xChar);
					}			
					
					iAttr = definition.attributes.begin();
					jAttr = definition.attributes.end();
					for (;iAttr!=jAttr;++iAttr)
					{
						xChar = xmlGetProp(chNode,(xmlChar *)iAttr->c_str());
						if (xChar!=NULL)
						{
							node.attributes[*iAttr] = (char *)xChar;
							xmlFree(xChar);
						}
					}
					
					std::vector<__nodeDef>::const_iterator i(definition.children.begin()),j(definition.children.end());
					for (;i!=j;++i)
							node.children.push_back(parse(*i,chNode->children,definition.chLimit));
					
					break;
				}
				
				chNode = chNode->next;
			}
		}
		
		xmlFreeNode(chNode);
		
		return node;
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

		while ( node!=NULL )
		{			
			if (xmlStrcmp(node->name,(xmlChar *)definition.name.c_str())!=0)
			{
				node = node->next;
				continue;		
			}

			if (chLimit!=-1)
			{
				if (chLimit<=0)
					return sampleArr;
				
				--chLimit;
			}

			sample.attributes.clear();
			
			sample.name.assign((char *)node->name);
			xChar = xmlNodeListGetString(document,node->children,1);
			if (xChar!=NULL)
			{
				sample.value.assign((char *)xChar);
				xmlFree(xChar);
			}
	
			iAttr = definition.attributes.begin();
			jAttr = definition.attributes.end();
			for (;iAttr!=jAttr;++iAttr)
			{
				xChar = xmlGetProp(node,(xmlChar *)iAttr->c_str());
				if (xChar!=NULL)
				{
					sample.attributes[*iAttr] = (char *)xChar;
					xmlFree(xChar);
				}
			}
	
			std::vector<__nodeDef>::const_iterator i(definition.children.begin()),j(definition.children.end());
			
			for (;i!=j;++i)
				sample.children.push_back(parse(*i,node->children,definition.chLimit));
			
			sampleArr.push_back(sample);
					
			node = node->next;
		}
		
		return sampleArr;
	}

	//-------------------------------------------------------------------

#endif
