/***************************************************************************
 *            file.h
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

#ifndef _IOFILE_H_
#define _IOFILE_H_

#include <libdodo/directives.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <libdodo/toolsMisc.h>
#include <libdodo/toolsFilesystem.h>
#include <libdodo/ioFileEx.h>
#include <libdodo/types.h>
#include <libdodo/ioChannel.h>
#include <libdodo/ipcThreadGuard.h>

namespace dodo
{
	namespace io
	{

#ifndef IOFILE_WO_XEXEC

		/**
		 * @enum fileOperationTypeEnum defines type of operation for hook
		 */
		enum fileOperationTypeEnum
		{
			FILE_OPERATION_READ,
			FILE_OPERATION_READSTRING,
			FILE_OPERATION_READSTREAM,
			FILE_OPERATION_READSTREAMSTRING,
			FILE_OPERATION_WRITE,
			FILE_OPERATION_WRITESTRING,
			FILE_OPERATION_WRITESTREAM,
			FILE_OPERATION_WRITESTREAMSTRING,
			FILE_OPERATION_OPEN,
			FILE_OPERATION_CLOSE
		};

#endif

		/**
		 * @enum fileOpenmodeEnum defines modes to open file
		 */
		enum fileOpenmodeEnum
		{
			FILE_OPENMODE_READ_ONLY,                ///< error if not exists file
			FILE_OPENMODE_READ_WRITE,               ///< creates if not exists
			FILE_OPENMODE_READ_WRITE_TRUNCATE,      ///< if exists=truncates
			FILE_OPENMODE_APPEND                    ///< for readin'; writin' to the end; you may skip parameter `pos` for write method
		};

		/**
		 * @enum fileFiletypeEnum defines file type you can create
		 */
		enum fileFiletypeEnum
		{
			FILE_FILETYPE_REG_FILE,         ///< regular file
			FILE_FILETYPE_TMP_FILE,         ///< temporary file[will be deleted after exit(or close)]
			FILE_FILETYPE_FIFO_FILE,        ///< FIFO file
			FILE_FILETYPE_CHAR_FILE         ///< CHAR file
		};

#ifndef IOFILE_WO_XEXEC

		/**
		 * @struct __xexexIoFileCollectedData defines data that could be retrieved from io::file(to modificate)[contains references]
		 */
		struct __xexexIoFileCollectedData
		{
			/**
			 * constructor
			 * @param operType defines xexec operation
			 * @param executor defines class that executed hook
			 */
			__xexexIoFileCollectedData(int &operType, void *executor);

			dodoString buffer;              ///< data buffer

			int &operType;                  ///< xexec operation

			void *executor;                 ///< class that executed hook
		};

#endif

		/**
		 * @class file provides disk I/O manipulations
		 */
		class file : public channel,
			     virtual public ipc::thread::guardHolder
		{
			private:

				/**
				 * copy constructor
				 * to prevent copying
				 */
				file(file &fd);

			public:

				/**
				 * constructor
				 * @param path defines path to the file
				 * @param fileType defines type of file[see fileFiletypeEnum]
				 * @param mode defines mode to open file[see fileOpenmodeEnum]
				 * @note if type is TMP_FILE path is ignored
				 */
				file(const dodoString &path = __dodostring__, short fileType = FILE_FILETYPE_REG_FILE, short mode = FILE_OPENMODE_READ_WRITE);

				/**
				 * destructor
				 */
				virtual ~file();

				/**
				 * open file
				 * @param path defines path to the file
				 * @param fileType defines type of file[see fileFiletypeEnum]
				 * @param mode defines mode to open file[see fileOpenmodeEnum]
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

				unsigned long pos;      ///< read/write/erase position[0 by default]

				bool over;              ///< if true block will be overwritten; for regular and temp files only
				bool append;            ///< if true, will append to the end of the file, even if pos is set

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

				dodoString path;        ///< file path
				short fileType;         ///< type of file

				FILE *handler;          ///< file handler

#ifndef IOFILE_WO_XEXEC

				__xexexIoFileCollectedData collectedData;   ///< data collected for xexec

#endif
		};
	};
};

#endif
