/*
NearestSegmentInfo.java
Copyright (C) 2015 CartoType Ltd.
See www.cartotype.com for more information.
*/

package com.cartotype;

public class NearestSegmentInfo
    {
    /** The index of the segment in the Route object, or -1 if there were no segments. */
    public int iSegmentIndex;
    /**
    The index of the line within the segment's path:
    line N goes from point N to point N + 1.
    */
    public int iLineIndex;
    /** The X coordinate of the point in the segment's path nearest to the other point, in map coordinates. */
    public double iNearestPointX;
    /** The Y coordinate of the point in the segment's path nearest to the other point, in map coordinates. */
    public double iNearestPointY;
    /** The distance from the other point to iNearestPoint in meters. */
    public double iDistanceToRoute;
    /** The distance of the nearest point along the route in meters. */
    public double iDistanceAlongRoute;
    /** The distance within the current segment in meters. */
    public double iDistanceAlongSegment;
    /** The estimated time of the nearest point, along the route, in seconds. */
    public double iTimeAlongRoute;
    /** The estimated time within the current segment, in seconds. */
    public double iTimeAlongSegment;
    /** The heading of the nearest line as a map angle taken anti-clockwise from rightwards. */
    public double iHeading;
    }
