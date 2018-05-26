/* Licensed to the Apache Software Foundation (ASF) under one or more
* contributor license agreements.  See the NOTICE file distributed with
* this work for additional information regarding copyright ownership.
* The ASF licenses this file to You under the Apache License, Version 2.0
* (the "License"); you may not use this file except in compliance with
* the License.  You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/


/*************************************************************************
 * File: deamonutils.cpp
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#include "deamonutils.h"
#include <sys/stat.h>

NS(NBPy)

    int CDeamonUtils::Begin()
    {
        int fds[2];
        if( socketpair(AF_UNIX,SOCK_STREAM,0,fds) == -1 )
        {
            perror( "socketpair()" );
            exit(-10);
        }

        pid_t pid, sid;
        /* Fork off the parent process */
        pid = fork();
        if (pid < 0) {
            exit(EXIT_FAILURE);
        }
        /* If we got a good PID, then we can exit the parent process. */
        if(pid > 0)
        {
            close(fds[1]);
            deamon_info ret;
            ssize_t r = read( fds[0], &ret, sizeof(ret) );
            if( r == sizeof(ret) )
            {
                printf(" >>>> daemon start err:%d  pid:%ld\n",ret.err,long( ret.pid) );
                exit(ret.err);
            } else { exit(-100); }
        }

        close(fds[0]);

        /* Change the file mode mask */
        umask(0177);
        /* Create a new SID for the child process */
        sid = setsid();
        if (sid < 0) {
            /* Log any failures here */
            exit(EXIT_FAILURE);
        }
        /* Close out the standard file descriptors */
        for( int fd = 0; fd < 3; fd ++ ){
            close(fd);
        }
        /* Change the current working directory */
        if ((chdir("./")) < 0) {
            exit(EXIT_FAILURE);
        }
        return fds[1];
    }

    void CDeamonUtils::Complete(int fd, int err)
    {
        if(fd <= 0)
            return;
        deamon_info ret;
        ret.err = err;
        ret.pid = getpid();
        write(fd, &ret, sizeof(ret) );
    }

    bool CPIDFileUtils::Generate(const char* szFilename)
    {
        int fd = ::open(szFilename,O_CREAT|O_APPEND|O_WRONLY,S_IWUSR|S_IRUSR|S_IROTH);
        if( !fd ) return false;

        pid_t pid = getpid();
        char buf[64] = {0};
        sprintf(buf,"%ld\n", (long)pid);

        ssize_t nWritten = -1;
        do
        {
            nWritten = write(fd,buf,strlen(buf));
        }while( (nWritten < 0) && (errno != EAGAIN && errno != EINTR) );

        close(fd);
        return (nWritten >= 0);
    }

    void CPIDFileUtils::Remove(const char* szFilename)
    {
        pid_t pid           = getpid();
        char buf[64]        = {0};
        char bufFile[1024]  = {0};

        int fd              = open(szFilename,O_RDONLY,S_IWUSR|S_IRUSR|S_IROTH);
        if( !fd )   return;
        ssize_t nRead = read( fd,bufFile,1024 );
        close(fd);

        //
        if( nRead < 1 )     return ;

		sprintf(buf, "%ld\n", (long)pid);
		if(strcmp(bufFile, buf) == 0)
		{
			::remove( szFilename );
			return;
		}

		fd = open(szFilename,O_WRONLY|O_TRUNC,S_IWUSR|S_IRUSR|S_IROTH);
		if( fd < 0 )    {
			return;
		}

		memset(buf, 0, 64);
		char *dst = buf;
		char *src = bufFile;
		char *end = src + nRead;
		size_t st = 0;
		while(src < end)
		{
			if(*src == '\n'){
				*dst = 0;
				if(atol(buf) == pid)
				{
					//ingore;
				}else{
					*dst = '\n';
					write(fd,buf,st + 1);
				}
				dst = buf;
				st = 0;
			}else{
				*dst = *src;
				dst ++;
				st ++;
			}
			src ++;
		}


        /*
        size_t last = 0;
        char* pData = bufFile;
        while( sscanf(pData,"%[^\n]",buf) && last <= nRead )
        {
            size_t len = strlen(buf);
            if( atol(buf) == pid )
            {
                ssize_t left = nRead - len - 1;
                if( left < 1 )
                {
                    ::remove( szFilename );
                    return;
                }
                fd = open(szFilename,O_WRONLY|O_TRUNC,S_IWUSR|S_IRUSR|S_IROTH);
                if( fd < 0 )    {
                    return;
                }
                if( last > 0 ) {
                    write(fd,bufFile,last + 1);
                }
                if( (left - last) > 0 ) {
                    write(fd,bufFile+last+len,left - last);
                }
                close(fd);
                return;
            }
            buf[0]   = 0;
            last    += len + 1;
            pData   += len + 1;
        }
        */
    }

NSE
