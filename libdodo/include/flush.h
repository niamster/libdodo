/***************************************************************************
 *            flush.h
 *
 *  Tue Oct 11 00:19:57 2005
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
 
#ifndef _FLUSH_H_
#define _FLUSH_H_

#include <directives.h>

#include <xexec.h>
#include <types.h>

namespace dodo
{

	/**
	 * @class flush is a base class for I/O operations.
	 * all I/O operations are blockable => read/write inSize/outSize block.
	 */
	class flush
	{
		friend class flushNBA;
		
		public:
		
			/**
			 * constructor
			 */
			flush();
			
			/**
			 * destructor
			 */
			virtual ~flush();
										
			unsigned long inSize;///< size of data block;
			unsigned long outSize;///< size of data block;
			
			dodoString buffer;///< before readin' or after writin' the storege sets to buffer; usefull for xexec

		protected:
		
			bool opened;///< indicates whether file(connection) opened or not
			
			/**
			 * @return descriptor of input stream
			 */
			virtual int getInDescriptor() const = 0;
			
			/**
			 * @return descriptor of output stream
			 */ 
			virtual int getOutDescriptor() const = 0;
	};

};

#endif
