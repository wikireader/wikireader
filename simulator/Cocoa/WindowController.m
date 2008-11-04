//
//  WindowController.m
//  CocoaSimulator
//
//  Created by daniel on 11/3/08.
//  Copyright 2008 caiaq. GPLv3.
//

#import "WindowController.h"
//#include "wikilib/wikilib.h"
#include <guilib.h>

/* wikireader glue level */
void display_refresh(void *ctx)
{
	WindowController *controller = ctx;
	[controller refreshDisplay];
}

@implementation WindowController

- (void) refreshTimerCallback: (NSTimer *) timer
{
	[self refreshDisplay];
}

- (void) refreshDisplay
{
	NSInteger x, y;

	for (x = 0; x < (NSInteger) [imageView frame].size.width; x++)
		for (y = 0; y < (NSInteger) [imageView frame].size.height; y++) {
			NSUInteger val = guilib_get_pixel(frameBuffer, x, y);
			NSUInteger rgba[4] = { val << 4, val << 4, val << 4, 0 };
			[imageRep setPixel: rgba atX: x y: y];
		}
		
	[imageView setNeedsDisplay];
}

- (void) awakeFromNib 
{
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
	
	frameBuffer = (unsigned char *) malloc(sizeof (unsigned char) * FRAMEBUFFER_SIZE);
	guilib_init(frameBuffer, self);
}

@end
