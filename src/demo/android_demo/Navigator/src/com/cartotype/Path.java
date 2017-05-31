/*
Path.java
Copyright (C) 2012 CartoType Ltd.
See www.cartotype.com for more information.
*/

package com.cartotype;

public interface Path
    {
    /** Gets the number of contours (sub-paths). */
    public int getContourCount();
    /** Gets the number of points in a contour. */
    public int getPointCount(int aContourIndex);
    /** Gets a point given its contour and point indexes. */
    public void getPoint(int aContourIndex,int aPointIndex,PathPoint aPoint);
    /** Returns true if the path is closed. */
    public boolean isClosed();
    }
