//
//  MainWindow.m
//  CocoaSimulator
//
//  Created by Daniel on 11.12.08.
//  Copyright 2008 caiaq. All rights reserved.
//

#import "MainWindow.h"
#include <wikilib.h>
#include <guilib.h>
#include <input.h>

/* wikireader glue level */
void fb_refresh(void)
{
	MainWindow *window = (MainWindow *) [NSApp mainWindow];
	[window refreshDisplay];
}

void fb_set_pixel(int x, int y, int v)
{
	MainWindow *window = (MainWindow *) [NSApp mainWindow];
	[window setPixel: v atX: x atY: y];
}

void fb_clear(void)
{
	MainWindow *window = (MainWindow *) [NSApp mainWindow];
	[window clear];
}

int wl_input_wait(struct wl_input_event *ev)
{
	MainWindow *window = (MainWindow *) [NSApp mainWindow];
	NSCondition *condition = [window getCondition];
	NSPoint event_location, local_point;
	NSSize view_size;

	ev->type = -1;
	
	do {
		[condition lock];
		[condition wait];
		[condition unlock];

		NSEvent *currentEvent = [NSApp currentEvent];
		
		switch ([currentEvent type]) {
		case NSKeyUp:
		case NSKeyDown:
			ev->type = WL_INPUT_EV_TYPE_KEYBOARD;
			ev->key_event.keycode = [[currentEvent characters] characterAtIndex: 0];
			ev->key_event.value = ([currentEvent type] == NSKeyUp) ? 0 : 1;
			break;
		case NSLeftMouseUp:
		case NSLeftMouseDown:
			ev->type = WL_INPUT_EV_TYPE_TOUCH;
			event_location = [currentEvent locationInWindow];
			local_point = [[window imageView] convertPoint:event_location fromView:nil];
			view_size = [[window imageView] bounds].size;
			ev->touch_event.x = local_point.x;
			ev->touch_event.y = view_size.height - local_point.y;
			ev->touch_event.value = ([currentEvent type] == NSLeftMouseUp) ? 0 : 1;
			break;
		}
	} while (ev->type == -1);

	return 0;
}

@implementation MainWindow

- (void)keyDown:(NSEvent *)event
{
	[condition lock];
	[condition signal];
	[condition unlock];
}

- (void)mouseDownInDisplay:(NSEvent *) event
{
	[condition lock];
	[condition signal];
	[condition unlock];
}

- (NSCondition *) getCondition
{
	return condition;
}

- (NSImageView *) imageView
{
	return imageView;
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
	[imageView setNeedsDisplay];
}

- (void) setPixel: (UInt32) val atX: (UInt32) x atY: (UInt32) y
{
	if (x >= FRAMEBUFFER_WIDTH || y >= FRAMEBUFFER_HEIGHT)
		return;
	
	frameBuffer[y * FRAMEBUFFER_WIDTH + x] = val;
}

- (void) wikiLibThread : (id) param
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	wikilib_init();
	guilib_init();
	wikilib_run();
	[pool release];
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
					bitsPerPixel: 24 ];
	[imageRep setSize: [imageView frame].size];
	NSImage *image = [[NSImage alloc] initWithSize: [imageView frame].size];

	[image addRepresentation: imageRep];
	[imageView setImage: image];
	condition = [[NSCondition alloc] init];
	frameBuffer = (unsigned char *) malloc(FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT);
	memset(frameBuffer, 0, FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT);

	[[NSNotificationCenter defaultCenter] addObserver:self
		selector:@selector(mouseDownInDisplay:)
		name:@"mouseEvent" object:imageView];
}

// NSApplication delegates
- (void)applicationDidBecomeActive: (NSNotification *) aNotification
{
	[NSThread detachNewThreadSelector:@selector(wikiLibThread:) toTarget: [NSApp mainWindow] withObject:nil];
}

@end
