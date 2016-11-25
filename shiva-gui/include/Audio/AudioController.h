/*
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef __SHIVA_AUDIO_AUDIOCONTROLLER__
#define __SHIVA_AUDIO_AUDIOCONTROLLER__
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#include <string>
#include <map>


//////////////////////////////////////////////////////////////////////////
#include "System/ProfileManager.h"


//////////////////////////////////////////////////////////////////////////

namespace ShivaGUI
{
	class AudioManager;
	class AudioClip;

	/// These for per-Activity audio management
	/// The shared resource is the SDL Mix_Chunk, which is managed by the AudioManager
	class AudioController
	{
	public:
		AudioController(AudioManager*);
		~AudioController();

		/// Loads event type and file type associations
		void LoadSettings( ProfileManager*, std::string themeFilename );

		/// Connects an event type with the audio clip file it should play
		void RegisterEvent(std::string eventType, std::string audioFilename, bool loopPlayback);

		/// Used at runtime for anything to issue an audio event
		/// This will trigger the associated audio clip to play
		/// Returns an ID to pass to IssueEventStop() if you want to stop the clip
		int IssueEventStart(std::string eventType);
		
		/// For stopping a clip from playing
		void IssueEventStop(int eventID);


	protected:

		AudioManager *_audioManager;

		/// Associates an event type with an audio clip
		std::map<std::string,AudioClip*> _eventTypes;
	};
}


//////////////////////////////////////////////////////////////////////////
#endif
*/
