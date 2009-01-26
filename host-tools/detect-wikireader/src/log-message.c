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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>


#define LOG_FILE_NAME ".det-wikird.log" 
static char* log_file;
static int log_fp;

int init_log_message(void)
{
	char* home = getenv("HOME");

	log_file = (char *)malloc(sizeof(char) * ( strlen(home) + strlen(LOG_FILE_NAME)));
	strcpy(log_file, home);
	strcat(log_file, LOG_FILE_NAME);

	log_fp = open(log_file,O_CREAT, 
		   S_IRUSR | S_IWUSR |
		   S_IRGRP | S_IROTH);

	if (log_fp < 0) {
		syslog(LOG_INFO, "can not open log file."); 
		return -1;
	}

	return 0;
} 

int log_message(char *message)
{
	if (log_fp < 0) 
		return -1;

	fprintf(log_fp,"%s\n",message);
	close(log_fp);

	return 0;
}
