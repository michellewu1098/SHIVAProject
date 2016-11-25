
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef __SHIVA_GUI_INPUTSYSTEM_INPUTCONTROLLER__
#define __SHIVA_GUI_INPUTSYSTEM_INPUTCONTROLLER__
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include <map>

//////////////////////////////////////////////////////////////////////////
#include "System/Window.h"
#include "Input/Debouncer.h"
#include "Input/Scanners/Scanner.h"
#include "Input/Scanners/Autoscanner.h"
#include "Input/InternalEvent.h"
#include "System/Activity.h"
#include "System/ProfileManager.h"
//////////////////////////////////////////////////////////////////////////

namespace ShivaGUI
{

	/// This takes raw events, processes them and issues internal events to the current Activity
	class InputController
	{
	public:
		InputController(ProfileManager*);
		~InputController();

		/// For loading settings from profile
		void LoadSettings(ProfileManager*);

		/// Processes the input
		/// Give it the current Activity to pass any events to
		void Update(float deltaTs, Activity *currentActivity);

		/// Any simple events are passed on immediately
		/// Complex events may be issued some time after the InputEvent
		void IssueEvent(InputEvent*, Activity *currentActivity);



	protected:

		// Keyboard info
			/// Enables keyboard input
			bool _useKeys;

			/// If a key input should be sent to just the originating window or all windows
			bool _shareKeyInputs;

			/// If a FIRSTFOCUS event should be issued on the first Update cycle
			bool _autoFirstSelect;

			bool _debounce;

			/// Is set to true when focus commands have not been issued yet
			bool _needFirstFocus;

			/// For handling immediate key events
			void HandleImmediateKeys(InputEvent*, Activity *currentActivity, GUIController *targetGUIController);

			/// Update for delayed key processing
			void HandleUpdateKeys(float deltaTs, Activity *currentActivity);

			/// Simple utility function that checks _shareKeyInputs and issues an event to either the Activity or GUIController
			void IssueKeyEvent(InternalEvent *outputEvent, Activity *currentActivity, GUIController *targetGUIController);

			/// Contains the events that are mapped for each key
			class MappingEventGroup
			{
			public:
				MappingEventGroup();
				MappingEventGroup(InternalEvent::EventType press, InternalEvent::EventType delay, InternalEvent::EventType repeat, InternalEvent::EventType release);
				InternalEvent::EventType PressEvent;
				InternalEvent::EventType DelayEvent;
				InternalEvent::EventType RepeatEvent;
				InternalEvent::EventType ReleaseEvent;
			};

			/// Associates input keys with the internal events they should issue
			std::map<int,MappingEventGroup> _keymapping;

			/// Each key has a Debouncer that filters the input
			/// We can't use the EventType any more because keys have different events that can be issued on press / delay / repeat / release
			std::map<int,Debouncer*> _debouncers;

			/// Values to be retrieved from profile and sent to Debouncer
			float _globalDebounceTrailTime, _globalDebounceDelayTime,  _globalDebounceSupTime;
			bool _globalRepeatEnable, _globalRepeatDebounced;
			float _globalRepeatDelayTime, _globalRepeatRateTime;


			/// Performs key-scanning operations
			Scanner *_scanner;

	};

}

//////////////////////////////////////////////////////////////////////////
#endif
