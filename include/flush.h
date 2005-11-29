/***************************************************************************
 *            flush.h
 *
 *  Tue Oct 11 00:19:57 2005
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
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
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
	class flush : public xexec, public dodoBase
	{
		public:
					
			/**
			 * @return self, casted to base class - dodoBase; 
			 * usefull to cast from child to parent;
			 * used in hooks
			 */		
			virtual dodoBase * const getSelf();
							
			/**
			 * constructor
			 */
			flush();
			
			/**
			 * destructor
			 */
			virtual ~flush();
			
			mutable long inSize;///< size of data block;
			mutable long outSize;///< size of data block;
			
			/**
			 * for streams, if you set, for example, outSize 10, and try to write 2, you'll have 2 your bytes and 8 strange bytes,
			 * so if you want to autodetect this, set to true; 
			 * only for strings(c-strings; std::string);
			 */
			mutable bool autoOutSize;///< true by default
			
			mutable std::string buffer;///< before readin' or after writin' the storege sets to buffer; usefull for xexec

		protected:
		
			mutable bool opened;///< indicates whether file(connection) opened or not
	};
};

#endif /*FLUSH_H_*/
