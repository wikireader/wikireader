#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <external_interface.h>
#include "fb.h"

struct fb_info fb;

void fb_update(void)
{
	if (fb.update)
		fb.update(&fb);
}

int fb_init (struct fb_info *info)
{
	if (!info)
		return -1;
	
	memcpy(&fb, info, sizeof(fb));

	if (fb.width 	!= WIDTH  ||
	    fb.height 	!= HEIGHT ||
	    fb.mem	== NULL)
		return -1;
	
	return 0;
}


