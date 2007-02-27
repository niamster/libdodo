/***************************************************************************
 *            systemProcesses.h
 *
 *  Tue Feb 27 08:38:55 2007
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
 
#ifndef _SYSTEMPROCESSES_H_
#define _SYSTEMPROCESSES_H_

#include <directives.h>
#include <types.h>
	
	namespace dodo
	{
	
		/**
		 * @typedef describes function to be passed as process
		 */
		typedef void *(*processFunc)(void *);

		/*
		 * @enum processTypeEnum describes type of process
		 */
		enum processTypeEnum
		{
			PROCESS_FORK,///< standart fork calling
			PROCESS_VFORK///< don't copys parent's page tables
		};

		/**
		 * @struct __processInfo describes process
		 */
		struct __processInfo
		{
			/**
			 * constuctor
			 */
			__processInfo();

			void *data;///< data that will be passed on run
                        bool isRunning;///< whether process is running
			short type;///< type of process creation[see processTypeEnum]
                        unsigned long position;///< position in queue
                        processFunc func;///< function to execute
                        short action;///< action on class destruction[see systemThreadOnDestructEnum]
                        unsigned long executed;///< amount of times thread was executed
                        unsigned long executeLimit;///< if more than one will be autodleted or with `sweepTrash` method; de
		};

		class systemProcesses
		{
			private:

				/**
				 * copy constructor
				 * to prevent copying
				 */
				systemProcesses(systemProcesses &sp);

			public:
				
				/**
				 * constructor
				 */
				systemProcess();

				/**
				 * destructor
				 */
				virtual ~systemProcess();
				
			protected:
				
				unsigned long processNum;///< number of registered processes
		};

	};

#endif
