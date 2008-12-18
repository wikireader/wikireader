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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include "check-wikireader.h"

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define BUF_LEN     ( 2 * ( EVENT_SIZE + 16 ) )

#define MONITOR_DIR "/media"
#define PATH_LEN 100
#define KERNEL "/kernel"

static int fd;
static int wd;
static char buffer[BUF_LEN];
static char path[PATH_LEN];

int init_monitor(void)
{
	fd = inotify_init();

	if ( fd < 0 ) {
		exit(EXIT_FAILURE);
	}

	wd = inotify_add_watch( fd, MONITOR_DIR,
				IN_MODIFY | IN_CREATE | IN_DELETE );

	return 0;
}

void run_monitor(void)
{
	int return_value;
	fd_set descriptors;
	struct timeval time_to_wait;

	FD_ZERO( &descriptors );
	FD_SET( fd, &descriptors );

	time_to_wait.tv_sec = 1;
	time_to_wait.tv_usec = 0;

	return_value = select ( fd + 1, &descriptors, NULL, NULL, &time_to_wait);

	if ( return_value < 0 ) {
		/* Error */
		syslog(LOG_INFO, "select erro.");
	} 
	else if ( ! return_value ) {
		/* Timeout */
	} 
	else if ( FD_ISSET ( fd, &descriptors ) ) {
		/* Process the inotify events */
		syslog(LOG_INFO, "data.");
		int length = read( fd, buffer, BUF_LEN );  

		if ( length < 0 ) {
			syslog(LOG_INFO, "read error.");
			return -1;
		}  

		struct inotify_event *event = ( struct inotify_event * ) &buffer[ 0 ];
		if ( event->len ) {
			if ( event->mask & IN_CREATE ) {
				if ( event->mask & IN_ISDIR ) {
					syslog(LOG_INFO, "The directory %s was created.\n", event->name );       
					strcpy(path, MONITOR_DIR);
					strcat(path, "/");
					strcat(path, event->name);
					strcat(path, KERNEL);
					check_wikireader(path);
				} else {
					syslog(LOG_INFO, "The file %s was created.\n", event->name );
				}
			}
			else if ( event->mask & IN_DELETE ) {
				if ( event->mask & IN_ISDIR ) {
					syslog(LOG_INFO, "The directory %s was deleted.\n", event->name );       
				} else {
					syslog(LOG_INFO, "The file %s was deleted.\n", event->name );
				}
			}
			else if ( event->mask & IN_MODIFY ) {
				if ( event->mask & IN_ISDIR ) {
					syslog(LOG_INFO, "The directory %s was modified.\n", event->name );
				} else {
					syslog(LOG_INFO, "The file %s was modified.\n", event->name );
				}
			}
		}
	}

}

void close_monitor(void)
{
	( void ) inotify_rm_watch( fd, wd );
	( void ) close( fd );
}
