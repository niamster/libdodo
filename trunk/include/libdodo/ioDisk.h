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
#include <libdodo/ioDiskTools.h>
#include <libdodo/ioDiskEx.h>
#include <libdodo/types.h>
#include <libdodo/io.h>

namespace dodo
{
	/**
	 * @enum ioDiskOperationTypeEnum describes type of operation for hook
	 */
	enum ioDiskOperationTypeEnum
	{
		IODISK_OPER_READ,
		IODISK_OPER_WRITE,
		IODISK_OPER_READSTREAM,
		IODISK_OPER_WRITESTREAM,
		IODISK_OPER_OPEN,
		IODISK_OPER_CLOSE
	};

	/**
	 * @enum ioDiskModesEnum descibes modes to open file
	 */
	enum ioDiskModesEnum
	{
		IODISK_OPENMODE_READ_ONLY,             ///< error if not exists file
		IODISK_OPENMODE_READ_WRITE,            ///< creates if not exists
		IODISK_OPENMODE_READ_WRITE_TRUNCATE,   ///< if exists=truncates
		IODISK_OPENMODE_APPEND                 ///< for readin'; writin' to the end; you may skip parameter `pos` for write method
	};

	/**
	 * @enum ioDiskFileToCreateEnum describes file type you can create
	 */
	enum ioDiskFileToCreateEnum
	{
		IODISK_FILETYPE_REG_FILE,  ///< regular file
		IODISK_FILETYPE_TMP_FILE,  ///< temporary file. will be deleted after you exit program(or close it)
		IODISK_FILETYPE_FIFO_FILE, ///< FIFO file
		IODISK_FILETYPE_CHAR_FILE  ///< CHAR file
	};

	/**
	 * @class ioDisk allows disk I/O manipulations
	 */

	class ioDisk : public io

	#ifndef IO_DISK_WO_XEXEC
					  , public xexec
	#endif

	{
			friend class ioSocket;

		private:

			/**
			 * cosrtructor prevents from copyin'
			 */
			ioDisk(ioDisk &fd);

		public:

			/**
			 * constructor
			 * @param path is path to the file
			 * @param type describes type of file with what manipulation will be made
			 * @param mode defines mode to open file 
			 * @note if type == TMP_FILE, u don't have to specify path
			 */
			ioDisk(const dodoString &path = __dodostring__, short fileType = IODISK_FILETYPE_REG_FILE, short mode = IODISK_OPENMODE_READ_WRITE);

			/**
			 * destructor
			 */
			virtual ~ioDisk();

			#ifndef IO_DISK_WO_XEXEC

			/**
			 * adds hook after the operation by callback
			 * @return number in list where function is set
			 * @param func is a pointer to function
			 * @param data is pointer to data toy want to pass to hook
			 */
			virtual int addPostExec(inExec func, void *data);

			/**
			 * adds hook before the operation by callback
			 * @return number in list where function is set
			 * @param func is a pointer to function
			 * @param data is pointer to data toy want to pass to hook
			 */
			virtual int addPreExec(inExec func, void *data);

				#ifdef DL_EXT

			/**
			 * set function from module that will be executed before/after the main action call
			 * the type of hook[pre/post] is defined in module
			 * @return number in list where function is set
			 * @param func is a pointer to function
			 * @param data is pointer to data toy want to pass to hook
			 * @param toInit indicates data that will path to initialize function
			 */
			virtual xexecCounts addExec(const dodoString &module, void *data, void *toInit = NULL);

			/**
			 * adds hook after the operation by callback
			 * @return number in list where function is set
			 * @param module is a path to module, whrere hook exists
			 * @param data is pointer to data toy want to pass to hook
			 * @param toInit indicates data that will path to initialize function
			 */
			virtual int addPostExec(const dodoString &module, void *data, void *toInit = NULL);

			/**
			 * adds hook after the operation by callback
			 * @return number in list where function is set
			 * @param module is a path to module, whrere hook exists
			 * @param data is pointer to data toy want to pass to hook
			 * @param toInit indicates data that will path to initialize function
			 */
			virtual int addPreExec(const dodoString &module, void *data, void *toInit = NULL);

				#endif

			#endif

			/**
			 * opens file
			 * @param path describes path to file
			 * @param mode defines mode to open file 
			 * @note closes previous opened file if needed
			 * if you want to create pipe, but not a pipe was created with the same name - false will be returned
			 * if you want to create regular file, but not regular file was created with the same name - false will be returned
			 */
			virtual void
			open(const dodoString &path, short fileType, short mode); 

			/**
			 * closes file
			 */
			virtual void
			close();

			/**
			 * read string
			 * @param data will be filled with data
			 * @param pos indicates position in file
			 */
			virtual void
			readString(dodoString &data, unsigned long pos = 0);
			/**
			 * read data
			 * @param data will be filled with data
			 * @param pos indicates position in file
			 */
			virtual void
			read(char * const data, unsigned long pos = 0);

			/**
			 * write string
			 * @param data will be written to file
			 * @param pos indicates position in file
			 */
			virtual void
			writeString(const dodoString &data, unsigned long pos = 0);

			/**
			 * write string
			 * @param data will be written to file
			 * @param pos indicates position in file
			 */
			virtual void
			write(const char * const data, unsigned long pos = 0);

			/**
			 * read string - null[or \n]-terminated string
			 * @param data will be filled with data
			 * @param pos indicates position in file [string that has pos-1 strings before]
			 * @note max size is inSize
			 */
			virtual void
			readStreamString(dodoString &data, unsigned long pos = 0);
			/**
			 * read data - null[or \n]-terminated string
			 * @param data will be filled with data
			 * @param pos indicates position in file [string that has pos-1 strings before]
			 * @note max size is inSize
			 */
			virtual void
			readStream(char * const data, unsigned long pos = 0);

			/**
			 * write string - null-terminated string [append only]
			 * @param data will be written to file
			 * @note max size is outSize
			 * @note - appends '\n'
			 */
			virtual void
			writeStreamString(const dodoString &data);

			/**
			 * write string - null-terminated string [append only]
			 * @param data will be written to file
			 * @note max size is outSize
			 * @note - appends '\n'
			 */
			virtual void
			writeStream(const char * const data);

			/**
			 * erase node on position
			 * @param pos indicates position in file
			 * @note for xexec  - no call for pre/postExec is performed, no operation type is set, 'cos it's only special type of write!!
			 */
			virtual void
			erase(unsigned long pos);
			
			/**
			 * flushes to disk
			 */
			virtual void
			flush();

			bool over;  ///< indicates whether overwrite or not; if you want to write to nonempty node error will be occured; for files, tmp_files only
			bool append;    ///< if true, will append to the end of the file, even pos is set.
			
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
			 * @return descriptor of input stream
			 */
			virtual int getInDescriptor() const;

			/**
			 * @return descriptor of output stream
			 */
			virtual int getOutDescriptor() const;

		private:

			dodoString path;    ///< file name
			short fileType; ///< type of file

			FILE *file;         ///< file handler
	};

};

#endif
