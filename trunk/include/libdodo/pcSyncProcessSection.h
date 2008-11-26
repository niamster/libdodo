/***************************************************************************
 *            pcSyncProcessSection.h
 *
 *  Sat Oct 20 02:00:55 2007
 *  Copyright  2007  Ni@m
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#ifndef _PCSYNCPROCESSSECTION_H_
#define _PCSYNCPROCESSSECTION_H_ 1

#include <libdodo/directives.h>

#include <fcntl.h>
#include <semaphore.h>

#include <libdodo/pcSyncSection.h>
#include <libdodo/toolsMisc.h>
#include <libdodo/toolsCode.h>
#include <libdodo/pcSyncProcessSectionEx.h>
#include <libdodo/types.h>

namespace dodo
{
	namespace pc
	{
		namespace sync
		{
			namespace process
			{
				/**
				 * @class section
				 * @brief performs atomic locks using semaphores
				 */
				class section : public sync::section
				{
					public:

						/**
						 * consructor
						 * @param value defines initial value of the section lock semaphore
						 * @param key defines shared memory key
						 * @note if key is empty - key will be generated
						 * if you want to share semaphore between different instances of process - set own key,
						 * otherwise(like fork) - it may be generated
						 * '/' automaticaly prepending to the key to conform sem_overview(7) man page
						 */
						section(unsigned int value = 1, const dodoString &key = __dodostring__);

						/**
						 * destructor
						 */
						virtual ~section();

						/**
						 * lock critical section
						 */
						virtual void acquire();

						/**
						 * unlock critical section
						 */
						virtual void release();

					protected:

						sem_t *keeper;                                                      ///< section
						dodoString key;                                                          ///< key for the section
				};
			};
		};
	};
};

#endif
