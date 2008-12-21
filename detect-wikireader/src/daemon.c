/*
 * (C) Copyright 2008 OpenMoko, Inc.
 * Author: xiangfu liu <xiangfu@openmoko.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include "daemon.h"

#define RUNNING_DIR "/"
#define LOCK_FILE_NAME ".det-wikird.lock"
static char* lock_file;

int check_lock_file()
{
	int lfp;
	char* home = getenv("HOME");

	lock_file = (char *)malloc(sizeof(char) * ( strlen(home) + strlen(LOCK_FILE_NAME)));
	strcpy(lock_file, home);
	strcat(lock_file, LOCK_FILE_NAME);

	lfp = open(lock_file, O_RDWR|O_CREAT, 
		   S_IRUSR | S_IWUSR |
		   S_IRGRP | S_IROTH);
	if (lfp < 0) {
		syslog(LOG_INFO, "can not open lock file."); 
		return -1;
	}
	if (lockf(lfp, F_TLOCK, 0) < 0) {
		syslog(LOG_INFO, "can not lock."); 
		return -1;
	}

	char str[10];
	sprintf(str, "%d\n", getpid());	/* first instance continues */
	write(lfp, str, strlen(str));		 /* record pid to lockfile */

	return 0;
}

int daemon_init(void) 
{
	if (getppid() == 1) 
		return -1; /* already a daemon */

        /* Our process ID and Session ID */
        pid_t pid, sid;
        
        /* Fork off the parent process */
        pid = fork();
        if (pid < 0) {
                exit(EXIT_FAILURE);
        }
        /* If we got a good PID, then
           we can exit the parent process. */
        if (pid > 0) {
                exit(EXIT_SUCCESS);
        }

        /* Change the file mode mask */
        umask(0);
                
        /* Open any logs here */        
                
        /* Create a new SID for the child process */
        sid = setsid();
        if (sid < 0) {
                /* Log the failure */
                exit(EXIT_FAILURE);
        }
        
        /* Change the current working directory */
        if ((chdir(RUNNING_DIR)) < 0) {
                /* Log the failure */
                exit(EXIT_FAILURE);
        }
        
	if ((check_lock_file()) < 0) {
		/* lock file file failure */
		exit(EXIT_FAILURE);
	}

	/* close all descriptors */
	int desc;
	for (desc = getdtablesize(); desc >= 0; --desc) 
		close(desc);

	return 0;
} 
