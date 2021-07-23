#pragma once
#include <imgui.h>
#include <interaction_lib/InteractionLib.h>
#include <interaction_lib/misc/InteractionLibPtr.h>
#include <memory>
#include <string>

namespace Interactions
{
	class IInteraction
	{
	public:
		IInteraction( ) : uid( InteractionCounter++ ) { }
		virtual ~IInteraction( ) { };

		void GazeInteract( IL::GazeFocusEvent& gaze )
		{
			if ( uid == gaze.id )
				OnGazeInteraction( gaze );
		}

		void PointInteract( IL::GazePointData& gaze )
		{
			OnPointInteraction( gaze );
		}

		virtual bool DoUI( ) = 0;
		virtual void DoIndicator( ) = 0;
		virtual void UpdateInteractor( ) = 0;
		virtual void OnGazeInteraction( IL::GazeFocusEvent& gaze ) = 0;
		virtual void OnPointInteraction( IL::GazePointData& gaze ) = 0;
		virtual std::string CreateConfigDetails( ) = 0;
		virtual void FromConfigDetails( const std::string& details ) = 0;

	protected:
		int uid;
		static int InteractionCounter;

	};
}