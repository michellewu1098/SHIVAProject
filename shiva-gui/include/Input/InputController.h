///-----------------------------------------------------------------------------------------------
/// \file InputController.h
/// \brief This takes raw events, processes them and issues internal events to the current Activity
/// \author Leigh McLoughlin
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_GUI_INPUTSYSTEM_INPUTCONTROLLER__
#define __SHIVA_GUI_INPUTSYSTEM_INPUTCONTROLLER__

#include <map>
#include <boost/algorithm/string.hpp>

#include "System/Window.h"
#include "Input/Debouncer.h"
#include "Input/Scanners/Autoscanner.h"
#include "System/Activity.h"
#include "System/ProfileManager.h"


namespace ShivaGUI
{
	class InputController
	{
	public:

		//----------------------------------------------------------------------------------
		/// \brief Ctor
		/// \param [in] _profile
		//----------------------------------------------------------------------------------
		InputController( ProfileManager* _profile );
		//----------------------------------------------------------------------------------
		/// \brief Dtor
		//----------------------------------------------------------------------------------
		~InputController();
		//----------------------------------------------------------------------------------
		/// \brief For loading settings from profile
		/// \param [in] _profile
		//----------------------------------------------------------------------------------
		void LoadSettings( ProfileManager* _profile );
		//----------------------------------------------------------------------------------
		/// \brief Processes the input
		/// Give it the current Activity to pass any events to
		/// \param [in] _deltaTs
		/// \param [in] _currentActivity
		//----------------------------------------------------------------------------------
		void Update( float _deltaTs, Activity* _currentActivity );
		//----------------------------------------------------------------------------------
		/// \brief Any simple events are passed on immediately
		/// Complex events may be issued some time after the InputEvent
		/// \param [in] _currentInputEvent
		/// \param [in] _currentActivity
		//----------------------------------------------------------------------------------
		void IssueEvent( InputEvent* _currentInputEvent, Activity* _currentActivity );
		//----------------------------------------------------------------------------------


	protected:


		// KEYBOARD INFO

		//----------------------------------------------------------------------------------
		/// \brief Enables keyboard input
		//----------------------------------------------------------------------------------
		bool m_useKeys;
		//----------------------------------------------------------------------------------
		/// \brief If a key input should be sent to just the originating window or all windows
		//----------------------------------------------------------------------------------
		bool m_shareKeyInputs;
		//----------------------------------------------------------------------------------
		/// \brief If a FIRSTFOCUS event should be issued on the first Update cycle
		//----------------------------------------------------------------------------------
		bool m_autoFirstSelect;
		//----------------------------------------------------------------------------------
		/// \brief Whether to debounce
		//----------------------------------------------------------------------------------
		bool m_debounce;
		//----------------------------------------------------------------------------------
		/// \brief Is set to true when focus commands have not been issued yet
		//----------------------------------------------------------------------------------
		bool m_needFirstFocus;
		//----------------------------------------------------------------------------------
		/// \brief For handling immediate key events
		/// \param [in] _currentInputEvent
		/// \param [in] _currentActivity
		/// \param [in] _targetGUIController
		//----------------------------------------------------------------------------------
		void HandleImmediateKeys( InputEvent* _currentInputEvent, Activity* _currentActivity, GUIController* _targetGUIController );
		//----------------------------------------------------------------------------------
		/// \brief Update for delayed key processing
		/// \param [in] _deltaTs
		/// \param [in] _currentActivity
		//----------------------------------------------------------------------------------
		void HandleUpdateKeys( float _deltaTs, Activity* _currentActivity );
		//----------------------------------------------------------------------------------
		/// \brief Simple utility function that checks _shareKeyInputs and issues an event to either the Activity or GUIController
		/// \param [in] _outputEvent
		/// \param [in] _currentActivity
		/// \param [in] _targetGUIController
		//----------------------------------------------------------------------------------
		void IssueKeyEvent( InternalEvent* _outputEvent, Activity* _currentActivity, GUIController* _targetGUIController );
		//----------------------------------------------------------------------------------
		
		// Contains the events that are mapped for each key
		
		class MappingEventGroup
		{
		public:

			//----------------------------------------------------------------------------------
			/// \brief Default ctor
			//----------------------------------------------------------------------------------
			MappingEventGroup();
			//----------------------------------------------------------------------------------
			/// \brief Ctor
			/// \param [in] _press Press event
			/// \param [in] _delay Delay event
			/// \param [in] _repeat Repeat event
			/// \param [in] _release Release event
			//----------------------------------------------------------------------------------
			MappingEventGroup( InternalEvent::EventType _press, InternalEvent::EventType _delay, InternalEvent::EventType _repeat, InternalEvent::EventType _release );
			//----------------------------------------------------------------------------------
			/// \brief Press event
			//----------------------------------------------------------------------------------
			InternalEvent::EventType PressEvent;
			//----------------------------------------------------------------------------------
			/// \brief Delay event
			//----------------------------------------------------------------------------------
			InternalEvent::EventType DelayEvent;
			//----------------------------------------------------------------------------------
			/// \brief Repeat event
			//----------------------------------------------------------------------------------
			InternalEvent::EventType RepeatEvent;
			//----------------------------------------------------------------------------------
			/// \brief Release event
			//----------------------------------------------------------------------------------
			InternalEvent::EventType ReleaseEvent;
			//----------------------------------------------------------------------------------

		};

		//----------------------------------------------------------------------------------
		/// \brief Associates input keys with the internal events they should issue
		//----------------------------------------------------------------------------------
		std::map< int, MappingEventGroup > m_keymapping;
		//----------------------------------------------------------------------------------
		/// \brief Each key has a Debouncer that filters the input
		/// We can't use the EventType any more because keys have different events that can be issued on press / delay / repeat / release
		//----------------------------------------------------------------------------------
		std::map< int, Debouncer* > m_debouncers;
		//----------------------------------------------------------------------------------
		// Values to be retrieved from profile and sent to Debouncer
		//----------------------------------------------------------------------------------
		/// \brief Debounce trail time
		//----------------------------------------------------------------------------------
		float m_globalDebounceTrailTime;
		//----------------------------------------------------------------------------------
		/// \brief Debounce delay time
		//----------------------------------------------------------------------------------
		float m_globalDebounceDelayTime;
		//----------------------------------------------------------------------------------
		/// \brief Debouce suppression time
		//----------------------------------------------------------------------------------
		float m_globalDebounceSupTime;
		//----------------------------------------------------------------------------------
		/// \brief Whether to enable global repeat
		//----------------------------------------------------------------------------------
		bool m_globalRepeatEnable;
		//----------------------------------------------------------------------------------
		/// \brief Whether to debounce global repeat
		//----------------------------------------------------------------------------------
		bool m_globalRepeatDebounced;
		//----------------------------------------------------------------------------------
		/// \brief Global repeat delay time
		//----------------------------------------------------------------------------------
		float m_globalRepeatDelayTime;
		//----------------------------------------------------------------------------------
		/// \brief Global repeat rate time
		//----------------------------------------------------------------------------------
		float m_globalRepeatRateTime;
		//----------------------------------------------------------------------------------
		/// \brief Performs key-scanning operations
		//----------------------------------------------------------------------------------
		Scanner *m_scanner;
		//----------------------------------------------------------------------------------

	};
}

#endif
