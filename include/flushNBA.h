/***************************************************************************
 *            flushNBA.h
 *
 *  Thu Sep 09 03:21:24 2006
 *  Copyright  2006  Ni@m
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


#ifndef _FLUSHNBA_H_
#define _FLUSHNBA_H_

#include <directives.h>

#include <types.h>
#include <flush.h>

namespace dodo
{ 	
	
	/**
	 * @struct __inOutDescriptors contains in and descriptors of the flush* classes
	 */
	struct __inOutDescriptors
	{
		int position;///< identificator of pair
		
		int in;///< in stream
		int out;///< out stream
	};
	
	/**
	 * @class flushNBA
	 */
	 class flushNBA
	 {
	 	
	 	private:
						 	
	 		/**
	 		 * copy constructor
			 * to prevent copying
	 		 */
	 		flushNBA(flushNBA &rt);
	 			 	
	 	public:
						 	
	 		/**
	 		 * constructor
	 		 */
	 		flushNBA();
	 		
	 		/**
	 		 * destructor
	 		 */
	 		virtual ~flushNBA();
	 		
	 		/**
	 		 * @return identificator of added stream
	 		 * @param fl is a reference on stream[flushDisk, flushSTD, flushSocket, flushSocketExchange]
	 		 */
	 		virtual int addFlush(const flush &fl);
	 	
	 	protected:
	 	
	 		std::vector<__inOutDescriptors> desc;///< flush descriptors
	 	
	 		int descs;///< descriptors counter
	 		
	 		__inOutDescriptors temp;///< temp storage for descriptors
	 };

};

#endif
