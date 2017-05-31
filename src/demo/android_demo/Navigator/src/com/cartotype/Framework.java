/*
Framework.java
Copyright (C) 2012-2015 CartoType Ltd.
See www.cartotype.com for more information.
*/

package com.cartotype;

import java.nio.ByteBuffer;

import android.graphics.Bitmap;

/**
\mainpage CartoType

\section introduction Introduction

CartoType is a portable C++ library for drawing maps, calculating routes
and providing navigation instructions. It uses map files created using the
generate_map_data_type1 tool supplied with the package.

\section javaapi The Java API

The Java API gives you access to CartoType functionality from Java.

CartoType.Framework is the most important class. Your application
can create a single Framework object and use it to create and manage a map.
It provides access to everything you need, including
map drawing, hit detection, searching, routing and navigation.
*/

/** The Framework class owns and manages the CartoType engine and the maps, fonts and style sheets. */
public class Framework
    {
    /** A constant for coordinates in degrees longitude (x) and latitude (y). */
    public static final int DEGREE_COORDS = 0;
    /** A constant for coordinates in screen pixels. */
    public static final int SCREEN_COORDS = 1;
    /** A constant for coordinates in map units. */
    public static final int MAP_COORDS = 2;
    /** A constant for coordinates in projected meters, not correcting for distortion introduced by the projection. */
    public static final int MAP_METER_COORDS = 3;

    /** A constant for aValidity in navigate() indicating that the time is valid. */
    public static final int TIME_VALID = 1;
    /** A constant for aValidity in navigate() indicating that the position is valid. */
    public static final int POSITION_VALID = 2;
    /** A constant for aValidity in navigate() indicating that the speed is valid. */
    public static final int SPEED_VALID = 4;
    /** A constant for aValidity in navigate() indicating that the course (bearing) is valid. */
    public static final int COURSE_VALID = 8;
    /** A constant for aValidity in navigate() indicating that the height is valid. */
    public static final int HEIGHT_VALID = 16;

    /** A flag for aStringMatchMethod in find() indicating a search for all strings starting with the target, ignoring case. */
    public static final int PREFIX_STRING_MATCH_FLAG = 1;
    /** A flag for aStringMatchMethod in find() indicating that non-alphanumeric characters should be ignored. */
    public static final int IGNORE_NON_ALPHANUMERICS_STRING_MATCH_FLAG = 2;
    /** A flag for aStringMatchMethod in find() indicating that accented and unaccented characters should compare equal. */
    public static final int FOLD_ACCENTS_STRING_MATCH_FLAG = 4;
    /** A flag for aStringMatchMethod in find() indicating that fuzzy matching should be done. */
    public static final int FUZZY_STRING_MATCH_FLAG = 8;
    /** A flag for aStringMatchMethod in find() indicating that fuzzy matching should be done. */
    public static final int FOLD_CASE_STRING_MATCH_FLAG = 16;
    /** A flag to enable fast searching: just get the first matches, not the most relevant ones. */
    public static final int FAST_STRING_MATCH_FLAG = 32;

    /** A constant for aStringMatchMethod in find() indicating an exact match, ignoring case. */
    public static final int EXACT_STRING_MATCH_METHOD = 0;
    /** A constant for aStringMatchMethod in find() indicating an exact match, ignoring case. */
    public static final int FOLD_CASE_STRING_MATCH_METHOD = FOLD_CASE_STRING_MATCH_FLAG;
    /** A constant for aStringMatchMethod in find() indicating a match of the string with the start of the target, ignoring case. */
    public static final int START_STRING_MATCH_METHOD = PREFIX_STRING_MATCH_FLAG;
    /** A constant for aStringMatchMethod in find() indicating a match of the string with the start of the target, ignoring case. */
    public static final int FUZZY_STRING_MATCH_METHOD = IGNORE_NON_ALPHANUMERICS_STRING_MATCH_FLAG | FOLD_ACCENTS_STRING_MATCH_FLAG | FUZZY_STRING_MATCH_FLAG | FOLD_CASE_STRING_MATCH_FLAG;

    /** A constant for a map type used in createWritableMap(): a writable map stored in memory. */
    public static final int MEMORY_WRITABLE_MAP_TYPE = 0;
    /** A constant for a map type used in createWritableMap(): a writable map stored in an SQLite database. */
    public static final int SQLITE_WRITABLE_MAP_TYPE = 1;

    /** A file type constant: PNG (Portable Network Graphics) image files. */
    public static final int PNG_FILE_TYPE = 0;
    /** A file type constant: JPEG (Joint Photographic Expert Group) image files. */
    public static final int JPEG_FILE_TYPE = 1;
    /** A file type constant: TIFF (Tagged Image File Format) image files. */
    public static final int TIFF_FILE_TYPE = 2;
    /** A file type constant: CTM1 (CartoType Map Data Type 1) map data files. */
    public static final int CTM1_FILE_TYPE = 3;
    /** A file type constant: CTMS (CartoType Map Data, Serialized) map data files. */
    public static final int CTMS_FILE_TYPE = 4;
    /** A file type constant: KML (Keyhole Markup Language) map data files. */
    public static final int KML_FILE_TYPE = 5;
    /** A file type constant: CTSQL (CartoType SQLite format) map data files. */
    public static final int CTSQL_FILE_TYPE = 6;

    /** An address part constant to select building names or numbers. */
    public static final int ADDRESS_PART_BUILDING = 0;
    /** An address part constant to select features or points of interest. */
    public static final int ADDRESS_PART_FEATURE = 1;
    /** An address part constant to select streets or roads. */
    public static final int ADDRESS_PART_STREET = 2;
    /** An address part constant to select sub-localities: suburbs and neighborhoods. */
    public static final int ADDRESS_PART_SUB_LOCALITY = 3;
    /** An address part constant to select localities: cities, towns and villages. */
    public static final int ADDRESS_PART_LOCALITY = 4;
    /** An address part constant to select islands. */
    public static final int ADDRESS_PART_ISLAND = 5;
    /** An address part constant to select lower-level administrative areas. */
    public static final int ADDRESS_PART_SUB_ADMIN_AREA = 6;
    /** An address part constant to select higher-level administrative areas. */
    public static final int ADDRESS_PART_ADMIN_AREA = 7;
    /** An address part constant to select countries. */
    public static final int ADDRESS_PART_COUNTRY = 8;
    /** An address part constant to select postal codes. */
    public static final int ADDRESS_PART_POST_CODE = 9;

    /** The maximum number of different routes which can be calculated using route profiles and displayed simultaneously. */
    public static final int MAX_ROUTES_DISPLAYED = 16;
    /** The maximum number of different waypoints which can be displayed; in practice there are unlikely to be more than 100. */
    public static final int MAX_WAYPOINTS_DISPLAYED = 1024 * 1024;
    /** The first ID reserved for route-related map objects. */
    public static final long ROUTE_ID_RESERVED_START = 0x40000000;
    /** A reserved map object ID for the first route. */
    public static final long FIRST_ROUTE_ID = ROUTE_ID_RESERVED_START;
    /** A reserved map object ID for the last route. */
    public static final long LAST_ROUTE_ID = ROUTE_ID_RESERVED_START + MAX_ROUTES_DISPLAYED - 1;
    /** A reserved map object ID for the start of the route. */
    public static final long ROUTE_START_ID = LAST_ROUTE_ID + 1;
    /** A reserved map object ID for the end of the route. */
    public static final long ROUTE_END_ID = LAST_ROUTE_ID + 2;
    /** A reserved map object ID for a point object at the current position on the route. */
    public static final long ROUTE_POSITION_ID = LAST_ROUTE_ID + 3;
    /** A reserved map object ID for a vector object centered on the current position on the route. */
    public static final long ROUTE_VECTOR_ID = LAST_ROUTE_ID + 4;
    /** A reserved map object ID for the first waypoint. */
    public static final long FIRST_WAYPOINT_ID = LAST_ROUTE_ID + 5;
    /** One more than the last ID reserved for route-related map objects. */
    public static final long ROUTE_ID_RESERVED_END = FIRST_WAYPOINT_ID + MAX_WAYPOINTS_DISPLAYED;

    /** A flag to make the center of the map follow the user's location. */
    public static final int FOLLOW_FLAG_LOCATION = 1;
    /** A flag to rotate the map to the user's heading. */
    public static final int FOLLOW_FLAG_HEADING = 2;
    /* A flag to set the map to a suitable zoom level for the user's speed. */
    public static final int FOLLOW_FLAG_ZOOM = 4;
    /** The map does not follow the user's location or heading. */
    public static final int FOLLOW_MODE_NONE = 0;
    /** The map is centred on the user's location. */
    public static final int FOLLOW_MODE_LOCATION = FOLLOW_FLAG_LOCATION;
    /** The map is centred on the user's location and rotated to the user's heading. */
    public static final int FOLLOW_MODE_LOCATION_HEADING = FOLLOW_FLAG_LOCATION | FOLLOW_FLAG_HEADING;
    /** The map is centred on the user's location and zoomed to a suitable level for the user's speed. */
    public static final int FOLLOW_MODE_LOCATION_ZOOM = FOLLOW_FLAG_LOCATION | FOLLOW_FLAG_ZOOM;
    /** The map is centred on the user's location, rotated to the user's heading, and zoomed to a suitable level for the user's speed. */
    public static final int FOLLOW_MODE_LOCATION_HEADING_ZOOM = FOLLOW_FLAG_LOCATION | FOLLOW_FLAG_HEADING | FOLLOW_FLAG_ZOOM;

    /** Navigation state: unknown state, or not navigating. */
    public static final int NAVIGATION_STATE_NONE = 0;
    /** Navigation state: a turn is approaching. */
    public static final int NAVIGATION_STATE_TURN = 1;
    /**
    Navigation state: turn round at a legal and safe place and go back in the reverse direction.
    This navigation state occurs when the vehicle is on the correct route but going the wrong way.
    */
    public static final int NAVIGATION_STATE_TURN_ROUND = 2;
    /**
    Navigation state: the vehicle is off the route and a new route has to be calculated.
    */
    public static final int NAVIGATION_STATE_NEW_ROUTE = 3;
    /**
    Navigation state: the end of the route is within one kilometre and there are no more junctions.
    */
    public static final int NAVIGATION_STATE_ARRIVAL = 4;
    /**
    Navigation state: the current position is off the route (further than a certain tolerated distance)
    and a new route has not yet been calculated.
    */
    public static final int NAVIGATION_STATE_OFF_ROUTE = 5;
    /**
    Navigation state: re-routing is needed and will be done when the next valid position is supplied.
    Occurs only if auto-re-routing is turned off.
    */
    public static final int NAVIGATION_STATE_RE_ROUTE_NEEDED = 6;

    /**
    Create a CartoType framework containing engine, map, style sheet and fonts.
    Return an opaque reference to it.
    */
    public Framework(String aMapFileName,String aStyleSheetFileName,String aFontFileName,int aViewWidth,int aViewHeight)
        {
        iFrameworkRef = create(aMapFileName,null,aStyleSheetFileName,aFontFileName,aViewWidth,aViewHeight);
        if (iFrameworkRef == 0)
            throw new RuntimeException("failed to create CartoType Framework object");
        }

    /**
    Create a CartoType framework containing engine, map, style sheet and fonts.
    If aMapEncryptionKey is non-null the map must have been encrypted using it.
    Return an opaque reference to it.
    */
    public Framework(String aMapFileName,byte[] aMapEncryptionKey,String aStyleSheetFileName,String aFontFileName,int aViewWidth,int aViewHeight)
        {
        iFrameworkRef = create(aMapFileName,aMapEncryptionKey,aStyleSheetFileName,aFontFileName,aViewWidth,aViewHeight);
        if (iFrameworkRef == 0)
            throw new RuntimeException("failed to create CartoType Framework object");
        }

    /** Creates a CartoType framework from parameters contained in a FrameworkParam object. */
    public Framework(FrameworkParam aParam)
        {
        iFrameworkRef = createWithParam(aParam);
        if (iFrameworkRef == 0)
            throw new RuntimeException("failed to create CartoType Framework object");
        }

    /**
    Loads another map, in addition to the main one loaded by the constructor.
    Return a CartoType error code: 0 = success.
    */
    public int loadMap(String aMapFileName)
        {
        return loadMap(aMapFileName,null);
        }

    /**
    Loads another map, in addition to the main one loaded by the constructor.
    If aMapEncryptionKey is non-null the map must have been encrypted using it.
    Returns a CartoType result code: 0 = success.
    */
    public native int loadMap(String aMapFileName,byte[] aMapEncryptionKey);

    /**
    Creates a writable (editable) map of the specified type and loads it.
    Only the type MEMORY_WRITABLE_MAP_TYPE is currently supported.
    Returns a CartoType result code: 0 = success.
    */
    public native int createWritableMap(int aType);

    /**
    Saves a map identified by its handle by writing it the specified format.
    Only writable map databases may be saved in this way.
    Only the CTMS_FILE_TYPE (CartoType Map Serialized)
    and KML_FILE_TYPE (Keyhole Markup Language) file types are currently supported.
    Returns a CartoType result code: 0 = success.
    */
    public native int saveMap(int aHandle,String aFileName,int aFileType);

    /**
    Reads map data from aFileName in the format given by aFileType and merges it into the map identified by aHandle,
    which must refer to a writable map database.
    Only the CTMS_FILE_TYPE (CartoType Map Serialized)
    and KML_FILE_TYPE (Keyhole Markup Language) file types are currently supported.
    Returns a CartoType result code: 0 = success.
    */
    public native int readMap(int aHandle,String aFileName,int aFileType);

    /**
    Writes a map image to a file of the specified type.
    Only PNG_FILE_TYPE is supported.
    Returns a CartoType result code: 0 = success.
    */
    public native int writeMapImage(String aFileName,int aFileType);

    /** Returns true if a map, identified by its handle, is known to be empty or does not exist. */
    public native boolean mapIsEmpty(int aHandle);

    /** Returns the number of maps currently loaded, including the main map and the in-memory map. */
    public native int mapCount();

    /**
    Unloads a map, selecting it by handle.
    It is illegal to unload the main map database or the in-memory database.
    Returns a CartoType result code: 0 = success.
    */
    public native int unloadMap(int aHandle);

    /**
    Enables or disables a map, selecting it by handle.
    Returns a CartoType result code: 0 = success.
    */
    public native int enableMap(int aHandle,boolean aEnable);

    /** Gets the handle of the map most recently loaded, or created using CreateWritableMap. */
    public native int getLastMapHandle();

    /** Gets the handle of the main map. */
    public native int getMainMapHandle();

    /** Gets the handle of the in-memory map used for routing. */
    public native int getMemoryMapHandle();

    /**
    Load a font in addition to any fonts already loaded.
    Return a CartoType error code: 0 = success.
    */
    public native int loadFont(String aFontFileName);

    /**
    Set a style sheet by loading it from a file.
    The index aIndex determines which style sheet to replace: 0 means the
    main style sheet, and numbers from 1 up refer to extra style sheets.
    Return a CartoType error code: 0 = success.
    */
    public int setStyleSheet(String aStyleSheetFileName,int aIndex)
        {
        return setStyleSheet(aStyleSheetFileName,null,aIndex);
        }

    /**
    Set a style sheet by loading it from data in memory.
    The index aIndex determines which style sheet to replace: 0 means the
    main style sheet, and numbers from 1 up refer to extra style sheets.
    Return a CartoType error code: 0 = success.
    */
    public int setStyleSheet(byte[] aData,int aIndex)
        {
        return setStyleSheet(null,aData,aIndex);
        }

    private native int setStyleSheet(String aStyleSheetFileName,byte[] aData,int aIndex);

    /**
    Reload a sheet from the file it was originally loaded from.
    This function can be used to apply changes after a style sheet is modified.
    The index aIndex determines which style sheet is reloaded: 0 means the
    main style sheet, and numbers from 1 up refer to extra style sheets.
    Only style sheets loaded from files can be reloaded.
    Return a CartoType error code: 0 = success.
    */
    public native int reloadStyleSheet(int aIndex);

    /**
    Load an extra style sheet from a file.
    Extra style sheets are compiled after the main style sheet.
    Return a CartoType error code: 0 = success.
    */
    public int appendStyleSheet(String aStyleSheetFileName)
        {
        return appendStyleSheet(aStyleSheetFileName,null);
        }

    /**
    Load an extra style sheet from data in memory.
    Extra style sheets are compiled after the main style sheet.
    Return a CartoType error code: 0 = success.
    */
    public int appendStyleSheet(byte[] aData)
        {
        return appendStyleSheet(null,aData);
        }

    private native int appendStyleSheet(String aStyleSheetFileName,byte[] aData);

    /**
    Delete the style sheet with the specified index.
    The main style sheet (index 0) may not be deleted.
    Other style sheets have indexes from 1 up, in the order of loading.
    Return a CartoType error code: 0 = success.
    */
    public native int deleteStyleSheet(int aIndex);

    /** Resizes the map view, supplying the new view width and height in pixels. */
    public int resize(int aViewWidth,int aViewHeight)
        {
        iBitmap = null;
        return resizeInternal(aViewWidth,aViewHeight);
        }

    private native int resizeInternal(int aViewWidth,int aViewHeight);

    /** Enable or disable a named map layer. */
    public native int enableLayer(String aLayerName,boolean aEnable);

    /** Set the resolution in dots per inch. */
    public native int setResolutionDpi(int aDpi);

    /**
    Sets the map's scale denominator.
    Returns a CartoType error code: 0 = success.
    */
    public native int setScale(int aScale);

    /** Gets the map's scale denominator. */
    public native int getScale();

    /**
    Configures the CartoType framework by reading an XML configuration file.
    */
    public native int configure(String aFileName);

    /**
    Sets the map's scale denominator in the current view,
    compensating for local scale distortion caused by the projection.
    Return a CartoType error code: 0 = success.
    */
    public native int setScaleDenominatorInView(int aScale);

    /**
    Gets the map's scale denominator for the center of the current view,
    compensating for local scale distortion caused by the projection.
    */
    public native int getScaleDenominatorInView();

    /**
    Gets the distance between two points.
    aCoordType may be DEGREE_COORDS, SCREEN_COORDS, MAP_COORDS or MAP_METER_COORDS.
    */
    public native double getDistanceInMeters(double aX1,double aY1,double aX2,double aY2,int aCoordType);

    private native void getMapInternal(byte[] aImage,int aMapType);

    /**
    Gets the current map as a 32bpp RGBA bitmap.
    The returned image is valid till the next call to getMap or getMemoryDataBaseMap.
    */
    public Bitmap getMap()
        {
        return getMapHelper(0);
        }

    /**
    Gets an image showing objects from the in-memory database only as a 32bpp RGBA bitmap.
    The returned image is valid till the next call to getMap or getMemoryDataBaseMap.
    */
    public Bitmap getMemoryDataBaseMap()
        {
        return getMapHelper(1);
        }

    private Bitmap getMapHelper(int aMapType)
        {
        if (iBitmap == null)
            {
            double p[] = new double[6];
            getViewDimensions(p,SCREEN_COORDS);
            int w = (int)p[4];
            int h = (int)p[5];
            iBitmap = Bitmap.createBitmap(w,h,Bitmap.Config.ARGB_8888);
            iBitmapData = new byte[w * 4 * h];
            iBuffer = ByteBuffer.wrap(iBitmapData);
            }
        getMapInternal(iBitmapData,aMapType);
        iBuffer.rewind();

        /*
        Hack: we have to change the bitmap because the Android hardware-accelerated canvas will not
        re-upload the bitmap unless it has a different generation number,
        and copyPixelsFromBuffer doesn't change the generation number.
        See http://osdir.com/ml/Android-Developers/2011-10/msg02215.html.
        */
        iBitmap.setPixel(0,0,0);

        iBitmap.copyPixelsFromBuffer(iBuffer);
        return iBitmap;
        }

    private native void getTileInternal(byte[] aImage,int aTileSizeInPixels,int aZoom,int aX,int aY);

    /**
    Get an image containing a tile of the required coordinates and size, using OpenStreetMap tile coordinates.
    The returned image is valid till the next call to getTileBitmap.
    This function works properly only if the map is in the Web Mercator projection.
    */
    public Bitmap getTileBitmap(int aTileSizeInPixels,int aZoom,int aX,int aY)
        {
        if (aTileSizeInPixels <= 0)
            return null;

        if (iTileBitmap == null || iTileBitmap.getWidth() != aTileSizeInPixels)
            {
            iTileBitmap = Bitmap.createBitmap(aTileSizeInPixels,aTileSizeInPixels,Bitmap.Config.ARGB_8888);
            iTileBitmapData = new byte[aTileSizeInPixels * 4 * aTileSizeInPixels];
            iTileBuffer = ByteBuffer.wrap(iTileBitmapData);
            }
        getTileInternal(iTileBitmapData,aTileSizeInPixels,aZoom,aX,aY);
        iTileBuffer.rewind();

        /*
        Hack: we have to change the bitmap because the Android hardware-accelerated canvas will not
        re-upload the bitmap unless it has a different generation number,
        and copyPixelsFromBuffer doesn't change the generation number.
        See http://osdir.com/ml/Android-Developers/2011-10/msg02215.html.
        */
        iTileBitmap.setPixel(0,0,0);

        iTileBitmap.copyPixelsFromBuffer(iTileBuffer);
        return iTileBitmap;
        }

    /**
    Enables or disables clipping the map background to the map bounds.
    Use this when creating map tiles to be overlaid on a larger map,
    so that the background (sea) color does not erase the larger map's graphics.
    The return value is the previous state.
    */
	public native boolean clipBackgroundToMapBounds(boolean aEnable);

    /**
    Sets the number of zoom levels by which to zoom out when creating a tile bitmap
    returned by getTileBitmap().

    The number may be 0, 1, or 2. If it is 0, each tile is self-contained. If it is 1,
    each tile is cut from a larger tile containing four tiles. If it is 2, each tile
    is cut from a larger tile containing sixteen tiles. The values 1 and 2 are useful
    in allowing longer labels to be used when creating small tiles; if not, many
    street labels will not be drawn because the street is cut in two by the tile border
    and the resulting sections are too short.

    The return value is the previous setting.
    */
	public native int setTileOverSizeZoomLevels(int aLevels);

    /**
    Enables or disables the drawing of the in-memory database when the rest of the map is
    drawn. The in-memory database is used for the route, its start and end, and the current
    vehicle position. If drawing the in-memory database is disabled, you can get a separate
    image containing the objects from the in-memory database by calling getMemoryDataBaseMapBitmap.
    */
    public native void enableDrawingMemoryDataBase(boolean aEnable);

    /**
    Load an icon from a file. It will be used whenever the ID aId is referenced
    in the style sheet, overriding any icon specified in the style sheet.

    The icon file may be in PNG or JPEG format.

    (aHotSpotX,aHotSpotY) is the point, relative to the top left corner of the icon, that should be
    made to coincide with the point of interest marked by the icon.

    (aLabelX,aLabelY) is the point, relative to the hot spot, at which the center
    of the baseline of any label text is placed.

    aLabelMaxLength is the maximum length of label text in pixels.
    Labels too long to be drawn on the icon are drawn next to it.

    Return a CartoType error code: 0 = success.
    */
    public native int loadIcon(String aFileName,String aId,int aHotSpotX,int aHotSpotY,int aLabelX,int aLabelY,int aLabelMaxLength);

    /**
    Unload an icon previously loaded using LoadIcon.

    Return a CartoType error code: 0 = success.
    */
    public native int unloadIcon(String aId);

    /** Turn perspective view on or off. */
    public native void setPerspective(boolean aPerspective);

    /** Return true if the perspective view is turned on, false otherwise. */
    public native boolean getPerspective();

    /** Zoom by the given factor. Return a CartoType error code: 0 = success. */
    public native int zoom(double aZoomFactor);

    /** Zoom in by a factor of 2. Return a CartoType error code: 0 = success. */
    public native int zoomIn();

    /** Zoom out by a factor of 2. Return a CartoType error code: 0 = success. */
    public native int zoomOut();

    /** Rotate the map about its center by an angle given in degrees. */
    public native void rotate(double aAngle);

    /** Set the map's orientation to an absolute angle given in degrees. Zero = north-up. */
    public native void setRotation(double aAngle);

    /** Get the current rotation angle of the map in degrees. */
    public native double getRotation();

    /** Pans (slides the map) by a certain distance in pixels, horizontally or vertically or both. */
    public native void pan(int aDx,int aDy);

    /** Sets the center of the view to a point given in degrees longitude and latitude. */
    public native int setViewCenterLatLong(double aLong,double aLat);

    /** Set the view to show a certain rectangle, with a margin in pixels, and at a minimum scale. */
    public int setView(double aX1,double aY1,double aX2,double aY2,int aCoordType,int aMarginInPixels,int aMinScaleDenominator)
        {
        return setViewRect(aX1,aY1,aX2,aY2,aCoordType,aMarginInPixels,aMinScaleDenominator);
        }
    private native int setViewRect(double aX1,double aY1,double aX2,double aY2,int aCoordType,int aMarginInPixels,int aMinScaleDenominator);

    /** Set the view to show a single map object, with a margin in pixels, and at a minimum scale. */
    public int setView(MapObject aMapObject,int aMarginInPixels,int aMinScaleDenominator)
        {
        return setViewObject(aMapObject,aMarginInPixels,aMinScaleDenominator);
        }
    private native int setViewObject(MapObject aMapObject,int aMarginInPixels,int aMinScaleDenominator);

    /** Set the view to show a group of map objects, with a margin in pixels, and at a minimum scale. */
    public int setView(MapObject[] aMapObjectArray,int aMarginInPixels,int aMinScaleDenominator)
        {
        return setViewObjects(aMapObjectArray,aMarginInPixels,aMinScaleDenominator);
        }
    private native int setViewObjects(MapObject[] aMapObjectArray,int aMarginInPixels,int aMinScaleDenominator);

    /**
    Get the view dimensions.
    If aPoint has two or more members put the view center in aPoint[0] and aPoint[1].
    If aPoint has six or more members, also put the view rectangle in aPoint[2] ... aPoint[5]
    in the order left, top, right, bottom.
    */
    public native void getViewDimensions(double[] aPoint,int aCoordType);

    /**
    Gets the extent of the map.
    If aPoint has two or more members put the view center in aPoint[0] and aPoint[1].
    If aPoint has six or more members, also put the view rectangle in aPoint[2] ... aPoint[5]
    in the order left, top, right, bottom.
    */
    public native void getMapExtent(double[] aPoint,int aCoordType);

    /** Gets the current map projection as a proj.4 parameter string if possible. If not returns the empty string. */
    public native String getProjectionAsProj4Param();

    /**
    Returns the integer ID of the last object inserted into a writable map.
    */
    public long getLastObjectId()
        {
        return iLastObjectId;
        }

    /**
    Inserts a map object into one of the writable maps, identifying the map by its handle.
    Uses the supplied ID if aId is non-zero, otherwise generates an ID which can be retrieved using getLastObjectId().
    This function fails if the map is not writable.
    If aReplace is true and aId is a duplicate of an existing object, the existing object is replaced, otherwise a duplicate
    causes the function to fail.
    Returns a CartoType result code: 0 = success.
    */
    public native int insertMapObject(int aMapHandle,int aMapObjectType,String aLayerName,Geometry aGeometry,
                                      String aStringAttributes,int aIntAttribute,long aId,boolean aReplace);

    /**
    Inserts a point map object into one of the writable maps, identifying the map by its handle.
    Uses the supplied ID if aId is non-zero, otherwise generates an ID which can be retrieved using getLastObjectId().
    This function fails if the map is not writable.
    If aReplace is true and aId is a duplicate of an existing object, the existing object is replaced, otherwise a duplicate
    causes the function to fail.
    Returns a CartoType result code: 0 = success.
    */
    public native int insertPointMapObject(int aMapHandle,String aLayerName,double aX,double aY,int aCoordType,
                                           String aStringAttributes,int aIntAttribute,long aId,boolean aReplace);

    /**
    Inserts a circular map object into one of the writable maps, identifying the map by its handle.
    The circle is defined by its center and radius, which may have different coordinate types.
    It is often convenient to supply a point in degrees and its radius in map meters.
    Uses the supplied ID if aId is non-zero, otherwise generates an ID which can be retrieved using getLastObjectId().
    This function fails if the map is not writable.
    If aReplace is true and aId is a duplicate of an existing object, the existing object is replaced, otherwise a duplicate
    causes the function to fail.
    Returns a CartoType result code: 0 = success.
    */
    public native int insertCircleMapObject(int aMapHandle,String aLayerName,double aCenterX,double aCenterY,int aCenterCoordType,
                                            double aRadius,int aRadiusCoordType,
                                            String aStringAttributes,int aIntAttribute,long aId,boolean aReplace);

    /**
    Inserts an envelope map object into one of the writable maps, identifying the map by its handle.
    An envelope is a continuous curve at a certain distance from some geometry, and can be regarded as
    a stroke of width aRadius drawn along a path.
    It is often convenient to supply the geometry in degrees and the radius in map meters.
    Uses the supplied ID if aId is non-zero, otherwise generates an ID which can be retrieved using getLastObjectId().
    This function fails if the map is not writable.
    If aReplace is true and aId is a duplicate of an existing object, the existing object is replaced, otherwise a duplicate
    causes the function to fail.
    Returns a CartoType result code: 0 = success.
    */
    public native int insertEnvelopeMapObject(int aMapHandle,String aLayerName,Geometry aGeometry,
                                              double aRadius,int aRadiusCoordType,
                                              String aStringAttributes,int aIntAttribute,long aId,boolean aReplace);

    /**
    Inserts an object by copying an existing object. If aEnvelopeRadius is greater than zero,
    converts the object to an envelope using the specified radius.
    Uses the supplied ID if aId is non-zero, otherwise generates an ID which can be retrieved using getLastObjectId().
    This function fails if the map is not writable.
    If aReplace is true and aId is a duplicate of an existing object, the existing object is replaced, otherwise a duplicate
    causes the function to fail.
    Returns a CartoType result code: 0 = success.
    */
    public native int insertCopyOfMapObject(int aMapHandle,String aLayername,MapObject aMapObject,double aEnvelopeRadius,
                                            int aRadiusCoordType,long aId,boolean aReplace);

    /**
    Deletes map objects from the writable map identified by aMapHandle. Objects with IDs in the range aStartId...aEndId
    inclusive are deleted. If aCondition is non-null and non-empty, only objects fulfilling the condition, which is
    a style sheet expression, are deleted.
    Returns the number of objects deleted.
    */
    public native int deleteMapObjects(int aMapHandle,long aStartId,long aEndId,String aCondition);

    /** Loads a map object from a map. Returns null if the object doesn't exist. */
    public native MapObject loadMapObject(int aMapHandle,long aObjectId);

    /** Calculates the area of a polygon as drawn in the current map. */
    public native double getPolygonArea(double[] aX,double[] aY,int aCoordType);

    /**
    Sets a style sheet variable to a string value.
    If the string value is null, clears the style sheet variable.
    Returns a CartoType error code: 0 = success.
    */
    public native int setStyleSheetVariable(String aVariableName,String aValue);

    /**
    Sets a style sheet variable to an integer value.
    Returns a CartoType error code: 0 = success.
    */
    public int setStyleSheetVariable(String aVariableName,int aValue)
        {
        return setStyleSheetVariable(aVariableName,Integer.toString(aValue));
        }

    /**
    Starts navigating from one point to another.
    Creates the route and displays it.
    Returns a CartoType error code: 0 = success.
    */
    public int startNavigation(double aStartX,double aStartY,int aStartCoordType,
                               double aEndX,double aEndY,int aEndCoordType)
        {
        double s[] = new double[2];
        s[0] = aStartX;
        s[1] = aStartY;
        convertCoords(s,aStartCoordType,MAP_COORDS);
        double e[] = new double[2];
        e[0] = aEndX;
        e[1] = aEndY;
        convertCoords(e,aEndCoordType,MAP_COORDS);

        // Put the X coords in s[] and the Y coords in e[].
        double temp = s[1];
        s[1] = e[0];
        e[0] = temp;
        return startNavigation(s,e,MAP_COORDS);
        }

    /**
    Starts navigating through a series of points.
    Creates the route and displays it.
    Returns a CartoType error code: 0 = success.
    */
    public native int startNavigation(double[] aX,double[] aY,int aCoordType);

    /** Creates a route without starting navigation, supplying a route profile and waypoints. */
    public Route createRoute(RouteProfile aProfile,double[] aX,double[] aY,int aCoordType)
        {
        long route_ref = createRouteHelper(false,aProfile,aX,aY,aCoordType,false,false,0);
        if (route_ref == 0)
            return null;
        return new Route(route_ref);
        }

    /**
    Creates an optimized route to visit a series of waypoints in any order.
    If aStartFixed is true, the first point in the route is always the first point supplied.
    If aEndFixed is true, the last point in the route is always the last point supplied.
    To create a circular route starting and ending at a predetermined place, but visiting the other points
    in any order, set aStartFixed and aEndFixed true, and make the first and last point the same.
    aIterations is the number of different routes to be tried.
    */
    public Route createBestRoute(RouteProfile aProfile,double[] aX,double[] aY,int aCoordType,
                                 boolean aStartFixed,boolean aEndFixed,int aIterations)
        {
        long route_ref = createRouteHelper(true,aProfile,aX,aY,aCoordType,aStartFixed,aEndFixed,aIterations);
        if (route_ref == 0)
            return null;
        return new Route(route_ref);
        }

    private native long createRouteHelper(boolean aBest,RouteProfile aProfile,double[] aX,double[] aY,int aCoordType,
                                          boolean aStartFixed,boolean aEndFixed,int aIterations);

    /**
    Stops navigating. Current routes remain in existence.
    Navigation can be resumed using startNavigation
    or enableNavigation.
    */
    public native void endNavigation();

    /**
    Enables or disables navigation. When navigation is disabled,
    the follow modes work but no navigation is performed even if there is a route.
    The return value is the previous state.
    */
    public native boolean enableNavigation(boolean aEnable);

    /** Returns true if navigation is enabled, false otherwise. */
    public native boolean navigationEnabled();

    /**
    Sets the main routing profile. If you do not call this function, the
    standard profile for private car routing is used.

    This function has no effect if routing is performed on a map
    with a built-in contraction-hierarchy routing table (created using makemap /route=ch),
    because such a table is created using a fixed routing profile which can be set in makemap.
    */
    public native void setMainProfile(RouteProfile aProfile);

    /**
    Adds a new routing profile. A Framework object starts with a single
    routing profile. Adding new profiles allows several different routes to be
    calculated for the same start and end point.

    This function has no effect if routing is performed on a map
    with a built-in contraction-hierarchy routing table (created using makemap /route=ch),
    because such a table is created using a fixed routing profile which can be set in makemap.

    Returns a CartoType error code: 0 = success.
    */
    public native int addProfile(RouteProfile aProfile);

    /**
    Chooses one among a set of alternative routes. The index aRouteIndex selects the route:
    index 0 is the route created from the main profile. Indexes from 1 upwards select routes
    created using profiles added using addProfile(), in order of their addition.

    Returns a CartoType error code: 0 = success.

    This function is most appropriately called after calling startNavigation() to create the routes,
    but before calling navigate() to supply a position.
    */
    public native int chooseRoute(int aRouteIndex);

    /**
    If aEnable is true, displays the current route and a maximum of 2 alternative routes.
    If not, removes the current route from the display.
    */
    public native int displayRoute(boolean aEnable);

    /**
    Returns the number of routes in existence.
    */
    public native int getRouteCount();

    /**
    Gets one of the current routes.
    Returns null if the selected route does not exist.
    */
    public Route getRoute(int aIndex)
        {
        long route_ref = copyRoute(aIndex);
        if (route_ref == 0)
            return null;
        return new Route(route_ref);
        }

    private native long copyRoute(int aIndex);

    /**
    Reads a route from XML and displays it.

    If aProfile is non-null it is associated with the route, although no check
    is made to ensure that the profile actually applies to the route.

    If aReplace is true, deletes all current routes and adds the new route.
    If aReplace is false, appends the new route to the list of current routes.

    Appending a route fails if the new route has a different start and end point,
    or intermediate waypoints, from the current routes.

    This function resets the navigation state.

    Returns a CartoType error code: 0 = success.
    */
    public native int readRouteFromXml(String aFileName,RouteProfile aProfile,boolean aReplace);

    /**
    Loads an existing route and displays it.

    If aProfile is non-null it is associated with the route, although no check
    is made to ensure that the profile actually applies to the route.

    If aReplace is true, deletes all current routes and adds the new route.
    If aReplace is false, appends the new route to the list of current routes.

    Appending a route fails if the new route has a different start and end point,
    or intermediate waypoints, from the current routes.

    This function resets the navigation state.

    Returns a CartoType error code: 0 = success.
    */
    public native int useRoute(Route aRoute,RouteProfile aProfile,boolean aReplace);

    /**
    Writes a route as XML, selecting it by its index.
    Returns a CartoType error code: 0 = success.
    */
    public int writeRouteAsXml(int aRouteIndex,String aFileName)
        {
        return writeRoute(aRouteIndex,null,aFileName);
        }

    /**
    Writes a route as XML.
    Returns a CartoType error code: 0 = success.
    */
    public int writeRouteAsXml(Route aRoute,String aFileName)
        {
        if (aRoute == null)
            return Error.INVALID_ARGUMENT;
        return writeRoute(-1,aRoute,aFileName);
        }

    private native int writeRoute(int aRouteIndex,Route aRoute,String aFileName);

    /**
    Loads navigation data for the current map. This function is optional. If it
    is not called, navigation data will be loaded automatically when first needed.
    It is provided so that the time overhead of loading navigation data can be incurred
    at a predictable stage, for example at application startup.

    Return a CartoType error code: 0 = success.
    No error is returned if navigation data has already been loaded.
    */
    public native int loadNavigationData();

    /**
    Updates the current position and, if navigating, updates the navigation state.
    If the position is invalid, guesses the position by extrapolating from
    the current speed, if known, and the previous position.

    This function is useful both during navigation, and, when not navigating,
    if the map should be updated to show the user's location.

    The map position, heading, rotation or zoom may be changed
    according to the current follow mode (see SetFollowMode).

    The map should be redrawn after a call to this function.
    Returns a CartoType error code: 0 = success.
    */
    public native int navigate(int aValidity,double aTime,double aLong,double aLat,
                               double aSpeed,double aCourse,double aHeight);

    /**
    Returns the navigation state as defined by the NAVIGATION_STATE_* constants.
    */
    public native int getNavigationState();

    /**
    Sets the navigator's minimum fix distance in meters.
    A location update is ignored if it is less than this distance from the location
    last supplied (and not ignored) to the navigate() function.
    */
    public native void setNavigatorMinimumFixDistance(int aMeters);

    /**
    Set the navigator's time-off-route tolerance in seconds.
    A new route is calculated if the current position has been off route for this long.
    */
    public native void setNavigatorTimeTolerance(int aSeconds);

    /**
    Sets the navigator's distance tolerance in meters.
    The navigation state is set to off-route if the current position is at least this
    far from any part of the route.
    */
    public native void setNavigatorDistanceTolerance(int aMeters);

    /**
    Turns auto-re-routing in navigation on or off.
    By default it is turned on. If not, If not, when re-routing is needed,
    the navigation state NAVIGATION_STATE_RE_ROUTE_NEEDED is entered and the route is
    recalculated the next time a valid location is provided by a call to navigate().
    This gives the application time to
    issue a warning if the routing algorithm is likely to be slow.
    */
    public native void setNavigatorAutoReRoute(boolean aAutoReRoute);

    /**
    Starts warning about objects from the layer aLayer.
    Warnings are generated for objects no further than a specified distance from the route,
    and a specified distance along the route, measured from the nearest on-route point to the object.

    If a warning has already been set up for aLayer, replace it with the new warning.

    Returns a CartoType error code (0 = success).
    */
    public native int addNearbyObjectWarning(String aLayer,double aMaxDistanceToRoute,double aMaxDistanceAlongRoute);

    /**
    Stops warning about nearby objects from the layer aLayer.

    Returns a CartoType error code.
    */
    public native int deleteNearbyObjectWarning(String aLayer);

    /**
    Gets objects from aLayer which are close enough to generate warnings.
    */
    public native MapObject[] getNearbyObjects(String aLayer,int aMaxObjectCount);

    /**
    Returns the distance from the current point on the route to the destination in meters.
    */
    public native double distanceToDestination();

    /**
    Returns the estimated time from the current point on the route to the destination in seconds.
    */
    public native double estimatedTimeToDestination();

    /**
    Returns true if the position is known.
    */
    public native boolean positionKnown();

    /**
    When navigating, gets the first turn and returns the distance to it in map metres.
    If there is no nearby turn set the turn type to Turn.TURN_NONE and return 0;
    */
    public native double getFirstTurn(Turn aTurn);

    /**
    When navigating, gets the second turn and returns the distance to it in map metres from the first turn.
    If there is no nearby second turn set the turn type to Turn.TURN_NONE and return 0;
    */
    public native double getSecondTurn(Turn aTurn);

    /**
    When navigating, gets the continuation turn if any
    and returns the distance to it in map metres from the first turn.
    If there is no continuation turn set the turn type to Turn.TURN_NONE and return 0;
    */
    public native double getContinuationTurn(Turn aTurn);

    /**
    Converts any number of pairs of coordinates from one type to another. Valid coordinate types are
    LAT_LONG_COORDS, MAP_COORDS or SCREEN_COORDS. In each pair the first coordinate is the x or longitude value
    and the second is the y or latitude value. If the array size is odd the last value is ignored.
    */
    public native void convertCoords(double[] aCoordArray,int aFromCoordType,int aToCoordType);

    /**
    Finds map objects within a certain radius of a point on the displayed map.
    The point and radius are given in display pixels.
    Unlike the other find functions, this function deals with the graphic representation of objects, so it
    will work correctly when the user clicks on an icon, but not on the actual location of the object represented by
    the icon.
    */
    public native MapObject[] findInDisplay(double aX,double aY,double aRadius,int aMaxObjectCount);

    /**
    Finds an address, using an Address object. At least one of the fields in the Address object
    must be non-null and non-empty for any map objects to be found. If aFuzzy is true,
    uses fuzzy matching.
    */
    public native MapObject[] findAddress(Address aAddress,int aMaxObjectCount,boolean aFuzzy);

    /**
    Finds part of an address used in an Address object, using the same search rules used for finding an entire address.
    If aIncremental is true, prefix-matching is used and this enables an address dialog to be populated incrementally;
    in this case it's a good idea to set aMaxObjectCount to a small number that is a suitable number of items to be displayed in
    a list box on the display, for example 20.

    The parameter aAddressPart is one of the constants starting with ADDRESS_PART_.
    */
    public native MapObject[] findAddressPart(String aText,int aAddressPart,int aMaxObjectCount,boolean aFuzzy,boolean aIncremental);

    /**
    Finds all the map objects in a certain rectangle, in a certain layer.
    */
    public native MapObject[] findInLayer(String aLayer,double aX1,double aY1,double aX2,double aY2,int aCoordType,int aMaxObjectCount);

    /**
    Finds map objects with attributes matching a certain string, searching all attributes and all layers.
    */
    public MapObject[] find(String aText,int aMatchMethod,int aMaxObjectCount)
        {
        return find(aMaxObjectCount,null,0,null,null,aText,aMatchMethod,null);
        }

    /**
    Finds map objects with attributes matching a certain string, restricting the search to selected attributes
    and layers. aLayers is a list of layer names separated by commas or spaces: for example, "lake,river".
    aAttributes is a list of string attribute names separated by commas or spaces, where $ stands for the main name
    of the object (the unnamed string attribute): for example, "$,ref". aLayers and aAttributes may be null or empty:
    if no layers are given, all layers are searched, and if no attributes are given, all attributes are searched.

    Wild cards may be specified using the non-character Unicode codepoint U+FFFF.
    */
    public MapObject[] find(String aText,int aMatchMethod,int aMaxObjectCount,String aLayers,String aAttributes)
        {
        return find(aMaxObjectCount,null,0,aLayers,aAttributes,aText,aMatchMethod,null);
        }

    /**
    A general find function, allowing any combination of parameters.

    The objects returned are in map coordinates.

    aClip, if non-null, restricts the search to objects intersecting a certain
    rectangle.

    aClipCoordType gives the type of coordinates used in aClip.

    aLayers, if non-null, is a list of layers separated by spaces or commas. If it
    is null all layers are searched.

    aAttributes is used in text searching (if aText is non-null). If aAttributes
    is null or empty, search all attributes, otherwise aAttributes is a list of
    attributes separated by spaces or commas; use "$" to indicate the label (the
    unnamed attribute).

    aText, if non-null, restricts the search to objects containing aText
    in one of their string attributes.

    aStringMatchMethod is used in text searching.

    aCondition, if non-null, is an expression using the same syntax as style sheet
    expressions (e.g., "Type==2") which must be fulfilled by all the objects.
    */
    public native MapObject[] find(int aMaxObjectCount,
                                   Rect aClip,
                                   int ClipCoordType,
                                   String aLayers,
                                   String aAttributes,
                                   String aText,
                                   int aStringMatchMethod,
                                   String aCondition);

    /**
    Finds all polygon objects containing the point (aX,aY).
    */
    public native MapObject[] findPolygonsContainingPoint(double aX,double aY,int aCoordType,int aMaxObjectCount);

    /**
    Finds all point objects contained in the polygon defined by the two arrays aX and aY.
    */
    public native MapObject[] findPointsInPolygon(double[] aX,double[] aY,int aCoordType,int aMaxObjectCount);

    /**
    Returns the height in feet at a single point, or -32768 if it is unavailable.
    This function requires the 'terrain-height-feet' layer.
    */
    public native int getHeight(double aX,double aY,int aCoordType);

    /**
    Gets an array of heights in feet for a set of points.
    Unavailable heights are set to -32768.
    This function requires the 'terrain-height-feet' layer.
    Return a CartoType error code: 0 = success.
    */
    public native int getHeights(double[] aX,double[] aY,int[] aTerrainHeightArray,int aCoordType);

    /**
    Gets an array of heights in feet for a route represented by the two arrays aX and aY.
    A line joining the points returned in aTerrainHeightArray is guaranteed to clear all
    heights between pairs of points.
    This function requires the 'terrain-height-feet' layer.
    Returns a CartoType error code: 0 = success.
    */
    public native int getHeightProfile(double[] aX,double[] aY,int[] aTerrainHeightArray,int aCoordType);

    /**
    Tells the framework whether to use the image server system to draw the map, caching images as needed.
    If aCacheSizeInBytes is greater than zero it gives the size of the image cache, otherwise the
    default size is used.
    */
    public native int useImageServer(boolean aEnable,int aCacheSizeInBytes);

    /**
    Returns a string summarizing a geocode for a point.

    The summary is a comma-separated list of all the non-empty fields in the corresponding address,
    as returned by GetAddress. The postcode if any is enclosed in square brackets.

    aMaxItems gives the maximum number of items to be used, including the main item.

    If aLocale is non-null it is used when getting names.
    If aLocale is null or empty, the locale set by setLocale is used.

    Calls to this function are limited in number in unlicensed (evaluation) versions of the CartoType library.
    */
    public native String getGeoCodeSummary(double aX,double aY,int aCoordType,int aMaxItems,String aLocale);

    /**
    Gets an address for a point.

    If aLocale is non-null it is used when getting names.
    If aLocale is null or empty, the locale set by setLocale is used.

    Calls to this function are limited in number in unlicensed (evaluation) versions of the CartoType library.

    Returns a CartoType error code: 0 = success.
    */
    public native int getAddress(Address aAddress,double aX,double aY,int aCoordType,String aLocale);

    /**
    Gets information about the nearest road to the point (aX,aY).
    Roads are defined as map objects allowed by the current routing filter and main route profile.
    Examine objects within a distance of aMaxDistanceInMeters only.
    The legal range for aMaxDistanceInMeters is from 5 meters to 1000 meters. Illegal values are clamped to that range.
    If aHeadingInDegrees is valid (not negative) use it to choose among roads going in different directions.
    */
    public native int getNearestRoad(NearestRoadInfo aInfo,double aX,double aY,int aCoordType,
                                     double aMaxDistanceInMeters,double aHeadingInDegrees);

    /**
    Moves the route position object and the route vector object to the nearest point on a road.
    Uses the supplied position and heading. If aHeadingInDegrees is not negative, uses it to
    choose among roads going in different directions.
    If aInfo is non-null, returns information about the nearest road.
    This function can be used whether or not navigation is being performed.

    Returns a CartoType error code: 0 = success.
    */
    public native int displayPositionOnNearestRoad(double aLong,double aLat,double aHeadingInDegrees,NearestRoadInfo aInfo,double aMaxDistanceInMeters);

    /**
    Sets the vehicle position when navigating as an offset from the center of the display,
    in units of the width and height of the display. For example, to show the vehicle
    in the middle but three quarters of the way down the display, use (0,0.25).

    Returns a CartoType error code: 0 = success.
    */
    public native int setVehiclePosOffset(double aXOffset,double aYOffset);

    /**
    Sets the locale used when getting the names of map objects when routing, geocoding, etc.
    The value of aLocale is usually a two-letter language identifier like 'en' for English or 'fr' for French.
    Passing an empty or null string sets the locale to 'no locale'.

    Returns a CartoType error code: 0 = success.
    */
    public native int setLocale(String aLocale);

    /**
    Returns the locale used when getting the names of map objects when routing, geocoding, etc.
    The returned value is either a null-terminated string, usually a two-letter language identifier like 'en' for English or 'fr' for French,
    or an empty string meaning 'no locale'.
    */
    public native String locale();

    /**
    Sets the follow mode, which controls the way the map tracks the user location and heading.
    The parameter aFollowMode is one of the FOLLOW_MODE_* constants.
    Returns a CartoType error code (0 = success).
    */
    public native int setFollowMode(int aFollowMode);

    /** Gets the follow mode, which controls the way the map tracks the user location and heading. */
    public native int getFollowMode();

    /**
    Adds traffic information, such as a speed restriction, prohibition on the use of a route, reduction in
    the number of lanes, notification of road works, etc. At present only speed restrictions are supported.

    This is the general function.
    See also the specialized functions addPolygonSpeedLimit(), addLineSpeedLimit(), addClosedLineSpeedLimit() and addForbiddenArea().

    Uses the supplied ID if aId is non-zero, otherwise generates an ID which can be retrieved using getLastObjectId().
    The ID can be used when calling DeleteTrafficInfo().

    aTrafficInfo gives the type of information (speed limit, etc.).

    aLocationRef defines the location (area, road, point, etc.). Only the values ERoadOrientationForwards and ERoadOrientationNone
    (which means the same in this context) is supported for iRoadOrientation in aLocationRef.

    aDriveOnLeft is used when adding a line speed limit to allow a highlight to be drawn on the driver's side, in the direction of travel.
    It should be set to true for left-driving countries like Britain, Ireland, Australia, India, Japan, Indonesia, etc.
    */
    public native int addTrafficInfo(long aId,TrafficInfo aTrafficInfo,LocationRef aLocationRef,boolean aDriveOnLeft);

    /**
    Adds a speed limit in kph, to be used when calculating routes, to all roads in a certain polygon.
    To specify particular roads, or an area enclosed by roads, use AddTrafficInfo().
    This allows the router to take account of slow-downs caused by traffic density, road works, etc.

    Uses the supplied ID if aId is non-zero, otherwise generates an ID which can be retrieved using getLastObjectId().
    The ID can be used when calling DeleteTrafficInfo().

    aSpeed is the speed to which travel is restricted, in kph. If aSpeed is zero or less, routing is forbidden in
    the given polygon.

    aVehicleTypes is a set of flags drawn from the vehicle type constants defined in RouteProfile.
    If aVehicleTypes is 0, all vehicles are affected.

    A polygon map object representing the speed limit is added to the layer 'traffic'.

    See also addTrafficInfo(), addLineSpeedlimit(), addForbiddenArea(), deleteTrafficInfo(), and clearTrafficInfo().
    */
    public int addPolygonSpeedLimit(long aId,Geometry aPolygon,double aSpeed,int aVehicleTypes)
        {
        TrafficInfo ti = new TrafficInfo();
        ti.iSpeed = aSpeed;
        ti.iVehicleTypes = aVehicleTypes;
        LocationRef lr = new LocationRef();
        lr.iType = LocationRef.LOCATION_REF_POLYGON;
        lr.iGeometry = aPolygon;
        return addTrafficInfo(aId,ti,lr,false);
        }

    /**
    Adds a speed limit in kph, to be used when calculating routes, to a route defined by a series of points.

    Uses the supplied ID if aId is non-zero, otherwise generates an ID which can be retrieved using getLastObjectId().
    The ID can be used when calling DeleteTrafficInfo().

    aLine is a series of points defining the road. The road is calculated as the best route through the points, using the route network,
    so to be certain of specifying the desired route you need to supply a point for every junction. The points are snapped to
    the nearest nodes in the route network. This means that at the moment you cannot set a speed limit for a range smaller
    than an arc between two nodes.

    aSpeed is the speed to which travel is restricted in kph.
    Values of zero or less indicate that the road is closed.
    Values higher than 255 indicate that there is no speed limit.

    aVehicleTypes is a set of flags drawn from the vehicle type constants defined in RouteProfile.
    If aVehicleTypes is 0, all vehicles are affected.

    aDriveOnLeft allows a highlight to be drawn on the driver's side, in the direction of travel.
    It should be set to true for left-driving countries like Britain, Ireland, Australia, India, Japan, Indonesia, etc.

    A line map object representing the speed limit is added to the layer 'traffic'.

    See also addTrafficInfo(), addClosedLineSpeedLimit(), addPolygonSpeedlimit(), addForbiddenArea(), deleteTrafficInfo(), and clearTrafficInfo().
    */
    public int addLineSpeedLimit(long aId,Geometry aLine,double aSpeed,int aVehicleTypes,boolean aDriveOnLeft)
        {
        TrafficInfo ti = new TrafficInfo();
        ti.iSpeed = aSpeed;
        ti.iVehicleTypes = aVehicleTypes;
        LocationRef lr = new LocationRef();
        lr.iType = LocationRef.LOCATION_REF_LINE;
        lr.iGeometry = aLine;
        return addTrafficInfo(aId,ti,lr,aDriveOnLeft);
        }

    /**
    Adds a speed limit in kph, to be used when calculating routes, to a polygon which is the interior of a
    closed line along roads, defined by a series of points.

    Uses the supplied ID if aId is non-zero, otherwise generates an ID which can be retrieved using getLastObjectId().
    The ID can be used when calling DeleteTrafficInfo().

    aLine is a series of points defining the polygon. The polygon is calculated as the best route through the points, using the route network,
    so to be certain of specifying the desired polygon you need to supply a point for every junction.

    aSpeed is the speed to which travel is restricted in kph.
    Values of zero or less indicate that the road is closed.
    Values higher than 255 indicate that there is no speed limit.

    aVehicleTypes is a set of flags drawn from the vehicle type constants defined in RouteProfile.
    If aVehicleTypes is 0, all vehicles are affected.

    A polygon map object representing the speed limit is added to the layer 'traffic'.

    See also addTrafficInfo(), addLineSpeedLimit(), addPolygonSpeedlimit(), addForbiddenArea(), deleteTrafficInfo(), and clearTrafficInfo().
    */
    public int addClosedLineSpeedLimit(long aId,Geometry aLine,double aSpeed,int aVehicleTypes)
        {
        TrafficInfo ti = new TrafficInfo();
        ti.iSpeed = aSpeed;
        ti.iVehicleTypes = aVehicleTypes;
        LocationRef lr = new LocationRef();
        lr.iType = LocationRef.LOCATION_REF_CLOSED_LINE;
        return addTrafficInfo(aId,ti,lr,false);
        }

    /**
    Marks a certain polygon as forbidden for routing.

    Uses the supplied ID if aId is non-zero, otherwise generates an ID which can be retrieved using getLastObjectId().
    The ID can be used when calling DeleteTrafficInfo().

    See also addTrafficInfo(), addLineSpeedLimit(), addPolygonSpeedLimit(), deleteTrafficInfo() and clearTrafficInfo().
    */
    public int addForbiddenArea(long aId,Geometry aPolygon)
        {
        TrafficInfo ti = new TrafficInfo();
        ti.iSpeed = 0;
        ti.iVehicleTypes = 0;
        LocationRef lr = new LocationRef();
        lr.iType = LocationRef.LOCATION_REF_POLYGON;
        lr.iGeometry = aPolygon;
        return addTrafficInfo(aId,ti,lr,false);
        }

    /**
    Deletes traffic information: a speed limit, forbidden area, or other information, referring
    to it by the ID returned when the restriction was added by addTrafficInfo(), addLineSpeedLimit(),
    addForbiddenArea(), etc.

    Use clearTrafficInfo() to delete all restrictions and forbidden areas.
    */
    public native int deleteTrafficInfo(long aId);

    /** Deletes all speed restrictions, forbidden areas and other traffic information. */
    public native void clearTrafficInfo();

    /**
    Enables or disables the use of traffic information when routing.
    Returns the previous state.
    */
    public native boolean enableTrafficInfo(boolean aEnable);

    /**
    Sets a named attribute, treating a string as a set of map object attributes of the form
    label|key1=value1|key2=value2| ... |keyN=valueN, where | is character code 0,
    An empty or zero-length name sets the label.
    An empty or zero-length value deletes a name-value pair.
    Any of aString, aKey or aValue may be null.

    Returns the new string, with the attribute set.
    */
    public native static String setAttribute(String aString,String aKey,String aValue);

    /**
    Constructs an integer attribute from an optional three-letter code, placed
    in the high 16 bits by encoding each letter in five bits, and a type number placed in
    the low 16 bits.

    The three-letter code is used only if it consists of exactly three lower-case letters
    in the range a...z.

    Three-letter codes are used for mnemonic purposes, as for example "pub" for a public house,
    and can be tested easily in style sheets.
    */
    public static int intAttribute(String aThreeLetterCode,int aType)
        {
        int a = 0;
        if (aThreeLetterCode != null && aThreeLetterCode.length() == 3)
            {
            char c0 = aThreeLetterCode.charAt(0);
            char c1 = aThreeLetterCode.charAt(1);
            char c2 = aThreeLetterCode.charAt(2);
            if (c0 >= 'a' && c0 <= 'z' && c1 >= 'a' && c1 <= 'z' && c2 >= 'a' && c2 <= 'z')
                a = ((c0 - 'a') << 27) | ((c1 - 'a') << 22) | ((c2 - 'a') << 17);
            }
        a |= (aType & 0xFFFF);
        return a;
        }

    protected void finalize()
        {
        destroy();
        }

    static
        {
        System.loadLibrary("cartotype");
        initGlobals();
        }

    private static native void initGlobals();
    private native long create(String aMapFileName,byte[] aMapEncryptionKey,String aStyleSheetFileName,String aFontFileName,int aViewWidth,int aViewHeight);
    private native long createWithParam(FrameworkParam aParam);
    private native void destroy();

    private long iFrameworkRef;
    private Bitmap iBitmap;
    private byte[] iBitmapData;
    private ByteBuffer iBuffer;
    private Bitmap iTileBitmap;
    private byte[] iTileBitmapData;
    private ByteBuffer iTileBuffer;
    private long iLastObjectId;
    }
