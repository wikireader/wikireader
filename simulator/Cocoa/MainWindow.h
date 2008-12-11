//
//  MainWindow.h
//  CocoaSimulator
//
//  Created by Daniel on 11.12.08.
//  Copyright 2008 caiaq. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface MainWindow : NSWindow {
	NSCondition *condition;
	IBOutlet NSImageView *imageView;
	NSBitmapImageRep *imageRep;
	NSTimer *refreshTimer;
	unsigned char *frameBuffer;
}

- (void) refreshDisplay;
- (void) clear;
- (void) setPixel: (UInt32) val atX: (UInt32) x atY: (UInt32) y;
- (NSCondition *) getCondition;

@end
