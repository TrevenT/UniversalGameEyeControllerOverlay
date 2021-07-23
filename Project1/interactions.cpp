#include "interactions.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <interaction_lib/InteractionLib.h>
#include <interaction_lib/misc/InteractionLibPtr.h>
#include "render.h"
#include <vector>
#include <fstream>
#include "IInteraction.h"
#include "KeyboardInteraction.h"
#include "MouseJoyStickInteraction.h"

#pragma comment( lib, "tobii_interaction_lib.lib" )

namespace Interactions
{
	int IInteraction::InteractionCounter = 0;

	IL::UniqueInteractionLibPtr intlib;

	bool open = true;
	bool InteractionChanged = true;
	ImFont* font;

	std::vector<std::shared_ptr<IInteraction>> interactions;
	IL::GazePointData data;

	void SaveInteractions( )
	{
		std::ofstream out = std::ofstream( "tobiiconfig.cfg", std::ios::trunc );

		if ( !out )
		{
			// well fuck?
			return;
		}

		for ( auto& interaction : interactions )
			out << interaction->CreateConfigDetails( ) << std::endl;

		out.close( );
	}

	void LoadInteractions( )
	{
		std::ifstream in = std::ifstream( "tobiiconfig.cfg" );

		if ( !in )
		{
			return;
		}

		interactions = {};

		std::string line;
		while ( std::getline( in, line ) )
		{
			if ( line.length( ) > 1 )
			{
				if ( line[0] == 'K' )
				{
					auto newdude = std::make_shared<KeyboardInteraction>( );
					newdude->FromConfigDetails( line.substr( 1 ) );
					interactions.push_back( newdude );
				}
				else if ( line[0] == 'M' )
				{
					auto newdude = std::make_shared<MouseJoystickInteraction>( );
					newdude->FromConfigDetails( line.substr( 1 ) );
					interactions.push_back( newdude );
				}
			}
		}
	}

	bool stupideyething = false;

	void DoUI( )
	{
		Render::BeginLayer( "InteractionLayer" );
		{
			if ( open )
				Render::RectFilled( 0, 0, 1920, 1080, { 1,1,1, 0.25 } );

			for ( auto& interaction : interactions )
				interaction->DoIndicator( );
		}
		Render::EndLayer( );

		if ( open )
		{
			ImGui::Begin( "Interaction Manager", nullptr, ImGuiWindowFlags_AlwaysAutoResize );
			{
				ImGui::Text( "Trenen's Tobii Tracker" );

				ImGui::Text( "Press <HOME> To Toggle UI" );

				if ( ImGui::Button( "Quit" ) )
				{
					interactions = {}; // safely deconstruct our interactions (some have stuff like threads)
					exit( 0 );
				} 
				ImGui::SameLine( );

				if ( ImGui::Button( "Save" ) )
					SaveInteractions( );

				ImGui::SameLine( );

				if ( ImGui::Button( "Load" ) )
					LoadInteractions( );

				ImGui::Checkbox( "Eye Visualisation", &stupideyething );

				if ( ImGui::Button( "New Keyboard Interaction" ) )
					interactions.push_back( std::make_unique<KeyboardInteraction>( ) );

				if ( ImGui::Button( "New Mouse Joystick Interaction" ) )
					interactions.push_back( std::make_unique<MouseJoystickInteraction>( ) );

				std::erase_if( interactions, 
					[] ( auto& interaction ) { return interaction->DoUI( ); } 
				);

				ImGui::Text( "Tool by TrevenT." );
			}
			ImGui::End( );
		}

		Render::BeginLayer( "EyeLayer" );
		{
			if ( data.validity && stupideyething )
			{
				Render::CircleFilled( data.x, data.y, 20, { 1,1,1,0.2 } );
				Render::Circle( data.x, data.y, 20, { 0,0,0,1 }, 12, 1 );
				//render::Text( font, 20, data.x + 23, data.y - 12, { 1,0,0,1 }, "<- eye here" );
			}
		}
		Render::EndLayer( );
	}

	void SetupTobii( )
	{
		intlib = IL::UniqueInteractionLibPtr( IL::CreateInteractionLib( IL::FieldOfUse::Interactive ) );
		intlib->CoordinateTransformAddOrUpdateDisplayArea( 1920.0f, 1080.0f );
		intlib->CoordinateTransformSetOriginOffset( 0, 0 );
		
		ImGui::GetIO( ).Fonts->AddFontFromFileTTF( "C:/windows/fonts/tahoma.ttf", 15 );
		font = ImGui::GetIO( ).Fonts->AddFontFromFileTTF( "C:/windows/fonts/tahoma.ttf", 40 );

		LoadInteractions( );
	}

	void GazeFocusEventCallback( IL::GazeFocusEvent data, void* )
	{
		if ( !open )
		{
			for ( auto& interaction : interactions )
			{
				interaction->GazeInteract( data );
			}
		}
	}

	void GazePointCallback( IL::GazePointData _data, void* )
	{
		data = _data;

		if ( !open )
		{
			for ( auto& interaction : interactions )
			{
				interaction->PointInteract( data );
			}
		}
	}

	void UpdateTobii( )
	{
		if ( InteractionChanged )
		{
			intlib->BeginInteractorUpdates( );
			for ( auto& interaction : interactions )
			{
				interaction->UpdateInteractor( );
			}
			intlib->CommitInteractorUpdates( );
			InteractionChanged = false;
		}

		intlib->SubscribeGazeFocusEvents( GazeFocusEventCallback, 0 );
		intlib->SubscribeGazePointData( GazePointCallback, 0 );
		intlib->Update( );
	}

	void ShowWindow( bool show )
	{
		open = show;
	}
}
