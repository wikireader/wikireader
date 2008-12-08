//
//  WindowController.m
//  CocoaSimulator
//
//  Created by daniel on 11/3/08.
//  Copyright 2008 caiaq. GPLv3.
//

#import "WindowController.h"
//#include "wikilib/wikilib.h"
#include "guilib.h"

static WindowController *controller = NULL;

/* wikireader glue level */
void fb_refresh(void)
{
	[controller refreshDisplay];
}

void fb_set_pixel(int x, int y, int v)
{
	[controller setPixel: v atX: x atY: y];
}

void fb_clear(void)
{
	[controller clear];
}


@implementation WindowController

- (void) refreshTimerCallback: (NSTimer *) timer
{
	[self refreshDisplay];
}

- (void) refreshDisplay
{
	NSInteger x, y;

	for (x = 0; x < FRAMEBUFFER_WIDTH; x++)
		for (y = 0; y < FRAMEBUFFER_HEIGHT; y++) {
			NSUInteger val = 255 - (frameBuffer[y * FRAMEBUFFER_WIDTH + x] * 0xf);
			NSUInteger rgba[4] = { val, val, val, 0 };
			[imageRep setPixel: rgba atX: x y: y];
		}
		
	[imageView setNeedsDisplay];
}

- (void) clear
{
	memset(frameBuffer, 0, FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT);
}

- (void) setPixel: (UInt32) val atX: (UInt32) x atY: (UInt32) y
{
	if (x >= FRAMEBUFFER_WIDTH || y >= FRAMEBUFFER_HEIGHT)
		return;
	
	frameBuffer[y * FRAMEBUFFER_WIDTH + x] = val;
}

- (void) awakeFromNib 
{
	if ([imageView frame].size.width != FRAMEBUFFER_WIDTH ||
		[imageView frame].size.height != FRAMEBUFFER_HEIGHT) {
			printf("ERROR! guilib's framebuffer size does not match canvas size!\n");
			return;
	}

	imageRep = [[NSBitmapImageRep alloc] 
					initWithBitmapDataPlanes: NULL
					pixelsWide: (NSInteger) [imageView frame].size.width
					pixelsHigh: (NSInteger) [imageView frame].size.height
					bitsPerSample: 8
					samplesPerPixel: 3
					hasAlpha: NO
					isPlanar: NO
					colorSpaceName: @"NSCalibratedRGBColorSpace"
					bytesPerRow: (NSInteger) [imageView frame].size.width * 3
					bitsPerPixel: 24
				];
	[imageRep setSize: [imageView frame].size];
	NSImage *image = [[NSImage alloc] initWithSize: [imageView frame].size];

	[image addRepresentation: imageRep];
	[imageView setImage: image];

/*
	refreshTimer = [NSTimer scheduledTimerWithTimeInterval: 1.0
								target: self
								selector: @selector(refreshTimerCallback:)
								userInfo: nil
								repeats: YES];
*/

	/* initalize wikireader library */
	//wikilib_init();
	
	controller = self;
	frameBuffer = (unsigned char *) malloc(FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT);
	memset(frameBuffer, 0, FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT);
	guilib_init();
}

@end
