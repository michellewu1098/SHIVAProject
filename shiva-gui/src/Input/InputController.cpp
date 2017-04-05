#include "Input/InputController.h"

//----------------------------------------------------------------------------------

ShivaGUI::InputController::InputController( ProfileManager *_profile )
{
	m_useKeys = false;
	m_shareKeyInputs = false;
	m_debounce = false;
	m_needFirstFocus = true;
	m_autoFirstSelect = false;

	m_globalDebounceTrailTime = 0.0f;
	m_globalDebounceSupTime = 0.0f;
	m_globalDebounceDelayTime = 0.0f;

	m_globalRepeatEnable = false;
	m_globalRepeatDebounced = false;
	m_globalRepeatDelayTime = 1.0f;
	m_globalRepeatRateTime = 0.1f;

	m_scanner = NULL;

	LoadSettings( _profile );
}

//----------------------------------------------------------------------------------

ShivaGUI::InputController::~InputController()
{
	delete m_scanner;

	for( std::map< int, Debouncer* >::iterator it = m_debouncers.begin(); it != m_debouncers.end(); ++it )
	{
		delete ( *it ).second;
		(*it).second = NULL;
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::InputController::LoadSettings( ProfileManager *_profile )
{
	if( _profile == NULL )
		return;

	// Retrieve keyboard settings
	if( _profile->ContainsOption( "keyinput" ) )
	{
		// We will only use the first option group of this name
		_profile->EnterOptionNode( "keyinput" );

		if( _profile->ContainsOption( "enablekeys" ) )
		{
			m_useKeys = true;
		}
		if( _profile->ContainsOption( "autofirstselect" ) )
		{
			m_autoFirstSelect = true;
		}
		if( _profile->ContainsOption( "shareinputs" ) )
		{
			m_shareKeyInputs = true;
		}
		if( _profile->ContainsOption( "debounce" ) )
		{
			_profile->EnterOptionNode( "debounce" );
			m_debounce = true;

			m_globalDebounceTrailTime = _profile->GetFloat( "trailTime", m_globalDebounceTrailTime );
			m_globalDebounceSupTime = _profile->GetFloat( "suppressionTime", m_globalDebounceSupTime );
			m_globalDebounceDelayTime = _profile->GetFloat( "delayTime", m_globalDebounceDelayTime );

			m_globalRepeatEnable = _profile->GetBoolean( "repeatEnable", m_globalRepeatEnable );
			m_globalRepeatDebounced = _profile->GetBoolean( "repeatDebounced", m_globalRepeatDebounced );
			m_globalRepeatDelayTime = _profile->GetFloat( "repeatDelay", m_globalRepeatDelayTime );
			m_globalRepeatRateTime = _profile->GetFloat( "repeatRate", m_globalRepeatRateTime );

			_profile->ExitOptionNode();
		}
		if( _profile->ContainsOption( "map" ) )
		{
			unsigned int numMapGroups = _profile->ContainsOption( "map" );
			for( unsigned int currentMapGroup = 0; currentMapGroup < numMapGroups; ++currentMapGroup )
			{
				_profile->EnterOptionNode( "map", currentMapGroup );

				float groupDebounceTrailTime = m_globalDebounceTrailTime;
				float groupDebounceSupTime = m_globalDebounceSupTime;
				float groupDebounceDelayTime = m_globalDebounceDelayTime;
				bool groupDebounce = false;

				bool groupRepeatEnable = m_globalRepeatEnable;
				bool groupRepeatDebounced = m_globalRepeatDebounced;
				float groupRepeatDelayTime = m_globalRepeatDelayTime;
				float groupRepeatRateTime = m_globalRepeatRateTime;

				if( _profile->ContainsOption( "debounce" ) )
				{
					_profile->EnterOptionNode( "debounce" );
					m_debounce = groupDebounce = true;

					groupDebounceTrailTime = _profile->GetFloat( "trailTime", groupDebounceTrailTime );
					groupDebounceSupTime = _profile->GetFloat( "suppressionTime", groupDebounceSupTime );
					groupDebounceDelayTime = _profile->GetFloat( "delayTime", groupDebounceDelayTime );

					groupRepeatEnable = _profile->GetBoolean( "repeatEnable", groupRepeatEnable );
					groupRepeatDebounced = _profile->GetBoolean( "repeatDebounced", groupRepeatDebounced );
					groupRepeatDelayTime = _profile->GetFloat( "repeatDelay", groupRepeatDelayTime );
					groupRepeatRateTime = _profile->GetFloat( "repeatRate", groupRepeatRateTime );

					_profile->ExitOptionNode();
				}

				unsigned int numKeys = _profile->ContainsOption( "key" );
				for( unsigned int i = 0; i < numKeys; i++ )
				{
					int key = _profile->GetInt( "key", 0, i );

					_profile->EnterOptionNode( "key", i );

					InternalEvent::EventType pressType = InternalEvent::ConvertEventType( _profile->GetString( "pressEvent", "invalid" ) );
					InternalEvent::EventType delayType = InternalEvent::ConvertEventType( _profile->GetString( "delayEvent", "invalid" ) );
					InternalEvent::EventType repeatType = InternalEvent::ConvertEventType( _profile->GetString( "repeatEvent", "invalid" ) );
					InternalEvent::EventType releaseType = InternalEvent::ConvertEventType( _profile->GetString( "releaseEvent", "invalid" ) );

					std::cout << "INFO: InputController Keymapping: " << key << " to pressType: " << pressType << " delay: " << delayType << " release: " << releaseType << std::endl;

					m_keymapping[ key ] = MappingEventGroup( pressType, delayType, repeatType, releaseType );

					// If we have options for the group or global debounce settings
					if( groupDebounce || m_debounce )
					{
						Debouncer *eventDebouncer = new Debouncer();
						eventDebouncer->SetTrailTime( groupDebounceTrailTime );
						eventDebouncer->SetSuppressionTime( groupDebounceSupTime );
						eventDebouncer->SetDelayTime( groupDebounceDelayTime );
						eventDebouncer->SetRepeat( groupRepeatEnable, groupRepeatRateTime, groupRepeatDelayTime, groupRepeatDebounced );
						m_debouncers[ key ] = eventDebouncer;
					}

					_profile->ExitOptionNode();
				}
				_profile->ExitOptionNode();
			}
		}
		if( _profile->ContainsOption( "scanning" ) )
		{
			_profile->EnterOptionNode( "scanning" );

			std::string scannerType = _profile->GetString( "type", "MISSING TAG" );

			if( boost::algorithm::iequals( scannerType, "autoscanner" ) )
			{
				Autoscanner *currentScanner = new Autoscanner();
				m_scanner = currentScanner;
				std::cout << "INFO: Profile loaded using Autoscanner" << std::endl;
				float delayTime = _profile->GetFloat( "delayTime", 1.0f );
				currentScanner->SetDelayTime( delayTime );
			}
			else
			{
				std::cerr << "WARNING: profile requests unknown scanner type: " << scannerType << std::endl;
			}

			_profile->ExitOptionNode();
		}

		_profile->ExitOptionNode();
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::InputController::Update( float _deltaTs, Activity *_currentActivity )
{
	if( m_debounce )
	{
		for( std::map< int, Debouncer* >::iterator it = m_debouncers.begin(); it != m_debouncers.end(); ++it )
		{
			// We assume that we have a mapping entry for this key, as this is done during setup for each key in the _debouncers map

			// Update our Debouncer and see if it gives any output
			Debouncer::OutputType currentResult = ( *it ).second->Update( _deltaTs );
			if( currentResult != Debouncer::OUT_NONE )
			{
				// Map the output of the Debouncer to an InternalEvent
				int keyID = ( *it ).first;
				InternalEvent::EventType outputEventType = InternalEvent::INVALID;
				if( currentResult == Debouncer::OUT_LEADING )
					outputEventType = m_keymapping[ keyID ].PressEvent;
				else if( currentResult == Debouncer::OUT_DELAYED )
					outputEventType = m_keymapping[ keyID ].DelayEvent;
				else if( currentResult == Debouncer::OUT_REPEATING )
					outputEventType = m_keymapping[ keyID ].RepeatEvent;
				else if( currentResult == Debouncer::OUT_TRAILING )
					outputEventType = m_keymapping[ keyID ].ReleaseEvent;

				if( outputEventType != InternalEvent::INVALID )
				{
					// We have a valid event type to deal with

					// TODO: debounced inputs are issued to all activities - should store window IDs and issue to those windows

					if( outputEventType == InternalEvent::NEXTFOCUS_LEFT ||
						outputEventType == InternalEvent::NEXTFOCUS_RIGHT ||
						outputEventType == InternalEvent::NEXTFOCUS_UP ||
						outputEventType == InternalEvent::NEXTFOCUS_DOWN )
					{
						if( m_needFirstFocus )
						{
							IssueKeyEvent( new InternalEvent( InternalEvent::FIRSTFOCUS ), _currentActivity, NULL );
							m_needFirstFocus = false;
						}
						else
						{
							IssueKeyEvent( new InternalEvent( outputEventType ), _currentActivity, NULL );
						}
					}
					else
						IssueKeyEvent( new InternalEvent( outputEventType ), _currentActivity, NULL );
				}

			}
			/*
			if( currentResult == Debouncer::OUT_TRAILING && outputEventType == InternalEvent::FOCUS_SELECT )
			{	// Special case for focus_select, which must be followed by a deselect
				IssueKeyEvent( new InternalEvent(InternalEvent::FOCUS_DESELECT), currentActivity, NULL);
			}
			*/
		}
	}
	if( m_scanner != NULL )
	{
		InternalEvent::EventType currentResult;
		if( m_scanner->Update( currentResult, _deltaTs ) )
		{
			IssueKeyEvent( new InternalEvent( currentResult ), _currentActivity, NULL );
		}
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::InputController::IssueEvent( InputEvent *_currentInputEvent, Activity *_currentActivity )
{
	if( _currentInputEvent == NULL || _currentActivity == NULL )
		return;

	GUIController *targetGUIController = _currentActivity->GetGUIController( _currentInputEvent->GetWindowID() );

	if( targetGUIController != NULL )
	{
		if( _currentInputEvent->GetType() == InputEvent::WINDOWCLOSE )
		{
			// MEMORY LEAK?
			targetGUIController->IssueEvent( new InternalEvent( InternalEvent::QUIT ) );
		}
		else if( _currentInputEvent->GetType() == InputEvent::WINDOWRESIZE )
		{
			// MEMORY LEAK?
			InternalEvent *currentOutputEvent = new InternalEvent( InternalEvent::WINDOW_RESIZE );
			int w, h;
			_currentInputEvent->GetWindowSize( w, h );
			currentOutputEvent->SetWindowSize( w, h );
			targetGUIController->IssueEvent( currentOutputEvent );
		}
		else if( _currentInputEvent->GetType() == InputEvent::MOUSEBUTTONDOWN )
		{
			// MEMORY LEAK?
			InternalEvent *currentOutputEvent = new InternalEvent( InternalEvent::POSITIONAL_SELECT );
			int x, y;
			_currentInputEvent->GetMousePosition( x, y );
			currentOutputEvent->SetPosition( x, y );
			targetGUIController->IssueEvent( currentOutputEvent );
		}
		else if( _currentInputEvent->GetType() == InputEvent::MOUSEBUTTONUP )
		{
			// MEMORY LEAK?
			InternalEvent *currentOutputEvent = new InternalEvent( InternalEvent::POSITIONAL_DESELECT );
			int x, y;
			_currentInputEvent->GetMousePosition( x, y );
			currentOutputEvent->SetPosition( x, y );
			targetGUIController->IssueEvent( currentOutputEvent );
		}
		else if( _currentInputEvent->GetType() == InputEvent::MOUSEMOTION )
		{
			// MEMORY LEAK?
			InternalEvent *currentOutputEvent = new InternalEvent( InternalEvent::POSITIONAL_DRAG );
			int x, y;
			_currentInputEvent->GetMousePosition( x, y );
			currentOutputEvent->SetPosition( x, y );
			targetGUIController->IssueEvent( currentOutputEvent );
		}
	}

//	if( currentInputEvent->GetType() == InputEvent::QUIT )
//		currentActivity->IssueEvent( new InternalEvent(InternalEvent::QUIT) );

	if( m_useKeys && ( _currentInputEvent->GetType() == InputEvent::KEYDOWN || _currentInputEvent->GetType() == InputEvent::KEYUP) )
		HandleImmediateKeys( _currentInputEvent, _currentActivity, targetGUIController );
}

//----------------------------------------------------------------------------------

void ShivaGUI::InputController::HandleImmediateKeys( InputEvent *_currentInputEvent, Activity *_currentActivity, GUIController *_targetGUIController )
{
	// Already checked: currentActivity != NULL && currentInputEvent != NULL

	if( m_debounce )
	{
		int keycode = _currentInputEvent->GetKeycode();

		if( m_debouncers.count(keycode) > 0 )
		{
			//std::cout << "issuing debounce input for key: " << keycode << std::endl;
			m_debouncers[ keycode ]->SetInput( _currentInputEvent->GetType() == InputEvent::KEYDOWN );
		}
	}
}

//----------------------------------------------------------------------------------

void ShivaGUI::InputController::IssueKeyEvent( InternalEvent *_outputEvent, Activity *_currentActivity, GUIController *_targetGUIController )
{
	// Scanning goes in here - events are passed to the scanner as well as the Activity
	if( m_scanner != NULL )
		m_scanner->SetInput( _outputEvent->GetType() );

	if( m_shareKeyInputs || _targetGUIController == NULL )
	{
		_currentActivity->IssueEvent( _outputEvent );
	}
	else
		_targetGUIController->IssueEvent( _outputEvent );
}

//----------------------------------------------------------------------------------

ShivaGUI::InputController::MappingEventGroup::MappingEventGroup()
{
	PressEvent = InternalEvent::INVALID;
	DelayEvent = InternalEvent::INVALID;
	RepeatEvent = InternalEvent::INVALID;
	ReleaseEvent = InternalEvent::INVALID;
}

//----------------------------------------------------------------------------------

ShivaGUI::InputController::MappingEventGroup::MappingEventGroup( InternalEvent::EventType _press, InternalEvent::EventType _delay, InternalEvent::EventType _repeat, InternalEvent::EventType _release )
{
	PressEvent = _press;
	DelayEvent = _delay;
	RepeatEvent = _repeat;
	ReleaseEvent = _release;
}

//----------------------------------------------------------------------------------