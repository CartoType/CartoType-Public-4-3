/*
CARTOTYPE_VECTOR_TILE.H
Copyright (C) 2015-2016 CartoType Ltd.
See www.cartotype.com for more information.
*/

#ifndef CARTOTYPE_VECTOR_TILE_H__
#define	CARTOTYPE_VECTOR_TILE_H__

#include <cartotype_framework.h>
#include <queue>
#include <set>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <memory>

namespace CartoType
{

class CMapLevelStore;
class CVectorTileServer;
class CVectorTileServerTask;
class CStackAllocator;
class CTransformingGc;

template<typename T> class TTaskOutputQueue
    {
    public:
    TTaskOutputQueue() = default;

    void Add(T aObject)
        {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (const auto& p : m_queue)
            {
            if (p == aObject)
                return;
            }
        m_queue.push_back(aObject);
        m_condition.notify_one();
        }

    T Remove()
        {
        std::unique_lock<std::mutex> lock(m_mutex);
        while (m_queue.empty())
            m_condition.wait(lock);
        T object = m_queue.front();
        m_queue.pop_front();
        return object;
        }

    T RemoveWithoutWaiting()
        {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_queue.empty())
            return T();
        T object = m_queue.front();
        m_queue.pop_front();
        return object;
        }

    bool Empty() const
        {
        return m_queue.empty();
        }

    TTaskOutputQueue(const TTaskOutputQueue&) = delete;
    TTaskOutputQueue& operator=(const TTaskOutputQueue&) = delete;

    protected:
    std::deque<T> m_queue;
    mutable std::mutex m_mutex;
    std::condition_variable m_condition;
    };

template<typename T> class TTaskQueue
    {
    public:
    TTaskQueue() = default;

    void Add(T aRequest)
        {
        std::lock_guard<std::mutex> lock(m_mutex);

        for (const auto& p : m_queue)
            {
            if (p == aRequest)
                return;
            }

        for (const auto& p : m_pending)
            {
            if (p == aRequest)
                return;
            }

        m_queue.push_back(aRequest);
        m_condition.notify_one();
        }

    T StartTask()
        {
        std::unique_lock<std::mutex> lock(m_mutex);

        // Loop until a task is found that's not already being handled.
        for (;;)
            {
            while (m_queue.empty())
                m_condition.wait(lock);
            T object = m_queue.back(); // get the most recently added item; this is a LIFO queue
            m_queue.pop_back();
            if (m_pending.insert(object).second) // the second element of the return value is true if the object was inserted, and not already there
                return object;
            }
        }

    void EndTask(T aRequest)
        {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_pending.erase(aRequest);
        }

    bool Empty() const
        {
        return m_queue.empty();
        }

    TTaskQueue(const TTaskQueue&) = delete;
    TTaskQueue& operator=(const TTaskQueue&) = delete;

    protected:
    std::deque<T> m_queue; // tasks not yet started
    std::set<T> m_pending; // tasks currently being handled
    mutable std::mutex m_mutex;
    std::condition_variable m_condition;
    };

class TTileSpec
    {
    public:
    bool operator==(const TTileSpec& aOther) const
        {
        return m_zoom == aOther.m_zoom &&
               m_x == aOther.m_x &&
               m_y == aOther.m_y &&
               m_type == aOther.m_type;
        }
    bool operator<(const TTileSpec& aOther) const
        {
        if (m_zoom < aOther.m_zoom)
            return true;
        if (m_zoom == aOther.m_zoom)
            {
            if (m_x < aOther.m_x)
                return true;
            if (m_x == aOther.m_x)
                {
                if (m_y < aOther.m_y)
                    return true;
                if (m_y == aOther.m_y)
                    return int(m_type) < int(aOther.m_type);
                }
            }
        return false;
        }

    int32 m_zoom = 0;
    int32 m_x = 0;
    int32 m_y = 0;

    enum TType
        {
        AllData,
        StaticDataOnly,
        DynamicDataOnly
        };

    TType m_type = AllData;
    };

class TTileRequest: public TTileSpec
    {
    public:
    TTileRequest() { }
    TTileRequest(const TTileSpec& aTileSpec,uint32 aGeneration):
        m_generation(aGeneration)
        {
        m_zoom = aTileSpec.m_zoom;
        m_x = aTileSpec.m_x;
        m_y = aTileSpec.m_y;
        m_type = aTileSpec.m_type;
        }

    bool operator==(const TTileRequest& aOther) const
        {
        return TTileSpec::operator==(aOther) && m_generation == aOther.m_generation;
        }
    bool operator<(const TTileRequest& aOther) const
        {
        if (TTileSpec::operator<(aOther))
            return true;
        if (TTileSpec::operator==(aOther) && m_generation < aOther.m_generation)
            return true;
        return false;
        }

    uint32 m_generation = 0;
    };

class TVectorObjectStyle
    {
    public:
    TVectorObjectStyle(): m_line_width(0), m_border_width(0) { }

    /** The fill color for lines and polygons. */
    TColor m_color;
    /** If non-null, the bitmap used as a texture to fill polygons; overrides m_color. */
    std::shared_ptr<CBitmap> m_texture;
    /** The line width; not used for polygons. */
    double m_line_width;
    /** The color of the border. */
    TColor m_border_color;
    /** The border width; for lines this is the width of an outer line to be drawn before the main line. */
    double m_border_width;
    /** If not empty, an array of dashes and gaps to be used for lines, and for polygon borders. */
    std::vector<float> m_dash_array;
    };

class TVectorObjectGroup
    {
    public:
    TVectorObjectGroup():
        m_layer_group(0),
        m_priority(0),
        m_type(ENoObjectType),
        m_object_array(nullptr),
        m_object_array_start(0),
        m_object_array_count(0)
        {
        }

    int32 m_layer_group;
    uint32 m_priority;
    TVectorObjectStyle m_style;
    TMapObjectType m_type;
    const CMapObjectArray* m_object_array;
    size_t m_object_array_start; // first object in m_object_array belonging to the group
    size_t m_object_array_count; // number of objects in the group
    };

/**
Data used when drawing the vector tile.
This data is owned by the graphics acceleration system
and will normally consist of vertex and index buffers.
*/
class CVectorTileDrawData
    {
    public:
    virtual ~CVectorTileDrawData() { }
    };

/**
A vector tile map store owns the map objects for
a tile, and their styles. Temporary objects of this class are created
as part of the process of creating a vector tile.
CVectorTileDrawData objects can keep the shared pointers returned by MapStore() and MapStyle() but must not
keep pointers or references to anything in ObjectGroupArray().
*/
class CVectorTileMapStore
    {
    public:
    CVectorTileMapStore(CFramework& aFramework,CVectorTileServer& aVectorTileServer,const TTileSpec& aTileSpec,std::shared_ptr<CMapStyle> aStyle,
                        bool aForGraphicsAcceleration,bool aProjectTiles);
    const std::shared_ptr<CMapStore> MapStore() const { return m_map_store; }
    const std::shared_ptr<CMapStyle> MapStyle() const { return m_style; }
    const std::vector<TVectorObjectGroup>& ObjectGroupArray() const { return m_object_group_array; }
    const TTileSpec& TileSpec() const { return m_tile_spec; }

    private:
    void GetGroups();
    void GetGroupsForLayers(size_t aStartLayer,size_t aEndLayer,int aPhase);
    void GetGroupsForLayerGroup(const CMapLevelStore& aStore,size_t aStart,size_t aEnd,int aPhase);

    std::shared_ptr<CMapStore> m_map_store; // the map store, projected to the rectangle (0,0,32768,32768)
    std::shared_ptr<CMapStyle> m_style;
    std::vector<TVectorObjectGroup> m_object_group_array;
    TTileSpec m_tile_spec;
    };

class CVectorTile
    {
    public:
    CVectorTile(const TTileRequest& aTileRequest,std::unique_ptr<CVectorTileDrawData> aDrawData);
    const TTileRequest TileRequest() const { return m_tile_request; }
    const CVectorTileDrawData& DrawData() const { return *m_draw_data; }

    private:
    TTileRequest m_tile_request;
    std::unique_ptr<CVectorTileDrawData> m_draw_data;
    };

/**
Create a class derived from CVectorTileHelper to use the vector tile system,
implementing the pure virtual functions.
*/
class CVectorTileHelper
    {
    public:
    virtual ~CVectorTileHelper() { }

    /**
    This function is called by one of the vector tile creation worker threads to
    create all the data needed to draw the tile using the Draw function.
    It is called once for each tile, when the tile is created. It must use the data obtained
    via CVectorTileMapStore::ObjectGroupArray, copying it rather than making references to it, because the
    object group data is destroyed after the call to CreateDrawData.

    The coordinates in the map objects are integers representing 64ths of pixels, referring to a notional 512 x 512 tile,
    and are thus in the range 0...32768.

    The dimensions in the styles are floating-point numbers representing full pixels.
    */
    virtual std::unique_ptr<CVectorTileDrawData> CreateDrawData(const CVectorTileMapStore& aVectorTileMapStore) = 0;

    /**
    This function is called by the drawing thread to draw a tile using the data in aDrawData.
    The transform aTransform converts the coordinates in the map objects to display pixels.
    */
    virtual void Draw(const CVectorTileDrawData& aDrawData,const TTransformFP& aTransform) = 0;

    /**
    This function is called by the drawing thread when one of the cached tiles is just about to
    be unloaded to reduce the cache to its maximum size.
    */
    virtual void OnTileUnloaded(const CVectorTile& /*aVectorTile*/) { }

    /**
    This data member tells the CVectorTileServer what type of information to put in the CVectorTileMapStore objects.
    If it is true, CVectorTileMapStore objects contain object groups suitable for use by graphics-accelerated drawing.
    If not, the object groups are not created; they are not necessary when using software drawing.
    */
    bool m_for_graphics_acceleration = true;

    /**
    This data member tells the CVectorTileServer whether to project vector tiles to
    the notional image size in 64ths of pixels. This is done
    by the Windows demo (CartoTypeGLDemo), but not by the CartoType GL system itself.
    */
    bool m_project_tiles = false;
    };

/**
A class to draw vector tiles using multiple threads.
To use it, create a CVectorTileServer, supplying a CVectorTileHelper object
with functions to convert vector tiles to any suitable format such as data
stored in graphics memory, and to draw the tile using that data.
*/
class CVectorTileServer
    {
    public:
    CVectorTileServer(CFramework& aFramework,CVectorTileHelper& aHelper,size_t aThreadCount,size_t aMaxZoomLevel = 20);
    ~CVectorTileServer();
    
    void Draw(TTileSpec::TType = TTileSpec::TType::AllData);
    std::vector<TTileSpec> GetTileList(TTileSpec::TType = TTileSpec::TType::AllData);
    void DeleteTilesOutsideView();
    void Invalidate(TTileSpec::TType aType);
    double ZoomLevelFromScaleDenominator(double aScaleDenominator) const;
    double ScaleDenominatorFromZoomLevel(double aZoomLevel) const;
    TTileSpec TileFromMapPoint(TPoint aMapPoint,size_t aZoomLevel) const;
    TRectFP TileBounds(const TTileSpec& aTileSpec) const;
    TTileRequest StartTask() { return m_task_queue.StartTask(); }
    void EndTask(const TTileRequest& aTask) { m_task_queue.EndTask(aTask); }
    void AddRequest(const TTileRequest& aRequest) { m_task_queue.Add(aRequest); }
    void AddTile(std::shared_ptr<CVectorTile> aTile) { m_tile_queue.Add(aTile); }
    std::shared_ptr<CMapStyle> GetStyleSheet(CFramework& aFramework,size_t aZoomLevel);
    std::unique_ptr<CVectorTileDrawData> CreateDrawData(const CVectorTileMapStore& aVectorTileMapStore);
    std::shared_ptr<CVectorTile> GetTile(const TTileSpec& aTileSpec,bool aTriggerTileCreation = true);
    bool ForGraphicsAcceleration() const { return m_helper.m_for_graphics_acceleration; }
    bool ProjectTiles() const { return m_helper.m_project_tiles; }

    static const int32 KImageSizeInPixels = 512;

    private:
    CVectorTileServer(const CVectorTileServer&) = delete;
    CVectorTileServer& operator=(const CVectorTileServer&) = delete;
    CVectorTileServer&& operator=(const CVectorTileServer&&) = delete;    
    
    void DrawOrGetTileList(std::vector<TTileSpec>* aTileList,TTileSpec::TType aType);

    CFramework& m_framework;
    CVectorTileHelper& m_helper;
    size_t m_max_zoom_level;
    TTaskQueue<TTileRequest> m_task_queue;
    TTaskOutputQueue<std::shared_ptr<CVectorTile>> m_tile_queue;
    static const size_t KMaxCacheItems = 64;
    std::vector<std::shared_ptr<CVectorTile>> m_tile_cache;
    std::vector<std::unique_ptr<CVectorTileServerTask>> m_task_array;
    std::vector<std::thread> m_thread_array;
    std::vector<std::shared_ptr<CMapStyle>> m_style_array;
    std::mutex m_style_array_mutex;
    TRectFP m_map_extent; // the map extent in map coordinates
    TRectFP m_level_0_tile_extent;
    double m_level_0_tile_width_in_metres;
    double m_pixel_size_in_metres;
    uint32 m_static_data_generation = 0;
    uint32 m_dynamic_data_generation = 0;
    uint32 m_combined_data_generation = 0;
    };

/**
A CVectorTileHelper implementation to draw using the ordinary method,
rather than using hardware acceleration.
*/
class CSoftwareVectorTileHelper: public CVectorTileHelper
    {
    public:
    CSoftwareVectorTileHelper(CFramework& aFramework);
    
    std::unique_ptr<CVectorTileDrawData> CreateDrawData(const CVectorTileMapStore& aVectorTileMapStore) override;
    void Draw(const CVectorTileDrawData& aDrawData,const TTransformFP& aTransform) override;

    private:
    CSoftwareVectorTileHelper(const CSoftwareVectorTileHelper&) = delete;
    CSoftwareVectorTileHelper& operator=(const CSoftwareVectorTileHelper&) = delete;
    CSoftwareVectorTileHelper&& operator=(CSoftwareVectorTileHelper&&) = delete;
    
    CFramework& m_framework;
    std::unique_ptr<CTransformingGc> m_gc;
    };

}

#endif
