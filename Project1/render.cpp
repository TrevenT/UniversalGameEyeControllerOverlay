#include "render.h"
#include <imgui_internal.h>
#include "imgui/backends/imgui_impl_win32.h"
#include "imgui/backends/imgui_impl_dx11.h"
#include <cmath>

namespace Render
{
	void Setup( HWND hwnd, DX::Graphics& gfx )
	{
		ImGui::CreateContext( );
		ImGui_ImplWin32_Init( hwnd );
		ImGui_ImplDX11_Init( gfx.device, gfx.context );
		ImGui_ImplWin32_EnableAlphaCompositing( hwnd );
	}

	void NewFrame( )
	{
		ImGui_ImplDX11_NewFrame( );
		ImGui_ImplWin32_NewFrame( );
	}

	void RenderFrame( )
	{
		ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData( ) );
	}

	void BeginLayer( const char* LayerName )
	{
		ImGuiIO& io = ImGui::GetIO( );
		ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f } );
		ImGui::Begin( LayerName, nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration );
		ImGui::SetWindowPos( ImVec2( 0, 0 ), ImGuiCond_Always );
		ImGui::SetWindowSize( ImVec2( io.DisplaySize.x, io.DisplaySize.y ), ImGuiCond_Always );
	}

	void EndLayer( )
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow( );
		window->DrawList->PushClipRectFullScreen( );
		ImGui::End( );
		ImGui::PopStyleVar( );
	}

	void Line( float x, float y, float x2, float y2, Colour Col, float thickness, bool allign )
	{
		x = std::floor( x );
		y = std::floor( y );
		x2 = std::floor( x2 );
		y2 = std::floor( y2 );
		ImGuiWindow* window = ImGui::GetCurrentWindow( );

		if ( allign )
		{
			window->DrawList->PathLineTo( ImVec2{ x + 0.5f, y + 0.5f } );
			window->DrawList->PathLineTo( ImVec2{ x2 + 0.5f, y2 + 0.5f } );
		}
		else
		{
			window->DrawList->PathLineTo( { x,y } );
			window->DrawList->PathLineTo( { x2,y2 } );
		}

		window->DrawList->PathStroke( ImGui::GetColorU32( { Col.x , Col.y , Col.z , Col.w } ), false, thickness );
	}

	void Rect( float x, float y, float x2, float y2, Colour Col, float thickness, float rounding, int rounding_corners_flags )
	{
		x = std::floor( x );
		y = std::floor( y );
		x2 = std::floor( x2 );
		y2 = std::floor( y2 );
		ImGuiWindow* window = ImGui::GetCurrentWindow( );
		window->DrawList->AddRect( { x,y }, { x2,y2 }, ImGui::GetColorU32( { Col.x , Col.y , Col.z , Col.w } ), rounding, rounding_corners_flags, thickness );
	}

	void RectFilled( float x, float y, float x2, float y2, Colour Col, float rounding, int rounding_corners_flags )
	{
		x = std::floor( x );
		y = std::floor( y );
		x2 = std::floor( x2 );
		y2 = std::floor( y2 );
		ImGuiWindow* window = ImGui::GetCurrentWindow( );
		window->DrawList->AddRectFilled( { x,y }, { x2,y2 }, ImGui::GetColorU32( { Col.x , Col.y , Col.z , Col.w } ), rounding, rounding_corners_flags );
	}

	void RectFilledMultiColor( float x, float y, float x2, float y2, Colour col_upr_left, Colour col_upr_right, Colour col_bot_right, Colour col_bot_left )
	{
		x = std::floor( x );
		y = std::floor( y );
		x2 = std::floor( x2 );
		y2 = std::floor( y2 );
		ImGuiWindow* window = ImGui::GetCurrentWindow( );
		window->DrawList->AddRectFilledMultiColor(
			{ x,y },
			{ x2,y2 },
			ImGui::GetColorU32( { col_upr_left.x, col_upr_left.y, col_upr_left.z, col_upr_left.w } ),
			ImGui::GetColorU32( { col_upr_right.x, col_upr_right.y, col_upr_right.z, col_upr_right.w } ),
			ImGui::GetColorU32( { col_bot_right.x, col_bot_right.y, col_bot_right.z, col_bot_right.w } ),
			ImGui::GetColorU32( { col_bot_left.x, col_bot_left.y, col_bot_left.z, col_bot_left.w } )
		);
	}

	void RectFilledMultiColorRounded( float x, float y, float x2, float y2, Colour col_upr_left, Colour col_upr_right, Colour col_bot_right, Colour col_bot_left, float rounding, int rounding_corners_flags )
	{
		x = std::floor( x );
		y = std::floor( y );
		x2 = std::floor( x2 );
		y2 = std::floor( y2 );
		if ( rounding == 0.0f )
		{
			RectFilledMultiColor( x, y, x2, y2, col_upr_left, col_upr_right, col_bot_right, col_bot_left );
		}
		else
		{
			auto rounded_end_x = x + rounding;
			auto rounded_end_y = y + rounding;

			auto rounded_end_x2 = x2 - rounding;
			auto rounded_end_y2 = y2 - rounding;

			// Main Area
			RectFilledMultiColor( rounded_end_x, rounded_end_y, rounded_end_x2, rounded_end_y2,
				col_upr_left, col_upr_right, col_bot_right, col_bot_left );

			// Top bar
			RectFilledMultiColor( rounded_end_x, y, rounded_end_x2, rounded_end_y,
				col_upr_left, col_upr_right, col_upr_right, col_upr_left );

			// left bar
			RectFilledMultiColor( x, rounded_end_y, rounded_end_x, rounded_end_y2,
				col_upr_left, col_upr_left, col_bot_left, col_bot_left );

			// Bottom Bar
			RectFilledMultiColor( rounded_end_x, rounded_end_y2, rounded_end_x2, y2,
				col_bot_left, col_bot_right, col_bot_right, col_bot_left );

			// right bar
			RectFilledMultiColor( rounded_end_x2, rounded_end_y, x2, rounded_end_y2,
				col_upr_right, col_upr_right, col_bot_right, col_bot_right );

			// Corners
			RectFilled( x, y, rounded_end_x + 1, rounded_end_y + 1, col_upr_left, rounding, rounding_corners_flags & CF_TopLeft );
			RectFilled( x, rounded_end_y2 - 1, rounded_end_x + 1, y2, col_bot_left, rounding, rounding_corners_flags & CF_BottomLeft );
			RectFilled( rounded_end_x2 - 1, y, x2, rounded_end_y + 1, col_upr_right, rounding, rounding_corners_flags & CF_TopRight );
			RectFilled( rounded_end_x2 - 1, rounded_end_y2 - 1, x2, y2, col_bot_right, rounding, rounding_corners_flags & CF_BottomRight );
		}
	}

	void Quad( float x, float y, float x2, float y2, float x3, float y3, float x4, float y4, Colour col, float thickness )
	{
		x = std::floor( x );
		y = std::floor( y );
		x2 = std::floor( x2 );
		y2 = std::floor( y2 );
		x3 = std::floor( x3 );
		y3 = std::floor( y3 );
		x4 = std::floor( x4 );
		y4 = std::floor( y4 );
		ImGuiWindow* window = ImGui::GetCurrentWindow( );
		window->DrawList->AddQuad(
			{ x,y },
			{ x2,y2 },
			{ x3,y3 },
			{ x4,y4 },
			ImGui::GetColorU32( { col.x, col.y, col.z, col.w } ),
			thickness
		);
	}

	void QuadFilled( float x, float y, float x2, float y2, float x3, float y3, float x4, float y4, Colour col )
	{
		x = std::floor( x );
		y = std::floor( y );
		x2 = std::floor( x2 );
		y2 = std::floor( y2 );
		x3 = std::floor( x3 );
		y3 = std::floor( y3 );
		x4 = std::floor( x4 );
		y4 = std::floor( y4 );
		ImGuiWindow* window = ImGui::GetCurrentWindow( );

		window->DrawList->AddQuadFilled(
			{ x,y },
			{ x2,y2 },
			{ x3,y3 },
			{ x4,y4 },
			ImGui::GetColorU32( { col.x, col.y, col.z, col.w } )
		);
	}

	void Triangle( float x, float y, float x2, float y2, float x3, float y3, Colour col, float thickness )
	{
		x = std::floor( x );
		y = std::floor( y );
		x2 = std::floor( x2 );
		y2 = std::floor( y2 );
		x3 = std::floor( x3 );
		y3 = std::floor( y3 );
		ImGuiWindow* window = ImGui::GetCurrentWindow( );

		window->DrawList->AddTriangle(
			{ x,y },
			{ x2,y2 },
			{ x3,y3 },
			ImGui::GetColorU32( { col.x, col.y, col.z, col.w } ),
			thickness
		);
	}

	void TriangleFilled( float x, float y, float x2, float y2, float x3, float y3, Colour col )
	{
		x = std::floor( x );
		y = std::floor( y );
		x2 = std::floor( x2 );
		y2 = std::floor( y2 );
		x3 = std::floor( x3 );
		y3 = std::floor( y3 );
		ImGuiWindow* window = ImGui::GetCurrentWindow( );

		window->DrawList->AddTriangleFilled(
			{ x,y },
			{ x2,y2 },
			{ x3,y3 },
			ImGui::GetColorU32( { col.x, col.y, col.z, col.w } )
		);
	}

	void Circle( float x, float y, float radius, Colour col, int num_segments, float thickness )
	{
		x = std::floor( x );
		y = std::floor( y );
		ImGuiWindow* window = ImGui::GetCurrentWindow( );

		window->DrawList->AddCircle(
			{ x,y },
			radius,
			ImGui::GetColorU32( { col.x, col.y, col.z, col.w } ),
			num_segments,
			thickness
		);
	}

	void CircleFilled( float x, float y, float radius, Colour col, int num_segments )
	{
		x = std::floor( x );
		y = std::floor( y );
		ImGuiWindow* window = ImGui::GetCurrentWindow( );

		window->DrawList->AddCircleFilled(
			{ x,y },
			radius,
			ImGui::GetColorU32( { col.x, col.y, col.z, col.w } ),
			num_segments
		);
	}

	void Text( float x, float y, Colour col, const char* text )
	{
		x = std::floor( x );
		y = std::floor( y );
		ImGuiWindow* window = ImGui::GetCurrentWindow( );

		window->DrawList->AddText(
			{ x,y },
			ImGui::GetColorU32( { col.x, col.y, col.z, col.w } ),
			text
		);
	}

	void Text( const ImFont* font, float font_size, float x, float y, Colour col, const char* text, float wrap_width )
	{
		x = std::floor( x );
		y = std::floor( y );
		ImGuiWindow* window = ImGui::GetCurrentWindow( );

		window->DrawList->AddText(
			font, font_size,
			{ x,y },
			ImGui::GetColorU32( { col.x, col.y, col.z, col.w } ),
			text,
			0,
			wrap_width
		);
	}
}