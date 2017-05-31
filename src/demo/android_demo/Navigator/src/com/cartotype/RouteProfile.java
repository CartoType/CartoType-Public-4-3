/*
RouteProfile.java
Copyright (C) 2012-2015 CartoType Ltd.
See www.cartotype.com for more information.
*/

package com.cartotype;

/**
A route profile controls the way a route is calculated. It provides average speeds
and weightings for different types of roads and specifies restrictions to be obeyed. 
*/
public class RouteProfile
    {
    /** A route profile type for private car navigation. */
    public static final int CAR_PROFILE = 0;
    /** A route profile type for walking. */
    public static final int WALKING_PROFILE = 1;
    /** A route profile type for cycling. */
    public static final int BICYCLE_PROFILE = 2;
    /** A route profile type for walking, preferring off-road paths. */
    public static final int HIKING_PROFILE = 3;
    
    /**
    Bit value for iVehicleType: this vehicle cannot go the wrong way along a one-way road.
    This flag should be cleared for pedestrian routing.
    */
    public static final int WRONG_WAY_FLAG = 0x00100000;
    /** Bit value for iVehicleType: this vehicle is a bicycle. */
    public static final int BICYCLE_ACCESS_FLAG = 0x0020000;
    /** Bit value for iVehicleType: this vehicle is a motorcycles. */
    public static final int MOTORCYCLE_ACCESS_FLAG = 0x0040000;
    /** Bit value for iVehicleType: this vehicle is a car. */
    public static final int CAR_ACCESS_FLAG = 0x0080000;
    /** Bit value for iVehicleType: this vehicle is a high occupancy vehicle. */
    public static final int HIGH_OCCUPANCY_ACCESS_FLAG = 0x0100000;
    /** Bit value for iVehicleType: this vehicle is a light goods vehicle. */
    public static final int LIGHT_GOODS_ACCESS_FLAG = 0x02000000;
    /** Bit value for iVehicleType: this vehicle is a heavy goods vehicle. */
    public static final int HEAVY_GOODS_ACCESS_FLAG = 0x04000000;
    /** Bit value for iVehicleType: this vehicle is a bus. */
    public static final int BUS_ACCESS_FLAG = 0x08000000;
    /** Bit value for iVehicleType: this vehicle is a taxi. */
    public static final int TAXI_ACCESS_FLAG = 0x10000000;
    /** Bit value for iVehicleType: this vehicle is a tourist bus. */
    public static final int TOURIST_BUS_ACCESS_FLAG = 0x20000000;
    /** Bit value for iVehicleType: this vehicle is an emergency vehicle. */
    public static final int EMERGENCY_ACCESS_FLAG = 0x40000000;
    /** Bit value for iVehicleType: this vehicle carries hazardous materials. */
    public static final int HAZARDOUS_ACCESS_FLAG = 0x80000000;

    public static final int MOTORWAY = 0;
    public static final int MOTORWAY_LINK = 1;
    public static final int TRUNK_ROAD = 2;
    public static final int TRUNK_ROAD_LINK = 3;
    public static final int PRIMARY_ROAD = 4;
    public static final int PRIMARY_ROAD_LINK = 5;
    public static final int SECONDARY_ROAD = 6;
    public static final int SECONDARY_ROAD_LINK = 7;
    public static final int TERTIARY_ROAD = 8;
    public static final int UNCLASSIFIED_ROAD = 9;
    public static final int RESIDENTIAL_ROAD = 10;
    public static final int TRACK = 11;
    public static final int SERVICE_ROAD = 12;
    public static final int PEDESTRIAN_ROAD = 13;
    public static final int VEHICULAR_FERRY = 14;
    public static final int PASSENGER_FERRY = 15;
    public static final int LIVING_STREET = 16;
    public static final int CYCLEWAY = 17;
    public static final int PATH = 18;
    public static final int FOOTWAY = 19;
    public static final int BRIDLEWAY = 20;
    public static final int STEPS = 21;
    public static final int UNKNOWN_ROAD_TYPE = 22;
    public static final int UNPAVED_ROAD = 23;
    public static final int OTHER_ROAD_TYPE_0 = 24;
    public static final int OTHER_ROAD_TYPE_1 = 25;
    public static final int OTHER_ROAD_TYPE_2 = 26;
    public static final int OTHER_ROAD_TYPE_3 = 27;
    public static final int OTHER_ROAD_TYPE_4 = 28;
    public static final int OTHER_ROAD_TYPE_5 = 29;
    public static final int OTHER_ROAD_TYPE_6 = 30;
    public static final int OTHER_ROAD_TYPE_7 = 31;
    public static final int ROAD_TYPE_COUNT = 32;
    
    /**
    Create a standard routing profile for private car journeys.
    It can then be modified as desired.
    */
    public RouteProfile()
        {
        construct(CAR_PROFILE);
        }
    
    /**
    Create a routing profile of a predefined type.
    It can then be modified as desired.
    */
    public RouteProfile(int aProfileType)
        {
        construct(aProfileType);
        }

    private native void construct(int aProfileType);
 
    static
        {
        initGlobals();
        }

    private static native void initGlobals();
    
    /**
    Flags taken from WRONG_WAY_FLAG ... HAZARDOUS_ACCESS_FLAG indicating the vehicle type.
    Route sections with restrictions matching any of these flags will not be taken.
    */
    public int iVehicleType;

    /** Speeds along roads in kilometres per hour. */
    public double iSpeed[] = new double[ROAD_TYPE_COUNT];
    
    /**
    Bonuses or penalties in notional km per hour to be added to road types to make them more or less likely to be used.
    For example, a penalty of 1kph is applied to walking routes along major roads because it is pleasanter to walk along quieter minor roads.
    */
    public double iBonus[] = new double[ROAD_TYPE_COUNT];

    /**
    This array of bit masks allows restrictions to be overridden for certain
    types of road. For example, to allow routing of heavy goods vehicles along
    tracks, even if they are forbidden to motor vehicles, set
    iRestrictionOverride[TRACK] to HEAVY_GOODS_ACCESS_FLAG.
    */
    public int iRestrictionOverride[] = new int[ROAD_TYPE_COUNT];
    
    /**
    The estimated time in seconds taken for any turn at a junction that is not
    a slight turn or simply going straight ahead.
    */
    public int iTurnTime = 4;

    /**
    The estimated time in seconds taken for a turn across the traffic:
    that is, a left turn in drive-on-right countries,
    or a right turn in drive-on-left countries. The time is used when turning
    from the right of way to a lower-rank road. It is doubled when
    turning from a lower-rank road on to the right of way.
    */
    public int iCrossTrafficTurnTime = 10;

    /**
    The estimated delay in seconds caused by traffic lights.
    */
    public int iTrafficLightTime = 30;

    /** If true, ignore the speed when routing; get the shortest route, not the fastest. */
    public boolean iShortest;
    
    /**
    The penalty applied to toll roads as a number between zero and one.
    The value 1 means that no toll roads will be used, 0.5 makes toll roads half as desirable as non-toll roads
    of the same road type, and so on. The value 0 means that no penalty is applied to toll roads.
    Values outside the range 0...1 are clamped to that range.
    */
    public double iTollPenalty;
    }
