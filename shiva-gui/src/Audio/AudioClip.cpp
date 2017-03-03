
#include "Audio/AudioClip.h"

#include "Audio/AudioManager.h"


ShivaGUI::AudioClip::AudioClip(AudioManager *manIn, Mix_Chunk *chunkIn, std::string filename, bool loopPlayback)
{
	_chunk = chunkIn;
	_loopPlayback = loopPlayback;
	m_audioManager = manIn;
	_playChannel = -1;
}

ShivaGUI::AudioClip::~AudioClip()
{
	// We must not delete the _chunk here because it is a shared resource
	// Let the AudioManager take care of this:
	m_audioManager->FreeSample(_filename);
}

void ShivaGUI::AudioClip::Play()
{
	_playChannel = Mix_PlayChannel(-1,_chunk,_loopPlayback ? -1 : 0);
}

void ShivaGUI::AudioClip::Stop()
{
	Mix_HaltChannel(_playChannel);
}
