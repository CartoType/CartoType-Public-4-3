#ifndef MAPFORM_H
#define MAPFORM_H

#include <QWidget>
#include <QPaintEvent>
#include <QResizeEvent>

#include "mainwindow.h"

namespace Ui
    {
    class MapForm;
    }

enum class TExtendedRouteProfileType
    {
    EDrive = CartoType::ECarRouteProfile,
    ECycle = CartoType::EBicycleRouteProfile,
    EWalk = CartoType::EWalkingRouteProfile,
    EHike = CartoType::EHikingRouteProfile,
    ECustom
    };

class MapForm: public QWidget, public CartoType::MNavigatorObserver
    {
    Q_OBJECT

    public:
    explicit MapForm(QWidget* aParent,MainWindow& aMainWindow,const QString& aMapFileName);
    ~MapForm();

    const QString& FileName() const { return m_filename; }
    bool Valid() const { return m_framework.get() != nullptr; }
    void Print(bool aPreview);
    void EnableDrawLegend(bool aEnable);
    void EnableDrawScale(bool aEnable);
    bool DrawLegendEnabled() const { return m_draw_legend; }
    bool DrawScaleEnabled() const { return m_draw_scale; }
    void EnableDrawRotator(bool aEnable);
    bool DrawRotatorEnabled() const { return m_draw_rotator; }
    void SetRotation(double aAngle);
    double Rotation() const { return m_framework->Rotation(); }
    void EnableDrawRange(bool aEnable);
    bool DrawRangeEnabled() const { return m_draw_range; }
    void Find();
    void FindAddress();
    size_t FoundItemCount() const { return m_found_object.size(); }
    void FindNext();
    void ReverseRoute();
    void DeleteRoute();
    void DeletePushpins();
    bool HasRoute() const { return m_framework->Route() != nullptr; }
    bool HasPushpins() const;
    void SetRouteProfileType(TExtendedRouteProfileType aRouteProfileType);
    TExtendedRouteProfileType RouteProfileType() const { return m_route_profile_type; }
    bool EditCustomRouteProfile(CartoType::TRouteProfile& aRouteProfile);
    void ViewRouteInstructions();
    void SaveRouteInstructions();
    void SaveRouteAsXml();
    void SaveRouteAsGpx();
    void LoadRouteFromXml();
    void SaveImageAsPng();
    bool HasWritableData() const;
    bool WritableDataChanged() const { return m_writable_map_changed; }
    void SaveWritableDataAsCtms();
    void LoadWritableDataFromCtms();
    void LoadWritableDataFromGpx();
    void SetScale();
    void GoToLocation();
    void GoToOrdnanceSurveyGridRef();
    void ChooseStyleSheet();
    void ReloadStyleSheet();
    bool EditCustomStyleSheet();
    void ChooseLayers();
    void SetMetricUnits(bool aEnable);
    bool MetricUnits() const { return m_metric_units; }
    void SetPerspective(bool aEnable);
    bool Perspective() const;
    bool MapIncludesGreatBritain() const;
    void SetUseCustomStyleSheet(bool aEnable);
    bool UsingCustomStyleSheet() const { return m_use_custom_style_sheet; }
    void OnCustomStyleSheetLoaded();

    private slots:
    void on_dial_valueChanged(int aValue);
    void ApplyStyleSheet(std::string aStyleSheet);

    private:
    // from QWidget
    void resizeEvent(QResizeEvent* aEvent) override;
    void paintEvent(QPaintEvent* aEvent) override;
    void mousePressEvent(QMouseEvent* aEvent) override;
    void mouseReleaseEvent(QMouseEvent* aEvent) override;
    void mouseMoveEvent(QMouseEvent* aEvent) override;
    void wheelEvent(QWheelEvent* aEvent) override;
    void closeEvent(QCloseEvent* aEvent) override;

    // from MNavigatorObserver
    void OnTurn(const CartoType::TNavigatorTurn& aFirstTurn,const CartoType::TNavigatorTurn* aSecondTurn,const CartoType::TNavigatorTurn* aContinuationTurn) override;
    void OnTurnRound() override;
    void OnNewRoute() override;
    void OnPositionKnown() override;
    void OnPositionUnknown() override;

    void StopDragging();
    void PanToDraggedPosition();
    void DrawRange();
    void DrawDrivingInstructions(CartoType::CGraphicsContext& aGc, const CartoType::TRect& aMapClientArea);
    const CartoType::TBitmap* MapBitmap(CartoType::TResult& aError,const CartoType::TRect& aMapClientArea,bool& aRedrawNeeded);
    void LeftButtonDown(int32_t aX,int32_t aY);
    void LeftButtonUp(int32_t aX,int32_t aY);
    void RightButtonDown(int32_t aX,int32_t aY);
    void RightButtonUp(int32_t aX,int32_t aY);
    void CalculateAndDisplayRoute();
    void ShowNextFoundObject();
    void GoToLocation(double aLong,double aLat);

    Ui::MapForm* m_ui;
    MainWindow& m_main_window;
    QString m_filename;
    std::shared_ptr<CartoType::CFrameworkMapDataSet> m_map_data_set;
    std::unique_ptr<CartoType::CFramework> m_framework;
    std::unique_ptr<CartoType::CGraphicsContext> m_extra_gc;
    std::unique_ptr<CartoType::CLegend> m_legend;
    uint32_t m_writable_map_handle = 0;
    bool m_writable_map_changed = false;
    class TRoutePoint
        {
        public:
        TRoutePoint(): m_id(0) { }
        CartoType::TPointFP m_point;
        uint64_t m_id;
        };
    std::vector<TRoutePoint> m_route_point_array;
    CartoType::CString m_first_turn_instruction;
    CartoType::CString m_remaining_distance_text;
    TExtendedRouteProfileType m_route_profile_type = TExtendedRouteProfileType::EDrive;
    bool m_metric_units = true;
    std::unique_ptr<QImage> m_map_image;
    bool m_using_image_server = false;
    CartoType::CMapObjectArray m_found_object;
    CartoType::CString m_find_text;
    bool m_find_text_prefix = false;
    bool m_find_text_fuzzy = false;
    CartoType::CAddress m_address;
    int m_found_object_index = 0;
    uint64_t m_found_object_id = 0;                 // map object ID of found object
    bool m_draw_legend = false;                     // if true draw the legend
    bool m_draw_scale = false;                      // if true draw the scale
    bool m_draw_rotator = true;                     // if true draw the rotator dial
    bool m_draw_range = false;                      // if true draw the range from the last point right-clicked
    uint64_t m_range_id0 = 0;
    uint64_t m_range_id1 = 0;
    bool m_draw_driving_instructions = false;       // if true, draw driving instructions
    bool m_map_drag_enabled = false;
    CartoType::TPoint m_map_drag_anchor;
    CartoType::TPoint m_map_drag_offset;
    bool m_left_click_simulates_navigation_fix = false;
    double m_on_route_time = 0;                     // simulated time in seconds on the route
    bool m_best_route = false;                      // calculate the best route through the waypoints; don't just take them in order
    QString m_pushpin_name;
    QString m_pushpin_desc;
    bool m_use_custom_style_sheet = false;
    CartoType::CStyleSheetData m_saved_style_sheet; // a copy of the ordinary style sheet kept while the custom style sheet is in use
    bool m_style_sheet_applied = false;
    };

#endif // MAPFORM_H
