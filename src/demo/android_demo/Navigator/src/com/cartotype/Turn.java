/*
Turn.java
Copyright (C) 2012-2015 CartoType Ltd.
See www.cartotype.com for more information.
*/

package com.cartotype;

/** Information about a turn from one route segment to another. */
public class Turn
    {
    /** No turn exists or is needed. This turn type is used at the start of a route. */
    public static final int TURN_NONE = 0; 
    /**
    A turn of less than 22.5 degrees left or right, unless this is a fork with two choices, in which
    case the turn will be bear right or bear left.
    */
    public static final int TURN_AHEAD = 1;
    /**
    A turn between 22.5 degrees and 67.5 degrees right
    or a turn through a smaller angle which is the rightmost of a fork with two choices.
    */
    public static final int TURN_BEAR_RIGHT = 2;
    /** A turn between 67.5 degrees and 112.5 degrees right. */
    public static final int TURN_RIGHT = 3;
    /** A turn between 112.5 degrees and 180 degrees right. */
    public static final int TURN_SHARP_RIGHT = 4;
    /** This turn type is use for U-turns: turns back along the same road. */
    public static final int TURN_AROUND = 5;
    /** A turn between 112.5 degrees and 180 degrees left. */
    public static final int TURN_SHARP_LEFT = 6;
    /** A turn between 67.5 degrees and 112.5 degrees left. */
    public static final int TURN_LEFT = 7;
    /**
    A turn between 22.5 degrees and 67.5 degrees left.
    or a turn through a smaller angle which is the leftmost of a fork with two choices.
    */
    public static final int TURN_BEAR_LEFT = 8;
    
    /** This junction does not involve a roundabout. */
    public static final int ROUNDABOUT_STATE_NONE = 0;
    /** This junction enters a roundabout. */
    public static final int ROUNDABOUT_STATE_ENTER = 1;
    /** This junction continues around a roundabout. */
    public static final int ROUNDABOUT_STATE_CONTINUE = 2;
    /** This junction exits a roundabout. */
    public static final int ROUNDABOUT_STATE_EXIT = 3;

    /** This junction continues around a roundabout. @deprecated: use ROUNDABOUT_STATE_CONTINUE. */
    public static final int ROUNDABOUT_STATE__CONTINUE = 2;
    
    public Turn()
        {
        iContinue = true;
        }
    
    /**
    Returns the turn command in the language of the current locale.
    @deprecated Use iInstructions directly.
    */
    public String TurnCommand()
        {
        return iInstructions;
        }
    
    /** The turn type: ahead, left, right, etc. */
    public int iTurnType;
    /** True if this turn is a continuation of the current road and no notification is needed. */
    public boolean iContinue;
    /** The roundabout state. */
    public int iRoundaboutState;
    /** The turn angle: 0 = straight ahead; negative = left, positive = right. */
    public double iTurnAngle;
    /** The junction to take, counting the current junction as 0, if this junction is part of a roundabout. */
    public int iExitNumber;
    /** The distance to the turn in metres. */
    public double iDistance;
    /** The estimated time to the turn in seconds. */
    public double iTime;
    /** The name of the road before the turn. */
    public String iFromName;
    /** The reference code (e.g., B4009) of the road before the turn. */
    public String iFromRef;
    /** The road type of the road before the turn: bit-fields as defined in the RoadType class. */
    public int iFromRoadType;
    /** The name of the road after the turn. */
    public String iToName;
    /** The reference code (e.g., B4009) of the road after the turn. */
    public String iToRef;
    /** The road type of the road after the turn: bit-fields as defined in the RoadType class. */
    public int iToRoadType;
    /** Instructions for the turn, in the current locale's language. */
    public String iInstructions;
    }
