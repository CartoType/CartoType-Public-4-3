/*
Error.java
Copyright (C) 2014-2015 CartoType Ltd.
See www.cartotype.com for more information.
*/

package com.cartotype;

/** This class holds error codes. */
public class Error
	{
    /** No error: a successful result. */
    public static final int NONE = 0;

    /** No error: a successful result. */
    public static final int SUCCESS = 0;

	/**
	GENERAL is used where an error has occurred but no other CartoType
	error is applicable. For example, this error code can be used
	when FreeType returns an error code for illegal TrueType hinting instructions.
	*/
	public static final int GENERAL = 1;

	/** NO_MEMORY is returned when an attempted memory allocation fails. */
	public static final int NO_MEMORY = 2;

	/**
	END_OF_DATA is returned by iterators and streams when no more data
	is available. It may be treated as an error or not depending on the
	circumstances.
	*/
	public static final int END_OF_DATA = 3;

	/**
	TEXT_UNMODIFIABLE is returned when an attempt is made to
	modify an unmodifiable string represented by a class derived
	from MString.
	*/
	public static final int TEXT_UNMODIFIABLE = 4;

	/**
	TEXT_TRUNCATED is returned when text inserted into
	a string derived from MString is too long for a fixed-size
	buffer.
	*/
	public static final int TEXT_TRUNCATED = 5;

	/**
	NOT_FOUND is returned whenever a resource like a file
	is not found, or a search fails. 
	It may be treated as an error or not depending on the
	circumstances.
	*/
	public static final int NOT_FOUND = 6;

	/**
	The code UNIMPLEMENTED is used for functions that have
	not yet been implemented, or for circumstances or options
	within functions that remain unimplemented because they
	are deemed to be either rarely or never useful.
	*/
	public static final int UNIMPLEMENTED = 7;

	/**
	If the graphics system detects an invalid outline, such as one
	that starts with a cubic control point, it returns
	INVALID_OUTLINE.
	*/
	public static final int INVALID_OUTLINE = 8;

	/**
	The code IO should be used for unexpected read or write
	errors on files or other data streams. This is for where you
	attempt to read data that should be there, but the OS says
	it's unavailable (e.g., someone has taken the CD out of the drive
	or the network is down). 
	*/
	public static final int IO = 9;

	/**
	CORRUPT is returned when illegal values are found when reading data
	from a file or other serialized form.
	*/
	public static final int CORRUPT = 10;

	/**
	This error is returned by database accessors, typeface accessors, etc.,
	to indicate that this data format is unknown to a particular accessor.
	It is not really an error but tells the caller to try the next
	accessor in the list.
	*/
	public static final int UNKNOWN_DATA_FORMAT = 11;

	/**
	This error is returned by transformation inversion if the transformation 
	has no inverse.
	*/
	public static final int NO_INVERSE = 12;

	/**
	This error is returned by the projection system if the projection 
	causes overflow.
	*/
	public static final int PROJECTION_OVERFLOW = 13;

	/**
	The code CANCEL is returned to cancel an asynchronous
	operation that uses callbacks. The callback or virtual function
	returns this code. The caller must terminate further processing
	on receipt of any error, but this one indicates that there has been
	no actual error.
	*/
	public static final int CANCEL = 14;

	/**
	This error is returned when an invalid argument has been given to a function.
	*/
	public static final int INVALID_ARGUMENT = 15;

	/**
	This error is returned by a data reader when it can not deal with the data version.
	*/
	public static final int UNKNOWN_VERSION = 16;

	/**
	This error is returned by the base library when reading data or calculations result
	in overflow.
	*/
	public static final int OVERFLOW = 17;

	/**
	Graphics functions can use this code to indicate that two graphics objects
	overlap.
	*/
	public static final int OVERLAP = 18;

	/**
	The error code returned by line intersection algorithms when the lines are parallel.
	*/
	public static final int PARALLEL_LINES = 19;

	/**
	Text was drawn along a path but the path was too short
	for all the text.
	*/
	public static final int PATH_LENGTH_EXCEEDED = 20;

	/**
	Text was drawn along a path but the maximum allowed angle between
	the baselines of successive characters was exceeded.
	*/
	public static final int MAX_TURN_EXCEEDED = 21;

	/**
	An attempt was made to draw a bitmap needing a color palette
	but no palette was provided.
	*/
	public static final int NO_PALETTE = 22;

	/**
	An attempt was made to insert a duplicate object into a collection
	class that does not allow duplicates.
	*/
	public static final int DUPLICATE = 23;

	/**
	The projection for converting latitude and longitude to map coordinates
	is unknown or unavailable.
	*/
	public static final int NO_PROJECTION = 24;

	/**
	A palette is full and no new entries can be added.
	*/
	public static final int PALETTE_FULL = 25;

	/**
	The dash array for drawing strokes is invalid.
	*/
	public static final int INVALID_DASH_ARRAY = 26;

	/**
	The central-path system failed to create a usable path.
	*/
	public static final int CENTRAL_PATH = 27;

	/**
	A route was needed by the navigation system
	but no route was available.
	*/
	public static final int NO_ROUTE = 28;

	/**
	There was an attempt to zoom beyond the legal zoom limits for a map.
	*/
	public static final int ZOOM_LIMIT_REACHED = 29;

	/**
	There was an attempt to project a map object that had already been projected.
	*/
	public static final int ALREADY_PROJECTED = 30;

	/**
	Conditions are too deeply nested in the style sheet.
	*/
	public static final int CONDITIONS_TOO_DEEPLY_NESTED = 31;

	/**
	There was an attempt to use a null font for drawing text.
	*/
	public static final int NULL_FONT = 32;

	/**
	An attempt to read data from the internet failed.
	*/
	public static final int INTERNET_IO = 33;

	/**
	Division by zero in an interpreted expression.
	*/
	public static final int DIVIDE_BY_ZERO = 34;

	/**
	A transform failed because an argument or result was out of range.
	*/
	public static final int TRANSFORM_FAILED = 35;

	/**
	Reading a bitmap from PNG format failed.
	*/
	public static final int PNG_READ = 36;

	/**
	Reading a bitmap from JPG format failed.
	*/
	public static final int JPG_READ = 37;

	/**
	An object did not intersect a specified region.
	*/
	public static final int NO_INTERSECTION = 38;

	/**
	An operation was interrupted, for example by another thread writing to a shared flag.
	*/
	public static final int INTERRUPT = 39;

	/**
	There was an attempt to use map databases of incompatible formats
	(TMapGrid values containing point format, datum and axis orientations)
	to draw a map or find objects in a map.
	*/
	public static final int MAP_DATABASE_FORMAT_MISMATCH = 40;

	/** A key supplied for encryption was too short. */
	public static final int ENCRYPTION_KEY_TOO_SHORT = 41;

	/** No encryption key has been set. */
	public static final int NO_ENCRYPTION_KEY = 42;

	/** A code for standard emergency messages. */
	public static final int EMERGENCY_MESSAGE = 43;

	/** A code for standard alert messages. */
	public static final int ALERT_MESSAGE = 44;

	/** A code for standard critical messages. */
	public static final int CRITICAL_MESSAGE = 45;

	/** A code for standard error messages. */
	public static final int MESSAGE = 46;

	/** A code for standard warning messages. */
	public static final int WARNING_MESSAGE = 47;

	/** A code for standard notice messages. */
	public static final int NOTICE_MESSAGE = 48;

	/** A code for standard information messages. */
	public static final int INFO_MESSAGE = 49;

	/** A code for standard debug messages. */
	public static final int DEBUG_MESSAGE = 50;

	/** A function has been called which is available only when navigating. */
	public static final int NOT_NAVIGATING = 51;

	/** The global framework object does not exist. */
	public static final int NO_FRAMEWORK = 52;

	/** The global framework object already exists. */
	public static final int FRAMEWORK_ALREADY_EXISTS = 53;

	/** A string was not transliterable. */
	public static final int UNTRANSLITERABLE = 54;

	/** Writing a bitmap to PNG format failed. */
	public static final int PNG_WRITE = 55;

	/** There was an attempt to write to a read-only map database. */
	public static final int READ_ONLY_MAP_DATABASE = 56;

	/** There was an error in the PROJ.4 projection library error other than a projection overflow. */
	public static final int PROJ4 = 57;
	
    /** A function was called from the unlicensed version of CartoType that is available only in the licensed version. */
    public static final int UNLICENSED = 58;

    /** No route could be created because there were no roads near the start point of a route section. */
    public static final int NO_ROADS_NEAR_START_OF_ROUTE = 59;

    /** No route could be created because there were no roads near the end point of a route section. */
    public static final int NO_ROADS_NEAR_END_OF_ROUTE = 60;

    /** No route could be created because the start and end point were not connected. */
    public static final int NO_ROUTE_CONNECTIVITY = 61;
	}
