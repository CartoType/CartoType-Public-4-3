#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMdiSubWindow>
#include <cartotype_framework.h>

namespace Ui
    {
    class MainWindow;
    }

class MapForm;

class MainWindow: public QMainWindow
    {
    Q_OBJECT

    public:
    explicit MainWindow(QWidget* aParent = nullptr);
    ~MainWindow();

    void ShowError(const char* aMessage,int aErrorCode);
    void UpdateSaveAddedData();
    void UpdateFindNext();
    void UpdateNorthUp();
    void UpdateDeleteOrSaveRoute();
    void UpdateDeletePushpins();
    void UpdateRouteProfile();
    void UpdateGoToGridRef();
    void UpdateStyleSheet();
    std::shared_ptr<CartoType::CFrameworkEngine> Engine() const
        {
        return m_engine;
        }
    const CartoType::MString& DefaultStyleSheetPath() const
        {
        return m_default_style_sheet_path;
        }
    void OnMapFormDestroyed(MapForm* aMapForm);
    const CartoType::TRouteProfile& CustomRouteProfile() const { return m_custom_route_profile; }
    const std::string& CustomStyleSheet() const { return m_custom_style_sheet; }
    void SetCustomStyleSheet(const std::string& aStyleSheet);

    private slots:
    void on_actionOpen_triggered();
    void on_actionAbout_CartoType_Maps_triggered();
    void on_mdiArea_subWindowActivated(QMdiSubWindow* aSubWindow);
    void on_actionScale_changed();
    void on_actionLegend_changed();
    void on_actionRotator_changed();
    void on_actionRange_changed();
    void on_actionFind_triggered();
    void on_actionFind_Next_triggered();
    void on_actionNorth_Up_triggered();
    void on_actionDelete_Route_triggered();
    void on_actionDrive_triggered();
    void on_actionCycle_triggered();
    void on_actionWalk_triggered();
    void on_actionHike_triggered();
    void on_actionCustom_Profile_triggered();
    void on_actionSave_Route_Instructions_triggered();
    void on_actionSave_Route_triggered();
    void on_actionSave_Route_as_GPX_triggered();
    void on_actionLoad_Route_triggered();
    void on_actionSave_Image_as_PNG_triggered();
    void on_actionFind_Address_triggered();
    void on_actionSave_Added_Data_as_CTMS_triggered();
    void on_actionImport_Data_from_CTMS_triggered();
    void on_actionImport_Data_from_GPX_file_triggered();
    void on_actionSet_Scale_triggered();
    void on_actionGo_to_Location_triggered();
    void on_actionGo_to_Ordnance_Survey_grid_reference_triggered();
    void on_actionChoose_Style_Sheet_triggered();
    void on_actionReload_Style_Sheet_triggered();
    void on_actionEdit_Custom_Style_Sheet_triggered();
    void on_actionLayers_triggered();
    void on_actionPerspective_View_changed();
    void on_actionMetric_Units_changed();
    void on_actionView_Route_Instructions_triggered();
    void on_actionEdit_Custom_Route_Profile_triggered();
    void on_actionSave_Custom_Route_Profile_triggered();
    void on_actionLoad_Custom_Route_Profile_triggered();
    void on_actionUse_Custom_Style_Sheet_changed();
    void on_actionLoad_Custom_Style_Sheet_triggered();
    void on_actionSave_Custom_Style_Sheet_triggered();
    void on_actionDelete_Pushpins_triggered();
    void on_actionReverse_Route_triggered();
    void on_actionPrint_triggered();
    void on_actionPrint_Preview_triggered();

    /*
    This is a manually connected slot. I don't use the "on_" naming convention because
    it causes a message like "QMetaObject::connectSlotsByName: No matching signal for on_actionOpenRecentFile_triggered()";
    that is because Qt Creator looks for slot functions starting with "on_" and
    tries to connect them up.
    */
    void OpenRecentFileTriggered();

    private:
    static const int KMaxRecentFiles = 8;

    // from QWidget
    void closeEvent(QCloseEvent* aEvent) override;

    void EnableMenuItems();
    void GetFontPath(CartoType::CString& aPath,const char* aFontName);
    CartoType::TResult LoadFont(const char* aFontName);
    CartoType::TResult InitCartoType();
    void LoadMap(const QString& aPath);
    void UpdateRecentFiles(bool aRemoveNonExistentFiles = false);
    void UpdateCustomRouteProfile();

    Ui::MainWindow* m_ui;   // note: this can't be kept in a std::unique_ptr; if you do that Qt Creator can't create slots and actions
    QAction* m_print_separator;  // the menu separator before the Print action
    QAction* m_recent_file_separator;  // the menu separator before the list of recent files
    QAction* m_recent_file_action[KMaxRecentFiles];  // menu items for recent files
    MapForm* m_map_form;    // the current map form if any; not owned
    QString m_app_path;
    QString m_cartotype_source_path;
    std::shared_ptr<CartoType::CFrameworkEngine> m_engine;
    CartoType::CString m_default_style_sheet_path;
    CartoType::TRouteProfile m_custom_route_profile;
    std::string m_custom_style_sheet;
    };

#endif // MAINWINDOW_H
