//
//  CartoType_DemoView.mm
//  CartoType Demo
//
//  Copyright (C) CartoType Ltd 2013.
//  See www.cartotype.com for more information.
//

#import "CartoType_DemoView.h"
#include <CoreImage/CoreImage.h>
#include <math.h>

@implementation CartoType_DemoView

@synthesize mapImage;
@synthesize legendImage;
@synthesize offset;
@synthesize scale;
@synthesize rotation;
@synthesize currentPoint;

- (id)initWithFrame:(CGRect)frame
    {
    if (self = [super initWithFrame:frame])
        {
        offset.x = offset.y = 0;
        scale = 1;
        rotation = 0;
        currentPoint.y = currentPoint.y = 0;
        }
    return self;
    }

- (void)drawRect:(CGRect)rect
    {
    CGContextRef context = UIGraphicsGetCurrentContext();
    CGFloat h = self.bounds.size.height;
    
    // Apply the transform representing current interactive panning, rotation and zooming.
    if (scale != 1 || rotation != 0)
        {
        CGContextTranslateCTM(context,currentPoint.x,currentPoint.y);
        CGContextScaleCTM(context,scale,scale);
        CGContextRotateCTM(context,rotation);
        CGContextTranslateCTM(context,-currentPoint.x,-currentPoint.y);
        }
    CGContextTranslateCTM(context,offset.x,offset.y);

    // Reflect the coordinate system vertically.
    CGContextScaleCTM(context,1,-1);
    CGContextTranslateCTM(context,0,-h);
    
    CGContextDrawImage(context,rect,mapImage);
    
    CGRect r = rect;
    r.size.width = CGImageGetWidth(legendImage) / 2; // hack: divide by assumed UI scale
    r.size.height = CGImageGetHeight(legendImage) / 2;
    CGContextDrawImage(context,r,legendImage);
    }



@end
