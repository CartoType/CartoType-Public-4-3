#include "mapform.h"
#include "ui_mapform.h"
#include "routedialog.h"
#include "ui_routedialog.h"
#include "finddialog.h"
#include "ui_finddialog.h"
#include "findaddressdialog.h"
#include "ui_findaddressdialog.h"
#include "scaledialog.h"
#include "ui_scaledialog.h"
#include "locationdialog.h"
#include "ui_locationdialog.h"
#include "layerdialog.h"
#include "ui_layerdialog.h"
#include "routeinstructionsdialog.h"
#include "ui_routeinstructionsdialog.h"
#include "routeprofiledialog.h"
#include "ui_routeprofiledialog.h"
#include "styledialog.h"
#include "ui_styledialog.h"
#include "util.h"

#include <stdio.h>

#include <QPainter>
#include <QFileDialog>
#include <QMessageBox>
#include <QStatusBar>
#include <QPrinter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>

static inline void SetString(QString& aDest,const CartoType::MString& aSource)
    {
    aDest.setUtf16(aSource.Text(),(int)aSource.Length());
    }

static inline void SetString(QLineEdit& aDest,const CartoType::MString& aSource)
    {
    QString qs;
    SetString(qs,aSource);
    aDest.setText(qs);
    }

static inline void SetString(CartoType::MString& aDest,const QLineEdit& aSource)
    {
    QString qs = aSource.text();
    aDest.Set(qs.utf16(),qs.length());
    }

MapForm::MapForm(QWidget* aParent,MainWindow& aMainWindow,const QString& aMapFileName):
    QWidget(aParent),
    m_ui(new Ui::MapForm),
    m_main_window(aMainWindow),
    m_filename(aMapFileName)
    {
    m_ui->setupUi(this);

    m_route_point_array.resize(2);
    m_route_point_array[0] = TRoutePoint();
    m_route_point_array[1] = TRoutePoint();

    // Create the CartoType data set object.
    CartoType::TText filename((const uint16_t*)aMapFileName.constData(),aMapFileName.length());
    CartoType::TResult error = 0;
    m_map_data_set = CartoType::CFrameworkMapDataSet::New(error,*m_main_window.Engine(),filename);
    if (error)
        {
        std::string s("cannot open map ");
        s += std::string(filename);
        m_main_window.ShowError(s.c_str(),error);
        return;
        }

    // Create the CartoType framework object.
    CartoType::CFramework::TParam param;
    param.iSharedEngine = m_main_window.Engine();
    param.iSharedMapDataSet = m_map_data_set;
    param.iStyleSheetFileName = m_main_window.DefaultStyleSheetPath();

    QRect rect(geometry());
    param.iViewWidth = rect.width();
    param.iViewHeight = rect.height();
    param.iTextIndexLevels = 1;

    m_framework = CartoType::CFramework::New(error,param);
    if (!error)
        {
        error = m_framework->CreateWritableMap(CartoType::EMemoryWritableMapType);
        m_writable_map_handle = m_framework->GetLastMapHandle();
        }
    if (error)
        {
        m_main_window.ShowError("error creating CartoType framework",error);
        return;
        }

    // Add a style for UK historic counties, but disable it so that it can be enabled if needed.
    static const char* historic_counties_style =
        "<CartoTypeStyleSheet><layer name='county/historic'>"
        "<shape border='grey+red' opacity='0.5' width='0.6pt'/>"
        "<scale max='1000000'><label case='upper' color='grey+red' font-size='8pt' font-weight='bold' letter-spacing='0.2em' opacity='0.5' position='horizontal' priority='-4' wrapWidth='20em'/></scale>"
        "</layer></CartoTypeStyleSheet>";
    m_framework->AppendStyleSheet((const uint8_t*)historic_counties_style,strlen(historic_counties_style));
    m_framework->EnableLayer("county/historic",false);

    // Tell the framework to send us navigation messages.
    m_framework->AddNavigatorObserver(this);

    m_framework->SetFollowMode(CartoType::EFollowModeNone);
    m_framework->SetLocale(m_metric_units ? "en_XX" : "en");

    // Create the initial map image.
    m_map_image.reset(new QImage(rect.width(),rect.height(),QImage::Format_ARGB32_Premultiplied));
    }

MapForm::~MapForm()
    {
    delete m_ui;
    }

void MapForm::OnTurn(const CartoType::TNavigatorTurn& /*aFirstTurn*/,const CartoType::TNavigatorTurn* /*aSecondTurn*/,const CartoType::TNavigatorTurn* /*aContinuationTurn*/)
    {
    }

void MapForm::OnTurnRound()
    {
    }

void MapForm::OnNewRoute()
    {
    }

void MapForm::OnPositionKnown()
    {
    }

void MapForm::OnPositionUnknown()
    {
    }

void MapForm::resizeEvent(QResizeEvent* aEvent)
    {
    if (m_framework &&
        aEvent->size().width() > 0 &&
        aEvent->size().height() > 0)
        {
        m_framework->Resize(aEvent->size().width(),aEvent->size().height());
        m_map_image.reset(new QImage(aEvent->size(),QImage::Format_ARGB32_Premultiplied));
        }
    }

void MapForm::paintEvent(QPaintEvent* /*aEvent*/)
    {
    CartoType::TResult error = 0;
    QRect rect(geometry());
    CartoType::TRect client_area(0,0,rect.width(),rect.height());
    bool redraw_needed = false;
    const CartoType::TBitmap* map_bitmap = MapBitmap(error,client_area,redraw_needed);
    if (map_bitmap && !error)
        {
        CopyBitmapToImage(*map_bitmap,*m_map_image);
        QPainter painter(this);
        painter.drawImage(0,0,*m_map_image);
        }
    }

void MapForm::mousePressEvent(QMouseEvent* aEvent)
    {
    if (aEvent->button() == Qt::MouseButton::LeftButton)
        LeftButtonDown(aEvent->x(),aEvent->y());
    else if (aEvent->button() == Qt::MouseButton::RightButton)
        RightButtonDown(aEvent->x(),aEvent->y());
    }

void MapForm::mouseReleaseEvent(QMouseEvent* aEvent)
    {
    if (aEvent->button() == Qt::MouseButton::LeftButton)
        LeftButtonUp(aEvent->x(),aEvent->y());
    else if (aEvent->button() == Qt::MouseButton::RightButton)
        RightButtonUp(aEvent->x(),aEvent->y());
    }

void MapForm::mouseMoveEvent(QMouseEvent* aEvent)
    {
    if (m_map_drag_enabled)
        {
        m_map_drag_offset.iX = aEvent->x() - m_map_drag_anchor.iX;
        m_map_drag_offset.iY = aEvent->y() - m_map_drag_anchor.iY;

        if (m_using_image_server)
            {
            m_framework->Pan(-m_map_drag_offset.iX,-m_map_drag_offset.iY);
            m_map_drag_offset = CartoType::TPoint(0,0);
            m_map_drag_anchor.iX = aEvent->x();
            m_map_drag_anchor.iY = aEvent->y();
            }

        update();
        }
    }

void MapForm::wheelEvent(QWheelEvent* aEvent)
    {
    if (aEvent->delta() == 0)
        return;

    // If the mouse pointer is in the window, pan the map so that the point under the mouse pointer stays where it is.
    CartoType::TRect r(0,0,width(),height());
    QPoint point = aEvent->pos();
    bool set_centre = false;
    CartoType::TPoint p(point.x(),point.y());
    if (r.Contains(p))
        {
        m_framework->ConvertPoint(p,CartoType::EScreenCoordType,CartoType::EMapCoordType);
        set_centre = true;
        }

    /*
    If using the image server, always zoom by a factor of 2 because
    the image server system snaps to zoom levels arranged in factors
    of 2, and zooming less will often have no effect because it
    will snap back to the previous level.
    */
    double zoom = m_using_image_server ? 2 : sqrt(2);

    int zoom_count = aEvent->delta() / QWheelEvent::DefaultDeltasPerStep;
    if (zoom_count == 0)
        zoom_count = aEvent->delta() >= 0 ? 1 : -1;

    zoom = pow(zoom,zoom_count);
    m_framework->Zoom(zoom);

    if (set_centre)
        m_framework->Pan(p.iX,p.iY,CartoType::EMapCoordType,point.x(),point.y(),CartoType::EScreenCoordType);

    update();
    }

void MapForm::closeEvent(QCloseEvent* aEvent)
    {
    if (m_writable_map_changed)
        {
        QMessageBox message_box(QMessageBox::Question,"Quit without saving","You have unsaved added data. Close without saving it?",
                                QMessageBox::Yes | QMessageBox::Cancel);
        QMessageBox::StandardButton button = (QMessageBox::StandardButton)message_box.exec();
        if (button == QMessageBox::Yes)
            {
            m_main_window.OnMapFormDestroyed(this);
            aEvent->accept();
            }
        else
            aEvent->ignore();
        }
    else
        {
        m_main_window.OnMapFormDestroyed(this);
        aEvent->accept();
        }
    }

void MapForm::StopDragging()
    {
    setMouseTracking(false);
    m_map_drag_enabled = false;
    PanToDraggedPosition();
    m_map_drag_offset = CartoType::TPoint(0,0);
    update();
    }

void MapForm::PanToDraggedPosition()
    {
    CartoType::TPoint to(m_map_drag_anchor);
    to += m_map_drag_offset;
    m_framework->Pan(m_map_drag_anchor,to);
    update();
    }

static void DrawBorder(CartoType::CGraphicsContext& aGc,const CartoType::TRect& aBounds,int aShrinkage,int aStrokeWidth,int aRadius,CartoType::TColor aBorderColor,CartoType::TColor aClearColor)
    {
    CartoType::TRect bounds(aBounds);
    bounds.iTopLeft.iX += aShrinkage;
    bounds.iTopLeft.iY += aShrinkage;
    bounds.iBottomRight.iX -= aShrinkage;
    bounds.iBottomRight.iY -= aShrinkage;

    CartoType::CContour contour;
    double half_stroke_width = aStrokeWidth * 32.0;
    double x1 = bounds.Left() * 64.0 + half_stroke_width;
    double x2 = bounds.Right() * 64.0 - half_stroke_width;
    double y1 = (bounds.Top() * 64.0 + bounds.Bottom() * 64.0) / 2.0;
    double y2 = y1;
    double width = (bounds.Height() - aStrokeWidth) * 64.0;
    contour.MakeRoundedRectangle(x1,y1,x2,y2,width,aRadius * 64);

    CartoType::TCircularPen pen;
    pen.iPenWidth = aStrokeWidth;
    aGc.SetPen(pen);
    aGc.SetColor(aClearColor);
    aGc.DrawShapeAndStroke(contour,aBorderColor);
    }

void MapForm::DrawDrivingInstructions(CartoType::CGraphicsContext& aGc,const CartoType::TRect& aMapClientArea)
    {
    double pt = m_framework->ResolutionDpi() / 72.0;
    static const CartoType::TColor natural_blue(0,135,189);

    // Create the fonts.
    CartoType::TFontSpec fs;
    int instruction_font_size = CartoType::Arithmetic::Round(9.0 * pt);
    int distance_and_time_font_size = CartoType::Arithmetic::Round(6.0 * pt);
    fs.SetToSize(instruction_font_size);
    CartoType::TFont instruction_font = m_framework->Font(fs);
    fs.SetToSize(distance_and_time_font_size);
    CartoType::TFont distance_and_time_font = m_framework->Font(fs);

    // Measure the text.
    CartoType::TTextParam text_param;
    text_param.iBaseline = CartoType::ETextBeforeEdgeBaseline;
    CartoType::TTextMetrics text_metrics;
    CartoType::TPoint p;
    instruction_font.DrawText(NULL,m_first_turn_instruction,p,text_param,text_metrics);
    CartoType::TFixed text_width = text_metrics.iAdvance.iX;
    distance_and_time_font.DrawText(NULL,m_remaining_distance_text,p,text_param,text_metrics);
    if (text_width < text_metrics.iAdvance.iX)
        text_width = text_metrics.iAdvance.iX;

    // Draw the cartouche.
    int border_width = CartoType::Arithmetic::Round(1.5 * pt);
    int line_gap_size = CartoType::Arithmetic::Round(1.5 * pt);
    int height = instruction_font_size + distance_and_time_font_size + line_gap_size * 3 + border_width * 2;
    int width = instruction_font_size * 24;
    if (width < text_width.Ceiling())
        width = text_width.Ceiling();
    width += line_gap_size * 2 + border_width * 2;

    CartoType::TRect bounds;
    int margin = CartoType::Arithmetic::Round(5 * pt);
    bounds.iTopLeft.iX = aMapClientArea.Left() + margin;
    bounds.iTopLeft.iY = aMapClientArea.Top() + margin;
    bounds.iBottomRight.iX = bounds.iTopLeft.iX + width;
    bounds.iBottomRight.iY = bounds.iTopLeft.iY + height;

    CartoType::TColor background(CartoType::KWhite);
    background.SetAlpha(224);
    int radius = CartoType::Arithmetic::Round(3.0 * pt);
    DrawBorder(aGc,bounds,0,border_width,radius,natural_blue,background);
    aGc.SetColor(natural_blue);

    // Draw the text.
    CartoType::TRect inner_bounds(bounds);
    inner_bounds.iTopLeft.iX += border_width;
    inner_bounds.iTopLeft.iY += border_width;
    inner_bounds.iBottomRight.iX -= border_width;
    inner_bounds.iBottomRight.iY -= border_width;

    aGc.SetColor(natural_blue);
    p = inner_bounds.iTopLeft;
    p.iX += line_gap_size;
    p.iY += line_gap_size;

    instruction_font.DrawText(&aGc,m_first_turn_instruction,p,text_param,text_metrics);
    p.iY += instruction_font_size + line_gap_size;
    distance_and_time_font.DrawText(&aGc,m_remaining_distance_text,p,text_param,text_metrics);
    }

const CartoType::TBitmap* MapForm::MapBitmap(CartoType::TResult& aError,const CartoType::TRect& aMapClientArea,bool& aRedrawNeeded)
    {
    if (!m_framework)
        return nullptr;

    // Get the map bitmap.
    const CartoType::TBitmap* bitmap = m_using_image_server ?
                                       m_framework->MemoryDataBaseMapBitmap(aError,&aRedrawNeeded) :
                                       m_framework->MapBitmap(aError,&aRedrawNeeded);

    /*
    If there are driving instructions or a legend, or there's a drag offset, copy the map bitmap into the extra GC,
    offset if necessary, and draw them on top.
    */
    if (!aError && (m_draw_driving_instructions || m_draw_legend || m_draw_scale || m_map_drag_offset.iX || m_map_drag_offset.iY))
        {
        // Create the extra GC if necessary.
        if (m_extra_gc == NULL || m_extra_gc->Bounds() != CartoType::TRect(0,0,bitmap->Width(),bitmap->Height()))
            m_extra_gc = m_framework->CreateGraphicsContext(bitmap->Width(),bitmap->Height());

        if (!aError)
            {
            assert(bitmap->DataBytes() == m_extra_gc->Bitmap()->DataBytes());
            assert(bitmap->RowBytes() == m_extra_gc->Bitmap()->RowBytes());

            // Copy the bitmap into the extra GC's bitmap. */
            if (m_map_drag_offset.iX || m_map_drag_offset.iY)
                {
                // Set the destination bitmap to white so that the edges don't have relics of the previous map.
                memset(const_cast<CartoType::uint8*>(m_extra_gc->Bitmap()->Data()),0xFF,m_extra_gc->Bitmap()->DataBytes());

                // Draw the source bitmap at the required offset.
                m_extra_gc->DrawBitmap(*bitmap,m_map_drag_offset);
                }
            else
                memcpy(const_cast<CartoType::uint8*>(m_extra_gc->Bitmap()->Data()),bitmap->Data(),bitmap->DataBytes());

            // Draw the driving instructions.
            if (m_draw_driving_instructions)
                DrawDrivingInstructions(*m_extra_gc,aMapClientArea);

            // Draw the legend.
            if (m_draw_legend || m_draw_scale)
                {
                if (!m_legend)
                    {
                    m_legend.reset(new CartoType::CLegend(*m_framework));
                    CartoType::TColor grey(90,90,90);
                    m_legend->SetTextColor(grey);

                    if (m_draw_legend || m_draw_scale)
                        {
                        m_legend->SetAlignment(CartoType::EAlignCenter);
                        CartoType::CString heading(m_framework->DataSetName());
                        m_framework->SetCase(heading,CartoType::ELowerCase);
                        m_framework->SetCase(heading,CartoType::ETitleCase);
                        m_legend->SetFontSize(10,"pt");
                        m_legend->AddTextLine(heading);
                        }

                    if (m_draw_legend)
                        {
                        m_legend->SetAlignment(CartoType::EAlignRight);
                        m_legend->SetFontSize(6,"pt");
                        CartoType::CString s;
                        s.SetAttribute("ref","M4");
                        m_legend->AddMapObjectLine(CartoType::ELineObject,"road/major",nullptr,CartoType::EMotorway,s,"motorway");
                        s.SetAttribute("ref","A40");
                        m_legend->AddMapObjectLine(CartoType::ELineObject,"road/major",nullptr,CartoType::ETrunkRoad,s,"trunk road");
                        s.SetAttribute("ref","A414");
                        m_legend->AddMapObjectLine(CartoType::ELineObject,"road/major",nullptr,CartoType::EPrimaryRoad,s,"primary road");
                        s.SetAttribute("ref","B4009");
                        m_legend->AddMapObjectLine(CartoType::ELineObject,"road/mid",nullptr,CartoType::ESecondaryRoad,s,"secondary road");
                        m_legend->AddMapObjectLine(CartoType::ELineObject,"road/mid",nullptr,CartoType::ETertiaryRoad,"High Street","tertiary road");
                        m_legend->AddMapObjectLine(CartoType::ELineObject,"road/minor",nullptr,CartoType::EPedestrianRoad,"","pedestrian road");
                        m_legend->AddMapObjectLine(CartoType::ELineObject,"road/minor",nullptr,CartoType::ETrack,"","track");
                        m_legend->AddMapObjectLine(CartoType::ELineObject,"road/major",nullptr,CartoType::ETrunkRoad | CartoType::ETunnelRoadTypeFlag,"","road tunnel");
                        m_legend->AddMapObjectLine(CartoType::ELineObject,"path","cyc",0,"","cycleway");
                        m_legend->AddMapObjectLine(CartoType::ELineObject,"path","bri",0,"","bridle path");
                        m_legend->AddMapObjectLine(CartoType::ELineObject,"path","foo",0,"","footpath");
                        m_legend->AddMapObjectLine(CartoType::ELineObject,"path","ste",0,"","steps");
                        m_legend->AddMapObjectLine(CartoType::EPolygonObject,"land/major","for",0,"Ashridge","forest or wood");
                        m_legend->AddMapObjectLine(CartoType::EPolygonObject,"land/minor","par",0,"Green Park","park, golf course or common");
                        m_legend->AddMapObjectLine(CartoType::EPolygonObject,"land/minor","gra",0,"","grassland");
                        m_legend->AddMapObjectLine(CartoType::EPolygonObject,"land/minor","orc",0,"","orchard, vineyard, etc.");
                        m_legend->AddMapObjectLine(CartoType::EPolygonObject,"land/minor","cmr",0,"","commercial or industrial");
                        m_legend->AddMapObjectLine(CartoType::EPolygonObject,"land/minor","cns",0,"","construction, quarry, landfill, etc.");
                        m_legend->AddMapObjectLine(CartoType::EPointObject,"amenity/minor","stn",0,"Berkhamsted","station");
                        }

                    if (m_draw_scale)
                        {
                        m_legend->SetAlignment(CartoType::EAlignCenter);
                        m_legend->SetFontSize(6,"pt");
                        m_legend->AddScaleLine(m_metric_units);
                        }

                    m_legend->SetBorder(CartoType::KGray,1,4,"pt");
                    CartoType::TColor b(CartoType::KWhite);
                    b.SetAlpha(224);
                    m_legend->SetBackgroundColor(b);
                    }

                std::unique_ptr<CartoType::CBitmap> legend_bitmap = m_legend->CreateLegend(1,"in",CartoType::Arithmetic::Max((int)m_framework->ScaleDenominator(),8000),m_framework->GetScaleDenominatorInView());
                if (legend_bitmap)
                    m_extra_gc->DrawBitmap(*legend_bitmap,CartoType::TPoint(bitmap->Width() - legend_bitmap->Width() - 16,16));
                }

            bitmap = m_extra_gc->Bitmap();
            }
        }

    return bitmap;
    }

void MapForm::EnableDrawLegend(bool aEnable)
    {
    if (aEnable != m_draw_legend)
        {
        m_draw_legend = aEnable;
        m_legend.reset();
        update();
        }
    }

void MapForm::EnableDrawScale(bool aEnable)
    {
    if (aEnable != m_draw_scale)
        {
        m_draw_scale = aEnable;
        m_legend.reset();
        update();
        }
    }

void MapForm::EnableDrawRotator(bool aEnable)
    {
    if (aEnable != m_draw_rotator)
        {
        m_draw_rotator = aEnable;
        if (m_draw_rotator)
            m_ui->dial->show();
        else
            m_ui->dial->hide();
        update();
        }
    }

void MapForm::DrawRange()
    {
    if (m_draw_range)
        {
        if (m_route_point_array[0].m_point != CartoType::TPointFP())
            {
            CartoType::TResult error = 0;
            CartoType::CGeometry g1 { m_framework->Range(error,nullptr,m_route_point_array[0].m_point.iX,m_route_point_array[0].m_point.iY,CartoType::EMapCoordType,600,true) };
            CartoType::CGeometry g2 { m_framework->Range(error,nullptr,m_route_point_array[0].m_point.iX,m_route_point_array[0].m_point.iY,CartoType::EMapCoordType,1200,true) };
            m_framework->InsertMapObject(0,CartoType::EPolygonObject,"range",g1,"",0,m_range_id0,true);
            m_framework->InsertMapObject(0,CartoType::EPolygonObject,"range",g2,"",0,m_range_id1,true);
            }
        }
    else
        {
        uint64_t count = 0;
        m_framework->DeleteMapObjects(0,m_range_id0,m_range_id0,count,"");
        m_framework->DeleteMapObjects(0,m_range_id1,m_range_id1,count,"");
        }
    update();
    }

void MapForm::EnableDrawRange(bool aEnable)
    {
    if (aEnable != m_draw_range)
        {
        m_draw_range = aEnable;
        DrawRange();
        }
    }

void MapForm::LeftButtonDown(int32_t aX,int32_t aY)
    {
    m_map_drag_enabled = true;
    m_map_drag_anchor.iX = aX;
    m_map_drag_anchor.iY = aY;
    setMouseTracking(true);
    }

void MapForm::LeftButtonUp(int32_t aX,int32_t aY)
    {
    m_map_drag_offset.iX = aX - m_map_drag_anchor.iX;
    m_map_drag_offset.iY = aY - m_map_drag_anchor.iY;

    if (m_left_click_simulates_navigation_fix)
        {
        // Supply a simulated navigation fix.
        if (m_map_drag_offset.iX == 0 && m_map_drag_offset.iY == 0)
            {
            CartoType::TNavigationData nav;
            nav.iValidity = CartoType::TNavigationData::EPositionValid | CartoType::TNavigationData::ETimeValid;
            nav.iPosition.iX = aX;
            nav.iPosition.iY = aY;
            m_on_route_time += 20;
            nav.iTime = m_on_route_time;
            m_framework->ConvertPoint(nav.iPosition.iX,nav.iPosition.iY,CartoType::EScreenCoordType,CartoType::EDegreeCoordType);
            m_framework->Navigate(nav);
            }
        }

    StopDragging();
    }

void MapForm::RightButtonDown(int32_t /*aX*/,int32_t /*aY*/)
    {

    }

void MapForm::RightButtonUp(int32_t aX,int32_t aY)
    {
    CartoType::TPointFP p(aX,aY);

    // Get the address of the point clicked on.
    CartoType::CAddress address;
    m_framework->GetAddress(address,aX,aY,CartoType::EScreenCoordType);

    CartoType::CString address_string;
    m_framework->ConvertPoint(p.iX,p.iY,CartoType::EScreenCoordType,CartoType::EDegreeCoordType);

    address_string = address.ToString(false,&p);

    address_string.Append("\r\n\r\n[");
    address_string += address.ToStringWithLabels();
    address_string.Append("]");

    // Add the altitude if available.
    double x[1] = { p.iX };
    double y[1] = { p.iY };
    int32_t alt[1];
    CartoType::TCoordSet cs;
    cs.iX = x;
    cs.iY = y;
    cs.iCount = 1;
    CartoType::TResult error = m_framework->GetHeights(cs,CartoType::EDegreeCoordType,alt);
    if (!error)
        {
        char buffer[32];
        sprintf(buffer," (altitude = %dm)",alt[0]);
        address_string.Append(buffer);
        }

    // If possible, get the Ordnance Survey National Grid reference.
    CartoType::CString os_grid_ref = CartoType::UKGridReferenceFromDegrees(p,3);
    if (os_grid_ref.Length())
        {
        address_string.Append("\r\n\r\nOrdnance Survey of Great Britain grid reference: ");
        address_string.Append(os_grid_ref);
        }

    // Get information about the objects clicked on - within 2 millimetres.
    CartoType::CMapObjectArray object;
    double pixel_mm = m_framework->ResolutionDpi() / 25.4;
    m_framework->FindInDisplay(object,100,aX,aY,int(ceil(2 * pixel_mm)));
    CartoType::CMapObject* pushpin = NULL;

    // See if we have a pushpin, and geocode the objects so that they can be sorted.
    for (auto& p : object)
        {
        if (p->LayerName() == "pushpin" && p->Type() == CartoType::EPointObject)
            pushpin = p.get();
        p->SetUserData((int)p->GetGeoCodeType());
        }

    // Sort objects on geocode and add information about them to the 'Address and nearby objects' field.
    if (object.size())
        {
        bool first_object = true;
        std::sort(object.begin(),object.end(),CartoType::MapObjectUserDataLessThan);
        for (auto& p : object)
            {
            // Skip unnamed map_extent and outline objects.
            if (!p->Label().Length())
                {
                if (p->LayerName() == "outline" || p->LayerName() == "map-extent")
                    continue;
                }
            if (first_object)
                {
                address_string.Append("\r\n\r\nNearby objects:\r\n");
                first_object = false;
                }
            address_string.Append(p->VerboseDescription().c_str());
            address_string.Append("\r\n");
            }
        }

    // Set up the route dialog.
    RouteDialog route_dialog(this);
    if (pushpin)
        {
        route_dialog.m_ui->addPushPin->setText("Edit pushpin");
        route_dialog.m_ui->addPushPin->setChecked(true);
        route_dialog.EnablePushPinEditing();
        }
    else
        {
        route_dialog.m_ui->cutPushPin->hide();
        if (m_route_point_array[0].m_point == CartoType::TPointFP())
            route_dialog.m_ui->setStartOfRoute->setChecked(true);
        else
            route_dialog.m_ui->setEndOfRoute->setChecked(true);
        }
    route_dialog.m_ui->visitWayPointsInAnyOrder->setChecked(m_best_route);

    QString qs;
    SetString(qs,address_string);
    route_dialog.m_ui->address->setPlainText(qs);

    CartoType::uint64 pushpin_id = 0;
    if (pushpin)
        {
        CartoType::TText value = pushpin->GetStringAttribute("");
        SetString(m_pushpin_name,value);
        value = pushpin->GetStringAttribute("desc");
        SetString(m_pushpin_desc,value);
        pushpin_id = pushpin->Id();
        }
    else
        {
        SetString(m_pushpin_name,address.ToString(false));
        SetString(m_pushpin_desc,address.ToString(true));
        }
    route_dialog.m_ui->pushPinName->setText(m_pushpin_name);
    route_dialog.m_ui->pushPinDesc->setPlainText(m_pushpin_desc);

    // Display the route dialog.
    if (route_dialog.exec() != QDialog::DialogCode::Accepted)
        return;

    bool create_route = false;
    m_best_route = route_dialog.m_ui->visitWayPointsInAnyOrder->isChecked();
    if (route_dialog.m_ui->setStartOfRoute->isChecked()) // set start of route
        {
        CartoType::TPointFP p(aX,aY);
        m_framework->ConvertPoint(p.iX,p.iY,CartoType::EScreenCoordType,CartoType::EMapCoordType);
        m_route_point_array[0].m_point = p;
        int32_t attrib = 0x6000; // set the level to one less than the top level; the vehicle will be drawn over it
        uint64_t id = CartoType::CFramework::KRouteStartId;
        m_framework->InsertPointMapObject(0,"route-start",p.iX,p.iY,CartoType::EMapCoordType,"",attrib,id,true);
        DrawRange();
        create_route = true;
        }
    else if (route_dialog.m_ui->setEndOfRoute->isChecked()) // set end of route
        {
        CartoType::TPointFP p(aX,aY);
        m_framework->ConvertPoint(p.iX,p.iY,CartoType::EScreenCoordType,CartoType::EMapCoordType);
        m_route_point_array[m_route_point_array.size() - 1].m_point = p;
        create_route = true;
        }
    else if (route_dialog.m_ui->addPushPin->isChecked()) // add or change a pushpin
        {
        m_pushpin_name = route_dialog.m_ui->pushPinName->text();
        m_pushpin_desc = route_dialog.m_ui->pushPinDesc->toPlainText();

        CartoType::CString string_attrib;
        string_attrib.Set(m_pushpin_name.utf16());
        if (m_pushpin_desc.length())
            {
            string_attrib.Append((CartoType::uint16)0);
            string_attrib.Append("desc=");
            string_attrib.Append(m_pushpin_desc.utf16());
            }
        double x = 0,y = 0;
        if (pushpin)
            {
            CartoType::TRect r { pushpin->CBox() };
            x = r.iTopLeft.iX;
            y = r.iTopLeft.iY;
            }
        else
            {
            x = aX;
            y = aY;
            m_framework->ConvertPoint(x,y,CartoType::EScreenCoordType,CartoType::EMapCoordType);
            }
        m_framework->InsertPointMapObject(m_writable_map_handle,"pushpin",x,y,CartoType::EMapCoordType,string_attrib,0,pushpin_id,true);
        m_main_window.UpdateSaveAddedData();
        m_main_window.UpdateDeletePushpins();

        if (route_dialog.m_ui->useAsWayPoint->isChecked())
            {
            TRoutePoint* waypoint = nullptr;

            for (int i = 1; waypoint == nullptr && i < (int)m_route_point_array.size() - 1; i++)
                if (m_route_point_array[i].m_id == pushpin_id)
                    waypoint = &m_route_point_array[i];
            if (waypoint == nullptr)
                {
                TRoutePoint rp;
                m_route_point_array.insert(m_route_point_array.begin() + m_route_point_array.size() - 1,rp);
                waypoint = &m_route_point_array[m_route_point_array.size() - 2];
                }
            if (waypoint)
                {
                waypoint->m_point.iX = x;
                waypoint->m_point.iY = y;
                waypoint->m_id = pushpin_id;
                create_route = true;
                }
            }

        m_writable_map_changed = true;
        }
    else if (route_dialog.m_ui->cutPushPin->isChecked()) // cut a pushpin
        {
        uint64_t count = 0;
        m_framework->DeleteMapObjects(m_writable_map_handle,pushpin_id,pushpin_id,count);
        for (int i = 1; i < (int)m_route_point_array.size() - 1; i++)
            if (m_route_point_array[i].m_id == pushpin_id)
                {
                m_route_point_array.erase(m_route_point_array.begin() + i);
                create_route = true;
                break;
                }
        m_writable_map_changed = true;
        m_main_window.UpdateSaveAddedData();
        m_main_window.UpdateDeletePushpins();
        }

    if (m_route_point_array[0].m_point == CartoType::TPointFP() ||
        m_route_point_array[m_route_point_array.size() - 1].m_point == CartoType::TPointFP())
        create_route = false;

    if (create_route)
        CalculateAndDisplayRoute();

    update();
    }

void MapForm::CalculateAndDisplayRoute()
    {
    CartoType::TCoordSet cs;
    cs.iX = &m_route_point_array[0].m_point.iX;
    cs.iY = &m_route_point_array[0].m_point.iY;
    cs.iCount = m_route_point_array.size();
    cs.iStep = int32_t(&m_route_point_array[1].m_point.iX - cs.iX);

    QApplication::setOverrideCursor(Qt::WaitCursor);
    CartoType::TResult error = 0;
    if (m_best_route && cs.iCount > 3)
        {
        CartoType::TRouteProfile profile(*m_framework->Profile(0));
        int iterations = cs.iCount * cs.iCount;
        if (iterations < 16)
            iterations = 16;
        else if (iterations > 256)
            iterations = 256;
        std::unique_ptr<CartoType::CRoute> r = m_framework->CreateBestRoute(error,profile,cs,CartoType::EMapCoordType,true,true,iterations);
        if (!error)
            error = m_framework->UseRoute(*r,true);
        }
    else
        error = m_framework->StartNavigation(cs,CartoType::EMapCoordType);
    QApplication::restoreOverrideCursor();

    if (error)
        m_main_window.ShowError("Routing error",error);
    else
        m_framework->DisplayRoute(true);

    const CartoType::CRoute* r = m_framework->Route();
    if (r)
        {
        CartoType::CString text;
        m_framework->AppendDistance(text,r->iDistance,m_metric_units);
        text.Append(" (");
        m_framework->AppendTime(text,r->iTime);
        text.Append(")");
        QString text_qs;
        text_qs.setUtf16(text.Text(),text.Length());
        m_main_window.statusBar()->showMessage(text_qs);
        }

    m_main_window.UpdateDeleteOrSaveRoute();
    }

void MapForm::Find()
    {
    FindDialog find_dialog(this,*m_framework);
    QString qs;
    qs.setUtf16(m_find_text.Text(),m_find_text.Length());
    find_dialog.m_ui->findText->setText(qs);
    find_dialog.m_ui->prefix->setChecked(m_find_text_prefix);
    find_dialog.m_ui->fuzzyMatch->setChecked(m_find_text_fuzzy);

    m_found_object_index = 0;
    while (find_dialog.exec() == QDialog::DialogCode::Accepted)
        {
        m_find_text.Set(find_dialog.m_ui->findText->text().utf16());
        m_find_text_prefix = find_dialog.m_ui->prefix->isChecked();
        m_find_text_fuzzy = find_dialog.m_ui->fuzzyMatch->isChecked();
        if (m_find_text.Length() == 0)
            continue;

        m_found_object = find_dialog.Find(true);
        if (m_found_object.size() == 0)
            {
            std::string s("Text ");
            s += std::string(m_find_text) + " not found";
            continue;
            }

        uint64_t count = 0;
        m_framework->DeleteMapObjects(0,m_found_object_id,m_found_object_id,count);
        m_found_object_id = 0;
        ShowNextFoundObject();
        break;
        }

    m_main_window.UpdateFindNext();
    }

void MapForm::FindAddress()
    {
    FindAddressDialog find_dialog;

    SetString(*find_dialog.m_ui->building,m_address.iBuilding);
    SetString(*find_dialog.m_ui->feature,m_address.iFeature);
    SetString(*find_dialog.m_ui->street,m_address.iStreet);
    SetString(*find_dialog.m_ui->subLocality,m_address.iSubLocality);
    SetString(*find_dialog.m_ui->locality,m_address.iLocality);
    SetString(*find_dialog.m_ui->island,m_address.iIsland);
    SetString(*find_dialog.m_ui->subAdminArea,m_address.iSubAdminArea);
    SetString(*find_dialog.m_ui->adminArea,m_address.iAdminArea);
    SetString(*find_dialog.m_ui->country,m_address.iCountry);
    SetString(*find_dialog.m_ui->postCode,m_address.iPostCode);

    bool stop = false;

    m_found_object_index = 0;
    while (!stop)
        {
        if (find_dialog.exec() == QDialog::DialogCode::Accepted)
            {
            m_found_object.clear();
            SetString(m_address.iBuilding,*find_dialog.m_ui->building);
            SetString(m_address.iFeature,*find_dialog.m_ui->feature);
            SetString(m_address.iStreet,*find_dialog.m_ui->street);
            SetString(m_address.iSubLocality,*find_dialog.m_ui->subLocality);
            SetString(m_address.iLocality,*find_dialog.m_ui->locality);
            SetString(m_address.iIsland,*find_dialog.m_ui->island);
            SetString(m_address.iSubAdminArea,*find_dialog.m_ui->subAdminArea);
            SetString(m_address.iAdminArea,*find_dialog.m_ui->adminArea);
            SetString(m_address.iCountry,*find_dialog.m_ui->country);
            SetString(m_address.iPostCode,*find_dialog.m_ui->postCode);

            CartoType::TResult error = m_framework->FindAddress(m_found_object,100,m_address,true);
            if (error || m_found_object.size() == 0)
                {
                m_main_window.ShowError("address not found",error);
                if (error)
                    stop = true;
                }
            else
                {
                ShowNextFoundObject();
                stop = true;
                }
            }
        else
            stop = true;
        }
    }

void MapForm::FindNext()
    {
    ShowNextFoundObject();
    }

void MapForm::ShowNextFoundObject()
    {
    auto& object = m_found_object[m_found_object_index];
    CartoType::uint32 memory_map_handle = m_framework->GetMemoryMapHandle();
    CartoType::TTextLiteral(found_layer,u"found");
    CartoType::TResult error = m_framework->InsertCopyOfMapObject(memory_map_handle,found_layer,*object,
                                                                  object->Type() == CartoType::EPointObject ? 50 : 0,
                                                                  CartoType::EMapMeterCoordType,m_found_object_id,true);
    if (!error)
        m_framework->SetView(*object,64,4000);
    if (error)
        return;

    CartoType::CString summary;
    m_framework->GeoCodeSummary(summary,*object);
    double length = m_framework->GetMapObjectLengthOrPerimeter(error,*object);
    double area = 0;
    if (!error)
        area = m_framework->GetMapObjectArea(error,*object);
    if (!error)
        {
        char buffer[64];
        if (area != 0)
            {
            if (m_metric_units)
                {
                if (area > 1000000)
                    sprintf(buffer,"; area = %.2f sq km",area / 1000000);
                else
                    sprintf(buffer,"; area = %.2f ha",area / 10000);
                }
            else
                {
                // Convert to acres.
                double acres = area / 4046.856422;
                if (acres > 640)
                    sprintf(buffer,"; area = %.2f sq mi",acres / 640);
                else
                    sprintf(buffer,"; area = %.2f acres",acres);
                }
            summary.Append(buffer);
            }
        else if (length != 0)
            {
            if (m_metric_units)
                {
                if (length > 1000)
                    sprintf(buffer,"; length = %.2f km",length / 1000);
                else
                    sprintf(buffer,"; length = %.2f m",length);
                }
            else
                {
                // Convert to yards.
                double yards = length / 0.9144;
                if (yards > 1760)
                    sprintf(buffer,"; length = %.2f mi",yards / 1760);
                else
                    sprintf(buffer,"; length = %.2f yards",yards);
                }
            summary.Append(buffer);
            }
        }
    QString summary_qs;
    summary_qs.setUtf16(summary.Text(),summary.Length());
    m_main_window.statusBar()->showMessage(summary_qs);

    m_found_object_index = (m_found_object_index + 1) % m_found_object.size();
    update();
    }

void MapForm::on_dial_valueChanged(int aValue)
    {
    m_framework->SetRotation(double(aValue) / 100.0);
    m_main_window.UpdateNorthUp();
    update();
    }

void MapForm::SetRotation(double aAngle)
    {
    m_framework->SetRotation(aAngle);
    m_ui->dial->setValue(int(aAngle * 100));
    update();
    }

void MapForm::ReverseRoute()
    {
    CartoType::TResult error = m_framework->ReverseRoutes();
    if (!error)
        {
        std::reverse(m_route_point_array.begin(),m_route_point_array.end());
        update();
        }
    }

void MapForm::DeleteRoute()
    {
    m_route_point_array.resize(2);
    m_route_point_array[0] = m_route_point_array[1] = TRoutePoint();
    m_framework->DeleteRoutes();
    m_main_window.UpdateDeleteOrSaveRoute();
    update();
    }

void MapForm::DeletePushpins()
    {
    uint64_t deleted_count = 0;
    m_framework->DeleteMapObjects(m_writable_map_handle,0,UINT64_MAX,deleted_count,"@layer=='pushpin'");
    m_main_window.UpdateDeletePushpins();
    update();
    }

bool MapForm::HasPushpins() const
    {
    CartoType::CMapObjectArray object_array;
    CartoType::TFindParam param;
    param.iMaxObjectCount = 1;
    param.iLayers = "pushpin";
    param.iMerge = false;
    m_framework->Find(object_array,param);
    return object_array.size() > 0;
    }

void MapForm::SetRouteProfileType(TExtendedRouteProfileType aRouteProfileType)
    {
    if (aRouteProfileType != m_route_profile_type)
        {
        m_route_profile_type = aRouteProfileType;
        if (aRouteProfileType == TExtendedRouteProfileType::ECustom)
            m_framework->SetMainProfile(m_main_window.CustomRouteProfile());
        else
            {
            CartoType::TRouteProfile p((CartoType::TRouteProfileType)m_route_profile_type);
            m_framework->SetMainProfile(p);
            }
        if (m_route_point_array[0].m_point != CartoType::TPointFP() &&
            m_route_point_array[m_route_point_array.size() - 1].m_point != CartoType::TPointFP())
            CalculateAndDisplayRoute();
        if (m_draw_range)
            DrawRange();
        m_main_window.UpdateRouteProfile();
        update();
        }
    }

void MapForm::ViewRouteInstructions()
    {
    const CartoType::CRoute* route = m_framework->Route();
    if (!route)
        return;

    RouteInstructionsDialog dialog(this);
    QString instructions;
    instructions.setUtf16(route->iInstructions.Text(),route->iInstructions.Length());
    dialog.m_ui->text->setPlainText(instructions);
    dialog.exec();
    }

bool MapForm::EditCustomRouteProfile(CartoType::TRouteProfile& aRouteProfile)
    {
    RouteProfileDialog dialog(this,aRouteProfile,m_metric_units);
    if  (dialog.exec() == QDialog::DialogCode::Accepted)
        {
        aRouteProfile = dialog.RouteProfile();
        if (m_route_profile_type == TExtendedRouteProfileType::ECustom)
            {
            m_framework->SetMainProfile(aRouteProfile);
            if (m_route_point_array[0].m_point != CartoType::TPointFP() &&
                m_route_point_array[m_route_point_array.size() - 1].m_point != CartoType::TPointFP())
                CalculateAndDisplayRoute();
            }
        return true;
        }
    return false;
    }

void MapForm::SaveRouteInstructions()
    {
    if (!m_framework->Route())
        return;

    std::string filename { GetSaveFile(*this,"Save route instructions","text files","txt") };
    if (filename.empty())
        return;

    CartoType::TResult error = 0;
    FILE* file = fopen(filename.c_str(),"w");
    if (!file)
        error = CartoType::KErrorIo;
    if (!error)
        {
        std::string text(m_framework->Route()->iInstructions);
        size_t n = fwrite(text.c_str(),text.length(),1,file);
        if (!n)
            error = CartoType::KErrorIo;
        }
    if (file)
        fclose(file);
    if (error)
        m_main_window.ShowError("failed to save the route instructions to a text file",error);
    }

void MapForm::SaveRouteAsXml()
    {
    if (!m_framework->Route())
        return;

    CartoType::CString path { GetSaveFile(*this,"Save Route","CartoType route files","ctroute") };
    if (!path.Length())
        return;
    CartoType::TResult error = m_framework->WriteRouteAsXml(*m_framework->Route(),path);
    if (error)
        m_main_window.ShowError("failed to save the route to a CTROUTE file",error);
    }

void MapForm::SaveRouteAsGpx()
    {
    if (!m_framework->Route())
        return;

    CartoType::CString path { GetSaveFile(*this,"Save route as GPX","GPX files","gpx") };
    if (!path.Length())
        return;

    CartoType::TResult error = m_framework->WriteRouteAsXml(*m_framework->Route(),path,CartoType::EGpxFileType);
    if (error)
        m_main_window.ShowError("failed to save the route to a GPX file",error);
    }

void MapForm::LoadRouteFromXml()
    {
    QString path = QFileDialog::getOpenFileName(this,"Load route","","GPX files or CartoType route files (*.gpx *.ctroute)");
    if (!path.length())
        return;
    CartoType::TResult error = m_framework->ReadRouteFromXml(path.utf16(),true);
    if (error)
        m_main_window.ShowError("failed to load a route from a CTROUTE file",error);
    else
        update();
    }

void MapForm::SaveImageAsPng()
    {
    CartoType::CString path { GetSaveFile(*this,"Save image as PNG","PNG files","png") };
    if (!path.Length())
        return;

    CartoType::TResult error = 0;
    std::unique_ptr<CartoType::CFileOutputStream> output_stream { CartoType::CFileOutputStream::New(error,path) };
    if (!error)
        {
        const CartoType::TBitmap* bitmap = m_framework->MapBitmap(error);
        if (!error)
            error = bitmap->WritePng(*output_stream,true);
        }
    if (error)
        m_main_window.ShowError("failed to save the image as a PNG file",error);
    }

bool MapForm::HasWritableData() const
    {
    return !m_framework->MapIsEmpty(m_writable_map_handle);
    }

void MapForm::SaveWritableDataAsCtms()
    {
    CartoType::CString path { GetSaveFile(*this,"Save added data as CTMS","CartoType serialized map files","ctms") };
    if (!path.Length())
        return;
    CartoType::TResult error = m_framework->SaveMap(m_writable_map_handle,path,CartoType::ECtmsFileType);
    if (error)
        m_main_window.ShowError("failed to save data to a CTMS file",error);
    else
        m_writable_map_changed = false;
    }

void MapForm::LoadWritableDataFromCtms()
    {
    QString qs = QFileDialog::getOpenFileName(this,"Import data from a CTMS file","","CartoType serialized map files (*.ctms)");
    CartoType::CString filename;
    filename.Set(qs.utf16(),qs.length());
    CartoType::TResult error = m_framework->ReadMap(m_writable_map_handle,filename,CartoType::ECtmsFileType);
    if (error)
        m_main_window.ShowError("failed to read data from a CTMS file",error);
    m_main_window.UpdateSaveAddedData();
    }

void MapForm::LoadWritableDataFromGpx()
    {
    QString filename = QFileDialog::getOpenFileName(this,"Import data from a GPX file","","GPX files (*.gpx)");
    CartoType::TResult error = m_framework->ReadGpx(m_writable_map_handle,filename.utf16());
    if (error)
        m_main_window.ShowError("failed to read data from a GPX file",error);
    m_main_window.UpdateSaveAddedData();
    }

void MapForm::SetScale()
    {
    ScaleDialog scale_dialog(this);
    QString scale_qs;
    scale_qs.setNum((int)m_framework->GetScaleDenominatorInView());
    scale_dialog.m_ui->scale->setText(scale_qs);
    if (scale_dialog.exec() == QDialog::DialogCode::Accepted)
        {
        scale_qs = scale_dialog.m_ui->scale->text();
        int new_scale = scale_qs.toInt();
        m_framework->SetScaleDenominatorInView(new_scale);
        update();
        }
    }

void MapForm::GoToLocation(double aLong,double aLat)
    {
    double minx,miny,maxx,maxy;
    m_framework->GetMapExtent(minx,miny,maxx,maxy,CartoType::EDegreeCoordType);
    if (aLong < minx)
        aLong = minx;
    else if (aLong > maxx)
        aLong = maxx;
    if (aLat < miny)
        aLat = miny;
    else if (aLat > maxy)
        aLat = maxy;

    m_found_object.clear();
    m_found_object_index = 0;
    CartoType::TTextLiteral(found_layer,u"found");
    m_framework->InsertCircleMapObject(0,found_layer,
                                       aLong,aLat,CartoType::EDegreeCoordType,50,CartoType::EMapMeterCoordType,
                                       nullptr,0,m_found_object_id,true);
    m_framework->SetScaleDenominator(25000);
    m_framework->SetViewCenter(aLong,aLat,CartoType::EDegreeCoordType);
    update();
    }

void MapForm::GoToLocation()
    {
    LocationDialog location_dialog(this);
    double minx,miny,maxx,maxy;
    m_framework->GetView(minx,miny,maxx,maxy,CartoType::EDegreeCoordType);
    double cur_long = (minx + maxx) / 2;
    double cur_lat = (miny + maxy) / 2;
    QString long_qs;
    long_qs.setNum(cur_long);
    location_dialog.m_ui->longitude->setText(long_qs);
    QString lat_qs;
    lat_qs.setNum(cur_lat);
    location_dialog.m_ui->latitude->setText(lat_qs);
    if (location_dialog.exec() == QDialog::DialogCode::Accepted)
        {
        double new_long = location_dialog.m_ui->longitude->text().toDouble();
        double new_lat = location_dialog.m_ui->latitude->text().toDouble();
        GoToLocation(new_long,new_lat);
        }
    }

void MapForm::GoToOrdnanceSurveyGridRef()
    {
    LocationDialog location_dialog(this);
    location_dialog.m_ui->lat_label->hide();
    location_dialog.m_ui->latitude->hide();
    location_dialog.m_ui->long_label->setText("grid reference");
    location_dialog.setWindowTitle("Go to Grid Reference");

    double minx,miny,maxx,maxy;
    m_framework->GetView(minx,miny,maxx,maxy,CartoType::EDegreeCoordType);
    double cur_long = (minx + maxx) / 2;
    double cur_lat = (miny + maxy) / 2;
    CartoType::TPointFP p(cur_long,cur_lat);
    CartoType::CString ref = CartoType::UKGridReferenceFromDegrees(p,3);
    QString ref_qs;
    ref_qs.setUtf16(ref.Text(),ref.Length());
    location_dialog.m_ui->longitude->setText(ref_qs);
    if (location_dialog.exec() == QDialog::DialogCode::Accepted)
        {
        ref_qs = location_dialog.m_ui->longitude->text();
        ref.Set(ref_qs.utf16());
        p = CartoType::PointInDegreesFromUKGridReference(ref);
        if (p.iX != 0 && p.iY != 0)
            GoToLocation(p.iX,p.iY);
        }
    }

void MapForm::ChooseStyleSheet()
    {
    QString qs = QFileDialog::getOpenFileName(this,"Choose Style Sheet","","CartoType style sheets (*.ctstyle *.xml)");
    CartoType::CString filename;
    filename.Set(qs.utf16(),qs.length());
    if (filename.Length())
        {
        CartoType::TResult error = m_framework->SetStyleSheet(filename);
        if (error)
            m_main_window.ShowError("error loading stylesheet",error);
        else
            {
            m_legend.reset();
            update();
            }
        }
    }

void MapForm::ReloadStyleSheet()
    {
    m_framework->ReloadStyleSheet();
    m_legend.reset();
    update();
    }

void MapForm::ApplyStyleSheet(std::string aStyleSheet)
    {
    if (aStyleSheet.length())
        {
        m_main_window.SetCustomStyleSheet(aStyleSheet);

        if (!m_use_custom_style_sheet)
            SetUseCustomStyleSheet(true);
        else
            {
            m_framework->SetStyleSheet((const uint8_t*)aStyleSheet.data(),aStyleSheet.length());
            m_legend.reset();
            update();
            }
        m_style_sheet_applied = true;
        }
    }

bool MapForm::EditCustomStyleSheet()
    {
    std::string saved_custom_style_sheet { m_main_window.CustomStyleSheet() };
    StyleDialog style_dialog(this,*m_framework,saved_custom_style_sheet);
    QMetaObject::Connection connection = connect(&style_dialog,SIGNAL(ApplyStyleSheet(std::string)),this,SLOT(ApplyStyleSheet(std::string)));

    if (style_dialog.HaveError())
        {
        std::string s { "style sheet parse error: " };
        s += style_dialog.ErrorMessage() + " at '" + style_dialog.ErrorLocation() + "'";
        m_main_window.ShowError(s.c_str(),0);
        return false;
        }

    m_style_sheet_applied = false;
    if (style_dialog.exec() == QDialog::DialogCode::Accepted)
        ApplyStyleSheet(style_dialog.EditedStyleSheetText());
    else if (m_style_sheet_applied)
        ApplyStyleSheet(saved_custom_style_sheet);

    disconnect(connection);
    return true;
    }

void MapForm::ChooseLayers()
    {
    LayerDialog layer_dialog(this);
    std::vector<CartoType::CString> layers { m_framework->LayerNames() };
    for (const auto& layer_name : layers)
        {
        QString qs_name; qs_name.setUtf16(layer_name.Text(),layer_name.Length());
        QListWidgetItem* item = new QListWidgetItem(qs_name);
        if (m_framework->LayerIsEnabled(layer_name))
            item->setCheckState(Qt::Checked);
        else
            item->setCheckState(Qt::Unchecked);
        layer_dialog.m_ui->list->addItem(item);
        }
    if (layer_dialog.exec() == QDialog::DialogCode::Accepted)
        {
        for (int i = 0; i < layer_dialog.m_ui->list->count(); i++)
            {
            QString qs_name = layer_dialog.m_ui->list->item(i)->text();
            CartoType::CString layer_name; layer_name.Set(qs_name.utf16());
            bool checked = layer_dialog.m_ui->list->item(i)->checkState() == Qt::Checked;
            m_framework->EnableLayer(layer_name,checked);
            }
        update();
        }
    }

void MapForm::SetMetricUnits(bool aEnable)
    {
    if (aEnable != m_metric_units)
        {
        m_metric_units = aEnable;
        m_framework->SetLocale(m_metric_units ? "en_xx" : "en");
        m_legend.reset();
        if (DrawScaleEnabled())
            update();
        }
    }

void MapForm::SetPerspective(bool aEnable)
    {
    if (aEnable != m_framework->Perspective())
        {
        m_framework->SetPerspective(aEnable);
        update();
        }
    }

bool MapForm::Perspective() const
    {
    return m_framework->Perspective();
    }

bool MapForm::MapIncludesGreatBritain() const
    {
    CartoType::TRectFP bounds;
    m_framework->GetMapExtent(bounds,CartoType::EDegreeCoordType);
    static CartoType::TRectFP gb(-10,49,4,62);
    bounds.Intersection(gb);
    return !bounds.IsEmpty();
    }

void MapForm::SetUseCustomStyleSheet(bool aEnable)
    {
    if (m_use_custom_style_sheet == aEnable)
        return;
    m_use_custom_style_sheet = aEnable;
    m_main_window.UpdateStyleSheet();
    if (aEnable)
        {
        m_saved_style_sheet = m_framework->GetStyleSheetData(0);

        // If there's no custom style sheet, copy the current style sheet into it.
        if (m_main_window.CustomStyleSheet().empty())
            m_main_window.SetCustomStyleSheet(m_framework->GetStyleSheetText(0));

        const std::string& style = m_main_window.CustomStyleSheet();
        m_framework->SetStyleSheet((const uint8_t*)style.data(),style.length());
        }
    else
        m_framework->SetStyleSheet(m_saved_style_sheet);
    m_legend.reset();
    update();
    }

void MapForm::OnCustomStyleSheetLoaded()
    {
    if (m_use_custom_style_sheet)
        {
        const std::string& style = m_main_window.CustomStyleSheet();
        m_framework->SetStyleSheet((const uint8_t*)style.data(),style.length());
        }
    }

void MapForm::Print(bool aPreview)
    {
    QPrinter printer(QPrinter::HighResolution);

    if (!aPreview)
        {
        QPrintDialog dialog(&printer,this);
        dialog.setWindowTitle(tr("Print Map"));
        if (dialog.exec() != QDialog::Accepted)
            return;
        }

    CartoType::TResult error = 0;
    std::unique_ptr<CartoType::CFramework> print_framework { m_framework->Copy(error) };
    if (error)
        {
        m_main_window.ShowError("failed to make a CartoType::CFramework copy for printing",error);
        return;
        }
    print_framework->SetResolutionDpi(printer.resolution());
    QRectF page_rect { printer.pageRect(QPrinter::DevicePixel) };

    auto CreateImage = [&](QRectF aPageRect)->QImage
        {
        print_framework->Resize(int(aPageRect.width()),int(aPageRect.height()));
        print_framework->SetScaleDenominator(m_framework->ScaleDenominator());
        CartoType::TRectFP view;
        m_framework->GetView(view,CartoType::EMapCoordType);
        print_framework->SetViewCenter((view.Left() + view.Right()) / 2,(view.Top() + view.Bottom()) / 2,CartoType::EMapCoordType);
        print_framework->SetRotation(m_framework->Rotation());
        print_framework->SetScaleDenominatorInView(m_framework->GetScaleDenominatorInView());

        const CartoType::TBitmap* map_bitmap = print_framework->MapBitmap(error);
        if (error)
            {
            m_main_window.ShowError("failed to draw the map for printing",error);
            return QImage();
            }

        CartoType::CLegend legend(*print_framework);
        legend.SetAlignment(CartoType::EAlignCenter);
        legend.SetFontSize(6,"pt");
        legend.SetTextColor(CartoType::KDarkGray);
        legend.AddScaleLine(m_metric_units);
        CartoType::TColor cartotype_red(178,31,41);
        legend.SetTextColor(cartotype_red);
        legend.SetFontFamily("DejaVu Serif");
        legend.AddTextLine("");
        legend.AddTextLine("CartoType                    www.cartotype.com");
        legend.SetBorder(CartoType::KGray,1,4,"pt");
        CartoType::TColor b(CartoType::KWhite);
        b.SetAlpha(200);
        legend.SetBackgroundColor(b);
        auto legend_bitmap = legend.CreateLegend(2,"in",CartoType::Arithmetic::Max((int)print_framework->ScaleDenominator(),8000),print_framework->GetScaleDenominatorInView());
        auto& gc = print_framework->GetMapGraphicsContext();
        gc.DrawBitmap(*legend_bitmap,CartoType::TPoint(aPageRect.width() - legend_bitmap->Width() - 16,aPageRect.height() - legend_bitmap->Height() - 16));

        QImage image { int(aPageRect.width()),int(aPageRect.height()),QImage::Format_ARGB32_Premultiplied };
        CopyBitmapToImage(*map_bitmap,image);
        return image;
        };
    QImage image { CreateImage(page_rect) };
    if (image.isNull())
        return;

    auto Paint = [&image,&page_rect,&CreateImage](QPrinter* aPrinter)->void
        {
        QRectF cur_page_rect { aPrinter->pageRect(QPrinter::DevicePixel) };
        if (cur_page_rect != page_rect)
            {
            image = CreateImage(cur_page_rect);
            if (image.isNull())
                return;
            page_rect = cur_page_rect;
            }

        QPainter painter;
        painter.begin(aPrinter);
        painter.drawImage(0,0,image);
        painter.end();
        };

    if (aPreview)
        {
        QPrintPreviewDialog preview(&printer,this);
        connect(&preview,&QPrintPreviewDialog::paintRequested,Paint);
        preview.exec();
        }
    else
        Paint(&printer);
    }
