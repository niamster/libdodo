/***************************************************************************
 *            systemProcessSharedDataCollectionGuard.h
 *
 *  Sat Oct 20 12:20:55 2007
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

#ifndef _SYSTEMPROCESSSHAREDDATACOLLECTIONGUARD_H_
#define _SYSTEMPROCESSSHAREDDATACOLLECTIONGUARD_H_

#include <pthread.h>
#include <time.h>

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/systemSharedDataCollectionGuard.h>
#include <libdodo/systemProcessSharedDataCollectionGuardEx.h>
#include <libdodo/processGuard.h>

namespace dodo
{
	/**
	 * @class systemProcessSharedDataCollectionGuard is to manage data between threads(based on POSIX threads)
	 */
	class systemProcessSharedDataCollectionGuard : public systemSharedDataCollectionGuard,
												   virtual public processGuardHolder
	{
		private:

			/**
			 * copy constructor
			 * to prevent copying
			 */
			systemProcessSharedDataCollectionGuard(systemProcessSharedDataCollectionGuard &sts);

		public:

			/**
			 * constructor
			 */
			systemProcessSharedDataCollectionGuard();

			/**
			 * destructor
			 */
			virtual ~systemProcessSharedDataCollectionGuard();

			/**
			 * adds data to became a shared
			 * @return position of shared in queue
			 * @param data describes data to be shared
			 */
			virtual unsigned long add(void *data);

			/**
			 * sets shared data to NULL
			 * @param position indicates on shared data to lock
			 */
			virtual void del(unsigned long position);

			/**
			 * locks, sets data, unlocks
			 * @param position indicates on shared data to lock
			 * @param data describes data to be set
			 */
			virtual void set(unsigned long position, void *data);

			/**
			 * locks, gets data, unlocks
			 * @return data points on shared data or NULL in error case
			 * @param position indicates on shared data to lock
			 */
			virtual const void *get(unsigned long position);
			
			/**
			 * @return list of ids of shared data in object
			 */
			virtual dodoList<unsigned long> getSharedDataIds();

		protected:

			/**
			 * searches shares by position
			 * @return true if found
			 * @param position describes position of wanted thread
			 * @note sets internal parameter 'current' to found share
			 */
			virtual bool getShare(unsigned long position);

			dodoList<__shareInfo> shares;              ///< list of shared data

			unsigned long shareNum;                     ///< number of registered shares

			dodoList<__shareInfo>::iterator current;   ///< iterator for list of shares[for matched with getShare method]
	};

};

#endif

