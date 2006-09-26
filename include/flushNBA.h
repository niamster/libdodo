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

#include <flushNBAEx.h>
#include <types.h>
#include <flush.h>

#include <poll.h>

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
	 		
	 		/**
	 		 * @param fl is identificator of stream to remove
	 		 */
	 		virtual void delFlush(int pos);
	 		
	 		/**
	 		 * @return true if flush* is ready to read
	 		 * @param pos is identificator of stream
	 		 * @param timeout describes amount of time to wait for result[in milliseconds]
	 		 * @note if timeout is negative - infinit timeout
	 		 */
	 		virtual bool isReadable(int pos, int timeout = 100);
	 			 		
	 		/**
	 		 * @return vector of true if flush* is ready to read
	 		 * @param pos is vector of identificators of streams
	 		 * @param timeout describes amount of time to wait for result[in milliseconds]
	 		 * @note if timeout is negative - infinit timeout
	 		 */
	 		virtual std::vector<bool> isReadable(const std::vector<int> &pos, int timeout = 100);	 		
	 		
	 		/**
	 		 * @return true if flush* is ready to write
	 		 * @param pos is identificator of stream
	 		 * @param timeout describes amount of time to wait for result[in milliseconds]
	 		 * @note if timeout is negative - infinit timeout
	 		 */
	 		virtual bool isWritable(int pos, int timeout = 100);
	 		
		protected:
		
			virtual void makeFalse(int count);
	 	
	 		std::vector<__inOutDescriptors> desc;///< flush descriptors
	 	
	 		int descs;///< descriptors counter
	 		
	 		__inOutDescriptors tempD;///< temp storage for descriptors
	 	
	 		std::vector<__inOutDescriptors>::iterator i;///< iterator for desc
	 		std::vector<__inOutDescriptors>::iterator j;///< iterator for desc
	 		
	 		std::vector<int>::const_iterator m;///< iterator for positions
	 		std::vector<int>::const_iterator n;///< iterator for positions
	 		
	 		std::vector<bool> tempRB;///< temporary to save results of avalaibility
	 		 
	 		pollfd fd;///< event capture for stream
	 		pollfd *fds;///< event capture for streams
	 };

};

#endif
