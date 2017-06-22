/*
CARTOTYPE_ADDRESS.H
Copyright (C) 2013-2014 CartoType Ltd.
See www.cartotype.com for more information.
*/

#ifndef CARTOTYPE_ADDRESS_H__
#define CARTOTYPE_ADDRESS_H__

#include <cartotype_string.h>

namespace CartoType
{

/** A structured address for use with FindAddress. */
class CAddress
    {
    public:
    void Clear();
    CString ToString(bool aFull = true,const TPointFP* aLocation = nullptr) const;
    CString ToStringWithLabels() const;
    
    /** The name or number of the building. */
    CString iBuilding;
    /** The name of a feature or place of interest. */
    CString iFeature;
    /** The street, road or other highway. */
    CString iStreet;
    /** The suburb, neighborhood, quarter or other subdivision of the locality. */
    CString iSubLocality;
    /** The village, town or city. */
    CString iLocality;
    /** The name of an island. */
    CString iIsland;
    /**
    The subsidiary administrative area: county, district, etc.
    By preference this is a level-6 area in the OpenStreetMap classification.
    Levels 7, 8 and 5 are used in that order if no level-6 area is found.
    */
    CString iSubAdminArea;
    /**
    The administrative area: state, province, etc.
    By preference this is a level-4 area in the OpenStreetMap classification.
    Level 3 is used if no level-4 area is found.
    */
    CString iAdminArea;
    /** The country. */
    CString iCountry;
    /** The postal code. */
    CString iPostCode;
    };

/** Address part codes used when searching for address parts separately. */
enum TAddressPart
    {
    EAddressPartBuilding,
    EAddressPartFeature,
    EAddressPartStreet,
    EAddressPartSubLocality,
    EAddressPartLocality,
    EAddressPartIsland,
    EAddressPartSubAdminArea,
    EAddressPartAdminArea,
    EAddressPartCountry,
    EAddressPartPostCode
    };

/**
A type used in addresses obtained by reverse geocoding.
It gives a coarse notion of what an object is.
Codes are ordered roughly from small to large.
*/
enum TGeoCodeType
    {
    EGeoCodeNone,
    EGeoCodePosition,
    EGeoCodeAddress,
    
    EGeoCodeBuilding,
    EGeoCodeFarm,

    EGeoCodeFootpath,
    EGeoCodeCycleway,

    EGeoCodeFerryRoute,
    EGeoCodeRailway,

    EGeoCodePedestrianRoad,
    EGeoCodeTrack,
    EGeoCodeServiceRoad,
    EGeoCodeResidentialRoad,
    EGeoCodeUnclassifiedRoad,
    EGeoCodeTertiaryRoad,
    EGeoCodeSecondaryRoad,
    EGeoCodePrimaryRoad,
    EGeoCodeTrunkRoad,
    EGeoCodeMotorway,

    EGeoCodeLandFeature,
    EGeoCodeWaterFeature,

    EGeoCodeLocality,
    EGeoCodeHamlet,

    EGeoCodePostCode,

    EGeoCodeAdminArea10,
    EGeoCodeAdminArea9,
    EGeoCodeNeighborhood,
    EGeoCodeVillage,
    EGeoCodeSuburb,

    EGeoCodeIsland,

    EGeoCodeAdminArea8,
    EGeoCodeTown,

    EGeoCodeAdminArea7,
    EGeoCodeCity,
    
    EGeoCodeAdminArea6,
    EGeoCodeAdminArea5,
    EGeoCodeAdminArea4,
    EGeoCodeAdminArea3,
    EGeoCodeAdminArea2,
    EGeoCodeAdminArea1,
    EGeoCodeCountry,
    EGeoCodeContinent
    };

/** A geocode item describes a single map object. */
class CGeoCodeItem
    {
    public:
    CGeoCodeItem():
        iGeoCodeType(EGeoCodeNone)
        {
        }

    TGeoCodeType iGeoCodeType;

    /**
    The name of the object in the locale used when requesting a geocode.
    For buildings, this may be a building number.
    */
    CString iName;
    /** The postal code if any. */
    CString iPostCode;
    };

}

#endif
