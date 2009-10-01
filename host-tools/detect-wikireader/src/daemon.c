/*
 * (C) Copyright 2008 Openmoko, Inc.
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
#include <assert.h>

#include "daemon.h"

#define RUNNING_DIR "/var/lock/"
#define PID_FILE_NAME "det-wikird"
#define PID_LENGTH 20

static char *pid_file = RUNNING_DIR PID_FILE_NAME;

int open_pid_file() {
	int fd = -1;

	fd = open(pid_file, O_RDWR|O_CREAT,
		  S_IRUSR | S_IWUSR |
		  S_IRGRP | S_IROTH);

	return fd;
}

/** Calls read() in a loop. Makes sure that as much as 'size' bytes,
 * unless EOF is reached or an error occured */
ssize_t loop_read(int fd, void *data, size_t size, int *type) {
	ssize_t ret = 0;
	int _type;

	assert(fd >= 0);
	assert(data);
	assert(size);

	if (!type) {
		_type = 0;
		type = &_type;
	}

	while (size > 0) {
		ssize_t r;

		if ((r = read(fd, data, size)) < 0)
			return r;

		if (r == 0)
			break;

		ret += r;
		data = (unsigned char *) data + r;
		size -= (size_t) r;
	}

	return ret;
}

/* Convert the string s to an unsigned integer in *ret_u */
int atou(const char *s, unsigned int *ret_u) {
	char *x = NULL;
	unsigned long l;

	assert(s);
	assert(ret_u);

	errno = 0;
	l = strtoul(s, &x, 0);

	/*  if (!x || *x || errno) {
	    if (!errno)
	    errno = EINVAL;
	    return -1;
	    }
	*/
	if ((unsigned int) l != l) {
		errno = ERANGE;
		return -1;
	}

	*ret_u = (unsigned int) l;

	return 0;
}

pid_t read_pid(char *fn, int fd)
{
	ssize_t r;
	char t[PID_LENGTH];
	unsigned int pid = 0;

	assert(fn);
	assert(fd >= 0);

	if ((r = loop_read(fd, t, sizeof(t)-1, NULL)) < 0) {
		syslog(LOG_INFO, "Failed to read PID file %s: %s", fn, strerror(errno));
		return (pid_t) -1;
	}

	if (r == 0)
		return (pid_t) -1;

	t[r] = 0;

	if (atou(t, &pid) < 0) {
		syslog(LOG_INFO, "Failed to parse PID file '%s'", fn);
		errno = EINVAL;
		return (pid_t) -1;
	}

	return (pid_t) pid;
}

int check_lock_file()
{
	int lfp;

	lfp = open_pid_file();

	if (lfp < 0) {
		syslog(LOG_INFO, "can not open lock file.");
		return -1;
	}

	if (lockf(lfp, F_TLOCK, 0) < 0) {
		syslog(LOG_INFO, "can not lock.");
		return -1;
	}

	char str[PID_LENGTH];
	sprintf(str, "%d", getpid());	/* first instance continues */
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

/* Kill a current running daemon. Return non-zero on success, -1
 * otherwise. If successful *pid contains the PID of the daemon
 * process. */
int kill_daemon(int sig, pid_t *pid) {
	int fd = -1;
	int ret = -1;
	pid_t _pid;

	if (!pid)
		pid = &_pid;

	if ((fd = open_pid_file()) < 0) {
		if (errno == ENOENT)
			errno = ESRCH;

		goto fail;
	}

	*pid = read_pid(pid_file, fd);
	if ( *pid == (pid_t) -1)
		goto fail;


	/* check the pid name */
	ret = kill(*pid, sig);

fail:
	if (fd >= 0) {
		int saved_errno = errno;
		close(fd);
		errno = saved_errno;
	}

	return ret;
}
