/*
#include "Audio/AudioController.h"
#include "Audio/AudioClip.h"
#include "Audio/AudioManager.h"
#include "Utility/tinyxml.h"

ShivaGUI::AudioController::AudioController(AudioManager *manIn)
{
	_audioManager = manIn;
}

ShivaGUI::AudioController::~AudioController()
{
	for( std::map<std::string,AudioClip*>::iterator it = _eventTypes.begin(); it != _eventTypes.end(); ++it )
	{
		delete (*it).second;
	}
	_eventTypes.clear();
}

void ShivaGUI::AudioController::LoadSettings( ProfileManager *profileManager, std::string themeFilename )
{
	// Load settings from theme
	if( !themeFilename.empty() )
	{
		TiXmlDocument doc( themeFilename.c_str() );

		if( doc.LoadFile() )
		{
			TiXmlElement* currentThemeNode = (TiXmlElement*) doc.RootElement();

			// Load event registration list
			*
			xml should look like this:

			<Audio>
				<event>
					<typename>MyButtonPress</typename>
					<wavfile>wobble.wav</wavfile>
					<loop>true</loop>
				</event>
				<event>
					<typename>MyButtonRelease</typename>
					<wavfile>somethingelse</wavfile>
				</event>
			</Audio>

			*
			if( currentThemeNode != NULL )
			{
				TiXmlElement *constContainerNode = currentThemeNode->FirstChildElement("Audio");
				if( constContainerNode != NULL )
				{
					TiXmlNode *child = 0;
					while( child = constContainerNode->IterateChildren( "event", child ) )
					{
						TiXmlElement *keyElement = child->FirstChildElement("typename");
						if( keyElement != NULL )
						{
							TiXmlElement *valueElement = child->FirstChildElement("wavfile");
							if( valueElement != NULL )
							{
								// Loop setting is optional, default is false
								bool loop = false;
								TiXmlElement *loopElement = child->FirstChildElement("loop");
								if( valueElement != NULL )
								{
									std::string loopStr = loopElement->GetText();
									if( loopStr == "true" || loopStr == "1" )
									{
										loop = true;
									}
								}

								std::string eventType = keyElement->GetText();
								std::string wavfile = valueElement->GetText();

								RegisterEvent(eventType,wavfile,loop);

								std::cout<<"INFO: Audio Event set: name: "<<eventType<<" file: "<<wavfile<<" loop: "<<loop<<std::endl;
							}
						}
					}
				}
			}
		}
		else
		{
			std::cerr<<"WARNING: AudioController could not load theme from xml: "<<themeFilename<<std::endl;
		}
	}
	else
	{
		std::cerr<<"WARNING: AudioController asked to load settings from blank theme file name"<<std::endl;
	}


	// Load settings from profile, overwriting any in theme
	profileManager->EnterOptionNode( "Audio" );
	int numConsts = profileManager->ContainsOption( "event" );
	for( int i = 0; i < numConsts; i++ )
	{
		if( profileManager->EnterOptionNode( "event", i ) )
		{
			// We can now query our const replacements
			std::string eventType = profileManager->GetString( "typename", "@INVALID" );
			std::string wavfile = profileManager->GetString( "wavfile", "@INVALID" );
			bool loop = profileManager->GetBoolean( "loop", false );

			RegisterEvent(eventType,wavfile,loop);

			std::cout<<"INFO: Audio Event set: name: "<<eventType<<" file: "<<wavfile<<" loop: "<<loop<<std::endl;

			profileManager->ExitOptionNode();
		}
	}
	profileManager->ExitOptionNode();


}

void ShivaGUI::AudioController::RegisterEvent(std::string eventType, std::string audioFilename, bool loopPlayback)
{
	AudioClip *clip = _audioManager->GetSample(audioFilename,loopPlayback);
	_eventTypes[eventType] = clip;
}

int ShivaGUI::AudioController::IssueEventStart(std::string eventType)
{
	AudioClip *clip = _eventTypes[eventType];
	if( clip != NULL )
	{
		return clip->Play();
	}
	return -1;
}

void ShivaGUI::AudioController::IssueEventStop(int eventID)
{
	AudioClip::Stop(eventID);
}
*/
