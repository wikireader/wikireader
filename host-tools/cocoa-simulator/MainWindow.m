// Copyright (c) 2009 Openmoko Inc.
//
// Authors   Daniel Mack
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#import "MainWindow.h"
#include <wikilib.h>
#include <guilib.h>
#include <input.h>

char *framebuffer;

/* wikireader glue level */
void fb_refresh(void)
{
	MainWindow *window = (MainWindow *) [NSApp mainWindow];
	[window refreshDisplay];
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

			if (ev->key_event.keycode == '?')
				ev->key_event.keycode = [currentEvent keyCode];

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
			NSUInteger val = guilib_get_pixel(x, y) * 0xff;
			NSUInteger rgba[4] = { val, val, val, 0 };
			[imageRep setPixel: rgba atX: x y: y];
		}

	[imageView setNeedsDisplay];
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
	framebuffer = (char *) malloc(FRAMEBUFFER_SIZE);

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
	[NSThread detachNewThreadSelector: @selector(wikiLibThread:)
							 toTarget: [NSApp mainWindow]
						   withObject: nil];
}

// GUI callbacks
- (IBAction) buttonPressed: (id) sender
{
	unsigned int code;

	switch ([sender tag]) {
	case 0:
		code = WL_INPUT_KEY_SEARCH;
		break;
	case 1:
		code = WL_INPUT_KEY_TREE;
		break;
	case 2:
		code = WL_INPUT_KEY_RANDOM;
		break;
	default:
		return;
	}

	NSEvent *curr = [NSApp currentEvent];
	NSEvent *ev = [NSEvent keyEventWithType: NSKeyDown
								   location: [curr locationInWindow]
							  modifierFlags: 0
							      timestamp: [curr timestamp]
							   windowNumber: [curr windowNumber]
							        context: [curr context]
								 characters: @"?"
				charactersIgnoringModifiers: nil
								  isARepeat: NO
								    keyCode: code ];

	[NSApp postEvent: ev atStart: YES];
}

@end
