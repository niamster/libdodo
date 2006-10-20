/***************************************************************************
 *            cgiFast.cc
 *
 *  Sat Aug  5 03:37:19 2006
 *  Copyright  2006  Ni@m
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
 
#include <cgiFast.h>

#ifdef FCGI_EXT
	
	using namespace dodo;

	//-------------------------------------------------------------------
	
	#ifdef PTHREAD_EXT
	
		pthread_mutex_t cgiFast::accept = PTHREAD_MUTEX_INITIALIZER;
	
	#endif

	//-------------------------------------------------------------------
	
	void
	dummyStackThread(cgiFastSTD *data)
	{
	}
	
	//-------------------------------------------------------------------

	cgiProc cgiFast::cgiF = &dummyStackThread;
	
	//-------------------------------------------------------------------

	cgiFastSTD::cgiFastSTD(cgiFastSTD &cf)
	{
	}
	
	//-------------------------------------------------------------------
	
	cgiFastSTD::cgiFastSTD(FCGX_Request *a_request) : request(a_request)
	{
	}	
		
	//-------------------------------------------------------------------
	
	cgiFastSTD::~cgiFastSTD()
	{
	}
	
	//-------------------------------------------------------------------
	
	void 
	cgiFastSTD::flush()
	{
		FCGX_FFlush(request->out);
	}
	
	//-------------------------------------------------------------------
	
	#include <tools.h>
	
	void 
	cgiFastSTD::print(const std::string &buf)
	{
		FCGX_PutStr(buf.c_str(), buf.size(), request->out);
	}
	
	//-------------------------------------------------------------------
	
	void 
	cgiFastSTD::read(char *buf, 
					unsigned int size)
	{
		FCGX_GetStr(buf, size, request->in);
	}
	
	//-------------------------------------------------------------------
	
	char *
	cgiFastSTD::getenv(char *buf)
	{
		return FCGX_GetParam(buf, request->envp);
	}
	
	//-------------------------------------------------------------------
	
	cgiFast::cgiFast(cgiFast &cf)
	{
	}
	
	//-------------------------------------------------------------------
	
	#ifdef PTHREAD_EXT
	
		cgiFast::cgiFast(bool a_threading,
						unsigned int a_threadsNum) : threading(a_threading),
													threadsNum(a_threadsNum)
		{
			pthread_mutexattr_t attr;
			
			pthread_mutexattr_init(&attr);
			pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
			pthread_mutex_init(&accept,&attr);
			
			pthread_mutexattr_destroy(&attr);
			
			FCGX_Init();
		}
	
	#else

		cgiFast::cgiFast()
		{
			FCGX_Init();
		}
		
	#endif
	
	//-------------------------------------------------------------------
	
	cgiFast::~cgiFast()
	{
		FCGX_Finish();
	}
	
	//-------------------------------------------------------------------
	
	void 
	cgiFast::setCGIFunction(cgiProc func)
	{
		cgiF = func;
	}
	
	//-------------------------------------------------------------------

	#ifdef PTHREAD_EXT

		void *
		cgiFast::stackThread(void *data)
		{
			FCGX_Request request;
			FCGX_InitRequest(&request, 0, 0);
			
			cgiFastSTD cfSTD(&request);
			
			while(true)
			{
				pthread_mutex_lock(&accept);
				FCGX_Accept_r(&request);
				pthread_mutex_unlock(&accept);
				
				cgiF(&cfSTD);
				
				FCGX_Finish_r(&request);
			}
			
			return NULL;
		}

	#endif
	
	//-------------------------------------------------------------------
	
	void 
	cgiFast::listen()
	{
		#ifdef PTHREAD_EXT
			if (threading)
			{
				pthread_t *id = new pthread_t[threadsNum];
				
				unsigned int i = 0;
				
				for (;i<threadsNum;++i)
       				pthread_create(&id[i], NULL, stackThread, NULL);
				
				for (i=0;i<threadsNum;++i)
					pthread_join(id[i], NULL);
					
				delete [] id;
			}
			else
			{			
				FCGX_Request request;
				FCGX_InitRequest(&request, 0, 0);
				
				cgiFastSTD cfSTD(&request);
				
				while(true)
				{
					FCGX_Accept_r(&request);
					
					cgiF(&cfSTD);
					
					FCGX_Finish_r(&request);
				}
			}
		#else
	
			FCGX_Request request;
			FCGX_InitRequest(&request, 0, 0);
			
			cgiFastSTD cfSTD(&request);
			
			while(true)
			{
				FCGX_Accept_r(&request);
				
				cgiF(&cfSTD);
				
				FCGX_Finish_r(&request);
			}		
		
		#endif
	}
	
	//-------------------------------------------------------------------

#endif
