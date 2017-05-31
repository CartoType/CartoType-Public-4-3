/*
RouteSegment.java
Copyright (C) 2012 CartoType Ltd.
See www.cartotype.com for more information.
*/

package com.cartotype;

public class RouteSegment implements Path
    {
    RouteSegment(Route aRoute) { iRoute = aRoute; }
    
    /** Gets the road type of the object of which this segment is a part. */
    public native int getRoadType();
    /**
    Gets the maximum legal speed (speed limit) for this segment, in kilometres per hour,
    or zero if no information is available. 
    */
    public native double getMaxSpeed();
    /** Gets the road name of the segment. */
    public native String getName();
    /** Gets the road reference of segment. */
    public native String getRef();
    /** Gets the distance in metres. */
    public native double getDistance();
    /** Gets the estimated time in seconds taken to traverse the segment, including the turn time returned by getTurnTime(). */
    public native double getTime();
    /** Gets the estimated time in seconds taken by the turn at the start of the segment; this is included in the value returned by getTime(). */
    public native double getTurnTime();
    /** Gets the turn type as defined in the Turn class. */
    public native int getTurnType();
    /** Returns true if this is a 'continue' turn, false if not. */
    public native boolean getContinue();
    /** Gets the roundabout state as defined in the Turn class. */
    public native int getRoundaboutState();
    /** Gets the turn angle. */
    public native double getTurnAngle();
    /** Gets the exit number, counting the current junction as 0, if this junction is part of a roundabout. */
    public native int getExitNumber();
    /** Gets the number of choices at this turning if known; 0 if not known. */
    public native int getChoices();
    
    /** Gets the number of contours (sub-paths). It is always 1 for this class. */
    public int getContourCount() { return 1; }
    /** Gets the number of points in a contour. */
    public native int getPointCount(int aContourIndex);
    /** Gets a point given its contour and point indexes. */
    public native void getPoint(int aContourIndex,int aPointIndex,PathPoint aPoint);
    /** Returns true if the path is closed. */
    public boolean isClosed() { return false; }
    
    private Route iRoute;       // a reference to the parent route, to keep it in existence
    private long iSegmentRef;
    }
