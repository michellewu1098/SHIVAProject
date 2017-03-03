
#include "Input/InputController.h"

#include <boost/algorithm/string.hpp>

ShivaGUI::InputController::InputController(ProfileManager *profile)
{
	_useKeys = false;
	_shareKeyInputs = false;
	_debounce = false;
	_needFirstFocus = true;
	_autoFirstSelect = false;

	_globalDebounceTrailTime = 0.0f;
	_globalDebounceSupTime = 0.0f;
	_globalDebounceDelayTime = 0.0f;

	_globalRepeatEnable = false;
	_globalRepeatDebounced = false;
	_globalRepeatDelayTime = 1.0f;
	_globalRepeatRateTime = 0.1f;

	_scanner = NULL;

	LoadSettings(profile);
}

ShivaGUI::InputController::~InputController()
{
	delete _scanner;

	for( std::map<int,Debouncer*>::iterator it = _debouncers.begin(); it != _debouncers.end(); ++it )
	{
		delete (*it).second;
	}
}

void ShivaGUI::InputController::LoadSettings(ProfileManager *profile)
{
	if( profile == NULL )
		return;

	// Retrieve keyboard settings
	if( profile->ContainsOption("keyinput") )
	{
		// We will only use the first option group of this name
		profile->EnterOptionNode("keyinput");

		if( profile->ContainsOption("enablekeys") )
		{
			_useKeys = true;
		}
		if( profile->ContainsOption("autofirstselect") )
		{
			_autoFirstSelect = true;
		}
		if( profile->ContainsOption("shareinputs") )
		{
			_shareKeyInputs = true;
		}
		if( profile->ContainsOption("debounce") )
		{
			profile->EnterOptionNode("debounce");
			_debounce = true;

			_globalDebounceTrailTime = profile->GetFloat("trailTime", _globalDebounceTrailTime);
			_globalDebounceSupTime = profile->GetFloat("suppressionTime", _globalDebounceSupTime);
			_globalDebounceDelayTime = profile->GetFloat("delayTime",_globalDebounceDelayTime);

			_globalRepeatEnable = profile->GetBoolean("repeatEnable",_globalRepeatEnable);
			_globalRepeatDebounced = profile->GetBoolean("repeatDebounced",_globalRepeatDebounced);
			_globalRepeatDelayTime = profile->GetFloat("repeatDelay",_globalRepeatDelayTime);
			_globalRepeatRateTime = profile->GetFloat("repeatRate",_globalRepeatRateTime);

			profile->ExitOptionNode();
		}
		if( profile->ContainsOption("map") )
		{
			unsigned int numMapGroups = profile->ContainsOption("map");
			for( unsigned int currentMapGroup = 0; currentMapGroup < numMapGroups; ++currentMapGroup )
			{
				profile->EnterOptionNode("map",currentMapGroup);

				float groupDebounceTrailTime = _globalDebounceTrailTime;
				float groupDebounceSupTime = _globalDebounceSupTime;
				float groupDebounceDelayTime = _globalDebounceDelayTime;
				bool groupDebounce = false;

				bool groupRepeatEnable = _globalRepeatEnable;
				bool groupRepeatDebounced = _globalRepeatDebounced;
				float groupRepeatDelayTime = _globalRepeatDelayTime;
				float groupRepeatRateTime = _globalRepeatRateTime;

				if( profile->ContainsOption("debounce") )
				{
					profile->EnterOptionNode("debounce");
					_debounce = groupDebounce = true;

					groupDebounceTrailTime = profile->GetFloat("trailTime", groupDebounceTrailTime);
					groupDebounceSupTime = profile->GetFloat("suppressionTime", groupDebounceSupTime);
					groupDebounceDelayTime = profile->GetFloat("delayTime",groupDebounceDelayTime);

					groupRepeatEnable = profile->GetBoolean("repeatEnable",groupRepeatEnable);
					groupRepeatDebounced = profile->GetBoolean("repeatDebounced",groupRepeatDebounced);
					groupRepeatDelayTime = profile->GetFloat("repeatDelay",groupRepeatDelayTime);
					groupRepeatRateTime = profile->GetFloat("repeatRate",groupRepeatRateTime);

					profile->ExitOptionNode();
				}

				unsigned int numKeys = profile->ContainsOption("key");
				for( unsigned int i = 0; i < numKeys; i++ )
				{
					int key = profile->GetInt("key",0,i);

					profile->EnterOptionNode("key",i);

					InternalEvent::EventType pressType = InternalEvent::ConvertEventType( profile->GetString("pressEvent","invalid") );
					InternalEvent::EventType delayType = InternalEvent::ConvertEventType( profile->GetString("delayEvent","invalid") );
					InternalEvent::EventType repeatType = InternalEvent::ConvertEventType( profile->GetString("repeatEvent","invalid") );
					InternalEvent::EventType releaseType = InternalEvent::ConvertEventType( profile->GetString("releaseEvent","invalid") );

					std::cout<<"INFO: InputController Keymapping: "<<key<<" to pressType: "<<pressType<<" delay: "<<delayType<<" release: "<<releaseType<<std::endl;

					_keymapping[key] = MappingEventGroup(pressType,delayType,repeatType,releaseType);

					// If we have options for the group or global debounce settings
					if( groupDebounce || _debounce )
					{
						Debouncer *eventDebouncer = new Debouncer();
						eventDebouncer->SetTrailTime(groupDebounceTrailTime);
						eventDebouncer->SetSuppressionTime(groupDebounceSupTime);
						eventDebouncer->SetDelayTime(groupDebounceDelayTime);
						eventDebouncer->SetRepeat(groupRepeatEnable,groupRepeatRateTime,groupRepeatDelayTime,groupRepeatDebounced);
						_debouncers[key] = eventDebouncer;
					}

					profile->ExitOptionNode();
				}
				profile->ExitOptionNode();
			}
		}
		if( profile->ContainsOption("scanning") )
		{
			profile->EnterOptionNode("scanning");

			std::string scannerType = profile->GetString("type", "MISSING TAG");

			if( boost::algorithm::iequals(scannerType,"autoscanner") )
			{
				Autoscanner *currentScanner = new Autoscanner();
				_scanner = currentScanner;
				std::cout<<"INFO: Profile loaded using Autoscanner"<<std::endl;
				float delayTime = profile->GetFloat("delayTime", 1.0f);
				currentScanner->SetDelayTime(delayTime);
			}
			else
			{
				std::cerr<<"WARNING: profile requests unknown scanner type: "<<scannerType<<std::endl;
			}

			profile->ExitOptionNode();
		}

		profile->ExitOptionNode();
	}


}

void ShivaGUI::InputController::Update(float deltaTs, Activity *currentActivity)
{
	if( _debounce )
	{
		for( std::map<int,Debouncer*>::iterator it = _debouncers.begin(); it != _debouncers.end(); ++it )
		{
			// We assume that we have a mapping entry for this key, as this is done during setup for each key in the _debouncers map

			// Update our Debouncer and see if it gives any output
			Debouncer::OutputType currentResult = (*it).second->Update(deltaTs);
			if( currentResult != Debouncer::OUT_NONE )
			{
				// Map the output of the Debouncer to an InternalEvent
				int keyID = (*it).first;
				InternalEvent::EventType outputEventType = InternalEvent::INVALID;
				if( currentResult == Debouncer::OUT_LEADING )
					outputEventType = _keymapping[keyID].PressEvent;
				else if( currentResult == Debouncer::OUT_DELAYED )
					outputEventType = _keymapping[keyID].DelayEvent;
				else if( currentResult == Debouncer::OUT_REPEATING )
					outputEventType = _keymapping[keyID].RepeatEvent;
				else if( currentResult == Debouncer::OUT_TRAILING )
					outputEventType = _keymapping[keyID].ReleaseEvent;

				if( outputEventType != InternalEvent::INVALID )
				{
					// We have a valid event type to deal with

					// TODO: debounced inputs are issued to all activities - should store window IDs and issue to those windows

					if( outputEventType == InternalEvent::NEXTFOCUS_LEFT ||
						outputEventType == InternalEvent::NEXTFOCUS_RIGHT ||
						outputEventType == InternalEvent::NEXTFOCUS_UP ||
						outputEventType == InternalEvent::NEXTFOCUS_DOWN )
					{
						if( _needFirstFocus )
						{
							IssueKeyEvent( new InternalEvent(InternalEvent::FIRSTFOCUS), currentActivity, NULL);
							_needFirstFocus = false;
						}
						else
						{
							IssueKeyEvent( new InternalEvent(outputEventType), currentActivity, NULL);
						}
					}
					else
						IssueKeyEvent( new InternalEvent(outputEventType), currentActivity, NULL);
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
	if( _scanner != NULL )
	{
		InternalEvent::EventType currentResult;
		if( _scanner->Update( currentResult, deltaTs ) )
		{
			IssueKeyEvent( new InternalEvent(currentResult), currentActivity, NULL);
		}
	}
}

void ShivaGUI::InputController::IssueEvent(InputEvent *currentInputEvent, Activity *currentActivity)
{
	if( currentInputEvent == NULL || currentActivity == NULL )
		return;

	GUIController *targetGUIController = currentActivity->GetGUIController( currentInputEvent->GetWindowID() );

	if( targetGUIController != NULL )
	{
		if( currentInputEvent->GetType() == InputEvent::WINDOWCLOSE )
			targetGUIController->IssueEvent( new InternalEvent(InternalEvent::QUIT) );
		else if( currentInputEvent->GetType() == InputEvent::WINDOWRESIZE )
		{
			InternalEvent *currentOutputEvent = new InternalEvent(InternalEvent::WINDOW_RESIZE);
			int w,h;
			currentInputEvent->GetWindowSize(w,h);
			currentOutputEvent->SetWindowSize(w,h);
			targetGUIController->IssueEvent( currentOutputEvent );
		}
		else if( currentInputEvent->GetType() == InputEvent::MOUSEBUTTONDOWN )
		{
			InternalEvent *currentOutputEvent = new InternalEvent(InternalEvent::POSITIONAL_SELECT);
			int x, y;
			currentInputEvent->GetMousePosition(x,y);
			currentOutputEvent->SetPosition(x,y);
			targetGUIController->IssueEvent( currentOutputEvent );
		}
		else if( currentInputEvent->GetType() == InputEvent::MOUSEBUTTONUP )
		{
			InternalEvent *currentOutputEvent = new InternalEvent(InternalEvent::POSITIONAL_DESELECT);
			int x, y;
			currentInputEvent->GetMousePosition(x,y);
			currentOutputEvent->SetPosition(x,y);
			targetGUIController->IssueEvent( currentOutputEvent );
		}
		else if( currentInputEvent->GetType() == InputEvent::MOUSEMOTION )
		{
			InternalEvent *currentOutputEvent = new InternalEvent(InternalEvent::POSITIONAL_DRAG);
			int x, y;
			currentInputEvent->GetMousePosition(x,y);
			currentOutputEvent->SetPosition(x,y);
			targetGUIController->IssueEvent( currentOutputEvent );
		}
	}

//	if( currentInputEvent->GetType() == InputEvent::QUIT )
//		currentActivity->IssueEvent( new InternalEvent(InternalEvent::QUIT) );

	if( _useKeys && (currentInputEvent->GetType() == InputEvent::KEYDOWN || currentInputEvent->GetType() == InputEvent::KEYUP) )
		HandleImmediateKeys(currentInputEvent,currentActivity,targetGUIController);

}

void ShivaGUI::InputController::HandleImmediateKeys(InputEvent *currentInputEvent, Activity *currentActivity, GUIController *targetGUIController)
{
	// Already checked: currentActivity != NULL && currentInputEvent != NULL

	if( _debounce )
	{
		int keycode = currentInputEvent->GetKeycode();

		if( _debouncers.count(keycode) > 0 )
		{
			//std::cout<<"issuing debounce input for key: "<<keycode<<std::endl;
			_debouncers[keycode]->SetInput( currentInputEvent->GetType() == InputEvent::KEYDOWN );
		}
	}
}

void ShivaGUI::InputController::IssueKeyEvent(InternalEvent *outputEvent, Activity *currentActivity, GUIController *targetGUIController)
{
	// Scanning goes in here - events are passed to the scanner as well as the Activity
	if( _scanner != NULL )
		_scanner->SetInput( outputEvent->GetType() );

	if( _shareKeyInputs || targetGUIController == NULL )
	{
		currentActivity->IssueEvent( outputEvent );
	}
	else
		targetGUIController->IssueEvent( outputEvent );
}


ShivaGUI::InputController::MappingEventGroup::MappingEventGroup()
{
	PressEvent=InternalEvent::INVALID;
	DelayEvent=InternalEvent::INVALID;
	RepeatEvent=InternalEvent::INVALID;
	ReleaseEvent=InternalEvent::INVALID;
}

ShivaGUI::InputController::MappingEventGroup::MappingEventGroup(InternalEvent::EventType press, InternalEvent::EventType delay, InternalEvent::EventType repeat, InternalEvent::EventType release)
{
	PressEvent=press;
	DelayEvent=delay;
	RepeatEvent=repeat;
	ReleaseEvent=release;
}
