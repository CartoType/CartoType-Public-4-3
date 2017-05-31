/*
Geometry.java
Copyright (C) 2013 CartoType Ltd.
See www.cartotype.com for more information.
*/

package com.cartotype;

public class Geometry
    {
    /**
    Creates a geometry object using the specified coordinate type.
    aCoordType may be Framework.DEGREE_COORDS, Framework.SCREEN_COORDS, Framework.MAP_COORDS or Framework.MAP_METER_COORDS.
    */
    public Geometry(int aCoordType)
        {
        construct(aCoordType);
        }

    /** Appends a point to the current contour. Returns a CartoType result code: 0 = success. */
    public native int appendPoint(double aX,double aY);
    
    /** Begins a new contour. Use this function when creating a geometry object with more than one contour. Returns a CartoType result code: 0 = success. */
    public native int beginContour();
    
    protected void finalize()
        {
        destroy();
        }
    
    private native void construct(int aCoordType);
    private native void destroy();

    private long iGeometryRef;
    }
