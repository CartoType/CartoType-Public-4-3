/*
CARTOTYPE_ROAD_TYPE.H
Copyright (C) 2013-2015 CartoType Ltd.
See www.cartotype.com for more information.
*/

#ifndef CARTOTYPE_ROAD_TYPE_H__
#define CARTOTYPE_ROAD_TYPE_H__

#include <cartotype_types.h>

namespace CartoType
{

/**
Standard types for road objects; level, bridge and tunnel flags
are also used for other types of objects.

Bit assignments:

bit 0: tunnel;
bit 1: toll;
bit 2: roundabout;
bit 3: restricted turn exists - see the _R string attribute for details;
bit 4: one way forward;
bit 5: one way backward;
bit 6: link;
bit 7: lower grade road;
bits 8...11: major road type;
bits 12...15: level;
bit 16: bridge;
bits 17...31: road access flags;
*/
enum TRoadType
	{
	/** The toll, roundabout, restriction and direction flags are held in bits 1...5. */
	ERoadRoutingMask = 0x3E,

	/** The road type is held in bits 6...11. */
	ERoadTypeMask = 0xFC0,

	/** The major road type is held in bits 8...11; bits 7 and 6 are used for finer distinctions. */
	EMajorRoadTypeMask = 0xF00,

	/**
	The level (0 = surface, positive = above surface, negative = below surface)
	is held as a signed nybble in bits 12...15. Levels above 0 are not necessarily
	bridges; they can be embankments or other raised structures, and levels below 0 are
	not necessarily tunnels. There are separate bits to mark bridges and tunnels:
	EBridgeRoadTypeFlag and ETunnelRoadTypeFlag.

	The level mask is also used for levels in the 'Type' attribute of non-road objects.
	*/
	ERoadLevelMask = 0xF000,

	/** A bit flag to identify tunnels. */
	ETunnelRoadTypeFlag = 1,

	/** A bit flag to identify bridges. */
	EBridgeRoadTypeFlag = 0x10000,

	/** The amount by which road levels are shifted. */
	ERoadLevelShift = 12,

	/** A bit flag to allow access ramps to be marked as sub-types of a major road type. */
	ERampRoadTypeFlag = 64,

	/** A synonym for ERampRoadTypeFlag. */
	ELinkRoadTypeFlag = ERampRoadTypeFlag,

	/**
	A bit flag to allow a distinction to be made between grades of road, within the major road type.
	It allows OSM data to distinguish trunk roads from primary roads in a backward-compatible way.
	*/
	ELowerGradeRoadTypeFlag = 128,

	/** Roads of unknown type. */
	EUnknownMajorRoadType = 0,

	/** Primary road with limited access (motorway, freeway, etc.). */
	EPrimaryLimitedAccessRoadType = 0x100,

	/** Primary road without limited access: UK 'A' road. */
	EPrimaryUnlimitedAccessRoadType = 0x200,

	/** Secondary road: UK 'B' road. */
	ESecondaryRoadType = 0x300,

	/** Local road or town or city street. */
	EMinorRoadType = 0x400,

	/** Byway: road or track open to motor vehicles. */
	EBywayRoadType = 0x500,

	/** Access ramp to limited access road. */
	EAccessRampType = 0x600,

	/** Service road or access road. */
	EServiceRoadType = 0x700,

	/** Vehicular Ferry route. */
	EVehicularFerryRoadType = 0x800,

	/** Passenger-only Ferry route. */
	EPassengerFerryRoadType = 0x900,

	/** Path or walkway for pedestrians. */
	EPathRoadType = 0xA00,

	/** Stairway or escalator for pedestrians. */
	EStairwayRoadType = 0xB00,

    /** Cycle path. */
    ECyclePathRoadType = 0xC00,

    /** Footpath. */
    EFootPathRoadType = 0xD00,
    
    /** User-defined road types. */
    EOtherRoadType0 = 0xE00,
    EOtherRoadType1 = 0xE00 | ELowerGradeRoadTypeFlag,
    EOtherRoadType2 = 0xE00 | ELinkRoadTypeFlag,
    EOtherRoadType3 = 0xE00 | ELowerGradeRoadTypeFlag | ELinkRoadTypeFlag,
    EOtherRoadType4 = 0xF00,
    EOtherRoadType5 = 0xF00 | ELowerGradeRoadTypeFlag,
    EOtherRoadType6 = 0xF00 | ELinkRoadTypeFlag,
    EOtherRoadType7 = 0xF00 | ELowerGradeRoadTypeFlag | ELinkRoadTypeFlag,

	/**
	A toll must be paid to use this road.
	More details may be available in string attributes.
	*/
	ETollRoadTypeFlag = 2,

	/** A bit flag indicating that the road is part of a roundabout. */
	ERoundaboutRoadTypeFlag = 4,

	/**
    A bit flag indicating that the road has one or more restrictions (e.g., right turn only). The details
    of the restrictions are stored in string attributes.
    */
	ERestrictionRoadTypeFlag = 8,

    /** The road direction and driving side is stored in two bits. */
    ERoadDirectionMask = 16 | 32,
    ERoadDirectionShift = 4,

    /** A road direction value indicating two-way traffic, driving on the right. */
    EDriveOnRightRoadDirection = 0,

    /** A road direction value indicating two-way traffic, driving on the left. */
    EDriveOnLeftRoadDirection = 48,

	/** A road direction value indicating that the road is one-way in the direction in which it is defined. */
	EOneWayForwardRoadDirection = 16,

	/** A road direction value indicating that the road is one-way in the reverse direction to that in which it is defined. */
	EOneWayBackwardRoadDirection = 32,

	/** The first raised level: the default level for a bridge or overpass. */
	EFirstOverRoadLevel = 0x1000,

	/** The first sub-surface level: the default value for an underpass. */
	EFirstUnderRoadLevel = 0xF000,

	/** The second sub-surface level: the default value for a tunnel. */
	ESecondUnderRoadLevel = 0xE000,

	/**
    Access rules.
    These flags occupy the bits used for the OSM type in non-road layers,
    so may be used only where an OSM type is not used.
    Each flag forbids access to a certain type of vehicle.
    */
	ERoadAccessMask = 0xFFFE0000,

    /** Access is forbidden to bicycles.*/
    EBicycleRoadAccessFlag = 0x20000,

    /** Access is forbidden to motorcycles.*/
    EMotorCycleRoadAccessFlag = 0x40000,

    /** Access is forbidden to motor cars.*/
    EMotorCarRoadAccessFlag = 0x80000,

    /** Access is forbidden to high-occupancy vehicles.*/
    EHighOccupancyRoadAccessFlag = 0x100000,

    /** Access is forbidden to light goods vehicles.*/
    EGoodsRoadAccessFlag = 0x200000,

    /** Access is forbidden to heavy goods vehicles.*/
    EHeavyGoodsRoadAccessFlag = 0x400000,

    /** Access is forbidden to buses.*/
    EBusRoadAccessFlag = 0x800000,

    /** Access is forbidden to taxis.*/
    ETaxiRoadAccessFlag = 0x1000000,

    /** Access is forbidden to tourist buses.*/
    ETouristBusRoadAccessFlag = 0x2000000,

    /** Access is forbidden to agricultural vehicles.*/
    EAgriculturalRoadAccessFlag = 0x4000000,

    /** Access is forbidden to forestry vehicles.*/
    EForestryRoadAccessFlag = 0x8000000,

    /** Access is forbidden to emergency vehicles.*/
    EEmergencyRoadAccessFlag = 0x10000000,

    /** Access is forbidden to vehicles carrying hazardous materials.*/
    EHazardousRoadAccessFlag = 0x20000000,

    /** Access is forbidden to wheelchairs.*/
    EWheelChairRoadAccessFlag = 0x40000000,

    /** Access is forbidden to vehicles with disabled persons' permits.*/
    EDisabledRoadAccessFlag = 0x80000000,

    EMotorVehicleRoadAccessMask = 0xFFFE0000 & ~(EBicycleRoadAccessFlag | EWheelChairRoadAccessFlag),
    EPublicServiceVehicleRoadAccessMask = EBusRoadAccessFlag | ETaxiRoadAccessFlag,
    
    // useful synonyms for road types
	EMotorway = EPrimaryLimitedAccessRoadType,
	EMotorwayLink = EPrimaryLimitedAccessRoadType | ELinkRoadTypeFlag,
	ETrunkRoad = EPrimaryUnlimitedAccessRoadType,
	ETrunkRoadLink = EPrimaryUnlimitedAccessRoadType | ELinkRoadTypeFlag,
	EPrimaryRoad = EPrimaryUnlimitedAccessRoadType | ELowerGradeRoadTypeFlag,
	EPrimaryRoadLink = EPrimaryUnlimitedAccessRoadType | ELowerGradeRoadTypeFlag | ELinkRoadTypeFlag,
	ESecondaryRoad = ESecondaryRoadType,
	ESecondaryRoadLink = ESecondaryRoadType | ELinkRoadTypeFlag,
	ETertiaryRoad = ESecondaryRoadType | ELowerGradeRoadTypeFlag,
	ETertiaryRoadLink = ESecondaryRoadType | ELowerGradeRoadTypeFlag | ELinkRoadTypeFlag,
	EUnclassifiedRoad = EMinorRoadType,
    EResidentialRoad = EMinorRoadType | ELowerGradeRoadTypeFlag,
	ETrack = EBywayRoadType,
	EServiceRoad = EServiceRoadType,
	EPedestrianRoad = EPathRoadType,
    EVehicularFerry = EVehicularFerryRoadType,
    EPassengerFerry = EPassengerFerryRoadType,
	};

inline bool RoadTypeIsOneWay(uint32 aRoadType)
    {
    return ((aRoadType & EOneWayForwardRoadDirection) << 1) != (aRoadType & EOneWayBackwardRoadDirection);
    }

}

#endif
