/*
RoadType.java
Copyright (C) 2012-2015 CartoType Ltd.
See www.cartotype.com for more information.
*/

package com.cartotype;

/** Bit fields used in road types. */
public class RoadType
    {
    /** The toll, roundabout, restriction and direction flags are held in bits 1...5. */
    public static final int ROUTING_MASK = 0x3E;

    /** The road type is held in bits 6...11. */
    public static final int ROAD_TYPE_MASK = 0xFC0;

    /** The major road type is held in bits 8...11; bits 7 and 6 are used for finer distinctions. */
    public static final int MAJOR_ROAD_TYPE_MASK = 0xF00;

    /**
    The level (0 = surface, positive = above surface, negative = below surface)
    is held as a signed nybble in bits 12...15. Levels above 0 are not necessarily
    bridges; they can be embankments or other raised structures, and levels below 0 are
    not necessarily tunnels. There are separate bits to mark bridges and tunnels:
    BRIDGE_ROAD_TYPE_FLAG and TUNNEL_ROAD_TYPE_FLAG.

    The level mask is also used for levels in the 'Type' attribute of non-road objects.
    */
    public static final int ROAD_LEVEL_MASK = 0xF000;

    /** A bit flag to identify tunnels. */
    public static final int TUNNEL_ROAD_TYPE_FLAG = 1;

    /** A bit flag to identify bridges. */
    public static final int BRIDGE_ROAD_TYPE_FLAG = 0x10000;

    /** The amount by which road levels are shifted. */
    public static final int ROAD_LEVEL_SHIFT = 12;

    /** Roads of unknown type. */
    public static final int UNKNOWN_MAJOR_ROAD_TYPE = 0;

    /** Primary road with limited access (motorway, freeway, etc.). */
    public static final int PRIMARY_LIMITED_ACCESS_ROAD_TYPE = 0x100;

    /** Primary road without limited access: UK 'A' road. */
    public static final int PRIMARY_UNLIMITED_ACCESS_ROAD_TYPE = 0x200;

    /** Secondary road: UK 'B' road. */
    public static final int SECONDARY_ROAD_TYPE = 0x300;

    /** Local road or town or city street. */
    public static final int MINOR_ROAD_TYPE = 0x400;

    /** Byway: road or track open to motor vehicles. */
    public static final int BYWAY_ROAD_TYPE = 0x500;

    /** Access ramp to limited access road. */
    public static final int ACCESS_RAMP_TYPE = 0x600;

    /** Service road or access road. */
    public static final int SERVICE_ROAD_TYPE = 0x700;

    /** Vehicular Ferry route. */
    public static final int VEHICULAR_FERRY_ROAD_TYPE = 0x800;

    /** Passenger-only Ferry route. */
    public static final int PASSENGER_FERRY_ROAD_TYPE = 0x900;

    /** Path or walkway for pedestrians. */
    public static final int PATH_ROAD_TYPE = 0xA00;

    /** Stairway or escalator for pedestrians. */
    public static final int STAIRWAY_ROAD_TYPE = 0xB00;

    /** Cycle path. */
    public static final int CYCLE_PATH_ROAD_TYPE = 0xC00;

    /** Footpath. */
    public static final int FOOTPATH_ROAD_TYPE = 0xD00;

	/**
	A toll must be paid to use this road.
	More details may be available in a the string attributes of a map object.
	*/
    public static final int TOLL_ROAD_TYPE_FLAG = 2;

    /** A bit flag indicating that the road is part of a roundabout. */
    public static final int ROUNDABOUT_ROAD_TYPE_FLAG = 4;

    /**
    A bit flag indicating that the road has one or more restrictions (e.g., right turn only). The details
    of the restrictions are stored in string attributes.
    */
    public static final int RESTRICTION_ROAD_TYPE_FLAG = 8;

    /** A bit flag indicating that the road is one-way in the direction in which it is defined. */
    public static final int ONE_WAY_FORWARD_ROAD_TYPE_FLAG = 16;

    /** A bit flag indicating that the road is one-way in the reverse direction to that in which it is defined. */
    public static final int ONE_WAY_BACKWARD_ROAD_TYPE_FLAG = 32;

    /** A bit flag to allow access ramps to be marked as sub-types of a major road type. */
    public static final int RAMP_ROAD_TYPE_FLAG = 64;

    /**
    A bit flag to allow a distinction to be made between grades of road, within the major road type.
    It allows OSM data to distinguish trunk roads from primary roads in a backward-compatible way.
    */
    public static final int LOWER_GRADE_ROAD_TYPE_FLAG = 128;

    /** The first raised level: the default level for a bridge or overpass. */
    public static final int FIRST_OVER_ROAD_LEVEL = 0x1000;

    /** The first sub-surface level: the default value for an underpass. */
    public static final int FIRST_UNDER_ROAD_LEVEL = 0xF000;

    /** The second sub-surface level: the default value for a tunnel. */
    public static final int SECOND_UNDER_ROAD_LEVEL = 0xE000;

    /**
    Access rules.
    These flags occupy the bits used for the OSM type in non-road layers,
    so may be used only where an OSM type is not used.
    Each flag forbids access to a certain type of vehicle.
    */
    public static final int ROAD_ACCESS_MASK = 0xFFFE0000;

    /** Access is forbidden to bicycles.*/
    public static final int BICYCLE_ROAD_ACCESS_FLAG = 0x20000;

    /** Access is forbidden to motorcycles.*/
    public static final int MOTOR_CYCLE_ROAD_ACCESS_FLAG = 0x40000;

    /** Access is forbidden to motor cars.*/
    public static final int MOTOR_CAR_ROAD_ACCESS_FLAG = 0x80000;

    /** Access is forbidden to high-occupancy vehicles.*/
    public static final int HIGH_OCCUPANCY_ROAD_ACCESS_FLAG = 0x100000;

    /** Access is forbidden to light goods vehicles.*/
    public static final int GOODS_ROAD_ACCESS_FLAG = 0x200000;

    /** Access is forbidden to heavy goods vehicles.*/
    public static final int HEAVY_GOODS_ROAD_ACCESS_FLAG = 0x400000;

    /** Access is forbidden to buses.*/
    public static final int BUS_ROAD_ACCESS_FLAG = 0x800000;

    /** Access is forbidden to taxis.*/
    public static final int TAXI_ROAD_ACCESS_FLAG = 0x1000000;

    /** Access is forbidden to tourist buses.*/
    public static final int TOURIST_BUS_ROAD_ACCESS_FLAG = 0x2000000;

    /** Access is forbidden to agricultural vehicles.*/
    public static final int AGRICULTURAL_ROAD_ACCESS_FLAG = 0x4000000;

    /** Access is forbidden to forestry vehicles.*/
    public static final int FORESTRY_ROAD_ACCESS_FLAG = 0x8000000;

    /** Access is forbidden to emergency vehicles.*/
    public static final int EMERGENCY_ROAD_ACCESS_FLAG = 0x10000000;

    /** Access is forbidden to vehicles carrying hazardous materials.*/
    public static final int HAZARDOUS_ROAD_ACCESS_FLAG = 0x20000000;

    /** Access is forbidden to wheelchairs.*/
    public static final int WHEEL_CHAIR_ROAD_ACCESS_FLAG = 0x40000000;

    /** Access is forbidden to vehicles with disabled persons' permits.*/
    public static final int DISABLED_ROAD_ACCESS_FLAG = 0x80000000;

    public static final int MOTOR_VEHICLE_ROAD_ACCESS_MASK = 0xFFFE0000 & ~(BICYCLE_ROAD_ACCESS_FLAG | WHEEL_CHAIR_ROAD_ACCESS_FLAG);
    public static final int PUBLIC_SERVICE_VEHICLE_ROAD_ACCESS_MASK = BUS_ROAD_ACCESS_FLAG | TAXI_ROAD_ACCESS_FLAG;
    }
