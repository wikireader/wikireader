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
#include <syslog.h>

extern char **environ;

int check_wikireader(char *path)
{
	if (access(path, R_OK) == 0 ) {
		syslog(LOG_INFO, "yes. this is wikireader sd card --%s--.", path );
		/*TODO: open websit wiki.openmoko.com
		 * open wiki-reader simulator
		 */
		system("firefox   http://wiki.openmoko.org");
		syslog(LOG_INFO, "firefox");
	} else {
		syslog(LOG_INFO, "no. this is not wikireader sd card --%s--.", path );
	}

	return 0;
}
