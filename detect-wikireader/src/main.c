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
#include <getopt.h>
#include "wiki-inotify.h"

#define RUNNING_DIR "/"
#define LOCK_FILE "/var/run/detect-wikireader.lock"
#define LOG_FILE "/var/log/detect-wikireader.log" 

int log_message(char *filename, char*message)
{
	FILE *logfile;
	logfile=fopen(filename,"a");
	if (!logfile) 
		return -1;
	fprintf(logfile,"%s\n",message);
	fclose(logfile);
	return 0;
} 

void signal_handler(int signo)
{
	switch(signo) {
	case SIGHUP:
		signal(SIGHUP,signal_handler);
		syslog(LOG_INFO,"hangup signal catched");
		break;
	case SIGTERM:
		/* catched signal sent by kill(1) command */ 
		syslog(LOG_INFO, "terminate signal catched");
		closelog(); 
		close_monitor();
		exit(EXIT_SUCCESS); 
		break;
	default:
		break;
	}
}

int daemon_init(void) 
{
	char str[10];

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
        
	int lfp;
	lfp = open(LOCK_FILE, O_RDWR|O_CREAT, 
		   S_IRUSR | S_IWUSR |
		   S_IRGRP | S_IROTH);
	if (lfp < 0) {
		syslog(LOG_INFO, "can not open lock file."); 
		exit(EXIT_FAILURE);
	}
	if (lockf(lfp, F_TLOCK, 0) < 0) {
		syslog(LOG_INFO, "can not lock."); 
		exit(EXIT_FAILURE); 
	}

	sprintf(str, "%d\n", getpid());	/* first instance continues */
	write(lfp, str, strlen(str));		 /* record pid to lockfile */

	/* close all descriptors */
	int desc;
	for (desc = getdtablesize(); desc >= 0; --desc) 
		close(desc);

	return 0;
} 

static void help(void)
{
	printf("Usage: detect-wikireader [options] ...\n"
		"  -h --help\t\t\tPrint this help message\n"
		"  -V --version\t\t\tPrint the version number\n"
		);
}
static void print_version(void)
{
	/* printf("detect-wikireader version %s\n", VERSION "+svn" DETECE_WIKIREADER_VERSION); */
}

static struct option opts[] = {
	{ "help", 0, 0, 'h' },
	{ "version", 0, 0, 'V' },
};

int main(int argc, char **argv)
{
	printf("detect-wikireader - (C) 2007-2008 by OpenMoko Inc.\n"
	       "This program is Free Software and has ABSOLUTELY NO WARRANTY\n\n");

	while (1) {
		int c, option_index = 0;
		c = getopt_long(argc, argv, "hV", opts,
				&option_index);
		if (c == -1)
			break;

		switch (c) {
		case 'h':
			help();
			exit(0);
			break;
		case 'V':
			print_version();
			exit(0);
			break;
		default:
			help();
			exit(2);
		}
	}

	if (daemon_init() != 0) { 
		printf("can't fork self\n"); 
		exit(EXIT_FAILURE); 
	} 

	openlog("openmoko-detect-wikireader", LOG_PID, LOG_USER); 
	syslog(LOG_INFO, "program started."); 

	/* arrange to catch the signal */ 
	signal(SIGCHLD,SIG_IGN); /* ignore child */
	signal(SIGTSTP,SIG_IGN); /* ignore tty signals */
	signal(SIGTTOU,SIG_IGN);
	signal(SIGTTIN,SIG_IGN);
	signal(SIGHUP,signal_handler); /* catch hangup signal */
	signal(SIGTERM,signal_handler); /* catch kill signal */
        
        /* Daemon-specific initialization goes here */
	int rt;
	if ((rt = init_monitor()) != 0) {
		syslog(LOG_INFO, "init_monitor error: %d", rt); 
		exit(EXIT_SUCCESS);
	}

        /* The Big Loop */
        while (1) {
		/* Do some task here ... */
		run_monitor();           
		sleep(1); /* wait 30 seconds */
        }

	exit(EXIT_SUCCESS);

}
