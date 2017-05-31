/*
Route.java
Copyright (C) 2012-2015 CartoType Ltd.
See www.cartotype.com for more information.
*/

package com.cartotype;

public class Route implements Path
    {
    Route(long aRouteRef) { iRouteRef = aRouteRef; }
    
    /** Gets the distance of the route in metres. */
    public native double getDistance();
    /** Gets the estimated time taken to traverse the route in seconds. */
    public native double getTime();
    /** Gets the number of route segments. */
    public native int getRouteSegmentCount();
    /** Gets a route segment. */
    public RouteSegment getRouteSegment(int aIndex)
        {
        if (aIndex < 0 || aIndex > getRouteSegmentCount())
            throw new RuntimeException("illegal route segment index");
        RouteSegment r = new RouteSegment(this);
        initRouteSegment(r,aIndex);
        return r;
        }
    /** Gets instructions for the route in the language of the current locale, or in English if that language is not available. */
    public native String getInstructions();
    /** Gets the total distance in metres of the parts of the route that are on toll roads. */
    public native double getTollRoadDistance();
    
    /**
    Gets information about the nearest route segment to a point given in map coordinates.

    The parameter aSection gives the current route section (a part of the route between waypoints;
    simple routes have a single section). The returned segment will always be in the current
    section or a following section. It is theoretically possible for a section to be completely skipped if it is very short or
    of zero length, so there is no constraint that section 0 must be followed by section 1 and not by section 2, etc.
    If aSection is negative it is ignored and the returned segment may be in any section.

    The parameter aPreviousDistanceAlongRoute should be provided (set to a value greater than zero)
    for multi-section routes, so that the correct route segment can be chosen if the route doubles back on itself.
    Positions further along the route will be given preference over those earlier in the route.
    */
    public NearestSegmentInfo getNearestSegment(double aX,double aY,NearestSegmentInfo aInfo,int aSection,double aPreviousDistanceAlongRoute)
        {
        NearestSegmentInfo info = new NearestSegmentInfo();
        getPointAlongRouteHelper(aX,aY,-1,-1,info,aSection,aPreviousDistanceAlongRoute);
        return info;
        }
    
    /** Gets information about a point a certain distance along a route. */
    public NearestSegmentInfo getPointAtDistance(double aDistanceInMeters)
        {
        NearestSegmentInfo info = new NearestSegmentInfo();
        if (aDistanceInMeters < 0)
            aDistanceInMeters = 0;
        getPointAlongRouteHelper(0,0,aDistanceInMeters,-1,info,0,0);
        return info;
        }

    /** Gets information about a point a certain estimated time along a route. */
    public NearestSegmentInfo getPointAtTime(double aTimeInSeconds)
        {
        NearestSegmentInfo info = new NearestSegmentInfo();
        if (aTimeInSeconds < 0)
            aTimeInSeconds = 0;
        getPointAlongRouteHelper(0,0,-1,aTimeInSeconds,info,0,0);
        return info;
        }

    /** Gets the number of contours (sub-paths). */
    public native int getContourCount();
    /** Gets the number of points in a contour. */
    public native int getPointCount(int aContourIndex);
    /** Gets a point given its contour and point indexes. */
    public native void getPoint(int aContourIndex,int aPointIndex,PathPoint aPoint);
    /** Returns true if the path is closed. */
    public boolean isClosed() { return false; }

    protected void finalize()
        {
        destroy();
        }
    static
        {
        initGlobals();
        }

    private native void initRouteSegment(RouteSegment aRouteSegment,int aIndex);
    private native void getPointAlongRouteHelper(double aX,double aY,double aDistance,double aTime,
                                                 NearestSegmentInfo aInfo,int aSection,double aPreviousDistanceAlongRoute);
    private static native void initGlobals();
    private native void destroy();

    long iRouteRef;
    }
