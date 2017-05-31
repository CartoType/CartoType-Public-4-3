//
//  CartoType_DemoViewController.h
//  CartoType Demo
//
//  Copyright (C) CartoType Ltd 2013-2015.
//  See www.cartotype.com for more information.
//

#import <UIKit/UIKit.h>
#import <CoreGraphics/CGContext.h>
#import <CartoType/CartoType.h>
#import <vector>

@class CartoType_DemoView;

@interface CartoType_DemoViewController: UIViewController <UIGestureRecognizerDelegate>
    {
	CartoTypeFramework* iFramework;
    CartoTypeLegend* iLegend;
	IBOutlet CartoType_DemoView* iDemoView;
    int iUiScale;
    CartoTypePoint iRouteStart;
    CartoTypePoint iRouteEnd;
    CartoTypeRoute* iRoute;
    std::vector<CartoTypePoint> iRoutePoints;
    }

@property (nonatomic, strong) CartoType_DemoView* iDemoView;

@end
