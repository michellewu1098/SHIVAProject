///-----------------------------------------------------------------------------------------------
/// \file AudioClip.h
/// \brief These are used only by the Audio Controller
/// The shared resource is the SDL Mix_Chunk, which is managed by the AudioManager
/// \author Leigh McLoughlin
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_AUDIO_AUDIOCLIP__
#define __SHIVA_AUDIO_AUDIOCLIP__

#include <string>
#include <map>

#include <SDL.h>
#include <SDL_mixer.h>


namespace ShivaGUI
{
	class AudioManager;

	class AudioClip
	{
	public:

		//----------------------------------------------------------------------------------
		/// \brief Ctor
		/// \param [in] _mainIn
		/// \param [in] _chunkIn
		/// \param [in] _filename
		/// \param [in] _loopPlayback
		//----------------------------------------------------------------------------------
		AudioClip( AudioManager *_manIn, Mix_Chunk *_chunkIn, std::string _filename, bool _loopPlayback );
		//----------------------------------------------------------------------------------
		/// \brief Dtor
		//----------------------------------------------------------------------------------
		~AudioClip();
		//----------------------------------------------------------------------------------
		/// \brief Play method
		//----------------------------------------------------------------------------------
		void Play();
		//----------------------------------------------------------------------------------
		/// \brief Stop method
		//----------------------------------------------------------------------------------
		void Stop();
		//----------------------------------------------------------------------------------
		/// \brief Sets the filename of the auditory feedback sound file method
		//----------------------------------------------------------------------------------
		void SetFilename(std::string _filename) { m_filename = _filename; }

	protected:

		//----------------------------------------------------------------------------------
		/// \brief Chunk
		//----------------------------------------------------------------------------------
		Mix_Chunk *m_chunk;
		//----------------------------------------------------------------------------------
		/// \brief Whether to play in loop
		//----------------------------------------------------------------------------------
		bool m_loopPlayback;
		//----------------------------------------------------------------------------------
		/// \brief Link to audio manager
		//----------------------------------------------------------------------------------
		AudioManager *m_audioManager;
		//----------------------------------------------------------------------------------
		/// \brief Filename
		//----------------------------------------------------------------------------------
		std::string m_filename;
		//----------------------------------------------------------------------------------
		/// \brief Play channel
		//----------------------------------------------------------------------------------
		int m_playChannel;
		//----------------------------------------------------------------------------------

	};
}

#endif
