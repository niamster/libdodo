/***************************************************************************
 *            ioStdio.h
 *
 *  Tue Nov 15 21:19:57 2005
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

#ifndef _IOSTDIOEX_H_
#define _IOSTDIOEX_H_

#include <libdodo/directives.h>

#include <libdodo/baseEx.h>

namespace dodo
{
	namespace io
	{
		/**
		 * IDs of functions where exception might be thrown
		 */
		enum stdioFunctionsID
		{
			STDIOEX_CLOSE,
			STDIOEX_OPEN,
			STDIOEX__READ,
			STDIOEX__WRITE,
			STDIOEX__READSTREAM,
			STDIOEX_FLUSH,
			STDIOEX_LOCKOUT,
			STDIOEX_LOCKIN,
			STDIOEX_INPUTTERINFO,
			STDIOEX_BLOCK
		};
	};
};

#endif