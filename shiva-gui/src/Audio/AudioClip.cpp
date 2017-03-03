#include "Audio/AudioClip.h"
#include "Audio/AudioManager.h"

//----------------------------------------------------------------------------------

ShivaGUI::AudioClip::AudioClip( AudioManager *_manIn, Mix_Chunk *_chunkIn, std::string _filename, bool _loopPlayback )
{
	m_chunk = _chunkIn;
	m_loopPlayback = _loopPlayback;
	m_audioManager = _manIn;
	m_playChannel = -1;
}

//----------------------------------------------------------------------------------

ShivaGUI::AudioClip::~AudioClip()
{
	// We must not delete the _chunk here because it is a shared resource
	// Let the AudioManager take care of this:
	m_audioManager->FreeSample( m_filename );
}

//----------------------------------------------------------------------------------

void ShivaGUI::AudioClip::Play()
{
	m_playChannel = Mix_PlayChannel( -1, m_chunk, m_loopPlayback ? -1 : 0 );
}

//----------------------------------------------------------------------------------

void ShivaGUI::AudioClip::Stop()
{
	Mix_HaltChannel( m_playChannel );
}

//----------------------------------------------------------------------------------
