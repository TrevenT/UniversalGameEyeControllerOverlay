#pragma once
#include <mutex>
#include "IInteraction.h"

namespace Interactions
{
	class MouseJoystickInteraction : public IInteraction
	{
	public:
		MouseJoystickInteraction( );
		~MouseJoystickInteraction( );
		bool DoUI( ) override;
		void DoIndicator( ) override;
		void UpdateInteractor( ) override;
		void OnGazeInteraction( IL::GazeFocusEvent& gaze ) override;
		void OnPointInteraction( IL::GazePointData& gaze ) override;
		void MouseThread( );
		std::string CreateConfigDetails( );
		void FromConfigDetails( const std::string& details );

	protected:
		std::thread mouseThread;
		bool shouldClose = false;
		std::mutex safetyMutex;
		std::mutex dMutex;

		ImVec2 centre = { 200.0f, 200.0f };
		float radius = 150.0f;
		float innerRadius = 50.0f;
		float color[4] = { 1,1,1,0.85 };
		bool focused = true;
		float dx = 0, dy = 0;
		float sens = 0.01f;
	};
}