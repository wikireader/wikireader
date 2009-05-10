//
//  WikiDisplay.m
//  CocoaSimulator
//
//  Created by Daniel on 12.12.08.
//  Copyright 2008 caiaq. All rights reserved.
//

#import "WikiDisplay.h"


@implementation WikiDisplay

- (void)mouseDown:(NSEvent *)theEvent
{
	[[NSNotificationCenter defaultCenter]
		postNotificationName:@"mouseEvent" object: self];
}

- (void)mouseUp:(NSEvent *)theEvent
{
	[[NSNotificationCenter defaultCenter]
		postNotificationName:@"mouseEvent" object: self];
}

@end
