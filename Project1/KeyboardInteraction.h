#pragma once
#include "IInteraction.h"

namespace Interactions
{
	class KeyboardInteraction : public IInteraction
	{
	public:
		~KeyboardInteraction( );

		bool DoUI( ) override;
		void DoIndicator( ) override;
		void OnGazeInteraction( IL::GazeFocusEvent& gaze ) override;
		void UpdateInteractor( ) override;
		void OnPointInteraction( IL::GazePointData& gaze ) override {}
		std::string CreateConfigDetails( );
		void FromConfigDetails( const std::string& details );

	protected:
		int key = 0;
		ImVec4 area = { 10, 10, 100, 100 };
		float color[4] = { 1,1,1,0.85 };
		bool focused = false;
		std::string displayName = "";

	};
}