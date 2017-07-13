/*
CARTOTYPE_FRAMEWORK.H
Copyright (C) 2012-2017 CartoType Ltd.
See www.cartotype.com for more information.
*/

#ifndef CARTOTYPE_FRAMEWORK_H__
#define CARTOTYPE_FRAMEWORK_H__

#include <cartotype_address.h>
#include <cartotype_bitmap.h>
#include <cartotype_find_param.h>
#include <cartotype_navigation.h>
#include <cartotype_stream.h>
#include <cartotype_string.h>
#include <cartotype_tile_param.h>
#include <cartotype_map_object.h>
#include <cartotype_graphics_context.h>
#include <cartotype_image_server_helper.h>
#include <cartotype_legend.h>
#include <cartotype_style_sheet_data.h>

#include <memory>
#include <set>

namespace CartoType
{

/**
\mainpage CartoType

\section introduction Introduction

CartoType is a portable C++ library for drawing maps, calculating routes
and providing navigation instructions. It uses map files created using the
makemap tool supplied with the package.

\section highlevelapi The Framework API

The CFramework class is the main API for CartoType and allows
you to build CartoType into your application.

You create a single CFramework
object and keep it in existence while your program is running.
It provides access to everything you need, including
map drawing, adding your own data to the map, handling user
interaction, searching, route calculation and turn-by-turn navigation.

The classes CFrameworkEngine and CFrameworkMapDataSet, in conjunction
with CFramework, allow you to use the model-view-controller pattern.
Usually, however, CFramework is all you need.
*/

class CEngine;
class CImageServer;
class CGcImageServerHelper;
class CMapDataAccessor;
class CPerspectiveGraphicsContext;
class MInternetAccessor;
class CWebMapServiceClient;
class CMap;
class CMapDrawParam;
class CMapDataBase;
class CMapStore;
class CMapStyle;
class CMemoryMapDataBase;
class CNavigator;
class CTileDataAccessor;
class CGraphicsContext;
class C32BitColorBitmapGraphicsContext;
class CStackAllocator;
class CTileServer;
class TMapTransform;
class CVectorTileServer;
class CVectorTileHelper;

/** States of the navigation system. */
enum TNavigationState
    {
    /** Unknown state, or not navigating. */
    ENoNavigationState,
    /** There is a turn ahead. */
    ETurnNavigationState,
    /** The recommended action is to turn round and go back. */
    ETurnRoundNavigationState,
    /** A new route has been calculated after a period off route. */
    ENewRouteNavigationState,
    /** The next significant point on the route is the destination. */
    EArrivalNavigationState,
    /** The current position is off the route. */ 
    EOffRouteNavigationState,
    /** Re-routing is needed and will be done when the next valid position is supplied. Occurs only if auto-re-routing is turned off. */
    EReRouteNeededNavigationState
    };

/** A flag to make the center of the map follow the user's location. */
const uint32 KFollowFlagLocation = 1;
/** A flag to rotate the map to the user's heading. */
const uint32 KFollowFlagHeading = 2;
/** A flag to set the map to a suitable zoom level for the user's speed. */
const uint32 KFollowFlagZoom = 4;

/** Flags controlling the way the map follows the user location and heading and automatically zooms. */
enum TFollowMode
    {
    /** The map does not follow the user's location or heading. */
    EFollowModeNone = 0,
    /** The map is centred on the user's location. */
    EFollowModeLocation = KFollowFlagLocation,
    /** The map is centred on the user's location and rotated to the user's heading. */
    EFollowModeLocationHeading = KFollowFlagLocation | KFollowFlagHeading,
    /** The map is centred on the user's location and zoomed to a suitable level for the user's speed. */
    EFollowModeLocationZoom = KFollowFlagLocation | KFollowFlagZoom,
    /** The map is centred on the user's location, rotated to the user's heading, and zoomed to a suitable level for the user's speed. */
    EFollowModeLocationHeadingZoom = KFollowFlagLocation | KFollowFlagHeading | KFollowFlagZoom
    };

/**
CFrameworkEngine holds a CTM1 data accessor and a standard font.
Together with a CFrameworkMapDataSet object it makes up the 'model' part of the model-view-controller pattern.
*/
class CFrameworkEngine
    {
    public:
    static std::unique_ptr<CFrameworkEngine> New(TResult& aError,const CString& aFontFileName,int32 aFileBufferSizeInBytes = 0,int32 aMaxFileBufferCount = 0,int32 aTextIndexLevels = 0);
    ~CFrameworkEngine();
    TResult Configure(const CString& aFilename);
    TResult LoadFont(const CString& aFontFileName);
    TResult LoadFont(const uint8* aData,size_t aLength,bool aCopyData);
    std::string Name() const;
    std::unique_ptr<CFrameworkEngine> Copy(TResult& aError) const;
    void CancelDrawing();

    // internal use only
    std::shared_ptr<CEngine> Engine() const { return iEngine; }

    private:
    CFrameworkEngine();
    CFrameworkEngine(const CFrameworkEngine&) = delete;
    CFrameworkEngine(CFrameworkEngine&&) = delete;
    CFrameworkEngine& operator=(const CFrameworkEngine&) = delete;
    CFrameworkEngine& operator=(CFrameworkEngine&&) = delete;

    std::shared_ptr<CEngine> iEngine;
    CString iConfigFileName;
    std::vector<CString> iFontFileName;
    int32 iFileBufferSizeInBytes;
    int32 iMaxFileBufferCount;
    int32 iTextIndexLevels;
    mutable std::string iName; // the name of the dataset as an XML string made of the file names of any loaded fonts
    };

/**
CFrameworkMapDataSet owns a set of map data used to draw a map.
Together with a CFrameworkEngine object it makes up the 'model' part of the model-view-controller pattern.
*/
class CFrameworkMapDataSet
    {
    public:
    static std::unique_ptr<CFrameworkMapDataSet> New(TResult& aError,CFrameworkEngine& aEngine,const CString& aMapFileName,const std::string* aEncryptionKey = nullptr);
    static std::unique_ptr<CFrameworkMapDataSet> New(TResult& aError,CFrameworkEngine& aEngine,std::unique_ptr<CMapDataBase> aDb);
    static std::unique_ptr<CFrameworkMapDataSet> New(TResult& aError,CFrameworkEngine& aEngine,const TTileParam& aTileParam);

    ~CFrameworkMapDataSet();

    std::unique_ptr<CFrameworkMapDataSet> Copy(TResult& aError,CFrameworkEngine& aEngine) const;
    TResult LoadMapData(CFrameworkEngine& aEngine,const CString& aMapFileName,const std::string* aEncryptionKey = nullptr);
    TResult LoadMapData(CFrameworkEngine& aEngine,const TTileParam& aTileParam);
    TResult LoadMapData(std::unique_ptr<CMapDataBase> aDb);
    TResult UnloadMapByHandle(uint32 aHandle);
    uint32 GetLastMapHandle() const;
    TResult CreateWritableMap(CFrameworkEngine& aEngine,TWritableMapType aType,CString aFileName = nullptr);
    TResult SaveMap(uint32 aHandle,const CString& aFileName,TFileType aFileType);
    TResult ReadMap(uint32 aHandle,const CString& aFileName,TFileType aFileType);
    bool MapIsEmpty(uint32 aHandle);
    std::unique_ptr<CMap> CreateMap(CFrameworkEngine& aEngine,int32 aViewWidth,int32 aViewHeight);
    uint32 GetMainMapHandle() const;
    uint32 GetMemoryMapHandle() const;
    size_t MapCount() const;
    uint32 MapHandle(size_t aIndex) const;
    bool MapIsWritable(size_t aIndex) const;
    std::vector<CString> LayerNames();
    
    TResult InsertMapObject(uint32 aMapHandle,TMapObjectType aType,const CString& aLayerName,const MPath& aGeometry,
                            const CString& aStringAttributes,int32 aIntAttribute,uint64& aId,bool aReplace);
    TResult InsertPointMapObject(uint32 aMapHandle,const CString& aLayerName,TPoint aPoint,
                                 const CString& aStringAttributes,int32 aIntAttribute,uint64& aId,bool aReplace);
    TResult InsertCircleMapObject(uint32 aMapHandle,const CString& aLayerName,TPoint aCenter,int32 aRadius,
                                  const CString& aStringAttributes,int32 aIntAttribute,uint64& aId,bool aReplace);
    TResult InsertEnvelopeMapObject(uint32 aMapHandle,const CString& aLayerName,const MPath& aGeometry,int32 aRadius,
                                    const CString& aStringAttributes,int32 aIntAttribute,uint64& aId,bool aReplace);
    TResult DeleteMapObjects(uint32 aMapHandle,uint64 aStartId,uint64 aEndId,uint64& aDeletedCount,CString aCondition = nullptr);
    std::unique_ptr<CMapObject> LoadMapObject(TResult& aError,uint32 aMapHandle,uint64 aId);
    TResult ReadGpx(uint32 aMapHandle,const CString& aFileName);
    std::string Name() const;
    std::string Proj4Param() const;

    // For internal use only.
    std::shared_ptr<CMapDataBaseArray> MapDataBaseArray() const { return iMapDataBaseArray; }
    CMapDataBase& MainDb() const { return *(*iMapDataBaseArray)[0]; }

    private:
    CFrameworkMapDataSet();
    CFrameworkMapDataSet(const CFrameworkMapDataSet&) = delete;
    CFrameworkMapDataSet(CFrameworkMapDataSet&&) = delete;
    CFrameworkMapDataSet& operator=(const CFrameworkMapDataSet&) = delete;
    CFrameworkMapDataSet& operator=(const CFrameworkMapDataSet&&) = delete;
    
    CMapDataBase* GetMapDb(uint32 aHandle);

    std::shared_ptr<CMapDataBaseArray> iMapDataBaseArray;
    std::unique_ptr<CTileDataAccessor> iTileDataAccessor;
    uint32 iLastMapHandle;
    uint32 iMemoryMapHandle;
    mutable std::string iName; // the name of the dataset as an XML string made of the file names of any loaded maps
    mutable std::string iProj4Param; // the proj.4 parameter string for the projection, if known; the empty string if not
    };

/**
An observer interface which receives notifications
of changes to the framework state. It is intended for the use
of higher-level GUI objects which need to update their display
after framework state has been changed programmatically. For example,
if a route is created, dynamic map objects need to be redrawn.
*/
class MFrameworkObserver
    {
    public:
    virtual ~MFrameworkObserver() { }

    /**
    This virtual function is called when the map view changes,
    which can be caused by panning, zooming, rotation, moving to a
    new location, or resizing the map.
    */
    virtual void OnViewChange() = 0;
    
    /**
    This virtual function is called when the map data changes,
    which can be caused by loading a new map, unloading a map,
    enabling or disabling a map, enabling or disabling a layer,
    or changing the style sheet.
    */
    virtual void OnMainDataChange() = 0;
    
    /**
    This virtual function is called when the dynamic data changes,
    which can be caused by creating or deleting a route, or
    inserting or deleting a pushpin or other dynamic map object.
    */
    virtual void OnDynamicDataChange() = 0;
    };

/** Parameters used to set the perspective view. */
class TPerspectiveParam
    {
    public:
    /** The position of the point on the terrain below the camera. */
    TPointFP iPosition;
    /** The type of the coordinates used by iPosition. */
    TCoordType iCoordType = EDegreeCoordType;
    /** If true, ignore iPosition, and set the camera position so that the location in the center of the display is shown. */
    bool iAutoPosition = true;
    /** The height of the camera above the terrain. The value 0 causes a default value to be used, which is the width of the display. */
    double iHeightInMeters = 0;
    /** The azimuth of the camera in degrees going clockwise, where 0 is N, 90 is E, etc. */
    double iAzimuthDegrees = 0;
    /** If true, ignore iAzimuthDegrees and use the current map orientation. */
    bool iAutoAzimuth = true;
    /** The declination of the camera downward from the horizontal plane. Values are clamped to the range -90...90. */
    double iDeclinationDegrees = 30;
    /** The amount by which the camera is rotated about its axis, after applying the declination, in degrees going clockwise. */
    double iRotationDegrees = 0;
    /** The camera's field of view in degrees. */
    double iFieldOfViewDegrees = 22.5;
    };

/**
The CFramework class provides a high-level API for CartoType.
through which map data can be loaded, maps can be created and viewed,
and routing and navigation can be performed.

It owns or references a CFrameworkEngine and a CFrameworkMapDataSet.

It is the 'view' part of the model-view-controller pattern. If there is only
one model it owns the engine and map data set; otherwise it can share
them with other CFramework objects.
*/
class CFramework: public MNavigatorObserver
    {
    public:
    static std::unique_ptr<CFramework> New(TResult& aError,
                                           const CString& aMapFileName,
                                           const CString& aStyleSheetFileName,
                                           const CString& aFontFileName,
                                           int32 aViewWidth,int32 aViewHeight,
                                           const std::string* aEncryptionKey = nullptr);
    static std::unique_ptr<CFramework> New(TResult& aError,
                                           std::shared_ptr<TTileParam> aTileParam,
                                           const CString& aStyleSheetFileName,
                                           const CString& aFontFileName,
                                           int32 aViewWidth,int32 aViewHeight);
    static std::unique_ptr<CFramework> New(TResult& aError,
                                           std::shared_ptr<CFrameworkEngine> aSharedEngine,
                                           std::shared_ptr<CFrameworkMapDataSet> aSharedMapDataSet,
                                           const CString& aStyleSheetFileName,
                                           int32 aViewWidth,int32 aViewHeight,
                                           const std::string* aEncryptionKey = nullptr);

    /**
    Parameters for creating a framework when more detailed control is needed.
    For example, file buffer size and the maximum number of buffers can be set.
    */
    class TParam
        {
        public:
        /** The first map. Must not be empty. */
        CString iMapFileName;
        /** The first style sheet. If this string is empty, the style sheet must be supplied in iStyleSheetText. */
        CString iStyleSheetFileName;
        /** The style sheet text; used if iStyleSheetFileName is empty. */
        std::string iStyleSheetText;
        /** The first font file. Must not be empty. */
        CString iFontFileName;
        /** The width of the initial map in pixels. Must be greater than zero. */
        int32 iViewWidth = 256;
        /** The height of the initial map in pixels. Must be greater than zero. */
        int32 iViewHeight = 256;
        /** If not empty, an encryption key to be used when loading the first map. */
        std::string iEncryptionKey;
        /** The file buffer size in bytes. If it is less than four the default value is used. */
        int32 iFileBufferSizeInBytes = 0;
        /** The maximum number of file buffers. If it is zero or less the default value is used. */
        int32 iMaxFileBufferCount = 0;
        /**
        The number of levels of the text index to load into RAM.
        Use values from 2 to 5 to make text searches faster, at the cost of using much more RAM.
        The value 0 causes the default number of levels to be loaded, which is 1.
        The value -1 disables text index loading.
        */
        int32 iTextIndexLevels = 0;
        /** If non-null, use this shared engine and do not use iMapFileName or iFontFileName. */
        std::shared_ptr<CFrameworkEngine> iSharedEngine;
        /** If non-null, use this shared dataset and do not use iMapFileName or iFontFileName. */
        std::shared_ptr<CFrameworkMapDataSet> iSharedMapDataSet;
        /** If non-null, use these tile parameters and do not use iMapFileName. */
        std::shared_ptr<TTileParam> iTileParam;
        };
    static std::unique_ptr<CFramework> New(TResult& aError,const TParam& aParam);

    ~CFramework();

    // copying and naming
    std::unique_ptr<CFramework> Copy(TResult& aError) const;
    std::string Name() const;
    std::string Proj4Param() const;

    // observers
    void AddObserver(MFrameworkObserver* aObserver);
    void RemoveObserver(MFrameworkObserver* aObserver);

    // general
    void License(const uint8* aKey,int32 aKeyLength);
    std::string Licensee() const;
    TResult Configure(const CString& aFilename);
    TResult LoadMap(const CString& aMapFileName,const std::string* aEncryptionKey = nullptr);
    TResult LoadMap(const TTileParam& aTileParam);
    TResult CreateWritableMap(TWritableMapType aType,CString aFileName = nullptr);
    TResult SaveMap(uint32 aHandle,const CString& aFileName,TFileType aFileType);
    TResult ReadMap(uint32 aHandle,const CString& aFileName,TFileType aFileType);
    TResult WriteMapImage(const CString& aFileName,TFileType aFileType);
    bool MapIsEmpty(uint32 aHandle);
    size_t MapCount() const;
    uint32 MapHandle(size_t aIndex) const;
    bool MapIsWritable(size_t aIndex) const;
    TResult UnloadMapByHandle(uint32 aHandle);
    TResult EnableMapByHandle(uint32 aHandle,bool aEnable);
    uint32 GetLastMapHandle() const;
    uint32 GetMainMapHandle() const;
    uint32 GetMemoryMapHandle() const;
    TResult LoadFont(const CString& aFontFileName);
    TResult LoadFont(const uint8* aData,size_t aLength,bool aCopyData);
    TResult SetStyleSheet(const CString& aStyleSheetFileName,size_t aIndex = 0);
    TResult SetStyleSheet(const uint8* aData,size_t aLength,size_t aIndex = 0);
    TResult SetStyleSheet(const CStyleSheetData& aStyleSheetData,size_t aIndex = 0);
    TResult ReloadStyleSheet(size_t aIndex = 0);
    void AppendStyleSheet(const CString& aStyleSheetFileName);
    void AppendStyleSheet(const uint8* aData,size_t aLength);
    TResult DeleteStyleSheet(size_t aIndex);
    std::string GetStyleSheetText(size_t aIndex) const;
    CStyleSheetData GetStyleSheetData(size_t aIndex) const;

    TResult Resize(int32 aViewWidth,int32 aViewHeight);
    TResult SetResolutionDpi(double aDpi);
    double ResolutionDpi() const;
    TResult SetScaleDenominator(double aScale);
    double ScaleDenominator() const;
    double MapUnitSize() const;
    TResult SetScaleDenominatorInView(double aScale);
    double GetScaleDenominatorInView() const;
    double GetDistanceInMeters(double aX1,double aY1,double aX2,double aY2,TCoordType aCoordType);
    double ScaleDenominatorFromZoomLevel(double aZoomLevel,int32 aImageSizeInPixels = 256) const;
    double ZoomLevelFromScaleDenominator(double aScaleDenominator,int32 aImageSizeInPixels = 256) const;
    
    void SetPerspective(bool aSet);
    void SetPerspective(const TPerspectiveParam& aParam);
    bool Perspective() const;
    TResult Zoom(double aZoomFactor);
    TResult Rotate(double aAngle);
    TResult SetRotation(double aAngle);
    double Rotation() const;
    TResult RotateRadians(double aAngle);
    TResult SetRotationRadians(double aAngle);
    double RotationRadians() const;
    TResult Pan(int aDx,int aDy);
    TResult Pan(const TPoint& aFrom,const TPoint& aTo);
    TResult Pan(double aFromX,double aFromY,TCoordType aFromCoordType,
                          double aToX,double aToY,TCoordType aToCoordType);
    TResult Scroll(double aXPages,double aYPages);
    TResult GetScrollInfo(int32& aXRange,int32& aYRange,TRect& aPosition) const;
    TResult SetScrollInfo(const TPoint &aPosition);
    TResult SetViewCenter(double aX,double aY,TCoordType aCoordType);
    TResult SetView(double aMinX,double aMinY,double aMaxX,double aMaxY,TCoordType aCoordType,int32 aMarginInPixels = 0,int32 aMinScaleDenominator = 0);
    void SetView(const CMapObject * const * aMapObjectArray,size_t aMapObjectCount,int32 aMarginInPixels,int32 aMinScaleDenominator);
    void SetView(const CMapObjectArray& aMapObjectArray,int32 aMarginInPixels,int32 aMinScaleDenominator);
    void SetView(const CMapObject& aMapObject,int32 aMarginInPixels,int32 aMinScaleDenominator);
    void SetViewToFillMap();
    TResult GetView(double& aMinX,double& aMinY,double& aMaxX,double& aMaxY,TCoordType aCoordType);
    TResult GetView(TRectFP& aView,TCoordType aCoordType);
    TResult GetView(TFixedSizeContour<4>& aView,TCoordType aCoordType);
    TResult GetMapExtent(double& aMinX,double& aMinY,double& aMaxX,double& aMaxY,TCoordType aCoordType);
    TResult GetMapExtent(TRectFP& aMapExtent,TCoordType aCoordType);
    CString GetProjectionAsProj4Param(); 
    
    TResult InsertMapObject(uint32 aMapHandle,TMapObjectType aType,const CString& aLayerName,const CGeometry& aGeometry,
                            const CString& aStringAttributes,int32 aIntAttribute,uint64& aId,bool aReplace);
    TResult InsertPointMapObject(uint32 aMapHandle,const CString& aLayerName,double aX,double aY,
                                 TCoordType aCoordType,const CString& aStringAttributes,int32 aIntAttribute,uint64& aId,bool aReplace);
    TResult InsertCircleMapObject(uint32 aMapHandle,const CString& aLayerName,
                                  double aCenterX,double aCenterY,TCoordType aCenterCoordType,double aRadius,TCoordType aRadiusCoordType,
                                  const CString& aStringAttributes,int32 aIntAttribute,uint64& aId,bool aReplace);
    TResult InsertEnvelopeMapObject(uint32 aMapHandle,const CString& aLayerName,const CGeometry& aGeometry,
                                    double aRadius,TCoordType aRadiusCoordType,
                                    const CString& aStringAttributes,int32 aIntAttribute,uint64& aId,bool aReplace);
    TResult InsertCopyOfMapObject(uint32 aMapHandle,const CString& aLayerName,const CMapObject& aObject,double aEnvelopeRadius,TCoordType aRadiusCoordType,uint64& aId,bool aReplace);
    TResult DeleteMapObjects(uint32 aMapHandle,uint64 aStartId,uint64 aEndId,uint64& aDeletedCount,CString aCondition = nullptr);
    std::unique_ptr<CMapObject> LoadMapObject(TResult& aError,uint32 aMapHandle,uint64 aId);
    TResult ReadGpx(uint32 aMapHandle,const CString& aFileName);
    CGeometry Range(TResult& aError,const TRouteProfile* aProfile,double aX,double aY,TCoordType aCoordType,double aTimeOrDistance,bool aIsTime);

    TResult EnableLayer(const CString& aLayerName,bool aEnable);
    bool LayerIsEnabled(const CString& aLayerName) const;
    std::vector<CString> LayerNames();

    TResult ConvertCoords(double* aCoordArray,size_t aCoordArraySize,TCoordType aFromCoordType,TCoordType aToCoordType);
    TResult ConvertCoords(const TWritableCoordSet& aCoordSet,TCoordType aFromCoordType,TCoordType aToCoordType);
    TResult ConvertPoint(double& aX,double& aY,TCoordType aFromCoordType,TCoordType aToCoordType);
    TResult ConvertPoint(TPoint& aPoint,TCoordType aFromCoordType,TCoordType aToCoordType);
    double PixelsToMeters(double aPixels) const;
    double MetersToPixels(double aMeters) const;
    CString DataSetName() const;

    // drawing the map
    const TBitmap* MapBitmap(TResult& aError,bool* aRedrawWasNeeded = nullptr);
    const TBitmap* MemoryDataBaseMapBitmap(TResult& aError,bool* aRedrawWasNeeded = nullptr);
    void EnableDrawingMemoryDataBase(bool aEnable);
    size_t ObjectsDrawn() const;
    void ForceRedraw();
    void CancelDrawing();
    TResult UseImageServer(MImageServerHelper* aImageServerHelper,bool aDrawMemoryDataBase = false,int32 aCacheSize = KDefaultImageCacheSize);
    TResult UseGcImageServer(bool aEnable,int32 aCacheSize = KDefaultImageCacheSize);
    TResult DrawUsingImageServer(void* aDeviceContext);
    bool UsingImageServer() const;
    TResult UseVectorTileServer(bool aEnable);
    bool ClipBackgroundToMapBounds(bool aEnable);
    bool DrawBackground(bool aEnable);
    int32 SetTileOverSizeZoomLevels(int32 aLevels);
    TResult DrawLabelsToLabelHandler(MLabelHandler& aLabelHandler);
    bool ObjectWouldBeDrawn(TResult& aError,uint64 aId,TMapObjectType aType,const CString& aLayer,int32 aIntAttrib,const CString& aStringAttrib);

    // adding and removing style sheet icons loaded from files
    TResult LoadIcon(const CString& aFileName,const CString& aId,const TPoint& aHotSpot,const TPoint& aLabelPos,int32 aLabelMaxLength);
    void UnloadIcon(const CString& aId);
    
    // using a web map service to get an underlay or overlay for the current map
    TResult UseWebMapService(MInternetAccessor& aInternetAccessor,const std::string& aUrl,const std::string& aUrlSuffix);
    const TBitmap* WebMapServiceBitmap(TResult& aError,bool* aRedrawWasNeeded = nullptr);
    
    // providing data for a web map service
    TResult WebMapServiceGetMap(const char* aWebMapServiceRequest,std::vector<uint8>& aData);

    // drawing Mercator, Plate Carree or arbitrary tiles
    const TBitmap* TileBitmap(TResult& aError,int32 aTileSizeInPixels,int32 aZoom,int32 aX,int32 aY,const TTileBitmapParam* aParam = nullptr);
    const TBitmap* TileBitmap(TResult& aError,int32 aTileSizeInPixels,const CString& aQuadKey,const TTileBitmapParam* aParam = nullptr);
    const TBitmap* TileBitmap(TResult& aError,int32 aTileWidth,int32 aTileHeight,const TRectFP& aBounds,TCoordType aCoordType,const TTileBitmapParam* aParam = nullptr);

    // finding map objects
    TResult Find(CMapObjectArray& aObjectArray,const TFindParam& aFindParam);
    TResult FindInDisplay(CMapObjectArray& aObjectArray,size_t aMaxObjectCount,double aX,double aY,double aRadius);
    TResult FindInLayer(CMapObjectArray& aObjectArray,size_t aMaxObjectCount,
                        const CString& aLayer,double aMinX,double aMinY,double aMaxX,double aMaxY,TCoordType aCoordType);
    TResult FindText(CMapObjectArray& aObjectArray,size_t aMaxObjectCount,const CString& aText,
                     TStringMatchMethod aMatchMethod,const CString& aLayers,const CString& aAttribs);
    TResult FindAddress(CMapObjectArray& aObjectArray,size_t aMaxObjectCount,const CAddress& aAddress,bool aFuzzy = false);
    TResult FindAddressPart(CMapObjectArray& aObjectArray,size_t aMaxObjectCount,const CString& aText,TAddressPart aAddressPart,bool aFuzzy,bool aIncremental);
    TResult FindBuildingsNearStreet(CMapObjectArray& aObjectArray,const CMapObject& aStreet);
    TResult FindPolygonsContainingPoint(CMapObjectArray& aObjectArray,size_t aMaxObjectCount,double aX,double aY,TCoordType aCoordType);
    TResult FindPointsInPolygon(CMapObjectArray& aObjectArray,size_t aMaxObjectCount,const double* aX,const double* aY,size_t aCoords,TCoordType aCoordType);

    // geocoding
    TResult GeoCodeSummary(CString& aSummary,const CMapObject& aMapObject);
    TResult GeoCodeSummary(CString& aSummary,double aX,double aY,TCoordType aCoordType);
    TResult GetAddress(CAddress& aAddress,const CMapObject& aMapObject);
    TResult GetAddress(CAddress& aAddress,double aX,double aY,TCoordType aCoordType);

    // getting information about a map object, taking account of the framework's projection
    double GetMapObjectArea(TResult& aError,const CMapObject& aMapObject) const;
    double GetMapObjectLengthOrPerimeter(TResult& aError,const CMapObject& aMapObject) const;
    double GetMapObjectDistanceFromPoint(TResult& aError,const CMapObject& aMapObject,
                                         double aPointX,double aPointY,TCoordType aPointCoordType,
                                         double& aNearestPointX,double& aNearestPointY,TCoordType aNearestPointCoordType);

    // terrain heights
    TResult GetHeights(const TCoordSet& aCoordSet,TCoordType aCoordType,int32* aTerrainHeightArray);
    TResult GetHeightProfile(const double* aX,const double* aY,int32* aTerrainHeightArray,int32* aObstacleHeightArray,size_t aCoordCount,TCoordType aCoordType,
                             double aRadiusInMetres,CMapObjectArray* aObstacleArray);

    // style sheet variables
    TResult SetStyleSheetVariable(const CString& aVariableName,const CString& aValue);
    TResult SetStyleSheetVariable(const CString& aVariableName,int32 aValue);
    
    // access to graphics
    std::unique_ptr<CBitmapGraphicsContext> CreateGraphicsContext(int32 aWidth,int32 aHeight);
    TFont Font(const TFontSpec& aFontSpec);
    CBitmapGraphicsContext& GetMapGraphicsContext();

    /** The default size of the cache used by the image server. */
    static const size_t KDefaultImageCacheSize = 10 * 1024 * 1024;

    // navigation
    static const size_t KMaxRoutesDisplayed = 16;    // allow a number of alternative routes well in excess of the expected maximum, which is probably 3
    static const size_t KMaxWayPointsDisplayed = 1024 * 1024;    // allow up to a million waypoints; in practice there are likely to be no more than 100

    enum
        {
        /** The first ID reserved for route-related map objects. */
        KRouteIdReservedStart = 0x40000000,

        /** A reserved map object ID for the first route. */
        KFirstRouteId = KRouteIdReservedStart,
        /** A reserved map object ID for the last route. */
        KLastRouteId = KRouteIdReservedStart + KMaxRoutesDisplayed - 1, 
        /** A reserved map object ID for the start of the route. */
        KRouteStartId,
        /** A reserved map object ID for the end of the route. */
        KRouteEndId,
        /** A reserved map object ID for a point object at the current position on the route. */
        KRoutePositionId,
        /** A reserved map object ID for a vector object centered on the current position on the route. */
        KRouteVectorId,
        /** A reserved map object ID for the first waypoint. */
        KFirstWayPointId,
        /** One more than the last ID reserved for route-related map objects. */
        KRouteIdReservedEnd = KFirstWayPointId + KMaxWayPointsDisplayed 
        };
    
    void SetPreferredRouterType(TRouterType aRouterType);
    TRouterType PreferredRouterType() const;
    TRouterType ActualRouterType() const;
    TResult StartNavigation(double aStartX,double aStartY,TCoordType aStartCoordType,
                            double aEndX,double aEndY,TCoordType aEndCoordType);
    TResult StartNavigation(const TCoordSet& aCoordSet,TCoordType aCoordType);
    void EndNavigation();
    bool EnableNavigation(bool aEnable);
    bool NavigationEnabled() const;
    TResult LoadNavigationData();
    bool NavigationDataHasGradients() const;
    TResult SetMainProfile(const TRouteProfile& aProfile);
    void AddProfile(const TRouteProfile& aProfile);
    TResult ChooseRoute(size_t aRouteIndex);
    const TRouteProfile* Profile(size_t aIndex) const;
    bool Navigating() const;
    void AddNavigatorObserver(MNavigatorObserver* aObserver);
    void RemoveNavigatorObserver(MNavigatorObserver* aObserver);
    TPoint RouteStart();
    TPoint RouteEnd();
    TResult DisplayRoute(bool aEnable = true);
    const CRoute* Route() const; 
    const CRoute* Route(size_t aIndex) const;
    std::unique_ptr<CRoute> CreateRoute(TResult& aError,const TRouteProfile& aProfile,const TCoordSet& aCoordSet,TCoordType aCoordType);
    std::unique_ptr<CRoute> CreateBestRoute(TResult& aError,const TRouteProfile& aProfile,const TCoordSet& aCoordSet,TCoordType aCoordType,bool aStartFixed,bool aEndFixed,size_t aIterations = 10);
    std::unique_ptr<CRoute> CreateRouteFromXml(TResult& aError,const TRouteProfile& aProfile,const CString& aFileNameOrData);
    TResult UseRoute(const CRoute& aRoute,bool aReplace);
    TResult ReadRouteFromXml(const CString& aFileNameOrData,bool aReplace);
    TResult WriteRouteAsXml(const CRoute& aRoute,const CString& aFileName,TFileType aFileType = ECartoTypeRouteFileType) const;
    TResult WriteRouteAsXmlString(const CRoute& aRoute,std::string& aXmlString,TFileType aFileType = ECartoTypeRouteFileType) const;
    const CRouteSegment* CurrentRouteSegment() const;
    const CRouteSegment* NextRouteSegment() const;
    size_t RouteCount() const;
    TResult ReverseRoutes();
    TResult DeleteRoutes();
    TResult Navigate(const TNavigationData& aNavData);
    const TNavigatorTurn& FirstTurn() const;
    const TNavigatorTurn& SecondTurn() const;
    const TNavigatorTurn& ContinuationTurn() const;
    TNavigationState NavigationState() const;
    bool NavigationPositionKnown() const;
    void SetNavigationMinimumFixDistance(int32 aMeters);
    void SetNavigationTimeOffRouteTolerance(int32 aSeconds);
    void SetNavigationDistanceOffRouteTolerance(int32 aMeters);
    void SetNavigationAutoReRoute(bool aAutoReRoute);
    TResult AddNearbyObjectWarning(const CString& aLayer,double aMaxDistanceToRoute,double aMaxDistanceAlongRoute);
    TResult DeleteNearbyObjectWarning(const CString& aLayer);
    TResult CopyNearbyObjects(const CString& aLayer,CMapObjectArray& aObjectArray,int32 aMaxObjectCount);
    double DistanceToDestination();
    double EstimatedTimeToDestination();
    void UseSerializedNavigationData(bool aEnable);
    TResult GetNearestRoad(TNearestRoadInfo& aInfo,double aX,double aY,TCoordType aCoordType,
                           double aMaxDistanceInMeters,double aHeadingInDegrees);
    TResult DisplayPositionOnNearestRoad(const TNavigationData& aNavData,TNearestRoadInfo* aInfo = nullptr,
                                         double aMaxDistanceInMeters = 100);
    TResult SetVehiclePosOffset(double aXOffset,double aYOffset);
    TResult SetFollowMode(TFollowMode aFollowMode);
    TFollowMode FollowMode() const;
    
    // locales
    void SetLocale(const char* aLocale);
    std::string Locale() const;

    // locale-dependent and configuration-dependent string handling
    void AppendDistance(MString& aString,double aDistanceInMeters,bool aMetricUnits);
    void AppendTime(MString& aString,double aTimeInSeconds);
    void SetCase(MString& aString,TLetterCase aCase);
    void AbbreviatePlacename(MString& aString);

    // traffic information and general location referencing
    TResult AddTrafficInfo(uint64& aId,const CTrafficInfo& aTrafficInfo,CLocationRef& aLocationRef,bool aDriveOnLeft);
    TResult AddPolygonSpeedLimit(uint64& aId,const CGeometry& aPolygon,double aSpeed,uint32 aVehicleTypes);
    TResult AddLineSpeedLimit(uint64& aId,const CGeometry& aLine,double aSpeed,uint32 aVehicleTypes,bool aDriveOnLeft);
    TResult AddClosedLineSpeedLimit(uint64& aId,const CGeometry& aLine,double aSpeed,uint32 aVehicleTypes);
    TResult AddForbiddenArea(uint64& aId,const CGeometry& aPolygon);
    TResult DeleteTrafficInfo(uint64 aId);
    void ClearTrafficInfo();
    TResult WriteTrafficMessageAsXml(MOutputStream& aOutput,const CTrafficInfo& aTrafficInfo,CLocationRef& aLocationRef);
    TResult WriteLineTrafficMessageAsXml(MOutputStream& aOutput,const CTrafficInfo& aTrafficInfo,const CString& aId,const CRoute& aRoute);
    TResult WriteClosedLineTrafficMessageAsXml(MOutputStream& aOutput,const CTrafficInfo& aTrafficInfo,const CString& aId,const CRoute& aRoute);
    bool EnableTrafficInfo(bool aEnable);

    // functions for internal use only
    TResult CompileStyleSheet(std::shared_ptr<CMapStyle>& aStyleSheet,double aScale);
    std::unique_ptr<CMapStore> NewMapStore(const CMapStyle& aStyleSheet,const TRect& aBounds,bool aUseFastAllocator);
    const CMapDataBase& MainDb() const { return iMapDataSet->MainDb(); }
    const TMapTransform& MapTransform() const;
    std::shared_ptr<CEngine> Engine() const { return iEngine->Engine(); }
    CMap& Map() const { return *iMap; }
    CGraphicsContext& Gc() const;
    std::unique_ptr<CFramework> CreateLegendFramework(TResult& aError) const;
    std::unique_ptr<CBitmap> CreateBitmapFromSvg(TResult& aError,MInputStream& aInputStream,TFileLocation& aErrorLocation,int32 aForcedWidth = 0);
    void SetUserData(std::shared_ptr<MUserData> aUserData) { iUserData = aUserData; }
    void SetLabelUpAngleInRadians(double aAngle);
    void SetLabelUpVector(TPointFP aVector);
    TPointFP LabelUpVector() const;

    private:
    CFramework();
    
    CFramework(const CFramework&) = delete;
    CFramework(CFramework&&) = delete;
    void operator=(const CFramework&) = delete;
    void operator=(CFramework&&) = delete;

    TResult Construct(const TParam& aParam);
    void HandleChangedMapData();
    void HandleChangedView() { iMapBitmapValidFlags = 0; ViewChanged(); }
    void SetViewHelper(const TRect& aBounds,int32 aMarginInPixels,int32 aMinScaleDenominator);
    TResult CreateTileServer(int32 aTileWidthInPixels,int32 aTileHeightInPixels);
    TResult SetRoutePositionAndVector(const TPoint& aPos,const TPoint& aVector);
    TResult CreateNavigator();
    std::unique_ptr<CRoute> CreateRouteHelper(TResult& aError,bool aBest,const TRouteProfile& aProfile,const TCoordSet& aCoordSet,TCoordType aCoordType,bool aStartFixed,bool aEndFixed,size_t aIterations);
    void SetCameraParam(TCameraParam& aCameraParam,double aViewWidth,double aViewHeight);

    // Notifying the framework observer.
    void ViewChanged() { for (auto p: iFrameworkObservers) p->OnViewChange(); }
    void MainDataChanged() { for (auto p : iFrameworkObservers) p->OnMainDataChange(); }
    void DynamicDataChanged() { for (auto p : iFrameworkObservers) p->OnDynamicDataChange(); }

    // virtual functions from MNavigatorObserver
    void OnTurn(const TNavigatorTurn& aFirstTurn,
                const TNavigatorTurn* aSecondTurn,
                const TNavigatorTurn* aContinuationTurn) override;
    void OnTurnRound() override;
    void OnNewRoute() override;
    void OnPositionKnown() override;
    void OnPositionUnknown() override;
    void OnAddNearbyObject(const CMapObject* aObject,double aDistanceToRoute,double aDistanceAlongRoute) override;
    void OnUpdateNearbyObject(const CMapObject* aObject,double aDistanceToRoute,double aDistanceAlongRoute) override;
    void OnRemoveNearbyObject(const CMapObject* aObject) override;
    
    void ClearStyleSheet();
    void ClearTurns();

    std::shared_ptr<CFrameworkEngine> iEngine;
    std::shared_ptr<CFrameworkMapDataSet> iMapDataSet;
    std::vector<std::unique_ptr<CStyleSheetData>> iStyleSheetData;
    std::unique_ptr<CMap> iMap;
    std::unique_ptr<CMapDrawParam> iMapDrawParam;
    std::unique_ptr<C32BitColorBitmapGraphicsContext> iGc;
    std::unique_ptr<CPerspectiveGraphicsContext> iPerspectiveGc;
    TPerspectiveParam iPerspectiveParam;
    std::set<MFrameworkObserver*> iFrameworkObservers;
    enum
        {
        EMapBitmapValid = 1,
        EMemoryMapBitmapValid = 2,
        EWebMapServiceBitmapValid = 4
        };
    uint32 iMapBitmapValidFlags = 0;
    bool iPerspective = false;
    bool iUseSerializedNavigationData = true;
    TRouterType iPreferredRouterType = TRouterType::Default;
    std::unique_ptr<CNavigator> iNavigator;
    std::set<MNavigatorObserver*> iNavigatorObservers;
    TNavigatorTurn iFirstTurn;
    TNavigatorTurn iSecondTurn;
    TNavigatorTurn iContinuationTurn;
    TNavigationState iNavigationState;
    TNavigatorParam iNavigatorParam;
    std::vector<TRouteProfile> iRouteProfile;
    bool iPositionKnown = false;
    TPointFP iVehiclePosOffset;
    std::unique_ptr<CTileServer> iTileServer;
    int32 iTileServerOverSizeZoomLevels = 1;
    std::unique_ptr<CBitmap> iTileBitmap;
    std::unique_ptr<CImageServer> iImageServer;
    std::unique_ptr<CVectorTileServer> iVectorTileServer;
    std::unique_ptr<CVectorTileHelper> iVectorTileHelper;
    std::unique_ptr<CWebMapServiceClient> iWebMapServiceClient;
    std::unique_ptr<CBitmap> iWebMapServiceBitmap;
    std::unique_ptr<CGcImageServerHelper> iGcImageServerHelper;
    std::string iLocale;
    TFollowMode iFollowMode = EFollowModeLocationHeadingZoom;
    mutable std::string iName; // the name of the dataset as an XML string containing names of maps, style sheets and fonts
    std::shared_ptr<MUserData> iUserData;
    };

/** A framework for finding map objects in a map, when the ability to draw the map is not needed. */
class CFindFramework
    {
    public:
    static std::unique_ptr<CFindFramework> New(TResult& aError,const CString& aMapFileName);
    TResult Find(CMapObjectArray& aObjectArray,const TFindParam& aFindParam);
    TResult FindRoads(CMapObjectArray& aObjectArray,int32 aMaxObjectCount,double aLong,double aLat,double aRadiusInMeters);

    private:
    CFindFramework() = default;
    TResult Construct(const CString& aMapFileName);

    std::unique_ptr<CMap> iMap;
    };

CString UKGridReferenceFromMapPoint(TPointFP aPointInMapMeters,int32 aDigits);
CString UKGridReferenceFromDegrees(TPointFP aPointInDegrees,int32 aDigits);
TRectFP MapRectFromUKGridReference(const CString& aGridReference);
TPointFP MapPointFromUKGridReference(const CString& aGridReference);
TPointFP PointInDegreesFromUKGridReference(const CString& aGridReference);
CString ExpandStreetName(const MString& aText);

}

#endif
