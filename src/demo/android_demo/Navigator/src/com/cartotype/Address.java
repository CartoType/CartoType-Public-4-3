/*
Address.java
Copyright (C) 2012-2015 CartoType Ltd.
See www.cartotype.com for more information.
*/

package com.cartotype;

/**
An address for use with the Framework.findAddress.
Any fields may be filled in or left empty.
*/
public class Address
    {
    /**
    Gets the address as a string.
    If aFull is true supplies the administrative divisions
    (county, state, province, etc.) and country.
    */
    public native String toString(boolean aFull);

    /** The name or number of the building. */
    public String iBuilding;
    /** The name of a feature or place of interest. */
    public String iFeature;
    /** The street, road or other highway. */
    public String iStreet;
    /** The suburb or neighborhood. */
    public String iSubLocality;
    /** The village, town or city. */
    public String iLocality;
    /** The island. */
    public String iIsland;
    /** The subsidiary administrative area: district, parish, etc. */
    public String iSubAdminArea;
    /** The administrative area: state, province, etc. */
    public String iAdminArea;
    /** The country. */
    public String iCountry;
    /** The postal code. */
    public String iPostCode;
    }
