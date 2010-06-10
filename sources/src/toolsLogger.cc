/***************************************************************************
 *            toolsLogger.cc
 *
 *  Fri Mar 21 2008
 *  Copyright  2008  Ni@m
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

#include <libdodo/directives.h>

#include <syslog.h>

#include <libdodo/toolsLogger.h>

#include <libdodo/ioChannel.h>
#include <libdodo/toolsTime.h>
#include <libdodo/pcSyncProcess.h>
#include <libdodo/pcSyncStack.h>
#include <libdodo/types.h>

using namespace dodo::tools;

const dodo::string logger::levels[] = {
    "INFO",
    "NOTICE",
    "DEBUG",
    "WARNING",
    "ERROR",
    "ALERT",
    "CRITICAL",
    "EMERGENCY",
    "USER"
};

//-------------------------------------------------------------------

const int logger::syslogLevels[] = {
    LOG_EMERG,
    LOG_ALERT,
    LOG_CRIT,
    LOG_ERR,
    LOG_WARNING,
    LOG_NOTICE,
    LOG_INFO,
    LOG_DEBUG,
    LOG_USER
};

//-------------------------------------------------------------------

logger::logger() : forward(false),
                   timeFormat(" %d/%m/%Y.%H-%M-%S: "),
                   handlersNum(0),
                   keeper(new pc::sync::process(0))
{
}

//-------------------------------------------------------------------

logger::~logger()
{
    delete keeper;
}

//-------------------------------------------------------------------

unsigned long
logger::add(short       level,
            io::channel *handler)
{
    pc::sync::stack pg(keeper);

    __log_map__ lm;

    lm.handler = handler;
    lm.level = level;
    lm.position = ++handlersNum;

    handlers.push_back(lm);

    return handlersNum;
}

//-------------------------------------------------------------------

void
logger::remove(unsigned long position)
{
    pc::sync::stack pg(keeper);

    dodoList<__log_map__>::iterator i(handlers.begin()), j(handlers.end());
    for (; i != j; ++i) {
        if (i->position == position) {
            handlers.erase(i);

            break;
        }
    }
}

//-------------------------------------------------------------------

void
logger::log(short            level,
            const dodo::string &msg)
{
    pc::sync::stack pg(keeper);

    if (level < 0 && level >= LOG_LEVEL_ENUMSIZE)
        return;

    dodoList<__log_map__>::iterator i(handlers.begin()), j(handlers.end());
    for (; i != j; ++i) {
        if (i->level == level) {
            if (i->handler != NULL) {
                i->handler->writeString(levels[level] + tools::time::byFormat(timeFormat, tools::time::now()) + msg + "\n");
                i->handler->flush();
            } else
                syslog(syslogLevels[level], "%s", msg.data());
        }
    }

    if (forward) {
        i = instance().handlers.begin();
        j = instance().handlers.end();
        for (; i != j; ++i) {
            if (i->level == level) {
                if (i->handler != NULL) {
                    i->handler->writeString(levels[level] + tools::time::byFormat(timeFormat, tools::time::now()) + msg + "\n");
                    i->handler->flush();
                } else
                    syslog(syslogLevels[level], "%s", msg.data());
            }
        }
    }
}

//-------------------------------------------------------------------

void
logger::setTimeFormat(const dodo::string &format)
{
    timeFormat = " " + format + ": ";
}

//-------------------------------------------------------------------
