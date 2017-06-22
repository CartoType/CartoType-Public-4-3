/*
CARTOTYPE_FIND_PARAM.H
Copyright (C) 2013-2016 CartoType Ltd.
See www.cartotype.com for more information.
*/

#ifndef CARTOTYPE_FIND_PARAM_H__
#define CARTOTYPE_FIND_PARAM_H__

#include <cartotype_types.h>

namespace CartoType
{

/** Parameters for the general find function. */
class TFindParam
    {
    public:
    /** The maximum number of objects to return; default = SIZE_MAX. */
    size_t iMaxObjectCount = SIZE_MAX;
    /** The clip rectangle; no clipping is done if iClip is empty. */
    TRectFP iClip;
    /** The coordinate type used for iClip; default = EMapCoordType. */
    TCoordType iClipCoordType = EMapCoordType;
    /** A list of layer names separated by spaces or commas. If it is empty all layers are searched. Layer names may contain the wild cards * and ?. */
    CString iLayers;
    /**
    iAttributes is used in text searching (if iText is non-null). If iAttributes
    is empty, search all attributes, otherwise iAttributes is a list of
    attributes separated by spaces or commas; use "$" to indicate the label (the
    unnamed attribute).
    */
    CString iAttributes;
    /**
    iText, if not empty, restricts the search to objects containing a string
    in one of their string attributes.
    */
    CString iText;
    /** The string matching method used for text searching; default = EStringMatchExact. */
    TStringMatchMethod iStringMatchMethod = EStringMatchExact;
    /**
    iCondition, if not empty, is a style sheet condition (e.g., "Type==2")
    which must be fulfilled by all the objects. 
    */
    CString iCondition;
    /** If iMerge is true (the default), adjoining objects with the same name and attributes may be merged into a single object. */
    bool iMerge = true;
    };

}

#endif
