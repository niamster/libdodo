/***************************************************************************
 *            toolsLogger.cc
 *
 *  Fri Mar 21 00:57:57 2008
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

#include <libdodo/toolsLogger.h>

using namespace dodo::tools;

const dodoString logger::levels[] = { "INFO",
									  "NOTICE",
									  "DEBUG",
									  "WARNING",
									  "ERROR",
									  "ALERT",
									  "CRITICAL",
									  "EMERGENCY",
									  "USER" };

//-------------------------------------------------------------------

const int logger::syslogLevels[] = { LOG_EMERG,
									 LOG_ALERT,
									 LOG_CRIT,
									 LOG_ERR,
									 LOG_WARNING,
									 LOG_NOTICE,
									 LOG_INFO,
									 LOG_DEBUG,
									 LOG_USER };

//-------------------------------------------------------------------

logger::logger() : handlersNum(0),
				   timeFormat(" %d/%m/%Y.%H-%M-%S: "),
				   forward(false)
{
}

//-------------------------------------------------------------------

logger::~logger()
{
}

//-------------------------------------------------------------------

unsigned long
logger::add(short level,
			io::channel *handler)
{
	protector tg(this);

	__logMap lm;

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
	protector tg(this);

	dodoList<__logMap>::iterator i(handlers.begin()), j(handlers.end());
	for (; i != j; ++i)
		if (i->position == position)
		{
			handlers.erase(i);

			break;
		}
}

//-------------------------------------------------------------------

void
logger::log(short level,
			const dodoString &msg)
{
	protector tg(this);

	if (level < 0 && level >= LOGGER_LEVELS)
		return;

	dodoList<__logMap>::iterator i(handlers.begin()), j(handlers.end());
	for (; i != j; ++i)
		if (i->level == level)
		{
			if (i->handler != NULL)
			{
				i->handler->writeStream(levels[level] + tools::time::byFormat(timeFormat, tools::time::now()) + msg + "\n");
				i->handler->flush();
			}
			else
				syslog(syslogLevels[level], msg.c_str());
		}

	if (forward)
	{
		i = getInstance().handlers.begin();
		j = getInstance().handlers.end();
		for (; i != j; ++i)
			if (i->level == level)
			{
				if (i->handler != NULL)
				{
					i->handler->writeStream(levels[level] + tools::time::byFormat(timeFormat, tools::time::now()) + msg + "\n");
					i->handler->flush();
				}
				else
					syslog(syslogLevels[level], msg.c_str());
			}
	}
}

//-------------------------------------------------------------------

void
logger::setTimeFormat(const dodoString &format)
{
	timeFormat = " " + format + ": ";
}

//-------------------------------------------------------------------
