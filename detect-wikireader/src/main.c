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
#include "daemon.h"
#include "config.h"
#include "detect-wikireader-version.h"


	
static int intervals = 1; /* default intervals is 1 second */
static char *monitor_dir = "/media"; /* check whick dirtory */

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

static void help(void)
{
	printf("Usage: detect-wikireader [options] ...\n"
		"  -h --help\t\t\tPrint this help message\n"
		"  -V --version\t\t\tPrint the version number\n"
		"  -D --daemonize\t\tDaemonize after startup\n"
		"  -k --kill\t\t\tKill a running daemon\n"
		"  -t --time\t\t\tCheck folder Intervals second(default 1 sec)\n"
		"  -d --dirtory\t\t\tCheck whick folder (default /media)\n"
		);
}
static void print_version(void)
{
	printf("det-wikird version %s\n", VERSION "+svn" DETECT_WIKIREADER_VERSION);
}

static struct option opts[] = {
	{ "help", 0, 0, 'h' },
	{ "version", 0, 0, 'V' },
	{ "daemon", 0, 0, 'D' },
	{ "kill", 0, 0, 'k' },
	{ "time", 1, 0, 't' },
	{ "dirtory", 1, 0, 'd' },
};

int main(int argc, char **argv)
{
	int daemon = 0;

	printf("det-wikird - (C) 2007-2008 by OpenMoko Inc.\n"
	       "This program is Free Software and has ABSOLUTELY NO WARRANTY\n\n");

	while (1) {
		int c, option_index = 0;
		c = getopt_long(argc, argv, "hVDkt:d:", opts,
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
		case 'D':
			daemon = 1;
			break;
		case 'k':
			break;
		case 't':
			intervals = atoi(optarg);
			break;
		case 'd':
			monitor_dir = optarg;
			break;
		default:
			help();
			exit(2);
		}
	}

	if (daemon) {
		if (daemon_init() != 0) { 
			printf("can't fork self\n"); 
			exit(EXIT_FAILURE); 
		} 
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
	if ((rt = init_monitor(monitor_dir)) != 0) {
		syslog(LOG_INFO, "init_monitor error: %d", rt); 
		exit(EXIT_SUCCESS);
	}

        /* The Big Loop */
        while (1) {
		/* Do some task here ... */
		run_monitor();           
		sleep(intervals);
        }

	exit(EXIT_SUCCESS);
}
