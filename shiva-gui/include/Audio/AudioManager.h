///-----------------------------------------------------------------------------------------------
/// \file AudioManager.h
/// \brief Audio manager class
/// \author Leigh McLoughlin
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_AUDIO_AUDIOMANAGER__
#define __SHIVA_AUDIO_AUDIOMANAGER__

#include <iostream>
#include "Audio/AudioClip.h"

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

		//----------------------------------------------------------------------------------
		/// \brief Ctor
		//----------------------------------------------------------------------------------
		AudioManager();
		//----------------------------------------------------------------------------------
		/// \brief Dtor
		//----------------------------------------------------------------------------------
		~AudioManager();
		//----------------------------------------------------------------------------------
		/// \brief The AudioClip is a convenience thing to contain playback controls and store settings
		/// \todo let the thing be an xml with options for stuff like looping, volume, fade in / fade out, etc
		/// \param [in] _filename
		//----------------------------------------------------------------------------------
		AudioClip* GetSample( std::string _filename );
		//----------------------------------------------------------------------------------
		/// \brief This is called automatically by the AudioClip destructor, so make sure you delete those AudioClips!
		/// \param [in] _filename
		//----------------------------------------------------------------------------------
		void FreeSample( std::string _filename );
		//----------------------------------------------------------------------------------

	protected:

		//----------------------------------------------------------------------------------
		/// \brief Whether audio capable
		//----------------------------------------------------------------------------------
		bool m_audioCapable;
		//----------------------------------------------------------------------------------
		/// \brief Get audio chunk
		/// \param [in] _filename
		//----------------------------------------------------------------------------------
		Mix_Chunk* GetChunk( std::string _filename );
		//----------------------------------------------------------------------------------
		/// \brief Free audio chunk
		/// \param [in] _filename
		//----------------------------------------------------------------------------------
		void FreeChunk( std::string _filename );
		//----------------------------------------------------------------------------------
		/// \brief Associates filenames with loaded audio chunks
		//----------------------------------------------------------------------------------
		std::map< std::string, Mix_Chunk* > m_loadedChunks;
		//----------------------------------------------------------------------------------
		/// \brief Number of references
		//----------------------------------------------------------------------------------
		std::map< std::string, int > m_sampleRefCount;
		//----------------------------------------------------------------------------------

	};
}

#endif
