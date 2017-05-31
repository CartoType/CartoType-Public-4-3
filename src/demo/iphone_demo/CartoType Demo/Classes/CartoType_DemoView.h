//
//  CartoType_DemoView.h
//  CartoType Demo
//
//  Copyright (C) CartoType Ltd 2009.
//  See www.cartotype.com for more information.
//

#import <UIKit/UIKit.h>


@interface CartoType_DemoView : UIView
    {
	CGImageRef mapImage;
    CGImageRef legendImage;
    CGPoint offset;         // offset applied by touches
    CGFloat scale;          // scale applied by touches
    CGFloat rotation;       // rotation applied by touches
    CGPoint currentPoint;   // last touch point, about which scaling and rotation are performed.
    }

@property (nonatomic) CGImageRef mapImage;
@property (nonatomic) CGImageRef legendImage;
@property (nonatomic) CGPoint offset;
@property (nonatomic) CGFloat scale;
@property (nonatomic) CGFloat rotation;
@property (nonatomic) CGPoint currentPoint;

@end
