
// CartoTypeDemoView.cpp : implementation of the CCartoTypeDemoView class
//

#include "stdafx.h"

// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "CartoTypeDemo.h"
#endif

#include "CartoTypeDemoDoc.h"
#include "CartoTypeDemoView.h"
#include "FindDialog.h"
#include "LatLongDialog.h"
#include "LayerDialog.h"
#include "SetScaleDialog.h"
#include "RouteDialog.h"
#include "MainFrm.h"
#include <fstream>
#include <cartotype_string_tokenizer.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCartoTypeDemoView

IMPLEMENT_DYNCREATE(CCartoTypeDemoView, CView)

BEGIN_MESSAGE_MAP(CCartoTypeDemoView, CView)
    ON_WM_CONTEXTMENU()
    ON_WM_RBUTTONUP()
    ON_WM_SIZE()
    ON_WM_ERASEBKGND()
    ON_WM_MOUSEWHEEL()
    ON_WM_MOUSEMOVE()
    ON_WM_MOUSELEAVE()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_DESTROY()
    ON_WM_TIMER()
    ON_COMMAND(ID_FILE_OPEN_IN_CURRENT_MAP,&CCartoTypeDemoView::OnFileOpenInCurrentMap)
    ON_COMMAND(ID_FILE_CLOSE,&CCartoTypeDemoView::OnFileClose)
    ON_UPDATE_COMMAND_UI(ID_FILE_CLOSE,&CCartoTypeDemoView::OnUpdateFileClose)
    ON_COMMAND(ID_FILE_SAVE_AS_PNG,&CCartoTypeDemoView::OnFileSaveAsPng)
    ON_COMMAND(ID_FILE_SAVE_AS_CTSQL,&CCartoTypeDemoView::OnFileSaveAsCtsql)
    ON_COMMAND(ID_FIND,&CCartoTypeDemoView::OnFind)
    ON_UPDATE_COMMAND_UI(ID_FIND,&CCartoTypeDemoView::OnUpdateFind)
    ON_COMMAND(ID_FIND_ADDRESS,&CCartoTypeDemoView::OnFindAddress)
    ON_UPDATE_COMMAND_UI(ID_FIND_ADDRESS,&CCartoTypeDemoView::OnUpdateFindAddress)
    ON_COMMAND(ID_FIND_NEXT,&CCartoTypeDemoView::OnFindNext)
    ON_UPDATE_COMMAND_UI(ID_FIND_NEXT,&CCartoTypeDemoView::OnUpdateFindNext)
    ON_COMMAND(ID_VIEW_RELOAD_STYLE_SHEET,&CCartoTypeDemoView::OnViewReloadStyleSheet)
    ON_UPDATE_COMMAND_UI(ID_VIEW_RELOAD_STYLE_SHEET,&CCartoTypeDemoView::OnUpdateViewReloadStyleSheet)
    ON_COMMAND(ID_VIEW_ZOOM_IN,&CCartoTypeDemoView::OnViewZoomIn)
    ON_COMMAND(ID_VIEW_ZOOM_OUT,&CCartoTypeDemoView::OnViewZoomOut)
    ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM_IN,&CCartoTypeDemoView::OnUpdateViewZoomIn)
    ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM_OUT,&CCartoTypeDemoView::OnUpdateViewZoomOut)
    ON_COMMAND(ID_VIEW_ROTATE,&CCartoTypeDemoView::OnViewRotate)
    ON_UPDATE_COMMAND_UI(ID_VIEW_ROTATE,&CCartoTypeDemoView::OnUpdateViewRotate)
    ON_COMMAND(ID_NORTH_UP,&CCartoTypeDemoView::OnViewNorthup)
    ON_UPDATE_COMMAND_UI(ID_NORTH_UP,&CCartoTypeDemoView::OnUpdateViewNorthup)
    ON_COMMAND(ID_VIEW_LATLONG,&CCartoTypeDemoView::OnViewLatlong)
    ON_UPDATE_COMMAND_UI(ID_VIEW_LATLONG,&CCartoTypeDemoView::OnUpdateViewLatlong)
    ON_COMMAND(ID_VIEW_STYLESHEET,&CCartoTypeDemoView::OnViewStylesheet)
    ON_UPDATE_COMMAND_UI(ID_VIEW_STYLESHEET,&CCartoTypeDemoView::OnUpdateViewStylesheet)
    ON_COMMAND(ID_VIEW_LAYERS,&CCartoTypeDemoView::OnViewLayers)
    ON_UPDATE_COMMAND_UI(ID_VIEW_LAYERS,&CCartoTypeDemoView::OnUpdateViewLayers)
    ON_COMMAND(ID_VIEW_LEGEND,&CCartoTypeDemoView::OnViewLegend)
    ON_UPDATE_COMMAND_UI(ID_VIEW_LEGEND,&CCartoTypeDemoView::OnUpdateViewLegend)
    ON_COMMAND(ID_VIEW_SET_SCALE,&CCartoTypeDemoView::OnViewSetScale)
    ON_UPDATE_COMMAND_UI(ID_VIEW_SET_SCALE,&CCartoTypeDemoView::OnUpdateViewSetScale)
    ON_COMMAND(ID_VIEW_PERSPECTIVE,&CCartoTypeDemoView::OnViewPerspective)
    ON_UPDATE_COMMAND_UI(ID_VIEW_PERSPECTIVE,&CCartoTypeDemoView::OnUpdateViewPerspective)
    ON_COMMAND(ID_VIEW_METRIC_UNITS,&CCartoTypeDemoView::OnViewMetricUnits)
    ON_UPDATE_COMMAND_UI(ID_VIEW_METRIC_UNITS,&CCartoTypeDemoView::OnUpdateViewMetricUnits)
    ON_COMMAND(ID_VIEW_DRIVING_INSTRUCTIONS,&CCartoTypeDemoView::OnViewDrivingInstructions)
    ON_UPDATE_COMMAND_UI(ID_VIEW_DRIVING_INSTRUCTIONS,&CCartoTypeDemoView::OnUpdateViewDrivingInstructions)
    ON_COMMAND(ID_VIEW_FLY_THROUGH,&CCartoTypeDemoView::OnViewFlyThrough)
    ON_UPDATE_COMMAND_UI(ID_VIEW_FLY_THROUGH,&CCartoTypeDemoView::OnUpdateViewFlyThrough)
    ON_COMMAND(ID_GO_ALONG_ROUTE,&CCartoTypeDemoView::OnGoAlongRoute)
    ON_UPDATE_COMMAND_UI(ID_GO_ALONG_ROUTE,&CCartoTypeDemoView::OnUpdateGoAlongRoute)
    ON_COMMAND(ID_ROUTE_DELETE_ROUTE,&CCartoTypeDemoView::OnDeleteRoute)
    ON_UPDATE_COMMAND_UI(ID_ROUTE_DELETE_ROUTE,&CCartoTypeDemoView::OnUpdateDeleteRoute)
    ON_COMMAND(ID_ROUTE_DRIVE,&CCartoTypeDemoView::OnSetDriveProfile)
    ON_UPDATE_COMMAND_UI(ID_ROUTE_DRIVE,&CCartoTypeDemoView::OnUpdateSetDriveProfile)
    ON_COMMAND(ID_ROUTE_CYCLE,&CCartoTypeDemoView::OnSetCycleProfile)
    ON_UPDATE_COMMAND_UI(ID_ROUTE_CYCLE,&CCartoTypeDemoView::OnUpdateSetCycleProfile)
    ON_COMMAND(ID_ROUTE_WALK,&CCartoTypeDemoView::OnSetWalkProfile)
    ON_UPDATE_COMMAND_UI(ID_ROUTE_WALK,&CCartoTypeDemoView::OnUpdateSetWalkProfile)
    ON_COMMAND(ID_ROUTE_HIKE,&CCartoTypeDemoView::OnSetHikeProfile)
    ON_UPDATE_COMMAND_UI(ID_ROUTE_HIKE,&CCartoTypeDemoView::OnUpdateSetHikeProfile)
    ON_COMMAND(ID_ROUTE_SHORTEST,&CCartoTypeDemoView::OnShortestRoute)
    ON_UPDATE_COMMAND_UI(ID_ROUTE_SHORTEST,&CCartoTypeDemoView::OnUpdateShortestRoute)
    ON_COMMAND(ID_ROUTE_USEGRADIENTS,&CCartoTypeDemoView::OnUseGradients)
    ON_UPDATE_COMMAND_UI(ID_ROUTE_USEGRADIENTS,&CCartoTypeDemoView::OnUpdateUseGradients)
    ON_COMMAND(ID_ROUTE_SAVE_INSTRUCTIONS,&CCartoTypeDemoView::OnSaveRouteInstructions)
    ON_UPDATE_COMMAND_UI(ID_ROUTE_SAVE_INSTRUCTIONS,&CCartoTypeDemoView::OnUpdateSaveRouteInstructions)
    ON_COMMAND(ID_ROUTE_SAVE_ROUTE_XML,&CCartoTypeDemoView::OnSaveRouteXml)
    ON_UPDATE_COMMAND_UI(ID_ROUTE_SAVE_ROUTE_XML,&CCartoTypeDemoView::OnUpdateSaveRouteXml)
    ON_COMMAND(ID_ROUTE_SAVE_ROUTE_GPX,&CCartoTypeDemoView::OnSaveRouteGpx)
    ON_UPDATE_COMMAND_UI(ID_ROUTE_SAVE_ROUTE_GPX,&CCartoTypeDemoView::OnUpdateSaveRouteGpx)
    ON_COMMAND(ID_ROUTE_LOAD_ROUTE_XML,&CCartoTypeDemoView::OnLoadRouteXml)
    ON_UPDATE_COMMAND_UI(ID_ROUTE_LOAD_ROUTE_XML,&CCartoTypeDemoView::OnUpdateLoadRouteXml)
    ON_COMMAND(ID_EDIT_LOAD_KML,&CCartoTypeDemoView::OnEditLoadKml)
    ON_UPDATE_COMMAND_UI(ID_EDIT_LOAD_KML,&CCartoTypeDemoView::OnUpdateEditLoadKml)
    ON_COMMAND(ID_EDIT_SAVE_KML,&CCartoTypeDemoView::OnEditSaveKml)
    ON_UPDATE_COMMAND_UI(ID_EDIT_SAVE_KML,&CCartoTypeDemoView::OnUpdateEditSaveKml)
    ON_COMMAND(ID_EDIT_LOAD_CTMS,&CCartoTypeDemoView::OnEditLoadCtms)
    ON_UPDATE_COMMAND_UI(ID_EDIT_LOAD_CTMS,&CCartoTypeDemoView::OnUpdateEditLoadCtms)
    ON_COMMAND(ID_EDIT_SAVE_CTMS,&CCartoTypeDemoView::OnEditSaveCtms)
    ON_UPDATE_COMMAND_UI(ID_EDIT_SAVE_CTMS,&CCartoTypeDemoView::OnUpdateEditSaveCtms)
    ON_COMMAND(ID_EDIT_LOAD_CTSQL,&CCartoTypeDemoView::OnEditLoadCtsql)
    ON_UPDATE_COMMAND_UI(ID_EDIT_LOAD_CTSQL,&CCartoTypeDemoView::OnUpdateEditLoadCtsql)
    ON_COMMAND(ID_EDIT_SAVE_CTSQL,&CCartoTypeDemoView::OnEditSaveCtsql)
    ON_UPDATE_COMMAND_UI(ID_EDIT_SAVE_CTSQL,&CCartoTypeDemoView::OnUpdateEditSaveCtsql)
    ON_COMMAND(ID_EDIT_LOAD_GPX,&CCartoTypeDemoView::OnEditLoadGpx)
    ON_UPDATE_COMMAND_UI(ID_EDIT_LOAD_GPX,&CCartoTypeDemoView::OnUpdateEditLoadGpx)
END_MESSAGE_MAP()

// CCartoTypeDemoView construction/destruction

CCartoTypeDemoView::CCartoTypeDemoView()
    {
    iRoutePointArray.resize(2);
    }

CCartoTypeDemoView::~CCartoTypeDemoView()
    {
    iRouteIter.reset(nullptr);
    iFramework.reset(nullptr);
    iImageServerHelper.reset(nullptr);
    iExtraGc.reset(nullptr);
    }

BOOL CCartoTypeDemoView::PreCreateWindow(CREATESTRUCT& cs)
{
    // TODO: Modify the Window class or styles here by modifying
    //  the CREATESTRUCT cs

    return CView::PreCreateWindow(cs);
}

const CartoType::TBitmap* CCartoTypeDemoView::MapBitmap(CartoType::TResult& aError,const CartoType::TRect& aMapClientArea,bool& aRedrawNeeded)
    {
    // Get the map bitmap.
    const CartoType::TBitmap* bitmap = iImageServerHelper ?
                                       iFramework->MemoryDataBaseMapBitmap(aError,&aRedrawNeeded) : 
                                       iFramework->MapBitmap(aError,&aRedrawNeeded);

    /*
    If there are driving instructions or a legend, or there's a drag offset, copy the map bitmap into the extra GC,
    offset if necessary, and draw them on top.
    */
    if (!aError && (iDrawDrivingInstructions || iDrawLegend || iMapDragOffset.iX || iMapDragOffset.iY))
        {
        // Create the extra GC if necessary.
        if (iExtraGc == NULL || iExtraGc->Bounds() != CartoType::TRect(0,0,bitmap->Width(),bitmap->Height()))
            iExtraGc = iFramework->CreateGraphicsContext(bitmap->Width(),bitmap->Height());

        if (!aError)
            {
            ASSERT(bitmap->DataBytes() == iExtraGc->Bitmap()->DataBytes());
            ASSERT(bitmap->RowBytes() == iExtraGc->Bitmap()->RowBytes());

            // Copy the bitmap into the extra GC's bitmap. */
            if (iMapDragOffset.iX || iMapDragOffset.iY)
                {
                // Set the destination bitmap to white so that the edges don't have relics of the previous map.
                memset(const_cast<CartoType::uint8*>(iExtraGc->Bitmap()->Data()),0xFF,iExtraGc->Bitmap()->DataBytes());

                // Draw the source bitmap at the required offset.
                iExtraGc->DrawBitmap(*bitmap,iMapDragOffset);
                }
            else
                memcpy(const_cast<CartoType::uint8*>(iExtraGc->Bitmap()->Data()),bitmap->Data(),bitmap->DataBytes());

            // Draw the driving instructions.
            if (iDrawDrivingInstructions)
                DrawDrivingInstructions(*iExtraGc,aMapClientArea);

            // Draw the legend.
            if (iDrawLegend)
                {
                if (!iLegend)
                    {
                    iLegend.reset(new CartoType::CLegend(*iFramework));
                    CartoType::TColor grey(90,90,90);
                    iLegend->SetTextColor(grey);
                    iLegend->SetAlignment(CartoType::EAlignCenter);
                    CartoType::CString dataset_name(iFramework->DataSetName());
                    iFramework->SetCase(dataset_name,CartoType::ELowerCase);
                    iFramework->SetCase(dataset_name,CartoType::ETitleCase);
                    iLegend->SetFontSize(10,"pt");
                    iLegend->AddTextLine(dataset_name);

                    iLegend->SetAlignment(CartoType::EAlignRight);
                    iLegend->SetFontSize(6,"pt");
                    CartoType::CString s;
                    s.SetAttribute("ref","M4");
                    iLegend->AddMapObjectLine(CartoType::ELineObject,"road/major",nullptr,CartoType::EMotorway,s,"motorway");
                    s.SetAttribute("ref","A40");
                    iLegend->AddMapObjectLine(CartoType::ELineObject,"road/major",nullptr,CartoType::ETrunkRoad,s,"trunk road");
                    s.SetAttribute("ref","A414");
                    iLegend->AddMapObjectLine(CartoType::ELineObject,"road/major",nullptr,CartoType::EPrimaryRoad,s,"primary road");
                    s.SetAttribute("ref","B4009");
                    iLegend->AddMapObjectLine(CartoType::ELineObject,"road/mid",nullptr,CartoType::ESecondaryRoad,s,"secondary road");
                    iLegend->AddMapObjectLine(CartoType::ELineObject,"road/mid",nullptr,CartoType::ETertiaryRoad,"High Street","tertiary road");
                    iLegend->AddMapObjectLine(CartoType::ELineObject,"path","cyc",0,"","cycleway");
                    iLegend->AddMapObjectLine(CartoType::ELineObject,"path","bri",0,"","bridle path");
                    iLegend->AddMapObjectLine(CartoType::ELineObject,"path","foo",0,"","footpath");
                    iLegend->AddMapObjectLine(CartoType::EPolygonObject,"land/major","for",0,"Ashridge","forest or wood");
                    iLegend->AddMapObjectLine(CartoType::EPolygonObject,"land/minor","par",0,"Green Park","park, golf course or common");
                    iLegend->AddMapObjectLine(CartoType::EPolygonObject,"land/minor","gra",0,"","grassland");
                    iLegend->AddMapObjectLine(CartoType::EPolygonObject,"land/minor","orc",0,"","orchard, vineyard, etc.");
                    iLegend->AddMapObjectLine(CartoType::EPolygonObject,"land/minor","cmr",0,"","commercial or industrial");
                    iLegend->AddMapObjectLine(CartoType::EPolygonObject,"land/minor","cns",0,"","construction, quarry, landfill, etc.");
                    iLegend->AddMapObjectLine(CartoType::EPointObject,"amenity/minor","stn",0,"Berkhamsted","station");

                    iLegend->SetAlignment(CartoType::EAlignCenter);
                    iLegend->AddScaleLine(iMetricUnits);

                    iLegend->SetBorder(CartoType::KGray,1,4,"pt");
                    CartoType::TColor b(CartoType::KWhite);
                    b.SetAlpha(224);
                    iLegend->SetBackgroundColor(b);
                    }

                std::unique_ptr<CartoType::CBitmap> legend_bitmap = iLegend->CreateLegend(1,"in",CartoType::Arithmetic::Max((int)iFramework->ScaleDenominator(),10000),iFramework->GetScaleDenominatorInView());
                if (legend_bitmap)
                    iExtraGc->DrawBitmap(*legend_bitmap,CartoType::TPoint(bitmap->Width() - legend_bitmap->Width() - 16,16));
                }

            bitmap = iExtraGc->Bitmap();
            }
        }

    return bitmap;
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

/** Draw the current driving instruction. */
void CCartoTypeDemoView::DrawDrivingInstructions(CartoType::CGraphicsContext& aGc,const CartoType::TRect& aMapClientArea)
    {
    double pt = iFramework->ResolutionDpi() / 72.0;
    static const CartoType::TColor natural_blue(0,135,189);

    // Create the fonts.	
    CartoType::TFontSpec fs;
    int instruction_font_size = CartoType::Arithmetic::Round(9.0 * pt);
    int distance_and_time_font_size = CartoType::Arithmetic::Round(6.0 * pt);
    fs.SetToSize(instruction_font_size);
    CartoType::TFont instruction_font = iFramework->Font(fs);
    fs.SetToSize(distance_and_time_font_size);
    CartoType::TFont distance_and_time_font = iFramework->Font(fs);

    // Measure the text.
    CartoType::TTextParam text_param;
    text_param.iBaseline = CartoType::ETextBeforeEdgeBaseline;
    CartoType::TTextMetrics text_metrics;
    CartoType::TPoint p;
    instruction_font.DrawText(NULL,iFirstTurnInstruction,p,text_param,text_metrics);
    CartoType::TFixed text_width = text_metrics.iAdvance.iX;
    distance_and_time_font.DrawText(NULL,iRemainingDistanceText,p,text_param,text_metrics);
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

    instruction_font.DrawText(&aGc,iFirstTurnInstruction,p,text_param,text_metrics);
    p.iY += instruction_font_size + line_gap_size;
    distance_and_time_font.DrawText(&aGc,iRemainingDistanceText,p,text_param,text_metrics);
    }

static void DrawBitmapWithAlpha(HDC aDeviceContext,const CartoType::TBitmap* aBitmap,const CartoType::TRect& aDestRect,const CartoType::TRect& aSourceRect)
    {
    HDC hdc = CreateCompatibleDC(aDeviceContext);
    BITMAPINFOHEADER bitmap_header;
    memset(&bitmap_header,0,sizeof(bitmap_header));
    bitmap_header.biSize = sizeof(BITMAPINFOHEADER);
    bitmap_header.biWidth = aBitmap->Width();
    bitmap_header.biHeight = aBitmap->Height();
    bitmap_header.biPlanes = 1;
    bitmap_header.biBitCount = 32;
    bitmap_header.biCompression = BI_RGB;
    bitmap_header.biSizeImage = 0;
    bitmap_header.biXPelsPerMeter = 1000;
    bitmap_header.biYPelsPerMeter = 1000;
    bitmap_header.biClrUsed = 0;
    bitmap_header.biClrImportant = 0;

    void* bitmap_data = NULL;
    HBITMAP hbitmap = CreateDIBSection(hdc,(const BITMAPINFO*)&bitmap_header,DIB_RGB_COLORS,&bitmap_data,NULL,0);

    // Reflect the data vertically and change from ABGR to BGRA.
    unsigned char* dest_row = (unsigned char*)bitmap_data;
    const unsigned char* source_row = aBitmap->Data() + (aBitmap->Height() - 1) * aBitmap->RowBytes();
    for (int y = 0; y < aBitmap->Height(); y++,dest_row += aBitmap->RowBytes(),source_row -= aBitmap->RowBytes())
        {
        const unsigned char* p = source_row;
        unsigned char* q = dest_row;
        unsigned char* end = q + aBitmap->RowBytes();
        while (q < end)
            {
            *q++ = p[1];
            *q++ = p[2];
            *q++ = p[3];
            *q++ = p[0];
            p += 4;
            }
        }
    SelectObject(hdc,hbitmap);

    BLENDFUNCTION bf;
    bf.BlendOp = AC_SRC_OVER;
    bf.BlendFlags = 0;
    bf.SourceConstantAlpha = 0xFF;
    bf.AlphaFormat = AC_SRC_ALPHA;

    AlphaBlend(aDeviceContext,aDestRect.Left(),aDestRect.Top(),aDestRect.Width(),aDestRect.Height(),
               hdc,aSourceRect.Left(),aSourceRect.Top(),aSourceRect.Width(),aSourceRect.Height(),
               bf);

    DeleteObject(hbitmap);
    DeleteDC(hdc);
    }

void CCartoTypeDemoView::OnDraw(CDC* pDC)
    {
#ifdef SHOW_DRAW_TIME
    clock_t start_time = clock();
#endif

    RECT clip;
    pDC->GetClipBox(&clip);
    CartoType::TRect dest_clip(clip.left,clip.top,clip.right,clip.bottom);

    RECT mc;
    GetClientRect(&mc);
    CartoType::TRect map_client_area(mc.left,mc.top,mc.right,mc.bottom);

    CartoType::TRect source_rect = map_client_area;
    CartoType::TRect dest_rect = map_client_area;

    // Return now if there is nothing to be drawn.
    dest_clip.Intersection(dest_rect);
    if (dest_clip.IsEmpty())
        return;
    CartoType::TRect source_clip(dest_clip);
    source_clip.Intersection(source_rect);
    if (source_clip.IsEmpty())
        return;

    // Get the map bitmap. If the image server is in use this will be the in-memory db only.
    CartoType::TResult error = 0;
    bool redraw_needed = false;
    const CartoType::TBitmap* map_bitmap = MapBitmap(error,map_client_area,redraw_needed);

    // Draw using the image server if desired.
    if (iImageServerHelper)
        {
        iFramework->DrawUsingImageServer(*pDC);
        DrawBitmapWithAlpha(*pDC,map_bitmap,dest_clip,source_clip);
        return;
        }

    if (error)
        {
        CCartoTypeDemoApp* app = (CCartoTypeDemoApp*)AfxGetApp();

        char buffer[256];
        const char* error_string = CartoType::ErrorString(error);
        if (error_string && error_string[0])
            sprintf_s(buffer,"failed to draw map: %s",error_string);
        else
            sprintf_s(buffer,"failed to draw map: error %d",(int)error);
        ::CString str(buffer);
        CMainFrame* main_window = (CMainFrame*)app->m_pMainWnd;
        main_window->SetMessageText(str);
        return;
        }

    // Blit the drawing surface to the window.
    BitmapInfoX bm;
    bm.bmiHeader.bV4Size = sizeof(BITMAPV4HEADER);
    bm.bmiHeader.bV4Width = map_bitmap->Width();
    bm.bmiHeader.bV4Height = -map_bitmap->Height();
    bm.bmiHeader.bV4Planes = 1;
    bm.bmiHeader.bV4BitCount = 32;
    bm.bmiHeader.bV4V4Compression = BI_BITFIELDS;
    bm.bmiHeader.bV4SizeImage = 0;
    bm.bmiHeader.bV4XPelsPerMeter = 1000;
    bm.bmiHeader.bV4YPelsPerMeter = 1000;
    bm.bmiHeader.bV4ClrUsed = 0;
    bm.bmiHeader.bV4ClrImportant = 0;
    bm.bmiHeader.bV4RedMask = 0xFF000000;
    bm.bmiHeader.bV4GreenMask = 0xFF0000;
    bm.bmiHeader.bV4BlueMask = 0xFF00;
    bm.bmiHeader.bV4AlphaMask = 0xFF;
    const unsigned char* map_data = (const unsigned char*)(map_bitmap->Data());

    /*
    Blit the whole GC if the clip rectangle is at the bottom left of the GC.
    This gets round a bug in StretchDIBits (when dealing with 32bpp bitmaps) that causes a y coordinate of 0 to
    be treated as the top, not the bottom, of the source bitmap.
    */
    if (source_clip.Top() > 0 && source_clip.Left() == 0 && source_clip.Bottom() == map_bitmap->Height())
        ::StretchDIBits(*pDC,
        0,0,map_bitmap->Width(),map_bitmap->Height(),
        0,0,map_bitmap->Width(),map_bitmap->Height(),
        map_data,(BITMAPINFO*)&bm,DIB_RGB_COLORS,SRCCOPY);
    else
        ::StretchDIBits(*pDC,dest_clip.Left(),dest_clip.Top(),dest_clip.Width(),dest_clip.Height(),
        source_clip.Left(),map_bitmap->Height() - source_clip.Bottom(),source_clip.Width(),source_clip.Height(),
        map_data,(BITMAPINFO*)&bm,DIB_RGB_COLORS,SRCCOPY);

#ifdef SHOW_DRAW_TIME
    clock_t end_time = clock();
    double milliseconds = double(end_time - start_time) / CLOCKS_PER_SEC * 1000;
    CShape_demoApp* app = (CShape_demoApp*)AfxGetApp();
    CMainFrame* main_window = (CMainFrame*)app->m_pMainWnd;
    CStatusBar& status_bar = main_window->StatusBar();
    char buffer[128];
    if (redraw_needed)
        sprintf(buffer,"%d objects drawn in %dms",(int)iFramework->ObjectsDrawn(),(int)(milliseconds + 0.5));
    else
        sprintf(buffer,"bitmap drawn in %dms",(int)(milliseconds + 0.5));
    ::CString str = buffer;
    status_bar.SetPaneText(0,str);
#endif
    }

void CCartoTypeDemoView::CalculateAndDisplayRoute()
    {
    CartoType::TCoordSet cs;
    cs.iX = &iRoutePointArray[0].iPoint.iX;
    cs.iY = &iRoutePointArray[0].iPoint.iY;
    cs.iCount = int32_t(iRoutePointArray.size());
    cs.iStep = int32_t(&iRoutePointArray[1].iPoint.iX - cs.iX);

    CartoType::TResult error = 0;
    if (iBestRoute && cs.iCount > 3)
        {
        CartoType::TRouteProfile profile(iRouteProfileType);
        size_t iterations = cs.iCount * cs.iCount;
        if (iterations < 16)
            iterations = 16;
        else if (iterations > 256)
            iterations = 256;
        auto r = iFramework->CreateBestRoute(error,profile,cs,CartoType::EMapCoordType,true,true,iterations);
        if (!error)
            error = iFramework->UseRoute(*r,true);
        }
    else
        error = iFramework->StartNavigation(cs,CartoType::EMapCoordType);
    if (error)
        {
        CCartoTypeDemoApp* app = (CCartoTypeDemoApp*)AfxGetApp();
        app->ShowError(_TEXT("Routing error"),error);
        }
    else
        {
        iFramework->DisplayRoute(true);
        if (iFramework->Route())
            iRouteIter.reset(new CartoType::TRouteIterator(*iFramework->Route()));

        bool at_end = false;
        SimulateNavigationFix(0.1,at_end);
        }
    }

void CCartoTypeDemoView::OnRButtonUp(UINT nFlags, CPoint point)
    {
    CView::OnRButtonUp(nFlags,point);
    CartoType::TPointFP p(point.x,point.y);

    // Get the address of the point clicked on.
    CartoType::CAddress address;
    iFramework->GetAddress(address,point.x,point.y,CartoType::EScreenCoordType);
    CCartoTypeDemoApp* app = (CCartoTypeDemoApp*)AfxGetApp();
    CMainFrame* main_window = (CMainFrame*)app->m_pMainWnd;

    CartoType::CString address_string;
    iFramework->ConvertPoint(p.iX,p.iY,CartoType::EScreenCoordType,CartoType::EDegreeCoordType);
    address_string = address.ToString(false,&p);

    address_string.Append((CartoType::uint16)0);
    main_window->SetMessageText((LPCTSTR)address_string.Text());

    if (address_string.Length())
        address_string.Delete(address_string.Length() - 1,address_string.Length());
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
    CartoType::TResult error = iFramework->GetHeights(cs,CartoType::EDegreeCoordType,alt);
    if (!error)
        {
        char buffer[32];
        sprintf_s(buffer," (altitude = %dm)",alt[0]);
        address_string.Append(buffer);
        }

    // If possible, get the Ordnance Survey National Grid reference.
    CartoType::CString os_grid_ref = CartoType::UKGridReferenceFromDegrees(p,3);
    if (os_grid_ref.Length())
        {
        address_string.Append("\r\n\r\nOrdnance Survey of Great Britain grid reference: ");
        address_string.Append(os_grid_ref);
        }

    // Get information about the objects clicked on - within 2mm.
    CartoType::CMapObjectArray object;
    double pixel_mm = iFramework->ResolutionDpi() / 25.4;
    iFramework->FindInDisplay(object,100,point.x,point.y,int(ceil(2 * pixel_mm)));
    CartoType::CMapObject* pushpin = nullptr;

    // See if we have a pushpin, and geocode the objects so that they can be sorted.
    for (auto& o : object)
        {
        if (o->LayerName() == "pushpin" && o->Type() == CartoType::EPointObject)
            pushpin = o.get();
        o->SetUserData((int)o->GetGeoCodeType());
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

    // Display the route dialog.
    CRouteDialog route_dialog(pushpin != NULL);
    if (pushpin)
        route_dialog.iButtonIndex = 2;
    else if (iRoutePointArray[0].iPoint == CartoType::TPointFP())
        route_dialog.iButtonIndex = 0;
    else
        route_dialog.iButtonIndex = 1;
    route_dialog.iBestRoute = iBestRoute;

    SetString(route_dialog.iAddress,address_string);

    CartoType::uint64 pushpin_id = 0;
    if (pushpin)
        {
        CartoType::TText value = pushpin->GetStringAttribute(CartoType::TText());
        iPushPinName.Set(value);
        value = pushpin->GetStringAttribute("desc");
        iPushPinDesc.Set(value);
        pushpin_id = pushpin->Id();
        }
    else
        {
        iPushPinName.Set(address.ToString(false));
        iPushPinDesc.Set(address.ToString(true));
        }
    SetString(route_dialog.iPushPinName,iPushPinName);
    SetString(route_dialog.iPushPinDesc,iPushPinDesc);

    if (route_dialog.DoModal() != IDOK)
        return;

    // Stop any route fly-through.
    StopFlyThrough();

    bool create_route = false;
    iBestRoute = route_dialog.iBestRoute != 0;
    if (route_dialog.iButtonIndex == 0) // set start of route
        {
        CartoType::TPointFP p(point.x,point.y);
        iFramework->ConvertPoint(p.iX,p.iY,CartoType::EScreenCoordType,CartoType::EMapCoordType);
        iRoutePointArray[0].iPoint = p;
        create_route = true;
        }
    else if (route_dialog.iButtonIndex == 1) // set end of route
        {
        CartoType::TPointFP p(point.x,point.y);
        iFramework->ConvertPoint(p.iX,p.iY,CartoType::EScreenCoordType,CartoType::EMapCoordType);
        iRoutePointArray[iRoutePointArray.size() - 1].iPoint = p;
        create_route = true;
        }
    else if (route_dialog.iButtonIndex == 2) // add or change a pushpin
        {
        SetString(iPushPinName,route_dialog.iPushPinName);
        SetString(iPushPinDesc,route_dialog.iPushPinDesc);
        CartoType::CString string_attrib;
        string_attrib.Set(iPushPinName);
        if (iPushPinDesc.Length())
            {
            string_attrib.Append((CartoType::uint16)0);
            string_attrib.Append("desc=");
            string_attrib.Append(iPushPinDesc);
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
            x = point.x;
            y = point.y;
            iFramework->ConvertPoint(x,y,CartoType::EScreenCoordType,CartoType::EMapCoordType);
            }
        CartoType::TTextLiteral(layer,u"pushpin");
        iFramework->InsertPointMapObject(iWritableMapHandle,layer,x,y,CartoType::EMapCoordType,string_attrib,0,pushpin_id,true);

        if (route_dialog.iUseAsWayPoint)
            {
            TRoutePoint* waypoint = nullptr;

            for (int i = 1; waypoint == nullptr && i < iRoutePointArray.size() - 1; i++)
                if (iRoutePointArray[i].iId == pushpin_id)
                    waypoint = &iRoutePointArray[i];
            if (waypoint == nullptr)
                {
                TRoutePoint rp;
                iRoutePointArray.insert(iRoutePointArray.begin() + iRoutePointArray.size() - 1,rp);
                waypoint = &iRoutePointArray[iRoutePointArray.size() - 2];
                }
            if (waypoint)
                {
                waypoint->iPoint.iX = x;
                waypoint->iPoint.iY = y;
                waypoint->iId = pushpin_id;
                create_route = true;
                }
            }   

        iWritableMapChanged = true;
        }
    else if (route_dialog.iButtonIndex == 3) // cut a pushpin
        {
        uint64_t count = 0;
        iFramework->DeleteMapObjects(iWritableMapHandle,pushpin_id,pushpin_id,count);
        for (int i = 1; i < iRoutePointArray.size() - 1; i++)
            if (iRoutePointArray[i].iId == pushpin_id)
                {
                iRoutePointArray.erase(iRoutePointArray.begin() + i);
                create_route = true;
                break;
                }
        iWritableMapChanged = true;
        }

    if (iRoutePointArray[0].iPoint == CartoType::TPointFP() ||
        iRoutePointArray[iRoutePointArray.size() - 1].iPoint == CartoType::TPointFP())
        create_route = false;

    if (create_route)
        CalculateAndDisplayRoute();

    Update();
    }

void CCartoTypeDemoView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
    theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CCartoTypeDemoView diagnostics

#ifdef _DEBUG
void CCartoTypeDemoView::AssertValid() const
{
    CView::AssertValid();
}

void CCartoTypeDemoView::Dump(CDumpContext& dc) const
{
    CView::Dump(dc);
}

CCartoTypeDemoDoc* CCartoTypeDemoView::GetDocument() const // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCartoTypeDemoDoc)));
    return (CCartoTypeDemoDoc*)m_pDocument;
}
#endif //_DEBUG

static void AppendDistance(CartoType::CString& aDistance,double aMetres,bool aMetricUnits)
    {
    char text[64];

    if (aMetricUnits)
        {
        if (aMetres < 50)
            sprintf_s(text,"%d meters",CartoType::Arithmetic::Round(aMetres));
        else if (aMetres < 250)
            sprintf_s(text,"%d meters",CartoType::Arithmetic::Round(aMetres / 10) * 10);
        else if (aMetres < 100000)
            sprintf_s(text,"%.1fkm",aMetres / 1000);
        else
            sprintf_s(text,"%.fkm",aMetres / 1000);
        }
    else
        {
        double yards = aMetres * 1.093613298;
        double miles = yards / 1760;
        if (yards < 55)
            sprintf_s(text,"%d yards",CartoType::Arithmetic::Round(yards));
        else if (yards < 220)
            sprintf_s(text,"%d yards",CartoType::Arithmetic::Round(yards / 10) * 10);
        else if (miles < 100)
            sprintf_s(text,"%.1f miles",miles);
        else
            sprintf_s(text,"%.f miles",miles);
        }

    aDistance.Append(text);
    }

static void AppendTime(CartoType::CString& aTime,double aSeconds)
    {
    char text[64];

    int hours = (int)aSeconds;
    int seconds = hours % 60;
    hours /= 60;
    int minutes = hours % 60;
    hours /= 60;

    int n = 0;
    if (hours > 1)
        n = sprintf_s(text,"%d hours",hours);
    else if (hours == 1)
        n = sprintf_s(text,"1 hour");
    if (hours > 0)
        n += sprintf_s(text + n,sizeof(text) - n," ");
    if (hours > 0 || minutes > 0)
        {
        if (minutes == 1)
            n += sprintf_s(text + n,sizeof(text) - n,"1 minute");
        else
            n += sprintf_s(text + n,sizeof(text) - n,"%d minutes",minutes);
        }
    if (hours == 0 && minutes == 0)
        {
        if (seconds == 1)
            sprintf_s(text,"1 second");
        else
            sprintf_s(text,"%d seconds",seconds);
        }
    aTime.Append(text);
    }

void CCartoTypeDemoView::CreateTurnInstructions()
    {
    iFirstTurnInstruction.Clear();
    iSecondTurnInstruction.Clear();
    iRemainingDistanceText.Clear();

    switch (iFramework->NavigationState())
        {
        case CartoType::ENoNavigationState:
            iFirstTurnInstruction.Set("not navigating");
            break;

        case CartoType::ETurnNavigationState:
        case CartoType::EArrivalNavigationState:
            {
            const CartoType::TNavigatorTurn& first_turn = iFramework->FirstTurn();
            const CartoType::TNavigatorTurn& second_turn = iFramework->SecondTurn();

            iFirstTurnInstruction = first_turn.iInstructions;
            if (second_turn.iTurnType != CartoType::ETurnNone)
                iSecondTurnInstruction = second_turn.iInstructions;

            AppendDistance(iRemainingDistanceText,iFramework->DistanceToDestination(),iMetricUnits);
            iRemainingDistanceText.Append(" of trip remaining; ");
            AppendTime(iRemainingDistanceText,iFramework->EstimatedTimeToDestination());
            CartoType::CBidiEngine bidi;
            iFirstTurnInstruction.Shape(CartoType::ELeftToRightForced,&bidi,false,false);
            iSecondTurnInstruction.Shape(CartoType::ELeftToRightForced,&bidi,false,false);
            }
            break;

        case CartoType::ETurnRoundNavigationState:
            iFirstTurnInstruction.Set("turn around as soon as possible");
            AppendDistance(iRemainingDistanceText,iFramework->DistanceToDestination(),iMetricUnits);
            iRemainingDistanceText.Append(" of trip remaining");
            break;

        case CartoType::ENewRouteNavigationState:
            iFirstTurnInstruction.Set("calculating a new route...");
            iFramework->DisplayRoute(true);
            iRouteIter = NULL;
            if (iFramework->Route())
                iRouteIter.reset(new CartoType::TRouteIterator(*iFramework->Route()));
            break;

        case CartoType::EOffRouteNavigationState:
            break;

        }
    }

void CCartoTypeDemoView::OnTurn(const CartoType::TNavigatorTurn& /*aFirstTurn*/,
                                const CartoType::TNavigatorTurn* /*aSecondTurn*/,
                                const CartoType::TNavigatorTurn* /*aContinuationTurn*/)
    {
    CreateTurnInstructions();
    Update();
    }

void CCartoTypeDemoView::OnTurnRound()
    {
    CreateTurnInstructions();
    Update();
    }

void CCartoTypeDemoView::OnNewRoute()
    {
    CreateTurnInstructions();
    Update();
    }

void CCartoTypeDemoView::OnPositionKnown()
    {
    CCartoTypeDemoApp* app = (CCartoTypeDemoApp*)AfxGetApp();
    ::CString str("position now known");
    CMainFrame* main_window = (CMainFrame*)app->m_pMainWnd;
    main_window->SetMessageText(str);
    }

void CCartoTypeDemoView::OnPositionUnknown()
    {
    CCartoTypeDemoApp* app = (CCartoTypeDemoApp*)AfxGetApp();
    ::CString str("position now unknown");
    CMainFrame* main_window = (CMainFrame*)app->m_pMainWnd;
    main_window->SetMessageText(str);

    }

void CCartoTypeDemoView::OnAddNearbyObject(const CartoType::CMapObject* aObject,double /*aDistanceToRoute*/,double /*aDistanceAlongRoute*/)
    {
    if (aObject->Label().Length())
        {
        ::CString str;
        SetString(str,aObject->Label());
        str += " now nearby";
        CCartoTypeDemoApp* app = (CCartoTypeDemoApp*)AfxGetApp();
        CMainFrame* main_window = (CMainFrame*)app->m_pMainWnd;
        main_window->SetMessageText(str);
        }
    }

void CCartoTypeDemoView::OnUpdateNearbyObject(const CartoType::CMapObject* /*aObject*/,double /*aDistanceToRoute*/,double /*aDistanceAlongRoute*/)
    {
    //+ to do
    }

void CCartoTypeDemoView::OnRemoveNearbyObject(const CartoType::CMapObject* aObject)
    {
    if (aObject->Label().Length())
        {
        ::CString str;
        SetString(str,aObject->Label());
        str += " no longer nearby";
        CCartoTypeDemoApp* app = (CCartoTypeDemoApp*)AfxGetApp();
        CMainFrame* main_window = (CMainFrame*)app->m_pMainWnd;
        main_window->SetMessageText(str);
        }
    }

void CCartoTypeDemoView::PanToDraggedPosition()
    {
    CartoType::TPoint to(iMapDragAnchor);
    to += iMapDragOffset;
    CartoType::TResult error = iFramework->Pan(iMapDragAnchor,to);

    if (error)
        ((CCartoTypeDemoApp*)AfxGetApp())->ShowError(_TEXT("Error panning to dragged position"),error);
    Update();
    }

void CCartoTypeDemoView::StopDragging()
    {
    iMapDragEnabled = false;
    PanToDraggedPosition();
    iMapDragOffset = CartoType::TPoint(0,0);
    TRACKMOUSEEVENT t;
    t.cbSize = sizeof(t);
    t.dwFlags = TME_LEAVE | TME_CANCEL;
    t.dwHoverTime = 0;
    t.hwndTrack = *this;
    TrackMouseEvent(&t);
    Update();
    }

void CCartoTypeDemoView::OnDestroy()
    {
    if (iWritableMapChanged &&
        AfxMessageBox(_TEXT("You have unsaved editable data (pushpins, etc). Save it now?"),MB_YESNO) == IDYES)
        OnEditSaveCtms();
    CView::OnDestroy();
    }

CartoType::TResult CCartoTypeDemoView::LoadStyleSheet(const ::CString& aPath)
    {
    CartoType::CString path;
    SetString(path,aPath);
    CartoType::TResult error = iFramework->SetStyleSheet(path);
    if (error)
        {
        CCartoTypeDemoApp* app = (CCartoTypeDemoApp*)AfxGetApp();
        app->ShowError(_TEXT("error loading stylesheet"),error);
        }
    else
        Update();
    iLegend.reset();
    return error;
    }

/** Simulate a navigation fix after moving along the route by aDriveDistance metres, or to the next turn if aDriveDistance is zero. */
void CCartoTypeDemoView::SimulateNavigationFix(double aDriveDistance,bool& aAtEnd)
    {
    if (!iRouteIter)
        {
        aAtEnd = true;
        return;
        }

    CartoType::TPoint p = iRouteIter->Position();
    if (aDriveDistance > 0)
        {
        iOnRouteTime += aDriveDistance / 13.888; // assume a speed of ~50kph = 13.888 metres per second.

        // Adjust the drive distance to the map unit size.
        aDriveDistance /= iFramework->MapUnitSize();

        // Move along the route by aDriveDistance.
        aAtEnd = !iRouteIter->Forward(aDriveDistance);
        p = iRouteIter->Position();
        }
    else
        {
        // Move along the route to the next turn.
        double distance = iFramework->FirstTurn().iDistance;
        if (distance >= 10 || iFramework->NavigationState() == CartoType::EArrivalNavigationState)
            p = iFramework->FirstTurn().iPosition;
        else if (iFramework->SecondTurn().iDistance > 0)
            {
            distance += iFramework->SecondTurn().iDistance;
            p = iFramework->SecondTurn().iPosition;
            }
        else
            {
            const CartoType::CRouteSegment* segment = iFramework->NextRouteSegment();
            if (segment)
                {
                const CartoType::TPoint* end = segment->iPath.End();
                if (end)
                    p = *end;
                }
            }
        iOnRouteTime += distance / 13.888;
        }

    // Simulate passing some GPS data to the navigator.
    CartoType::TNavigationData nav;
    nav.iValidity = CartoType::TNavigationData::EPositionValid;
    nav.iPosition.iX = p.iX;
    nav.iPosition.iY = p.iY;
    nav.iTime = iOnRouteTime;
    iFramework->ConvertPoint(nav.iPosition.iX,nav.iPosition.iY,CartoType::EMapCoordType,CartoType::EDegreeCoordType);
    iFramework->Navigate(nav);
    }

// CCartoTypeDemoView message handlers

void CCartoTypeDemoView::OnInitialUpdate()
    {
    __super::OnInitialUpdate();

    EnableScrollBarCtrl(SB_BOTH,false);
    CartoType::TResult error = CartoType::KErrorNone;

    CCartoTypeDemoApp* app = (CCartoTypeDemoApp*)AfxGetApp();
    ASSERT_VALID(app);
    CCartoTypeDemoDoc* doc = GetDocument();
    ASSERT_VALID(doc);


    CartoType::CFramework::TParam param;
    param.iSharedEngine = app->Engine();
    param.iSharedMapDataSet = doc->MapDataSet();
    param.iStyleSheetFileName = app->DefaultStyleSheetName();
    RECT rect;
    GetClientRect(&rect);
    int w = rect.right - rect.left;
    int h = rect.bottom - rect.top;
    param.iViewHeight = h;
    param.iViewWidth = w;
    param.iTextIndexLevels = 1;

    iFramework = CartoType::CFramework::New(error,param);
    if (!error)
        {
        error = iFramework->CreateWritableMap(CartoType::EMemoryWritableMapType);
        iWritableMapHandle = iFramework->GetLastMapHandle();
        }
    if (error)
        {
        app->ShowError(_TEXT("error creating CartoType framework"),error);
        return;
        }

    // Tell the framework to send us navigation messages.
    iFramework->AddNavigatorObserver(this);

    // Check the extent.
    double min_long,min_lat,max_long,max_lat;
    error = iFramework->GetMapExtent(min_long,min_lat,max_long,max_lat,CartoType::EDegreeCoordType);
    if (!error)
        {
        char buffer[256];
        sprintf_s(buffer,"map database extent: %g%s %g%s ... %g%s %g%s\n",
                  min_long < 0 ? -min_long : min_long,min_long < 0 ? "W" : "E",
                  min_lat < 0 ? -min_lat : min_lat,min_lat < 0 ? "S" : "N",
                  max_long < 0 ? -max_long : max_long,max_long < 0 ? "W" : "E",
                  max_lat < 0 ? -max_lat : max_lat,max_lat < 0 ? "S" : "N");
        OutputDebugStringA(buffer);
        }

    iFramework->SetFollowMode(CartoType::EFollowModeNone);
    iFramework->SetLocale(iMetricUnits ? "en_xx" : "en");

    if (iUsingImageServer)
        {
        iImageServerHelper.reset(new CMyImageServerHelper);
        iFramework->UseImageServer(iImageServerHelper.get());
        }
 
    // Do not set the pixel size here; use the default value of 144dpi.
    // Add a call to iFramework->SetResolutionDpi to set the pixel size.

    TestCode();
    }

void CCartoTypeDemoView::TestCode()
    {
    }

void CCartoTypeDemoView::OnSize(UINT nType,int cx,int cy)
    {
    __super::OnSize(nType,cx,cy);
    if (iFramework)
        {
        iFramework->Resize(cx,cy);
        Update();
        }
    }

BOOL CCartoTypeDemoView::OnEraseBkgnd(CDC* /*pDC*/)
    {
    // Suppress erasure of the background.
    return TRUE;
    }

BOOL CCartoTypeDemoView::OnMouseWheel(UINT nFlags,short zDelta,CPoint point)
    {
    if (zDelta == 0)
        return TRUE;

    CCartoTypeDemoApp* app = (CCartoTypeDemoApp*)AfxGetApp();

    // If the mouse pointer is in the window, pan the map so that the point under the mouse pointer stays where it is.
    RECT r;
    GetClientRect(&r);
    bool set_centre = false;
    CartoType::TPoint p;
    ScreenToClient(&point);
    if (point.x >= r.left && point.x < r.right && point.y >= r.top && point.y <= r.bottom)
        {
        point.x -= r.left;
        point.y -= r.top;
        p = CartoType::TPoint(point.x,point.y);
        iFramework->ConvertPoint(p,CartoType::EScreenCoordType,CartoType::EMapCoordType);
        set_centre = true;
        }

    /*
    If using the image server, always zoom by a factor of 2 because
    the image server system snaps to zoom levels arranged in factors
    of 2, and zooming less will often have no effect because it
    will snap back to the previous level.
    */
    double zoom = iUsingImageServer ? 2 : sqrt(app->ZoomFactor());

    int zoom_count = zDelta / WHEEL_DELTA;
    if (zoom_count == 0)
        zoom_count = zDelta >= 0 ? 1 : -1;

    zoom = pow(zoom,zoom_count);
    iFramework->Zoom(zoom);

    if (set_centre)
        iFramework->Pan(p.iX,p.iY,CartoType::EMapCoordType,point.x,point.y,CartoType::EScreenCoordType);

    Update();
    return TRUE;
    }


void CCartoTypeDemoView::OnMouseMove(UINT /*nFlags*/,CPoint point)
    {
    if (iMapDragEnabled)
        {
        iMapDragOffset.iX = point.x - iMapDragAnchor.iX;
        iMapDragOffset.iY = point.y - iMapDragAnchor.iY;

        if (iUsingImageServer)
            {
            iFramework->Pan(-iMapDragOffset.iX,-iMapDragOffset.iY);
            iMapDragOffset = CartoType::TPoint(0,0);
            iMapDragAnchor.iX = point.x;
            iMapDragAnchor.iY = point.y;
            }

        CDC* dc = GetDC();
        if (dc)
            OnDraw(dc);
        }
    }

void CCartoTypeDemoView::OnMouseLeave()
    {
    StopDragging();
    }

void CCartoTypeDemoView::OnLButtonDown(UINT nFlags,CPoint point)
    {
    iMapDragEnabled = true;
    iMapDragAnchor.iX = point.x;
    iMapDragAnchor.iY = point.y;
    TRACKMOUSEEVENT t;
    t.cbSize = sizeof(t);
    t.dwFlags = TME_LEAVE;
    t.dwHoverTime = 0;
    t.hwndTrack = *this;
    TrackMouseEvent(&t);
    }

void CCartoTypeDemoView::OnLButtonUp(UINT nFlags,CPoint point)
    {
    iMapDragOffset.iX = point.x - iMapDragAnchor.iX;
    iMapDragOffset.iY = point.y - iMapDragAnchor.iY;

    if (iLeftClickSimulatesNavigationFix)
        {
        // Supply a simulated navigation fix.	
        if (iMapDragOffset.iX == 0 && iMapDragOffset.iY == 0)
            {
            CartoType::TNavigationData nav;
            nav.iValidity = CartoType::TNavigationData::EPositionValid | CartoType::TNavigationData::ETimeValid;
            nav.iPosition.iX = point.x;
            nav.iPosition.iY = point.y;
            iOnRouteTime += 20;
            nav.iTime = iOnRouteTime;
            iFramework->ConvertPoint(nav.iPosition.iX,nav.iPosition.iY,CartoType::EScreenCoordType,CartoType::EDegreeCoordType);
            iFramework->Navigate(nav);
            }
        }

    StopDragging();
    }

void CCartoTypeDemoView::OnFileOpenInCurrentMap()
    {
    ::CString filename;
    CartoType::TResult error = CartoType::KErrorGeneral;
    if (AfxGetApp()->DoPromptFileName(filename,AFX_IDS_OPENFILE,OFN_HIDEREADONLY | OFN_FILEMUSTEXIST,true,NULL))
        {
        CartoType::CString f;
        SetString(f,filename);
        if (f.Last(10) == ".ctm1_ctci")
            {
            std::string key("password");
            error = iFramework->LoadMap(f,&key);
            }
        else
            error = iFramework->LoadMap(f);
        if (!error)
            iMapHandle.push_back(iFramework->GetLastMapHandle());
        }
    if (error)
        {
        CCartoTypeDemoApp* app = (CCartoTypeDemoApp*)AfxGetApp();
        app->ShowError(_TEXT("failed to load map"),error);
        }
    if (error == CartoType::KErrorNone)
        Update();
    }

void CCartoTypeDemoView::OnFileClose()
    {
    size_t index = iMapHandle.size();
    if (index)
        {
        index--;
        iFramework->UnloadMapByHandle(iMapHandle[index]);
        iMapHandle.erase(iMapHandle.begin() + index);
        Update();
        }
    }

void CCartoTypeDemoView::OnUpdateFileClose(CCmdUI *pCmdUI)
    {
    pCmdUI->Enable(iMapHandle.size() > 0);
    }

void CCartoTypeDemoView::OnFileSaveAsPng()
    {
    CFileDialog file_dialog(false,_TEXT(".png"),NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,TEXT("PNG files (*.png)|*.png||"));
    if (file_dialog.DoModal() == IDOK)
        {
        CString p = file_dialog.GetPathName();
        CartoType::CString path;
        SetString(path,p);
        CartoType::TResult error = 0;
        std::unique_ptr<CartoType::CFileOutputStream> output_stream { CartoType::CFileOutputStream::New(error,path) };
        if (output_stream)
            {
            const CartoType::TBitmap* bitmap = iFramework->MapBitmap(error);
            if (!error)
                error = bitmap->WritePng(*output_stream,false);
            }
        if (error)
            {
            CCartoTypeDemoApp* app = (CCartoTypeDemoApp*)AfxGetApp();
            app->ShowError(_TEXT("failed to save as PNG"),error);
            }
        }
    }

void CCartoTypeDemoView::OnFileSaveAsCtsql()
    {
    CFileDialog file_dialog(false,_TEXT(".ctsql"),NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,TEXT("CartoType SQLite database files (*.ctsql)|*.ctsql||"));
    if (file_dialog.DoModal() == IDOK)
        {
        CString p = file_dialog.GetPathName();
        CartoType::CString path;
        SetString(path,p);
        CartoType::TResult error = iFramework->SaveMap(iFramework->GetMainMapHandle(),path,CartoType::ECtsqlFileType);
        if (error)
            {
            CCartoTypeDemoApp* app = (CCartoTypeDemoApp*)AfxGetApp();
            app->ShowError(_TEXT("failed to save data to a CTSQL file"),error);
            }
        }
    }

void CCartoTypeDemoView::ShowNextFoundObject()
    {
    const auto& object = iFoundObject[iFoundObjectIndex];

    CartoType::CString summary;
    iFramework->GeoCodeSummary(summary,*object);
    CartoType::TResult error = 0;
    double length = iFramework->GetMapObjectLengthOrPerimeter(error,*object);
    double area = 0;
    if (!error)
        area = iFramework->GetMapObjectArea(error,*object);
    if (!error)
        {
        char buffer[64];
        if (area != 0)
            {
            if (iMetricUnits)
                {
                if (area > 1000000)
                    sprintf_s(buffer,"; area = %.2f sq km",area / 1000000);
                else
                    sprintf_s(buffer,"; area = %.2f ha",area / 10000);
                }
            else
                {
                // Convert to acres.
                double acres = area / 4046.856422;
                if (acres > 640)
                    sprintf_s(buffer,"; area = %.2f sq mi",acres / 640);
                else
                    sprintf_s(buffer,"; area = %.2f acres",acres);
                }
            summary.Append(buffer);
            }
        else if (length != 0)
            {
            if (iMetricUnits)
                {
                if (length > 1000)
                    sprintf_s(buffer,"; length = %.2f km",length / 1000);
                else
                    sprintf_s(buffer,"; length = %.2f m",length);
                }
            else
                {
                // Convert to yards.
                double yards = length / 0.9144;
                if (yards > 1760)
                    sprintf_s(buffer,"; length = %.2f mi",yards / 1760);
                else
                    sprintf_s(buffer,"; length = %.2f yards",yards);
                }
            summary.Append(buffer);
            }
        }

    CCartoTypeDemoApp* app = (CCartoTypeDemoApp*)AfxGetApp();
    CMainFrame* main_window = (CMainFrame*)app->m_pMainWnd;
    summary.Append((CartoType::uint16)0);
    main_window->SetMessageText((LPCTSTR)summary.Text());

    CartoType::uint32 memory_map_handle = iFramework->GetMemoryMapHandle();
    CartoType::TTextLiteral(found_layer,u"found");
    if (!error)
        error = iFramework->InsertCopyOfMapObject(memory_map_handle, found_layer, *object,
                                                  object->Type() == CartoType::EPointObject ? 50 : 0, CartoType::EMapMeterCoordType, iFoundObjectId, true);

    if (!error)
        iFramework->SetView(*object,64,4000);
    if (error)
        app->ShowError(_TEXT("Error displaying found object"),error);

    iFoundObjectIndex = (iFoundObjectIndex + 1) % iFoundObject.size();
    Update();
    }

void CCartoTypeDemoView::OnFind()
    {
    CFindTextDialog find_dialog(*iFramework,iFindText,iFindTextPrefix,iFindTextFuzzy);

    bool stop = false;
    iFoundObjectIndex = 0;
    while (!stop)
        {
        if (find_dialog.DoModal() == IDOK)
            {
            find_dialog.iFindText.TrimLeft();
            SetString(iFindText,find_dialog.iFindText);
            iFindTextPrefix = find_dialog.iPrefix != 0;
            iFindTextFuzzy = find_dialog.iFuzzy != 0;
            iFoundObject.clear();
            auto selected_object = find_dialog.SelectedObject();
            if (selected_object)
                iFoundObject.push_back(std::move(selected_object));
            else
                {
                CWaitCursor cursor;
                CartoType::TStringMatchMethod string_match_method = CartoType::TStringMatchMethod(CartoType::EStringMatchFoldCaseFlag | CartoType::EStringMatchFoldAccentsFlag | CartoType::EStringMatchIgnoreNonAlphanumericsFlag);
                if (find_dialog.iPrefix)
                    string_match_method = CartoType::TStringMatchMethod(string_match_method | CartoType::EStringMatchPrefixFlag);
                if (find_dialog.iFuzzy)
                    string_match_method = CartoType::TStringMatchMethod(string_match_method | CartoType::EStringMatchFuzzyFlag);

                CartoType::TResult error = iFramework->FindText(iFoundObject,30000,iFindText,string_match_method,"","");
                }

            if (iFoundObject.size() == 0)
                {
                ::CString msg("Text ");
                msg += find_dialog.iFindText;
                msg += " not found";
                AfxMessageBox(msg);
                }
            else
                {
                uint64_t count = 0;
                iFramework->DeleteMapObjects(0,iFoundObjectId,iFoundObjectId,count);
                iFoundObjectId = 0;
                ShowNextFoundObject();

                // Insert pushpins for found objects.
                iFramework->DeleteMapObjects(0,0,UINT64_MAX,count,"@layer=='pushpin' and Type==3");
                CartoType::CString layer("pushpin");
                for (const auto& p : iFoundObject)
                    {
                    auto centre = p->CenterOfGravity();
                    uint64_t id = 0;
                    iFramework->InsertPointMapObject(0,layer,centre.iX,centre.iY,CartoType::EMapCoordType,"",3,id,false);
                    }

                stop = true;
                }
            }
        else
            stop = true;
        }
    }

void CCartoTypeDemoView::OnUpdateFind(CCmdUI *pCmdUI)
    {
    pCmdUI->Enable();
    }

void CCartoTypeDemoView::OnFindAddress()
    {
    CFindAddressDialog find_dialog;
    SetString(find_dialog.iBuilding,iAddress.iBuilding);
    SetString(find_dialog.iFeature,iAddress.iFeature);
    SetString(find_dialog.iStreet,iAddress.iStreet);
    SetString(find_dialog.iSubLocality,iAddress.iSubLocality);
    SetString(find_dialog.iLocality,iAddress.iLocality);
    SetString(find_dialog.iSubAdminArea,iAddress.iSubAdminArea);
    SetString(find_dialog.iAdminArea,iAddress.iAdminArea);
    SetString(find_dialog.iCountry,iAddress.iCountry);
    SetString(find_dialog.iPostCode,iAddress.iPostCode);

    bool stop = false;

    iFoundObjectIndex = 0;
    while (!stop)
        {
        if (find_dialog.DoModal() == IDOK)
            {
            CWaitCursor cursor;

            iFoundObject.clear();
            CartoType::TResult error = 0;
            SetString(iAddress.iBuilding,find_dialog.iBuilding);
            SetString(iAddress.iFeature,find_dialog.iFeature);
            SetString(iAddress.iStreet,find_dialog.iStreet);
            SetString(iAddress.iSubLocality,find_dialog.iSubLocality);
            SetString(iAddress.iLocality,find_dialog.iLocality);
            SetString(iAddress.iSubAdminArea,find_dialog.iSubAdminArea);
            SetString(iAddress.iAdminArea,find_dialog.iAdminArea);
            SetString(iAddress.iCountry,find_dialog.iCountry);
            SetString(iAddress.iPostCode,find_dialog.iPostCode);
            error = iFramework->FindAddress(iFoundObject,100,iAddress,true);

            if (iFoundObject.size() == 0)
                {
                ::CString msg("address not found");
                AfxMessageBox(msg);
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

void CCartoTypeDemoView::OnUpdateFindAddress(CCmdUI *pCmdUI)
    {
    pCmdUI->Enable();
    }

void CCartoTypeDemoView::OnFindNext()
    {
    ShowNextFoundObject();
    }

void CCartoTypeDemoView::OnUpdateFindNext(CCmdUI *pCmdUI)
    {
    pCmdUI->Enable(iFoundObject.size() > 1);
    }

void CCartoTypeDemoView::OnViewReloadStyleSheet()
    {
    iFramework->ReloadStyleSheet();
    iLegend.reset();
    Update();
    }

void CCartoTypeDemoView::OnUpdateViewReloadStyleSheet(CCmdUI *pCmdUI)
    {
    pCmdUI->Enable();
    }

void CCartoTypeDemoView::OnViewZoomIn()
    {
    CCartoTypeDemoApp* app = (CCartoTypeDemoApp*)AfxGetApp();
    iFramework->Zoom(app->ZoomFactor());
    Update();
    }

void CCartoTypeDemoView::OnUpdateViewZoomIn(CCmdUI *pCmdUI)
    {
    pCmdUI->Enable();
    }

void CCartoTypeDemoView::OnViewZoomOut()
    {
    CCartoTypeDemoApp* app = (CCartoTypeDemoApp*)AfxGetApp();
    iFramework->Zoom(1 / app->ZoomFactor());
    Update();
    }

void CCartoTypeDemoView::OnUpdateViewZoomOut(CCmdUI *pCmdUI)
    {
    pCmdUI->Enable();
    }

void CCartoTypeDemoView::OnViewRotate()
    {
    iFramework->Rotate(15);
    Update();
    }

void CCartoTypeDemoView::OnUpdateViewRotate(CCmdUI *pCmdUI)
    {
    pCmdUI->Enable(!iUsingImageServer);
    }

void CCartoTypeDemoView::OnViewNorthup()
    {
    iFramework->SetRotation(0);
    Update();
    }

void CCartoTypeDemoView::OnUpdateViewNorthup(CCmdUI *pCmdUI)
    {
    pCmdUI->Enable(iFramework->Rotation() != 0);
    }

void CCartoTypeDemoView::OnViewLatlong()
    {
    CLatLongDialog lat_long_dialog;

    double minx,miny,maxx,maxy;
    iFramework->GetView(minx,miny,maxx,maxy,CartoType::EDegreeCoordType);
    double cur_long = (minx + maxx) / 2;
    double cur_lat = (miny + maxy) / 2;
    iFramework->GetMapExtent(minx,miny,maxx,maxy,CartoType::EDegreeCoordType);

    char buffer[32];
    sprintf_s(buffer,"%.4f",cur_long);
    SetString(lat_long_dialog.iLongitude,buffer);
    sprintf_s(buffer,"%.4f",cur_lat);
    SetString(lat_long_dialog.iLatitude,buffer);

    if (lat_long_dialog.DoModal() == IDOK)
        {
        CartoType::TTextBuffer<32> lo;
        SetString(lo,lat_long_dialog.iLongitude);
        CartoType::TTextBuffer<32> la;
        SetString(la,lat_long_dialog.iLatitude);
        size_t dummy;
        double x,y;
        lo.ToDouble(x,dummy);
        la.ToDouble(y,dummy);

        if (x < minx)
            x = minx;
        else if (x > maxx)
            x = maxx;

        if (y < miny)
            y = miny;
        else if (y > maxy)
            y = maxy;

        CartoType::TResult error = iFramework->SetViewCenter(x,y,CartoType::EDegreeCoordType);
        if (error)
            {
            CCartoTypeDemoApp* app = (CCartoTypeDemoApp*)AfxGetApp();
            app->ShowError(_TEXT("Set center error"),error);
            }
        Update();
        }
    }

void CCartoTypeDemoView::OnUpdateViewLatlong(CCmdUI *pCmdUI)
    {
    pCmdUI->Enable();
    }

void CCartoTypeDemoView::OnViewStylesheet()
    {
    CFileDialog file_dialog(true,_TEXT(".xml"),NULL,0,TEXT("style sheets (*.xml)|*.xml||"));
    if (file_dialog.DoModal() == IDOK)
        LoadStyleSheet(file_dialog.GetPathName());
    }

void CCartoTypeDemoView::OnUpdateViewStylesheet(CCmdUI *pCmdUI)
    {
    pCmdUI->Enable();
    }

void CCartoTypeDemoView::OnViewLayers()
    {
    CLayerDialog dialog(*iFramework);
    if (dialog.DoModal() == IDOK)
        Update();
    }

void CCartoTypeDemoView::OnUpdateViewLayers(CCmdUI *pCmdUI)
    {
    pCmdUI->Enable();
    }

void CCartoTypeDemoView::OnViewLegend()
    {
    iDrawLegend = !iDrawLegend;
    iFramework->ForceRedraw();
    Update();
    }

void CCartoTypeDemoView::OnUpdateViewLegend(CCmdUI *pCmdUI)
    {
    pCmdUI->Enable();
    }

void CCartoTypeDemoView::OnViewSetScale()
    {
    CSetScaleDialog dialog;
    dialog.iScale.Format(_TEXT("%d"),(int)iFramework->ScaleDenominator());
    if (dialog.DoModal() == IDOK)
        {
        CartoType::TTextBuffer<32> scale_text;
        SetString(scale_text,dialog.iScale);
        size_t length = 0;
        CartoType::int32 new_scale = (int)iFramework->ScaleDenominator();
        scale_text.ToInt32(new_scale,length);
        iFramework->SetScaleDenominatorInView(new_scale);
        Update();
        }
    }

void CCartoTypeDemoView::OnUpdateViewSetScale(CCmdUI *pCmdUI)
    {
    pCmdUI->Enable();
    }

void CCartoTypeDemoView::OnViewPerspective()
    {
    iFramework->SetPerspective(!iFramework->Perspective());
    Update();
    }

void CCartoTypeDemoView::OnUpdateViewPerspective(CCmdUI *pCmdUI)
    {
    pCmdUI->Enable();
    pCmdUI->SetCheck(iFramework->Perspective());
    }

void CCartoTypeDemoView::OnViewMetricUnits()
    {
    iMetricUnits = !iMetricUnits;
    iFramework->SetLocale(iMetricUnits ? "en_xx" : "en");
    iLegend.reset();
    if (iDrawLegend || iDrawDrivingInstructions)
        {
        if (iDrawDrivingInstructions)
            CreateTurnInstructions();
        iFramework->ForceRedraw();
        Update();
        }
    }

void CCartoTypeDemoView::OnUpdateViewMetricUnits(CCmdUI *pCmdUI)
    {
    pCmdUI->Enable();
    pCmdUI->SetCheck(iMetricUnits);
    }

void CCartoTypeDemoView::OnViewDrivingInstructions()
    {
    iDrawDrivingInstructions = !iDrawDrivingInstructions;
    iFramework->ForceRedraw();
    Update();
    }

void CCartoTypeDemoView::OnUpdateViewDrivingInstructions(CCmdUI *pCmdUI)
    {
    pCmdUI->Enable(iFramework->Route() != NULL);
    pCmdUI->SetCheck(iDrawDrivingInstructions);
    }

void CCartoTypeDemoView::StopFlyThrough()
    {
    if (iFlyThroughTimer)
        {
        KillTimer(iFlyThroughTimer);
        iFlyThroughTimer = 0;
        iRouteIter = NULL;
        iFramework->SetFollowMode(CartoType::EFollowModeNone);
        iFramework->SetPerspective(false);
        iFramework->SetRotation(0);
        Update();
        }
    }

void CCartoTypeDemoView::OnViewFlyThrough()
    {
    // If doing a fly-through, cancel it.
    if (iFlyThroughTimer)
        {
        StopFlyThrough();
        return;
        }

    iRouteIter.reset(new CartoType::TRouteIterator(*iFramework->Route()));
    if (iRouteIter)
        {
        iFramework->SetScaleDenominator(5000);
        iFramework->SetRotationRadians(iRouteIter->Direction());
        iFramework->SetPerspective(true);
        iFramework->SetFollowMode(CartoType::EFollowModeLocationHeading);
        iFlyThroughTimer = (UINT)SetTimer(1,250,0);
        }
    }

void CCartoTypeDemoView::OnUpdateViewFlyThrough(CCmdUI *pCmdUI)
    {
    pCmdUI->Enable(iFramework->Navigating());
    pCmdUI->SetCheck(iFlyThroughTimer != 0);
    }

void CCartoTypeDemoView::OnGoAlongRoute()
    {
    bool at_end = false;
    SimulateNavigationFix(0,at_end);

    // Temporarily turn the follow mode on so that the view jumps to the position.
    iFramework->SetFollowMode(CartoType::EFollowModeLocation);
    iFramework->SetFollowMode(CartoType::EFollowModeNone);

    // Uncomment to test position extrapolation.
    // CartoType::TNavigationData n;
    // n.iValidity = CartoType::TNavigationData::ESpeedValid;
    // n.iSpeed = 100;
    // iFramework->Navigate(n);

    Update();
    }

void CCartoTypeDemoView::OnUpdateGoAlongRoute(CCmdUI *pCmdUI)
    {
    pCmdUI->Enable(iFramework->Navigating());
    }

void CCartoTypeDemoView::OnDeleteRoute()
    {
    StopFlyThrough();
    iRoutePointArray.resize(2);
    iRoutePointArray[0] = iRoutePointArray[1] = TRoutePoint();
    iFramework->DeleteRoutes();
    Update();
    }

void CCartoTypeDemoView::OnUpdateDeleteRoute(CCmdUI *pCmdUI)
    {
    pCmdUI->Enable(iFramework->Route() != NULL);
    }

void CCartoTypeDemoView::SetProfile(CartoType::TRouteProfileType aProfileType)
    {
    StopFlyThrough();
    iRouteProfileType = aProfileType;
    CartoType::TRouteProfile p(iRouteProfileType);
    p.iShortest = iShortestRoute;
    if (!iUseGradients)
        p.iGradientFlags = 0;
    iFramework->SetMainProfile(p);
    if (iRoutePointArray[0].iPoint != CartoType::TPointFP() &&
        iRoutePointArray[iRoutePointArray.size() - 1].iPoint != CartoType::TPointFP())
        CalculateAndDisplayRoute();
    }

void CCartoTypeDemoView::OnSetDriveProfile()
    {
    SetProfile(CartoType::ECarRouteProfile);
    }

void CCartoTypeDemoView::OnUpdateSetDriveProfile(CCmdUI *pCmdUI)
    {
    pCmdUI->SetCheck(iRouteProfileType == CartoType::ECarRouteProfile);
    }

void CCartoTypeDemoView::OnSetCycleProfile()
    {
    SetProfile(CartoType::EBicycleRouteProfile);
    }

void CCartoTypeDemoView::OnUpdateSetCycleProfile(CCmdUI *pCmdUI)
    {
    pCmdUI->SetCheck(iRouteProfileType == CartoType::EBicycleRouteProfile);
    }

void CCartoTypeDemoView::OnSetWalkProfile()
    {
    SetProfile(CartoType::EWalkingRouteProfile);
    }

void CCartoTypeDemoView::OnUpdateSetWalkProfile(CCmdUI *pCmdUI)
    {
    pCmdUI->SetCheck(iRouteProfileType == CartoType::EWalkingRouteProfile);
    }

void CCartoTypeDemoView::OnSetHikeProfile()
    {
    SetProfile(CartoType::EHikingRouteProfile);
    }

void CCartoTypeDemoView::OnUpdateSetHikeProfile(CCmdUI* pCmdUI)
    {
    pCmdUI->SetCheck(iRouteProfileType == CartoType::EHikingRouteProfile);
    }

void CCartoTypeDemoView::OnShortestRoute()
    {
    iShortestRoute = !iShortestRoute;
    SetProfile(iRouteProfileType);
    }

void CCartoTypeDemoView::OnUpdateShortestRoute(CCmdUI* pCmdUI)
    {
    pCmdUI->SetCheck(iShortestRoute);
    }

void CCartoTypeDemoView::OnUseGradients()
    {
    iUseGradients = !iUseGradients;
    SetProfile(iRouteProfileType);
    }

void CCartoTypeDemoView::OnUpdateUseGradients(CCmdUI* pCmdUI)
    {
    pCmdUI->Enable(iFramework->NavigationDataHasGradients());
    pCmdUI->SetCheck(iUseGradients);
    }

void CCartoTypeDemoView::OnSaveRouteInstructions()
    {
    CFileDialog file_dialog(false,_TEXT(".txt"),NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,TEXT("Text files (*.txt)|*.txt||"));
    if (file_dialog.DoModal() == IDOK)
        {
        CString p = file_dialog.GetPathName();
        CartoType::CString path;
        SetString(path,p);
        std::string path_utf8(path);
        CartoType::TResult error = 0;
        FILE* file = nullptr;
        fopen_s(&file,path_utf8.c_str(),"w");
        if (!file)
            error = CartoType::KErrorIo;
        if (!error)
            {
            std::string text(iFramework->Route()->iInstructions);
            size_t n = fwrite(text.c_str(),text.length(),1,file);
            if (!n)
                error = CartoType::KErrorIo;
            }
        fclose(file);
        if (error)
            {
            CCartoTypeDemoApp* app = (CCartoTypeDemoApp*)AfxGetApp();
            app->ShowError(_TEXT("failed to save the route instructions to a text file"),error);
            }
        Update();
        }
    }

void CCartoTypeDemoView::OnUpdateSaveRouteInstructions(CCmdUI *pCmdUI)
    {
    pCmdUI->Enable(iFramework->Route() != nullptr);
    }

void CCartoTypeDemoView::OnSaveRouteXml()
    {
    CFileDialog file_dialog(false,_TEXT(".ctroute"),NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,TEXT("CartoType Route files (*.ctroute)|*.ctroute||"));
    if (file_dialog.DoModal() == IDOK)
        {
        CString p = file_dialog.GetPathName();
        CartoType::CString path;
        SetString(path,p);
        CartoType::TResult error = iFramework->WriteRouteAsXml(*iFramework->Route(),path);
        if (error)
            {
            CCartoTypeDemoApp* app = (CCartoTypeDemoApp*)AfxGetApp();
            app->ShowError(_TEXT("failed to save the route to a .ctroute file"),error);
            }
        Update();
        }
    }

void CCartoTypeDemoView::OnUpdateSaveRouteXml(CCmdUI *pCmdUI)
    {
    pCmdUI->Enable(iFramework->Route() != nullptr);
    }

void CCartoTypeDemoView::OnSaveRouteGpx()
    {
    CFileDialog file_dialog(false,_TEXT(".gpx"),NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,TEXT("GPX files (*.gpx)|*.gpx||"));
    if (file_dialog.DoModal() == IDOK)
        {
        CString p = file_dialog.GetPathName();
        CartoType::CString path;
        SetString(path,p);
        CartoType::TResult error = iFramework->WriteRouteAsXml(*iFramework->Route(),path,CartoType::EGpxFileType);
        if (error)
            {
            CCartoTypeDemoApp* app = (CCartoTypeDemoApp*)AfxGetApp();
            app->ShowError(_TEXT("failed to save the route to a GPX file"),error);
            }
        Update();
        }
    }

void CCartoTypeDemoView::OnUpdateSaveRouteGpx(CCmdUI *pCmdUI)
    {
    pCmdUI->Enable(iFramework->Route() != nullptr);
    }

void CCartoTypeDemoView::OnLoadRouteXml()
    {
    CFileDialog file_dialog(true,_TEXT(".ctroute"),NULL,0,TEXT("CartoType Route files (*.ctroute)|*.ctroute|GPX files (*.gpx)|*.gpx||"));
    if (file_dialog.DoModal() == IDOK)
        {
        CString p = file_dialog.GetPathName();
        CartoType::CString path;
        SetString(path,p);
        CartoType::TResult error = iFramework->ReadRouteFromXml(path,true);
        if (iFramework->Route())
            {
            iRouteIter.reset(new CartoType::TRouteIterator(*iFramework->Route()));
            bool at_end = false;
            SimulateNavigationFix(5,at_end);
            }
        if (error)
            {
            CCartoTypeDemoApp* app = (CCartoTypeDemoApp*)AfxGetApp();
            app->ShowError(_TEXT("failed to read a route from a file"),error);
            }
        Update();
        }
    }

void CCartoTypeDemoView::OnUpdateLoadRouteXml(CCmdUI *pCmdUI)
    {
    pCmdUI->Enable();
    }

void CCartoTypeDemoView::OnTimer(UINT_PTR /*nIDEvent*/)
    {
    if (iFlyThroughTimer && iRouteIter)
        {
        // Go forward 25 metres.
        bool at_end = false;
        SimulateNavigationFix(25,at_end);

        // Stop the fly-through if we have reached the end.
        if (at_end)
            StopFlyThrough();

        Update();
        }
    }

void CCartoTypeDemoView::OnEditLoadKml()
    {
    CFileDialog file_dialog(true,_TEXT(".kml"),NULL,0,TEXT("KML files (*.kml)|*.kml||"));
    if (file_dialog.DoModal() == IDOK)
        {
        CString p = file_dialog.GetPathName();
        CartoType::CString path;
        SetString(path,p);
        CartoType::TResult error = iFramework->ReadMap(iWritableMapHandle,path,CartoType::EKmlFileType);
        if (error)
            {
            CCartoTypeDemoApp* app = (CCartoTypeDemoApp*)AfxGetApp();
            app->ShowError(_TEXT("failed to read data from a KML file"),error);
            }
        Update();
        }
    }

void CCartoTypeDemoView::OnUpdateEditLoadKml(CCmdUI *pCmdUI)
    {
    pCmdUI->Enable();
    }

void CCartoTypeDemoView::OnEditSaveKml()
    {
    CFileDialog file_dialog(false,_TEXT(".kml"),NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,TEXT("KML files (*.kml)|*.kml||"));
    if (file_dialog.DoModal() == IDOK)
        {
        CString p = file_dialog.GetPathName();
        CartoType::CString path;
        SetString(path,p);
        CartoType::TResult error = iFramework->SaveMap(iWritableMapHandle,path,CartoType::EKmlFileType);
        if (error)
            {
            CCartoTypeDemoApp* app = (CCartoTypeDemoApp*)AfxGetApp();
            app->ShowError(_TEXT("failed to write data to a KML file"),error);
            }
        else
            iWritableMapChanged = false;
        }
    }

void CCartoTypeDemoView::OnUpdateEditSaveKml(CCmdUI *pCmdUI)
    {
    pCmdUI->Enable(!iFramework->MapIsEmpty(iWritableMapHandle));
    }

void CCartoTypeDemoView::OnEditLoadCtms()
    {
    CFileDialog file_dialog(true,_TEXT(".ctms"),NULL,0,TEXT("CTMS files (*.ctms)|*.ctms||"));
    if (file_dialog.DoModal() == IDOK)
        {
        CString p = file_dialog.GetPathName();
        CartoType::CString path;
        SetString(path,p);
        CartoType::TResult error = iFramework->ReadMap(iWritableMapHandle,path,CartoType::ECtmsFileType);
        if (error)
            {
            CCartoTypeDemoApp* app = (CCartoTypeDemoApp*)AfxGetApp();
            app->ShowError(_TEXT("failed to read data from a CTMS file"),error);
            }
        Update();
        }
    }

void CCartoTypeDemoView::OnUpdateEditLoadCtms(CCmdUI *pCmdUI)
    {
    pCmdUI->Enable();
    }

void CCartoTypeDemoView::OnEditSaveCtms()
    {
    CFileDialog file_dialog(false,_TEXT(".ctms"),NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,TEXT("CTMS files (*.ctms)|*.ctms||"));
    if (file_dialog.DoModal() == IDOK)
        {
        CString p = file_dialog.GetPathName();
        CartoType::CString path;
        SetString(path,p);
        CartoType::TResult error = iFramework->SaveMap(iWritableMapHandle,path,CartoType::ECtmsFileType);
        if (error)
            {
            CCartoTypeDemoApp* app = (CCartoTypeDemoApp*)AfxGetApp();
            app->ShowError(_TEXT("failed to save data to a CTMS file"),error);
            }
        else
            iWritableMapChanged = false;
        }
    }

void CCartoTypeDemoView::OnUpdateEditSaveCtms(CCmdUI *pCmdUI)
    {
    pCmdUI->Enable(!iFramework->MapIsEmpty(iWritableMapHandle));
    }

void CCartoTypeDemoView::OnEditLoadCtsql()
    {
    CFileDialog file_dialog(true,_TEXT(".ctsql"),NULL,0,TEXT("CTSQL files (*.ctsql)|*.ctsql||"));
    if (file_dialog.DoModal() == IDOK)
        {
        CString p = file_dialog.GetPathName();
        CartoType::CString path;
        SetString(path,p);
        CartoType::TResult error = iFramework->ReadMap(iWritableMapHandle,path,CartoType::ECtsqlFileType);
        if (error)
            {
            CCartoTypeDemoApp* app = (CCartoTypeDemoApp*)AfxGetApp();
            app->ShowError(_TEXT("failed to read data from a CTSQL file"),error);
            }
        Update();
        }
    }

void CCartoTypeDemoView::OnUpdateEditLoadCtsql(CCmdUI *pCmdUI)
    {
    pCmdUI->Enable();
    }

void CCartoTypeDemoView::OnEditSaveCtsql()
    {
    CFileDialog file_dialog(false,_TEXT(".ctsql"),NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,TEXT("CartoType SQLite database files (*.ctsql)|*.ctsql||"));
    if (file_dialog.DoModal() == IDOK)
        {
        CString p = file_dialog.GetPathName();
        CartoType::CString path;
        SetString(path,p);
        CartoType::TResult error = iFramework->SaveMap(iWritableMapHandle,path,CartoType::ECtsqlFileType);
        if (error)
            {
            CCartoTypeDemoApp* app = (CCartoTypeDemoApp*)AfxGetApp();
            app->ShowError(_TEXT("failed to save data to a CTSQL file"),error);
            }
        else
            iWritableMapChanged = false;
        }
    }

void CCartoTypeDemoView::OnUpdateEditSaveCtsql(CCmdUI *pCmdUI)
    {
    pCmdUI->Enable(!iFramework->MapIsEmpty(iWritableMapHandle));
    }

void CCartoTypeDemoView::OnEditLoadGpx()
    {
    CFileDialog file_dialog(true,_TEXT(".gpx"),NULL,0,TEXT("GPX files (*.gpx)|*.gpx||"));
    if (file_dialog.DoModal() == IDOK)
        {
        CString p = file_dialog.GetPathName();
        CartoType::CString path;
        SetString(path,p);
        CartoType::TResult error = iFramework->ReadGpx(iWritableMapHandle,path);
        if (error)
            {
            CCartoTypeDemoApp* app = (CCartoTypeDemoApp*)AfxGetApp();
            app->ShowError(_TEXT("failed to read data from a GPX file"),error);
            }
        Update();
        }
    }

void CCartoTypeDemoView::OnUpdateEditLoadGpx(CCmdUI *pCmdUI)
    {
    pCmdUI->Enable();
    }
