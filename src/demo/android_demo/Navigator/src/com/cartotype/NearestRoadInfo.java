package com.cartotype;

/**
Information about the nearest road to a certain point.
It is returned by Framework.getNearestRoad.
*/
public class NearestRoadInfo
    {
    /** The road type of the arc. */
    public int iRoadType;
    /**
    The maximum legal speed in kilometres per hour. A value of zero means there is no known speed limit,
    or the standard speed limit for the type of road applies.
    */
    public double iMaxSpeed;
    /** The standard name of the road. */
    public String iName;
    /** The road reference of the road. */
    public String iRef;
    /** The x coordinate of the nearest point on the road, in map coordinates, to the chosen point. */
    public double iNearestPointX;
    /** The y coordinate of the nearest point on the road, in map coordinates, to the chosen point. */
    public double iNearestPointY;
    /** The distance from the chosen point to iNearestPoint in meters. */
    public double iDistance;
    /** The heading of the nearest segment in degrees. */
    public double iHeadingInDegrees;
    /** The x coordinate of the heading of the nearest segment as a unit vector. */
    public double iHeadingVectorX;
    /** The x coordinate of the heading of the nearest segment as a unit vector. */
    public double iHeadingVectorY;
    }
