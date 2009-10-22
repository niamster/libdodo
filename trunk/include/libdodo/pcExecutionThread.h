/***************************************************************************
 *            pcExecutionThread.h
 *
 *  Wed Oct 07 2009
 *  Copyright  2009  Ni@m
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

#ifndef _PCEXECUTIONTHREAD_H_
#define _PCEXECUTIONTHREAD_H_ 1

#include <libdodo/directives.h>

#include <libdodo/pcExecutionJob.h>
#include <libdodo/types.h>

namespace dodo {
    namespace pc {
        namespace execution {
            struct __thread__;

            /**
             * @class thread
             * @brief provides thread management functionality
             */
            class thread : public job {
              public:

                /**
                 * constructor
                 * @param func defines function to execute
                 * @param data defines process data
                 * @param action defines action on object destruction if thread is running, @see onDestructionEnum
                 * @param detached defines whether the thread will be detached
                 * @param stackSize defines stack size of the thread in bytes
                 */
                thread(routine       func,
                       void          *data,
                       short         action,
                       bool          detached = false,
                       unsigned long stackSize = 8388608);

#ifdef DL_EXT
                /**
                 * constructor
                 * @param module defines path to the library[if not in ldconfig db] or library name
                 * @param data defines thread data
                 * @param toInit defines library init data
                 */
                thread(const dodoString &module,
                       void             *data,
                       void             *toInit = NULL);
#endif

                /**
                 * copy constructor
                 */
                thread(const thread &);

                /**
                 * destructor
                 */
                virtual ~thread();

                /**
                 * execute thread
                 */
                virtual void run();

                /**
                 * stop thread
                 */
                virtual void stop();

                /**
                 * wait for thread termination
                 * @return status of the job
                 */
                virtual int wait();

                /**
                 * @return true if thread is running
                 */
                virtual bool isRunning() const;

#ifdef DL_EXT
                /**
                 * @struct __module__
                 * @brief defines data that is returned from initModule in the library
                 */
                struct __module__ {
                    char  name[64];         ///< name of module
                    char  discription[256]; ///< discription of module
                    char  hook[64];         ///< name of function in module that will be a hook
                    char  cookie[32];       ///< cookie that would be passed to deinitModule
                    bool  detached;         ///< true if thread is detached
                    int   stackSize;        ///< size of stack for thread in bytes
                    short action;           ///< action on object destruction, @see onDestructionEnum
                };

                /**
                 * @typedef initModule
                 * @brief defines type of init function for library
                 * @param data defines user data
                 * @note name in the library must be initPcExecutionThreadModule
                 */
                typedef __module__ (*initModule)(void *data);

                /**
                 * @typedef deinitModule
                 * @brief defines type of deinit function for library
                 * @param cookie defines cookie data returned from initModule
                 * @note name in the library must be deinitPcExecutionThreadModule
                 */
                typedef void (*deinitModule)(char cookie[32]);

                /**
                 * @return info about library
                 * @param module defines path to the library[if not in ldconfig db] or library name
                 * @param toInit defines library init data
                 */
                static __module__ module(const dodoString &module,
                                         void             *toInit = NULL);
#endif

              private:

                mutable __thread__ * handle;             ///< thread handle
            };
        };
    };
};
#endif
