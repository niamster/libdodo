/***************************************************************************
 *            flushDisk.h
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
 
#ifndef _FLUSHDISK_H_
#define _FLUSHDISK_H_

#include <directives.h>


#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <tools.h>
#include <flushDiskTools.h>
#include <flushDiskEx.h>
#include <types.h>
#include <flush.h>

namespace dodo
{
	/**
	 * @enum flushDiskOperationTypeEnum describes type of operation for hook
	 */
	enum flushDiskOperationTypeEnum
	{
		FLUSHDISK_OPER_READ,
		FLUSHDISK_OPER_WRITE,
		FLUSHDISK_OPER_READSTREAM,
		FLUSHDISK_OPER_WRITESTREAM,
		FLUSHDISK_OPER_OPEN,
		FLUSHDISK_OPER_CLOSE
	};
	
	/**
	 * @enum flushDiskModesEnum descibes modes to open file
	 */
	enum flushDiskModesEnum
	{
		OPENMODE_READ_ONLY,///< error if not exists file
		OPENMODE_READ_WRITE,///< creates if not exists
		OPENMODE_READ_WRITE_TRUNCATE,///< if exists = truncates
		OPENMODE_APPEND///< for readin'; writin' to the end; you may skip parameter `pos` for write method
	};
	
	/**
	 * @enum flushDiskFileToCreateEnum describes file type you can create
	 */
	enum flushDiskFileToCreateEnum
	{
		FILETYPE_REG_FILE,///< regular file
		FILETYPE_TMP_FILE,///< temporary file. will be deleted after you exit program(or close it)
		FILETYPE_FIFO_FILE///< FIFO file
	};
	
	/**
	 * @class flushDisk allows disk I/O manipulations
	 */
	
	class flushDisk : public flush
	
	#ifndef FLUSH_DISK_WO_XEXEC
								, public xexec
	#endif
	
	{
		friend class flushSocket;
		
		private:
		
			/**
			 * cosrtructor prevents from copyin'
			 */
			flushDisk(flushDisk &fd);
		
		public:
							
			/**
			 * constructor
			 * @param type describes type of file with what manipulation will be made
			 * @param path is path to the file
			 * 
			 * if type == TMP_FILE, u don't have to specify path
			 */
			flushDisk(short type, const std::string &path = __string__);
			
			/**
			 * destructor
			 */
			virtual ~flushDisk();
						
			#ifndef FLUSH_DISK_WO_XEXEC
			
				/**
				 * adds hook after the operation by callback
				 * @return number in list where function is set
				 * @param func is a pointer to function
				 * @param data is pointer to data toy want to pass to hook
				 */			
				virtual int addPostExec(inExec func, void *data) const;
				
				/**
				 * adds hook before the operation by callback
				 * @return number in list where function is set
				 * @param func is a pointer to function
				 * @param data is pointer to data toy want to pass to hook
				 */
				virtual int addPreExec(inExec func, void *data) const;
				
				#ifdef DL_EXT
	
					/**
					 * set function from module that will be executed before/after the main action call
					 * the type of hook[pre/post] is defined in module
					 * @return number in list where function is set
					 * @param func is a pointer to function
					 * @param data is pointer to data toy want to pass to hook
					 * @param toInit indicates data that will path to initialize function
					 */			
					virtual xexecCounts addExec(const std::string &module, void *data, void *toInit = NULL) const;
								
					/**
					 * adds hook after the operation by callback
					 * @return number in list where function is set
					 * @param module is a path to module, whrere hook exists
					 * @param data is pointer to data toy want to pass to hook
					 * @param toInit indicates data that will path to initialize function
					 */
					virtual int addPostExec(const std::string &module, void *data, void *toInit = NULL) const;
					
					/**
					 * adds hook after the operation by callback
					 * @return number in list where function is set
					 * @param module is a path to module, whrere hook exists
					 * @param data is pointer to data toy want to pass to hook
					 * @param toInit indicates data that will path to initialize function
					 */
					virtual int addPreExec(const std::string &module, void *data, void *toInit = NULL) const;
					
				#endif	
			
			#endif
						
			/**
			 * opens file
			 * @param path describes path to file
			 * closes previous opened file if needed
			 * if u want to create pipe, but not a pipe was created with the same name - false will be returned
			 * if u want to create regular file, but not regular file was created with the same name - false will be returned
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif			 
							open(const std::string &path = __string__) const;///if opened previous file, closes it		
			
			/**
			 * closes file
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							close() const;
							
			/**
			 * read string
			 * @param data will be filled with data
			 * @param pos indicates position in file
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							readString(std::string &data, unsigned long pos = 0) const;
			/**
			 * read data
			 * @param data will be filled with data
			 * @param pos indicates position in file
			 */							
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							read(char * const data, unsigned long pos = 0) const;	
			
			/**
			 * write string
			 * @param data will be written to file
			 * @param pos indicates position in file
			 */	
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							writeString(const std::string &data, unsigned long pos = 0);
										
			/**
			 * write string
			 * @param data will be written to file
			 * @param pos indicates position in file
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							write(const char * const data, unsigned long pos = 0);
							
			/**
			 * read string - null[or \n]-terminated string
			 * @param data will be filled with data
			 * @param pos indicates position in file [string that has pos-1 strings before]
			 * @note max size is inSize
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							readStreamString(std::string &data, unsigned long pos = 0) const;
			/**
			 * read data - null[or \n]-terminated string
			 * @param data will be filled with data
			 * @param pos indicates position in file [string that has pos-1 strings before]
			 * @note max size is inSize
			 */							
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							readStream(char * const data, unsigned long pos = 0) const;
			
			/**
			 * write string - null-terminated string [append only]
			 * @param data will be written to file
			 * @note max size is outSize
			 * @note - appends '\n'
			 */	
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							writeStreamString(const std::string &data);
										
			/**
			 * write string - null-terminated string [append only]
			 * @param data will be written to file
			 * @note max size is outSize
			 * @note - appends '\n'
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							writeStream(const char * const data);
							
			/**
			 * erase node on position
			 * @param pos indicates position in file
			 * @note for xexec  - no call for pre/postExec is performed, no operation type is set, 'cos it's only special type of write!!
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							erase(unsigned long pos);
			/**
			 * flushes to disk
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							flush();
			
			bool over;///< indicates whether overright or not; if tou want to write to nonempty node error will be occured; for files,tmp_files only
			mutable short mode;///< mode to open file; if you change it then you have to reopen!
			
			/**
			 * @return path of the opened file
			 */			
			virtual std::string getPath() const;

			mutable short fileType;///< type of file; if you change then it you have to reopen!

			mutable bool append;///< if true, will append to the end of the file, even pos is set.
			
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
		
			mutable std::string path;///< file name
					
			mutable FILE *file;///< file handler
	};

};

#endif
