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
#include <regs.h>
#include <samo.h>
#include <stdlib.h>
#include <malloc-simple.h>
#include <msg.h>


#define MB 1024 * 1024
#define KB 1024

void perf_test(void)
{
	char *mem_src = NULL, *mem_dst = NULL, *file = NULL;
	int fd = -1, read;
	unsigned int file_size;

	mem_src = malloc_simple(MB, MEM_TAG_PERF_M1);
	if (!mem_src) {
		msg(MSG_INFO, "no mem_src: out of mem ?\n");
		malloc_status_simple();
		goto out;
	}

	mem_dst = malloc_simple(MB, MEM_TAG_PERF_M2);
	if (!mem_dst) {
		msg(MSG_INFO, "no mem_dst: out of mem ?\n");
		malloc_status_simple();
		goto out;
	}

	file = malloc_simple(MB, MEM_TAG_PERF_M3);
	if (!file) {
		msg(MSG_INFO, "no file: out of mem ?\n");
		malloc_status_simple();
		goto out;
	}

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
		read = disk_read(0, file + (fd * 512 * 256), fd * 255, 255);

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
		goto out;
	}

	prof_start(PROF_fread);
	read = wl_read(fd, file, file_size);
	prof_stop(PROF_fread);

	if (read != file_size) {
		msg(MSG_INFO, "Could not read file '8dcec2': read process aborted after %i bytes\n", read);
		goto out;
	}

	msg(MSG_INFO, "memcpy speed: 1MB/%dms, SD card: 1MB/%dms, fatfs: 1MB/%dms\n",
	    (prof_container[PROF_memcpy].calls == 0 ? 0 :
	     ((prof_container[PROF_memcpy].total_time / MCLK_MHz) / prof_container[PROF_memcpy].calls / 1000)),
	    (prof_container[PROF_sd_read].calls == 0 ? 0 :
	     ((prof_container[PROF_sd_read].total_time / MCLK_MHz) / prof_container[PROF_sd_read].calls / 1000)),
	    (prof_container[PROF_fread].calls == 0 ? 0 :
	     ((prof_container[PROF_fread].total_time / MCLK_MHz) / prof_container[PROF_fread].calls / (file_size / 100 / KB) / 100)));

out:
	if (fd)
		wl_close(fd);

	if (file)
		free_simple(file, MEM_TAG_PERF_F3);

	if (mem_dst)
		free_simple(mem_dst, MEM_TAG_PERF_F4);

	if (mem_src)
		free_simple(mem_src, MEM_TAG_PERF_F5);

	return;
}

