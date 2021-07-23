#include "MouseJoyStickInteraction.h"
#include <imgui.h>
#include <thread>
#include <string>
#include <sstream>
#include "render.h"

using namespace std::chrono_literals;

namespace Interactions
{
	extern IL::UniqueInteractionLibPtr intlib;
	extern bool InteractionChanged;
	extern bool open;

	MouseJoystickInteraction::MouseJoystickInteraction( ) :
		IInteraction( ),
		mouseThread( [this] ( ) { this->MouseThread( ); } )
	{
		mouseThread.detach( );
	}

	MouseJoystickInteraction::~MouseJoystickInteraction( )
	{
		shouldClose = true;
		std::lock_guard<std::mutex> lock( safetyMutex );
	}

	bool MouseJoystickInteraction::DoUI( )
	{
		bool doDelete = false;
		if ( ImGui::TreeNode( std::format( "{} - Mouse JoyStick", uid ).c_str( ) ) )
		{
			ImGui::ColorEdit4( std::format( "{}##{}", "Colour", uid ).c_str( ), color );

			InteractionChanged |= ImGui::SliderFloat( std::format( "{}##{}", "X", uid ).c_str( ), &centre.x, 0, 1920, "%f", 1 );
			InteractionChanged |= ImGui::SliderFloat( std::format( "{}##{}", "Y", uid ).c_str( ), &centre.y, 0, 1080, "%f", 1 );
			InteractionChanged |= ImGui::SliderFloat( std::format( "{}##{}", "Outer Radius", uid ).c_str( ), &radius, 40, 300, "%f", 1 );
			InteractionChanged |= ImGui::SliderFloat( std::format( "{}##{}", "DeadZone Radius", uid ).c_str( ), &innerRadius, 20, radius, "%f", 1 );
			InteractionChanged |= ImGui::SliderFloat( std::format( "{}##{}", "Sens", uid ).c_str( ), &sens, 0.00, 0.2, "%f", 1 );

			if ( ImGui::Button( std::format( "{}##{}", "Delete", uid ).c_str( ) ) )
			{
				doDelete = true;
				intlib->RemoveInteractor( uid );
			}

			ImGui::TreePop( );
		}

		return doDelete;
	}

	void MouseJoystickInteraction::DoIndicator( )
	{
		const auto alpha = ( focused || open ) ? 1.0f : 0.4f;
		Render::Circle( centre.x, centre.y, radius, ImVec4{ color[0], color[1], color[2], color[3] * alpha }, 30, 5 );
		Render::Circle( centre.x, centre.y, innerRadius, ImVec4{ color[0], color[1], color[2], color[3] * alpha }, 30, 5 );
	}

	void MouseJoystickInteraction::UpdateInteractor( )
	{
		//IL::Rectangle rect = { 
		//	centre.x - radius, 
		//	centre.y - radius,
		//	centre.x + radius, 
		//	centre.y + radius 
		//};
		//intlib->AddOrUpdateInteractor( uid, rect, 0 );
	}

	void MouseJoystickInteraction::OnGazeInteraction( IL::GazeFocusEvent& gaze )
	{

	}

	void MouseJoystickInteraction::OnPointInteraction( IL::GazePointData& gaze )
	{
		std::lock_guard<std::mutex> lock( dMutex );
		{
			dx = gaze.x - centre.x;
			dy = gaze.y - centre.y;
			const float dst = ( dx * dx + dy * dy );

			focused = gaze.validity &&
				dst < radius* radius&&
				dst > innerRadius * innerRadius;
		}
	}

	void MouseJoystickInteraction::MouseThread( )
	{
		std::lock_guard<std::mutex> lock( safetyMutex );
		while ( !shouldClose )
		{
			{
				std::lock_guard<std::mutex> lock( dMutex );
				if ( focused )
				{
					INPUT input;
					ZeroMemory( &input, sizeof( INPUT ) );
					input.type = INPUT_MOUSE;
					input.mi.dwFlags |= MOUSEEVENTF_MOVE;
					input.mi.dx = dx * sens;
					input.mi.dy = dy * sens;
					SendInput( 1, &input, sizeof( INPUT ) );
				}
			}

			std::this_thread::sleep_for( 0.01s );
		}
	}

	std::string MouseJoystickInteraction::CreateConfigDetails( )
	{
		return std::format( "M{} {} {} {} {} {} {} {}", centre.x, centre.y, radius, innerRadius, color[0], color[1], color[2], color[3], sens );
	}

	void MouseJoystickInteraction::FromConfigDetails( const std::string& details )
	{
		std::stringstream ss;
		ss << details;
		ss >> centre.x >> centre.y >> radius >> innerRadius >> color[0] >> color[1] >> color[2] >> color[3] >> sens;
	}
}