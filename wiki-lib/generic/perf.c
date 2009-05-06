/*
 * Copyright (C) 2009
 *
 * Marek Lindner
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of version 2 of the GNU General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA
 *
 */



#include <perf.h>
#include <profile.h>
#include <string.h>
#include <diskio.h>
#include <file-io.h>
#include <msg.h>
#include <regs.h>
#include <wikireader.h>


#define MB 1024 * 1024
#define KB 1024

void perf_test(void)
{
	char mem_src[MB], mem_dst[MB], file[MB];
	int fd, read;
	unsigned int file_size;

	prof_start(PROF_memset);
	memset(mem_src, 1, MB);
	prof_stop(PROF_memset);

	prof_start(PROF_memcpy);
	memcpy(mem_dst, mem_src, MB);
	prof_stop(PROF_memcpy);

	prof_start(PROF_memcmp);
	memcmp(mem_src, mem_dst, MB);
	prof_stop(PROF_memcmp);

	prof_start(PROF_sd_read);
	for (fd = 0; fd < 8; fd++) {
		read = my_disk_read(0, file + (fd * 512 * 256), fd * 255, 255);

		if (read != RES_OK) {
			prof_stop(PROF_sd_read);
			msg(MSG_INFO, "Could not read sd card: %i\n", read);
			goto out;
		}
	}
	prof_stop(PROF_sd_read);

	fd = wl_open("8dcec2", WL_O_RDONLY);

	if (fd < 0) {
		msg(MSG_INFO, "Could not read file '8dcec2': file not found\n");
		goto out;
	}

	wl_fsize(fd, &file_size);

	if (file_size > MB) {
		msg(MSG_INFO, "Could not read file '8dcec2': file size bigger than %i bytes\n", MB);
		goto close;
	}

	prof_start(PROF_fread);
	read = wl_read(fd, file, file_size);
	prof_stop(PROF_fread);

	if (read != file_size) {
		msg(MSG_INFO, "Could not read file '8dcec2': read process aborted after %i bytes\n", read);
		goto close;
	}

	msg(MSG_INFO, "memcpy speed: 1MB/%dms, SD card: 1MB/%dms, fatfs: 1MB/%dms\n",
		(prof_container[PROF_memcpy].calls == 0 ? 0 :
		((prof_container[PROF_memcpy].total_time / MCLK_MHz) / prof_container[PROF_memcpy].calls / 1000)),
		(prof_container[PROF_sd_read].calls == 0 ? 0 :
		((prof_container[PROF_sd_read].total_time / MCLK_MHz) / prof_container[PROF_sd_read].calls / 1000)),
		(prof_container[PROF_fread].calls == 0 ? 0 :
		((prof_container[PROF_fread].total_time / MCLK_MHz) / prof_container[PROF_fread].calls / (file_size / 100 / KB) / 100)));

close:
	wl_close(fd);
out:
	return;
}

