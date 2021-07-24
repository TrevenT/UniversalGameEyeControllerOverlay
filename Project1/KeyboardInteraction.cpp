#include "KeyboardInteraction.h"
#include <imgui.h>
#include <imgui_internal.h>
#include "imgui/misc/cpp/imgui_stdlib.h"
#include <format>
#include <string>
#include <sstream>
#include "render.h"
#include "inputs.h"

namespace Interactions
{
	extern IL::UniqueInteractionLibPtr intlib;
	extern bool InteractionChanged;
	extern ImFont* font;
	extern bool open;

	KeyboardInteraction::~KeyboardInteraction( )
	{
		intlib->RemoveInteractor( uid );
	}

	bool KeyboardInteraction::DoUI( )
	{
		bool doDelete = false;
		if ( ImGui::TreeNode( std::format( "{} - Key", uid ).c_str( ) ) )
		{
			ImGui::ColorEdit4( std::format( "{}##{}", "Colour", uid ).c_str( ), color );
			ImGui::Combo( "Key", &key, Inputs::keyNames, 95, 15 );

			InteractionChanged |= ImGui::SliderFloat( std::format( "{}##{}", "X", uid ).c_str( ), &area.x, 0, 1920, "%f", 1 );
			InteractionChanged |= ImGui::SliderFloat( std::format( "{}##{}", "Y", uid ).c_str( ), &area.y, 0, 1080, "%f", 1 );
			InteractionChanged |= ImGui::SliderFloat( std::format( "{}##{}", "W", uid ).c_str( ), &area.z, 0, 200, "%f", 1 );
			InteractionChanged |= ImGui::SliderFloat( std::format( "{}##{}", "H", uid ).c_str( ), &area.w, 0, 200, "%f", 1 );

			ImGui::InputText( "Custom Name", &displayName );

			if ( ImGui::Button( std::format( "{}##{}", "Delete", uid ).c_str( ) ) )
			{
				doDelete = true;
				intlib->RemoveInteractor( uid );
			}

			ImGui::TreePop( );
		}

		return doDelete;
	}

	void KeyboardInteraction::DoIndicator( )
	{
		const auto alpha = ( focused || open ) ? 1.0f : 0.4f;
		Render::Rect( area.x, area.y, area.x + area.z, area.y + area.w, ImVec4{ color[0], color[1], color[2], color[3] * alpha }, 5, Render::CF_All );
		const auto txt = displayName.size() > 0 ? displayName : std::format( "{}", Inputs::keyNames[key] );
		const auto size = font->CalcTextSizeA( 40, FLT_MAX, 0, txt.c_str( ) );
		Render::Text( font, 40, ( area.x + area.z / 2 ) - ( size.x / 2 ), ( area.y + area.w / 2 ) - ( size.y / 2 ), ImVec4{ color[0], color[1], color[2], color[3] * alpha }, txt.c_str( ) );
	}

	void KeyboardInteraction::OnGazeInteraction( IL::GazeFocusEvent& gaze )
	{
		INPUT input;
		ZeroMemory( &input, sizeof( INPUT ) );

		auto keyToPress = Inputs::keys[key];
		bool isMouse = keyToPress == MK_LBUTTON || keyToPress == MK_RBUTTON;

		focused = gaze.hasFocus;

		if ( isMouse )
		{
			input.type = INPUT_MOUSE;
			if ( keyToPress == MK_RBUTTON )
				input.mi.dwFlags = focused ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_RIGHTUP;
			else
				input.mi.dwFlags = focused ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_LEFTUP;

			//input.mi.dwFlags |= KEYEVENTF_EXTENDEDKEY;
		}
		else
		{
			input.type = INPUT_KEYBOARD;
			input.ki.wScan = MapVirtualKey( keyToPress, MAPVK_VK_TO_VSC );
			input.ki.wVk = keyToPress;
			input.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY | KEYEVENTF_UNICODE;

			if ( !focused )
				input.ki.dwFlags |= KEYEVENTF_KEYUP;
		}

		SendInput( 1, &input, sizeof( INPUT ) );
	}

	void KeyboardInteraction::UpdateInteractor( )
	{
		IL::Rectangle rect = { area.x, area.y, area.z, area.w };
		intlib->AddOrUpdateInteractor( uid, rect, 0 );
	}

	std::string KeyboardInteraction::CreateConfigDetails( )
	{
		return std::format( "K{} {} {} {} {} {} {} {} {}", key, area.x, area.y, area.z, area.w, color[0], color[1], color[2], color[3] );
	}

	void KeyboardInteraction::FromConfigDetails( const std::string& details )
	{
		std::stringstream ss;
		ss << details;
		ss >> key >> area.x >> area.y >> area.z >> area.w >> color[0] >> color[1] >> color[2] >> color[3];
	}
}