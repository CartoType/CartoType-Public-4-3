/*
MapObject.java
Copyright (C) 2012 CartoType Ltd.
See www.cartotype.com for more information.
*/

package com.cartotype;

public class MapObject implements Path
    {
    public static final int POINT_TYPE = 0;
    public static final int LINE_TYPE = 1;
    public static final int POLYGON_TYPE = 2;
    public static final int ARRAY_TYPE = 3;

    /** Gets the number of contours (sub-paths). */
    public native int getContourCount();
    /** Gets the number of points in a contour. */
    public native int getPointCount(int aContourIndex);
    /** Gets a point given its contour and point indexes. */
    public native void getPoint(int aContourIndex,int aPointIndex,PathPoint aPoint);
    /** Returns true if the path is closed. */
    public native boolean isClosed();

    /** Gets the label or name of the object. */
    public native String getLabel();
    /** Gets the integer attribute selected by aIndex. */
    public native int getIntegerAttribute(int aIndex);
    /** Gets the number of integer attributes. */
    public native int getIntegerAttributeCount();
    /** Gets a string attribute. If aName is null this function is the same as GetLabel. */
    public native String getStringAttribute(String aName);
    /**
    Gets all the string attributes as an array of key-value pairs.
    An empty key indicates that the attribute is the label.
    Returns null if there are no string attributes.
    */
    public native String[] getStringAttributes();
    /** Gets the ID of the object. */
    public native long getId();
    /** Gets the type of the object. */
    public native int getType();
    /** Gets the name of the object's layer. */
    public native String getLayerName();

    /** Gets the area of a map object in square meters. Returns 0 if it is a point or a line. */
    public native double getArea();
    /** Gets the length or perimeter of a map object in meters. Returns 0 if it is a point. */
    public native double getLengthOrPerimeter();
    /**
    Gets the center of a map object, defined as the center of gravity for polygons and arrays,
    half way along the path for lines, and the point itself for points. The returned point
    is in map coordinates.
    */
    public native void getCenter(PathPoint aPoint);
    /** Gets the axis-aligned bounds of the map object, in map coordinates. */
    public native void getBounds(Rect aRect);
    /**
    Gets the distance from a map object to the route, if known, or < 0 if unknown.
    This function uses the internal map attribute _d0, set by Framework.getNearbyObjects.
    */
    public native double getDistanceToRoute();
    /**
    Gets the distance from the current point to the on-route point nearest to a map object, along the route, if known,
    or < 0 if unknown. This function uses the internal map attribute _d1, set by Framework.getNearbyObjects.
    */
    public native double getDistanceAlongRoute();
    /**
    Gets a string summarizing a geocode for a map object.

    The summary is a comma-separated list of all the non-empty fields in the corresponding address,
    as returned by GetAddress. The postcode if any is enclosed in square brackets.

    aMaxItems gives the maximum number of items to be used, including the main item.

    If aLocale is non-null it is used when getting names.
    If aLocale is null or empty, the locale set by Framework.setLocale is used.

    Calls to this function are limited in number in unlicensed (evaluation) versions of the CartoType library.
    */
    public native String getGeoCodeSummary(int aMaxItems,String aLocale);
    /**
    Gets an address for a map object.

    If aLocale is non-null it is used when getting names.
    If aLocale is null or empty, the locale set by Framework.setLocale is used.

    Calls to this function are limited in number in unlicensed (evaluation) versions of the CartoType library.

    Returns a CartoType error code: 0 = success.
    */
    public native int getAddress(Address aAddress,String aLocale);

    /** Information returned by GetMatch. */
    public static class Match
        {
        /** The name of the attribute in which the matched text was found. */
        public String iKey;
        /** The value of the attribute in which the matched text was found. */
        public String iValue;
        /** The start position of the matched text within the value. */
        public int iStart;
        /** The end position of the matched text within the value. */
        public int iEnd;
        }

    /**
    Finds the first string attribute matching aText, using aMatchMethod, and returns information about it in aMatch.
    aMatchMethod uses the constants Framework.EXACT_STRING_MATCH_METHOD, etc.
    If aAttributes is non-null this function examines only attributes in the supplied comma-separated list,
    otherwise it examines all attributes except those starting with an underscore, which by convention are non-textual.
    Attribute names may contain the wild cards ? and *.
    If aPhrase is true, matches of phrases within the attribute are allowed.

    Returns a CartoType error code: 0 = success.
    */
    public native int getMatch(Match aMatch,String aText,int aMatchMethod,String aAttributes,boolean aPhrase);

    protected void finalize()
        {
        destroy();
        }
    static
        {
        initGlobals();
        }

    private static native void initGlobals();
    private native void destroy();

    private long iObjectRef;
    private long iFrameworkRef;
    }
