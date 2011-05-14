/***************************************************************************
 *            pcExecutionWorkqueue.h
 *
 *  Sun Jul 25 2010
 *  Copyright  2010  Dmytro Milinevskyy
 *  milinevskyy@gmail.com
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

#ifndef _PCEXECUTIONWORKQUEUE_H_
#define _PCEXECUTIONWORKQUEUE_H_ 1

#include <libdodo/directives.h>

#include <libdodo/pcExecutionJob.h>
#include <libdodo/types.h>

namespace dodo {
    namespace pc {
        namespace sync {
            class thread;
        };

        namespace notification {
            class thread;
        };

        namespace execution {
            class thread;

            /**
             * @class workqueue
             * @brief provides workqueue/thread pool
             */
            class workqueue {
              public:

                /**
                 * constructor
                 * @param maxThreads defines max amount of active threads
                 * @param minThreads defines min amount of active threads
                 * @param minIdleTime defines amount of time(in seconds) thread does nothing if amount of threads greater than minThreads
                 */
                workqueue(unsigned int maxThreads,
                        unsigned int minThreads = 1,
                        unsigned long minIdleTime = 60);

                /**
                 * destructor
                 */
                ~workqueue();

                /**
                 * add a routine for execution
                 * @param routine defines routine for execution
                 * @param data defines data passed to the routine
                 */
                void add(routine routine,
                        void *data);

                /**
                 * workqueue worker routine
                 * @return execution status
                 * @param data defines user data
                 */
                static int worker(workqueue *queue);

              protected:

                struct __work__;

                dodo::dlList<__work__ *> tasks;  ///< pending work

                dodo::dlList<execution::thread *> active;  ///< active thread
                dodo::dlList<execution::thread *> inactive;  ///< inactive thread

                unsigned int maxThreads; ///< max amount of active threads
                unsigned int minThreads; ///< min amount of active threads
                unsigned long minIdleTime; ///< amount of time(in seconds) thread does nothing if amount of threads greater than minThreads

                sync::thread *tasksProtector; ///< tasks queue protector
                sync::thread *threadsProtector; ///< threads queues protector

                notification::thread *notification; ///< thread wake handler

                bool closing;                   ///< closing flag
            };
        };
    };
};

#endif
