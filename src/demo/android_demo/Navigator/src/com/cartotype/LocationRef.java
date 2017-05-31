/*
LocationRef.java
Copyright (C) 2015 CartoType Ltd.
See www.cartotype.com for more information.
*/

package com.cartotype;

public class LocationRef
    {
    /** A location reference type: a line in the route network. */
    public static final int LOCATION_REF_LINE = 0;
    /** A location reference type: a point on the earth's surface. */
    public static final int LOCATION_REF_GEO_COORDINATE = 1;
    /** A location reference type: a point on a line in the route network. */
    public static final int LOCATION_REF_POINT_ALONG_LINE = 2;
    /** A location reference type: a point on the route network providing access to a nearby POI: the first point is the point on the line, the second is the POI. */
    public static final int LOCATION_REF_POINT_WITH_ACCESS_POINT = 3;
    /** A location reference type: a circle defined as a point and a radius. */
    public static final int LOCATION_REF_CIRCLE = 4;
    /** A location reference type: a rectangle aligned to the grid of latitude and longitude: it is defined using two points at opposite corners. */
    public static final int LOCATION_REF_RECTANGLE = 5;
    /** A location reference type: a polygon defined using a set of points. */
    public static final int LOCATION_REF_POLYGON = 6;
    /** A location reference type: a closed line in the route network. */
    public static final int LOCATION_REF_CLOSED_LINE = 7;
    
    public static final int SIDE_OF_ROAD_NONE = 0;
    public static final int SIDE_OF_ROAD_RIGHT = 1;
    public static final int SIDE_OF_ROAD_LEFT = 2;
    public static final int SIDE_OF_ROAD_BOTH = 3;

    public static final int ROAD_ORIENTATION_NONE = 0;
    public static final int ROAD_ORIENTATION_FORWARDS = 1;
    public static final int ROAD_ORIENTATION_BACKWARDS = 2;
    public static final int ROAD_ORIENTATION_BOTH = 3;
    
    /** The type of this location reference: one of the LOCATION_REF_* constants. */
    public int iType;
    /** The arbitrary ID of the location reference. */
    public String iId;
    /** The point or points. */
    public Geometry iGeometry;
    /** The radius, if this is a circle. */
    public double iRadiusInMeters;
    /** The side of the road, if relevant: one of the SIDE_OF_ROAD_* constants. */
    public int iSideOfRoad;
    /** The road orientation, if relevant: one of the ROAD_ORIENTATION_* constants. */
    public int iRoadOrientation;
    }
