#pragma once
#include <imgui.h>
#include "window.h"
#include "dx11.h"

namespace Render
{
    using Colour = ImVec4;

    void Setup( HWND hwnd, DX::Graphics& gfx );
    void NewFrame( );
    void RenderFrame( );

    void BeginLayer( const char* LayerName );
    void EndLayer( );

    enum CornerFlags : int
    {
        CF_TopLeft = 1 << 0,
        CF_TopRight = 1 << 1,
        CF_BottomLeft = 1 << 2,
        CF_BottomRight = 1 << 3,
        CF_Top = CF_TopLeft | CF_TopRight,
        CF_Bot = CF_BottomLeft | CF_BottomRight,
        CF_Left = CF_TopLeft | CF_BottomLeft,
        CF_Right = CF_TopRight | CF_BottomRight,
        CF_All = CF_Top | CF_Bot
    };

    using font = ImFont;

    void Line( float x, float y, float x2, float y2, Colour Col, float thickness = 1.0f, bool allign = true );
    void Rect( float x, float y, float x2, float y2, Colour col, float thickness = 1.0f, float rounding = 0.0f, int rounding_corners_flags = CF_All );
    void RectFilled( float x, float y, float x2, float y2, Colour col, float rounding = 0.0f, int rounding_corners_flags = CF_All );
    void RectFilledMultiColor( float x, float y, float x2, float y2, Colour col_upr_left, Colour col_upr_right, Colour col_bot_right, Colour col_bot_left );
    void RectFilledMultiColorRounded( float x, float y, float x2, float y2, Colour col_upr_left, Colour col_upr_right, Colour col_bot_right, Colour col_bot_left, float rounding = 1.0f, int rounding_corners_flags = CF_All );
    void Quad( float x, float y, float x2, float y2, float x3, float y3, float x4, float y4, Colour col, float thickness = 1.0f );
    void QuadFilled( float x, float y, float x2, float y2, float x3, float y3, float x4, float y4, Colour col );
    void Triangle( float x, float y, float x2, float y2, float x3, float y3, Colour col, float thickness = 1.0f );
    void TriangleFilled( float x, float y, float x2, float y2, float x3, float y3, Colour col );
    void Circle( float x, float y, float radius, Colour col, int num_segments = 12, float thickness = 1.0f );
    void CircleFilled( float x, float y, float radius, Colour col, int num_segments = 12 );
    void Text( const font* font, float font_size, float x, float y, Colour col, const char* text, float wrap_width = 0.0f );
}