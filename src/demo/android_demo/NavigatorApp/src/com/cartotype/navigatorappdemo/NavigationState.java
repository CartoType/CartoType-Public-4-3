package com.cartotype.navigatorappdemo;

/** Constants for the navigation state. */
public class NavigationState
    {
    /** No action is needed. */
    public static final int NO_ACTION = 0;
    /** A turn is approaching. */
    public static final int TURN = 1;
    /**
    Turn round at a legal and safe place and go back in the reverse direction.
    This navigation state occurs when the vehicle is on the correct route but going the wrong way.
    */
    public static final int TURN_ROUND = 2;
    /**
    This state occurs when the vehicle is off the route and a new route has to be calculated.
    */
    public static final int NEW_ROUTE = 3;
    /**
    This state occurs when the end of the route is within one kilometre and there are no more junctions.
    */
    public static final int ARRIVAL = 4;
    /**
    The current position is off the route (further than a certain tolerated distance)
    and a new route has not yet been calculated.
    */
    public static final int OFF_ROUTE = 5;
    }
