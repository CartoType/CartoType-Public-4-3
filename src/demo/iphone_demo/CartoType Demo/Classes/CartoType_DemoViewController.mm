//
//  CartoType_DemoViewController.mm
//  CartoType Demo
//
//  Copyright (C) CartoType Ltd 2013-2016.
//  See www.cartotype.com for more information.
//


#import "CartoType_DemoViewController.h"
#import "CartoType_DemoView.h"

@implementation CartoType_DemoViewController

@synthesize iDemoView;

// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
-(void)viewDidLoad 
    {
	[super viewDidLoad];
    
    // Create a pan gesture recognizer.
    UIPanGestureRecognizer* my_pan_recognizer = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(handlePanGesture:)];
    my_pan_recognizer.delegate = self;
    [self.view addGestureRecognizer:my_pan_recognizer];
    
    // Create a pinch gesture recognizer.
    UIPinchGestureRecognizer* my_pinch_recognizer = [[UIPinchGestureRecognizer alloc] initWithTarget:self action:@selector(handlePinchGesture:)];
    my_pinch_recognizer.delegate = self;
    [self.view addGestureRecognizer:my_pinch_recognizer];
    
    // Create a rotation gesture recognizer.
    UIRotationGestureRecognizer* my_rotation_recognizer = [[UIRotationGestureRecognizer alloc] initWithTarget:self action:@selector(handleRotationGesture:)];
    my_rotation_recognizer.delegate = self;
    [self.view addGestureRecognizer:my_rotation_recognizer];
    
    // Create a tap gesture recognizer.
    UITapGestureRecognizer* my_tap_recognizer = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleTapGesture:)];
    my_tap_recognizer.delegate = self;
    [self.view addGestureRecognizer:my_tap_recognizer];
 
    int width = iDemoView.frame.size.width;
    int height = iDemoView.frame.size.height;
    iUiScale = [[UIScreen mainScreen] scale];
    width *= iUiScale;
    height *= iUiScale;
    iRoute = nil;
    
    CartoTypeFrameworkParam* param = [[CartoTypeFrameworkParam alloc] init];
    param.mapFileName = @"santa_cruz";
    param.styleSheetFileName = @"standard";
    param.fontFileName = @"DejaVuSans";
    param.viewWidth = width;
    param.viewHeight = height;
    //param.textIndexLevels = 1;
    
    // Create the framework object.
    iFramework = [[CartoTypeFramework alloc] initWithParam:param];
    
    // Create the legend and put a scale bar in it.
    iLegend = [[CartoTypeLegend alloc] initWithFramework:iFramework];
    [iLegend addScaleLine:true];
	
    // Draw the first map.
	iDemoView.mapImage = [iFramework getMapBitmap];
    iDemoView.legendImage = [iLegend createLegendOfWidth:2 unit:"in" scaleDenominator:[iFramework getScaleDenominator] scaleDenominatorInView:[iFramework getScaleDenominatorInView]];
	
	self.iDemoView.multipleTouchEnabled = YES;
	self.iDemoView.scale = 1;
	
	[self.iDemoView setNeedsDisplay];
    }

-(BOOL)gestureRecognizer:(UIGestureRecognizer*)aRecognizer shouldRecognizeSimultaneouslyWithGestureRecognizer:(UIGestureRecognizer*)aOtherRecognizer
    {
    return true;
    }

static CGFloat RadiansToDegrees(CGFloat radians) { return radians / M_PI * 180; }

-(void)setRouteEnd:(CGPoint)aPoint
    {
    iRouteStart = iRouteEnd;
    iRouteEnd.x = aPoint.x * iUiScale;
    iRouteEnd.y = aPoint.y * iUiScale;
    [iFramework convertPoint:&iRouteEnd from:ScreenCoordType to:DegreeCoordType];
    if (iRouteStart.x && iRouteStart.y)
        {
        CTResult error = [iFramework startNavigationFrom:iRouteStart startCoordType:DegreeCoordType to:iRouteEnd endCoordType:DegreeCoordType];
        if (!error)
            {
            iRoute = [iFramework getRoute];
            iRoutePoints.clear();
            for (int i = 0; i < [iRoute getRouteSegmentCount]; i++)
                {
                CartoTypeRouteSegment* segment = [iRoute getRouteSegmentByIndex:i];
                int contours = [segment contourCount];
                for (int contour = 0; contour < contours; contour++)
                    {
                    int points = [segment pointCount:contour];
                    for (int point = 0; point < points; point++)
                        {
                        CartoTypePathPoint p;
                        [segment getPoint:contour pointIndex:point point:&p];
                        CartoTypePoint q;
                        q.x = p.x;
                        q.y = p.y;
                        [iFramework convertPoint:&q from:MapCoordType to:DegreeCoordType];
                        iRoutePoints.push_back(q);
                        }
                    }
                }
            iDemoView.mapImage = [iFramework getMapBitmap];
            [iDemoView setNeedsDisplay];
            }
        }
    }

-(void)acceptGesture;
    {
    [iFramework panX:(int)(-iDemoView.offset.x * iUiScale) andY:(int)(-iDemoView.offset.y * iUiScale)];
    if (iDemoView.scale != 1 || iDemoView.rotation != 0)
        {
        double w = iDemoView.bounds.size.width;
        double h = iDemoView.bounds.size.height;
        double cx = iDemoView.bounds.origin.x + w / 2;
        double cy = iDemoView.bounds.origin.y + h / 2;
        int x = int((cx - iDemoView.currentPoint.x) * iUiScale);
        int y = int((cy - iDemoView.currentPoint.y) * iUiScale);
        [iFramework panX:-x andY:-y];
        [iFramework zoom:iDemoView.scale];
        [iFramework rotate:RadiansToDegrees(iDemoView.rotation)];
        [iFramework panX:x andY:y];
        }
    
    iDemoView.offset = CGPointZero;
    iDemoView.scale = 1;
    iDemoView.rotation = 0;
    iDemoView.currentPoint = CGPointZero;

    iDemoView.mapImage = [iFramework getMapBitmap];
    iDemoView.legendImage = [iLegend createLegendOfWidth:2 unit:"in" scaleDenominator:[iFramework getScaleDenominator] scaleDenominatorInView:[iFramework getScaleDenominatorInView]];
    [iDemoView setNeedsDisplay];
    }

-(void)rejectGesture
    {
    iDemoView.offset = CGPointZero;
    iDemoView.scale = 1;
    iDemoView.rotation = 0;
    iDemoView.currentPoint = CGPointZero;
    [iDemoView setNeedsDisplay];
    }

-(IBAction)handlePanGesture:(UIPanGestureRecognizer*)aRecognizer
    {
    if ([aRecognizer state] == UIGestureRecognizerStateChanged)
        {
        iDemoView.offset = [aRecognizer translationInView:nil];
        iDemoView.currentPoint = [aRecognizer locationInView:nil];
        [iDemoView setNeedsDisplay];
        }
    else if ([aRecognizer state] == UIGestureRecognizerStateRecognized)
        {
        [self acceptGesture];
        [aRecognizer setTranslation:CGPointZero inView:nil];
        }
    else if ([aRecognizer state] == UIGestureRecognizerStateCancelled)
        [self rejectGesture];
    }

-(IBAction)handlePinchGesture:(UIPinchGestureRecognizer*)aRecognizer
    {
    if ([aRecognizer state] == UIGestureRecognizerStateChanged)
        {
        iDemoView.scale = [aRecognizer scale];
        iDemoView.currentPoint = [aRecognizer locationInView:nil];
        [iDemoView setNeedsDisplay];
        }
    else if ([aRecognizer state] == UIGestureRecognizerStateRecognized)
        {
        [self acceptGesture];
        [aRecognizer setScale:1];
        }
    else if ([aRecognizer state] == UIGestureRecognizerStateCancelled)
        [self rejectGesture];
    }

-(IBAction)handleRotationGesture:(UIRotationGestureRecognizer*)aRecognizer
    {
    if ([aRecognizer state] == UIGestureRecognizerStateChanged)
        {
        iDemoView.rotation = [aRecognizer rotation];
        iDemoView.currentPoint = [aRecognizer locationInView:nil];
        [iDemoView setNeedsDisplay];
        }
    else if ([aRecognizer state] == UIGestureRecognizerStateRecognized)
        {
        [self acceptGesture];
        [aRecognizer setRotation:0];
        }
    else if ([aRecognizer state] == UIGestureRecognizerStateCancelled)
        [self rejectGesture];
    }

-(IBAction)handleTapGesture:(UITapGestureRecognizer*)aRecognizer
    {
    if ([aRecognizer state] == UIGestureRecognizerStateRecognized)
        [self setRouteEnd:[aRecognizer locationInView:nil]];
    }

-(void)didReceiveMemoryWarning
    {
	// Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
	
	// Release any cached data, images, etc that aren't in use.
    }

-(void)viewDidUnload
    {
	// Release any retained subviews of the main view.
	// e.g. self.myOutlet = nil;
    }

@end
