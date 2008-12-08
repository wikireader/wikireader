//
//  WindowController.h
//  CocoaSimulator
//
//  Created by daniel on 11/3/08.
//  Copyright 2008 caiaq. GPLv3.
//

#import <Cocoa/Cocoa.h>

@interface WindowController : NSWindowController {
	IBOutlet NSImageView *imageView;
	NSBitmapImageRep *imageRep;
	NSTimer *refreshTimer;
	unsigned char *frameBuffer;
}

- (void) refreshDisplay;
- (void) clear;
- (void) setPixel: (UInt32) val atX: (UInt32) x atY: (UInt32) y;

@end
