//
//  CartoType_DemoAppDelegate.m
//  CartoType Demo
//
//  Copyright (C) CartoType Ltd 2009-2016.
//  See www.cartotype.com for more information.
//

#import "CartoType_DemoAppDelegate.h"
#import "CartoType_DemoViewController.h"

@implementation CartoType_DemoAppDelegate

@synthesize window;
@synthesize viewController;


-(void)applicationDidFinishLaunching:(UIApplication *)application
    {
    // Override point for customization after app launch    
    [window setRootViewController:viewController];
    [window makeKeyAndVisible];
    }


@end
