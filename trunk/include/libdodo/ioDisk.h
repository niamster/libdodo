/***************************************************************************
 *            ioDisk.h
 *
 *  Tue Oct 8 08:19:57 2005
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

#ifndef _IODISK_H_
#define _IODISK_H_

#include <libdodo/directives.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <libdodo/tools.h>
#include <libdodo/toolsFilesystem.h>
#include <libdodo/ioDiskEx.h>
#include <libdodo/types.h>
#include <libdodo/io.h>
#include <libdodo/systemThreadGuard.h>

namespace dodo
{
	/**
	 * @enum ioDiskOperationTypeEnum defines type of operation for hook
	 */
	enum ioDiskOperationTypeEnum
	{
		IODISK_OPERATION_READ,
		IODISK_OPERATION_READSTRING,
		IODISK_OPERATION_READSTREAM,
		IODISK_OPERATION_READSTREAMSTRING,
		IODISK_OPERATION_WRITE,
		IODISK_OPERATION_WRITESTRING,
		IODISK_OPERATION_WRITESTREAM,
		IODISK_OPERATION_WRITESTREAMSTRING,
		IODISK_OPERATION_OPEN,
		IODISK_OPERATION_CLOSE
	};

	/**
	 * @enum ioDiskModesEnum defines modes to open file
	 */
	enum ioDiskModesEnum
	{
		IODISK_OPENMODE_READ_ONLY,              ///< error if not exists file
		IODISK_OPENMODE_READ_WRITE,             ///< creates if not exists
		IODISK_OPENMODE_READ_WRITE_TRUNCATE,    ///< if exists=truncates
		IODISK_OPENMODE_APPEND                  ///< for readin'; writin' to the end; you may skip parameter `pos` for write method
	};

	/**
	 * @enum ioDiskFileToCreateEnum defines file type you can create
	 */
	enum ioDiskFileToCreateEnum
	{
		IODISK_FILETYPE_REG_FILE,   ///< regular file
		IODISK_FILETYPE_TMP_FILE,   ///< temporary file[will be deleted after exit(or close)]
		IODISK_FILETYPE_FIFO_FILE,  ///< FIFO file
		IODISK_FILETYPE_CHAR_FILE   ///< CHAR file
	};

	/**
	 * @class ioDisk provides disk I/O manipulations
	 */

	class ioDisk : public io,
				   virtual public systemThreadGuardHolder
	{
			friend class ioNetwork;

		private:

			/**
			 * copy constructor
			 * to prevent copying
			 */
			ioDisk(ioDisk &fd);

		public:

			/**
			 * constructor
			 * @param path defines path to the file
			 * @param fileType defines type of file[see ioDiskFileToCreateEnum]
			 * @param mode defines mode to open file[see ioDiskModesEnum]
			 * @note if type is TMP_FILE path is ignored
			 */
			ioDisk(const dodoString &path = __dodostring__, short fileType = IODISK_FILETYPE_REG_FILE, short mode = IODISK_OPENMODE_READ_WRITE);

			/**
			 * destructor
			 */
			virtual ~ioDisk();

#ifndef IODISK_WO_XEXEC

			/**
			 * add hook after the operation
			 * @return id of the hook method
			 * @param func defines hook function
			 * @param data defines data that will be passed to hook function
			 */
			virtual int addPostExec(inExec func, void *data);

			/**
			 * add hook before the operation
			 * @return id of the hook method
			 * @param func defines hook function
			 * @param data defines data that will be passed to hook function
			 */
			virtual int addPreExec(inExec func, void *data);

#ifdef DL_EXT

			/**
			 * add hook after the operation
			 * @return id of the hook method
			 * @param path defines path to the library[if not in ldconfig db] or library name
			 * @param data defines data that will be passed to hook function
			 * @param toInit defines data that will be passed to the init function
			 */
			virtual int addPostExec(const dodoString &path, void *data, void *toInit = NULL);

			/**
			 * add hook after the operation
			 * @return id of the hook method
			 * @param path defines path to the library[if not in ldconfig db] or library name
			 * @param data defines data that will be passed to hook function
			 * @param toInit defines data that will be passed to the init function
			 */
			virtual int addPreExec(const dodoString &path, void *data, void *toInit = NULL);

			/**
			 * set hook from the library that will be executed before/after the operation
			 * @return id of the hook method
			 * @param path defines path to the library[if not in ldconfig db] or library name
			 * @param data defines data that will be passed to hook function
			 * @param toInit defines data that will be passed to the init function
			 * @note type of hook[pre/post] is defined in the library
			 */
			virtual __xexecCounts addExec(const dodoString &path, void *data, void *toInit = NULL);

#endif

#endif

			/**
			 * open file
			 * @param path defines path to the file
			 * @param fileType defines type of file[see ioDiskFileToCreateEnum]
			 * @param mode defines mode to open file[see ioDiskModesEnum]
			 * @note if type is TMP_FILE path is ignored
			 */
			virtual void open(const dodoString &path, short fileType, short mode);
			
			/**
			 * close file
			 */
			virtual void close();

			/**
			 * @param data defines buffer that will be filled
			 * @note not more then inSize(including '\0')
			 */
			virtual void readString(dodoString &data);

			/**
			 * @param data defines buffer that will be filled
			 * @note not more then inSize(including '\0')
			 */
			virtual void read(char * const data);

			/**
			 * @param data defines data that will be written
			 */
			virtual void writeString(const dodoString &data);

			/**
			 * @param data defines data that will be written
			 */
			virtual void write(const char * const data);

			/**
			 * read from stream - '\0' or '\n' - terminated string
			 * @param data defines buffer that will be filled
			 * @note not more then inSize(including '\0')
			 */
			virtual void readStreamString(dodoString &data);

			/**
			 * read from stream - '\0' or '\n' - terminated string
			 * @param data defines buffer that will be filled
			 * @note not more then inSize(including '\0')
			 */
			virtual void readStream(char * const data);

			/**
			 * write to stream - '\0' - terminated string
			 * @param data defines data that will be written
			 */
			virtual void writeStreamString(const dodoString &data);

			/**
			 * write to stream - '\0' - terminated string
			 * @param data defines data that will be written
			 */
			virtual void writeStream(const char * const data);

			/**
			 * flush output
			 */
			virtual void flush();

			/**
			 * erase node on position
			 */
			virtual void erase();
			
			unsigned long pos;///< read/write/erase position[0 by default]

			bool over;      ///< if true block will be overwritten; for regular and temp files only
			bool append;    ///< if true, will append to the end of the file, even if pos is set

			/**
			 * @return path of the opened file
			 */
			virtual dodoString getPath() const;

			/**
			 * @return type of the opened file
			 */
			virtual short getFileType() const;

		protected:

			/**
			 * @return descriptor of the input stream
			 */
			virtual int getInDescriptor() const;

			/**
			 * @return descriptor of the output stream
			 */
			virtual int getOutDescriptor() const;

			/**
			 * @param data defines buffer that will be filled
			 * @note not more then inSize(including '\0')
			 */
			virtual void _read(char * const data);

			/**
			 * read from stream - '\0' or '\n' - terminated string
			 * @param data defines buffer that will be filled
			 * @note not more then inSize(including '\0')
			 */
			virtual void _readStream(char * const data);

			/**
			 * @param data defines data that will be written
			 */
			virtual void _write(const char * const data);

			/**
			 * write to stream - '\0' - terminated string
			 * @param data defines data that will be written
			 */
			virtual void _writeStream(const char * const data);

		private:

			dodoString path;    ///< file path
			short fileType;     ///< type of file

			FILE *file;         ///< file handler
	};

};

#endif
