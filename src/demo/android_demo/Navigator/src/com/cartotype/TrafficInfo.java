/*
TrafficInfo.java
Copyright (C) 2015 CartoType Ltd.
See www.cartotype.com for more information.
*/

package com.cartotype;

/** Traffic information, normally used in combination with a location reference. */
public class TrafficInfo
    {
    /** Speeds of this value or greater mean there is no speed limit. */
    public static final double NO_SPEED_LIMIT = 255;
    
    /** Vehicle types affected by this information, taken from the RouteProfile vehicle access flags; use 0 to select all vehicle types. */
    public int iVehicleTypes;
    /** Permitted or expected speed in kph: NO_SPEED_LIMIT or greater means no speed limit; 0 or less means the route is forbidden. */
    public double iSpeed;
    /** This information applies to vehicles with this minimum weight in metric tons. */
    public double iWeight;
    /** True if there are road works. */
    public boolean iWorks;
    /** True if a toll applies. */
    public boolean iToll;
    /** If greater than zero, the number of lanes open. */
    public int iLanes;
    }
