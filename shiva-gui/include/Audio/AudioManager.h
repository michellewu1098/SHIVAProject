
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef __SHIVA_AUDIO_AUDIOMANAGER__
#define __SHIVA_AUDIO_AUDIOMANAGER__
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#include <string>
#include <map>

#include <SDL.h>
#include <SDL_mixer.h>

//////////////////////////////////////////////////////////////////////////
#include "Audio/AudioClip.h"


//////////////////////////////////////////////////////////////////////////

namespace ShivaGUI
{

	/*
	* Options for design:
	* 1. Static method in AudioManager takes a string 'event type'
	*    Theme and Profiles can specify which audio clip to play for each incoming type
	*    Advantage is simplicity, also user code can issue their own events of whatever typestrings they want
	*    A standard set of strings will need to be defined for events that are issued by widgets
	*    These could be set in layout, overridden in theme
	*    Actually, these can always be set in the theme along with the event-type <-> audio files to play association
	*     i.e. the theme developer calls the events whatever they want
	*    -> Better variation, GUIManager has IssueAudioEvent(str) which it passes to an AudioManager
	* 2. Use InternalEvent
	*
	* - Need events for starting and stopping a clip
	* - Do we assume the user will only ever be playing a clip once? Would be nice not to limit this
	* - Needs to share actual resources
	*
	* Ok, treat SDL Mix_chunk as the shared resource, no need for separate class
	* Will have an AudioController which is per-Activity and has functions for RegisterEvent and IssueEvent
	* AudioManager is used to create and destroy AudioControllers, to load and destroy Mix_chunks
	*/
	class AudioManager
	{
	public:
		AudioManager();
		~AudioManager();

		//AudioController* CreateAudioController();

		/// The AudioClip is a convenience thing to contain playback controls and store settings
		/// TODO: let the thing be an xml with options for stuff like looping, volume, fade in / fade out, etc
		AudioClip* GetSample(std::string filename);

		/// This is called automatically by the AudioClip destructor, so make sure you delete those AudioClips!
		void FreeSample(std::string filename);



	protected:

		bool _audioCapable;

		Mix_Chunk* GetChunk(std::string filename);

		void FreeChunk(std::string filename);

		/// Associates filenames with loaded audio chunks
		std::map<std::string,Mix_Chunk*> _loadedChunks;

		/// Number of references
		std::map<std::string,int> _sampleRefCount;
	};
}


//////////////////////////////////////////////////////////////////////////
#endif
