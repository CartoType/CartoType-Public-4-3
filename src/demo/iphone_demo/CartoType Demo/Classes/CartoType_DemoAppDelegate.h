//
//  CartoType_DemoAppDelegate.h
//  CartoType Demo
//
//  Copyright (C) CartoType Ltd 2009.
//  See www.cartotype.com for more information.
//

#import <UIKit/UIKit.h>

@class CartoType_DemoViewController;

@interface CartoType_DemoAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    CartoType_DemoViewController *viewController;
}

@property (nonatomic, strong) IBOutlet UIWindow *window;
@property (nonatomic, strong) IBOutlet CartoType_DemoViewController *viewController;

@end

