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
	  
	xmlTools::xmlTools() : parceLevel(0)
	{
	}
	 
	//-------------------------------------------------------------------
	 
	xmlTools::~xmlTools()
	{
	}
	 
	//-------------------------------------------------------------------
	
	__node 
	xmlTools::parceFile(const __nodeDef &definition, 
						const std::string &file, 
						long chLimit)
	{
		document = xmlParseFile(file.c_str());
		if (document == NULL)
			#ifndef NO_EX
				error = xmlGetLastError();
				throw baseEx(ERRMODULE_LIBXML2,XMLTOOLS_PARCEFILE,ERR_LIBXML2,error->code,error->message,__LINE__,__FILE__);
			#else
				return __node;
			#endif
		
		return parce(definition, chLimit);
	}

	//-------------------------------------------------------------------

	__node 
	xmlTools::parce(const __nodeDef &definition, 
						long chLimit)
	{
		xmlNodePtr chNode = xmlDocGetRootElement(document);
		if (chNode == NULL)
			#ifndef NO_EX
				error = xmlGetLastError();
				throw baseEx(ERRMODULE_LIBXML2,XMLTOOLS_PARCE,ERR_LIBXML2,error->code,error->message,__LINE__,__FILE__);
			#else
				return __node;
			#endif
		
		return parce(definition, chNode, chLimit);
	}

	//-------------------------------------------------------------------

	__node 
	xmlTools::parce(const __nodeDef &definition, 
					xmlNodePtr chNode, 
					long chLimit)
	{
		__node sample;
		
		
		return sample;
	}

	//-------------------------------------------------------------------

#endif
