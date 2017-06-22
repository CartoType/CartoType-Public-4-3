/*
CARTOTYPE_LEGEND.H
Copyright (C) 2015-2016 CartoType Ltd.
See www.cartotype.com for more information.
*/

#ifndef CARTOTYPE_LEGEND_H__
#define CARTOTYPE_LEGEND_H__

#include <cartotype_graphics_context.h>
#include <vector>
#include <memory>

namespace CartoType
{

class CLegendObjectParam;
class CFramework;

/**
The CLegend class creates bitmaps showing sample map objects, with optional text lines and a scale,
to be used as a map legend or for visualisation in the style sheet editor.

Dimensions are specified with units, which may be "pt" (point), "pc" (pica), "cm" (centimetre),
"mm" (millimetre), "in" (inch), or "px" (pixel). An empty or unrecognised unit is taken as "px".

It's relatively expensive to construct a CLegend object, so it is best to keep such an object in
existence rather than creating one as a temporary object in a draw loop.
*/
class CLegend
    {
    public:
    CLegend(const CFramework& aFramework);
    ~CLegend();
    
    std::unique_ptr<CBitmap> CreateLegend(double aWidth,const char* aUnit,double aScaleDenominator,double aScaleDenominatorInView);
    void Clear();

    void AddMapObjectLine(TMapObjectType aType,const CString& aLayer,const char* aOsmType,int32 aIntAttrib,const CString& aStringAttrib,const CString& aLabel);
    void AddTextLine(const CString& aText);
    void AddScaleLine(bool aMetricUnits);

    void SetMainStyleSheet(const uint8* aData,size_t aLength);
    void SetExtraStyleSheet(const uint8* aData,size_t aLength);
    void SetBackgroundColor(TColor aColor);
    void SetBorder(TColor aColor,double aStrokeWidth,double aRadius,const char* aUnit);
    void SetMarginWidth(double aMarginWidth,const char* aUnit);
    void SetMinLineHeight(double aLineHeight,const char* aUnit);
    void SetFontFamily(const CString& aFontFamily);
    void SetFontSize(double aFontSize,const char* aUnit);
    void SetTextColor(TColor aTextColor);
    void SetAlignment(TAlign aAlignment);
    void SetPolygonRotation(double aDegrees);

    private:
    int32 Pixels(double aDimension,const char* aUnit);
    void DrawScale(CGraphicsContext& aGc,const CLegendObjectParam& aParam,int32 aX,int32 aY,int32 aWidth);

    std::unique_ptr<CFramework> m_framework;
    std::vector<CLegendObjectParam> m_object_array;
    TColor m_background_color { KWhite };
    TColor m_border_color { KGray };
    int32 m_border_width_in_pixels;
    int32 m_border_radius_in_pixels;
    int32 m_margin_width_in_pixels;
    int32 m_min_line_height_in_pixels;
    TFontSpec m_font_spec;
    TColor m_color { KGray };
    TAlign m_alignment;
    double m_polygon_rotation { 0 };
    double m_metre;
    double m_pt;
    double m_inch;
    };

}

#endif // CARTOTYPE_LEGEND_H__
