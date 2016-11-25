#include "Audio/AudioManager.h"
#include "Audio/AudioController.h"
#include "Audio/AudioClip.h"

#include <iostream>

ShivaGUI::AudioManager::AudioManager()
{
	_audioCapable = false;
	int audio_rate = 22050;
	Uint16 audio_format = AUDIO_S16SYS;
	int audio_channels = 2;
	int audio_buffers = 1024; // 4096 gives a noticable delay

	if( Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) == -1 )
	{
		std::cerr<<"ERROR: Cannot open audio. Sound disabled."<<std::endl;
		std::cerr<<"       SDL Mixer Error: "<<Mix_GetError()<<std::endl;
	}
	else
	{
		_audioCapable = true;

		int frequency, channels;
		Uint16 format;
		Mix_QuerySpec(&frequency, &format, &channels);

		// Taken from SDL documentation:
		std::string format_str="Unknown";
		switch(format)
		{
			case AUDIO_U8: format_str="Unsigned 8-bit"; break;
			case AUDIO_S8: format_str="Signed 8-bit"; break;
			case AUDIO_U16LSB: format_str="Unsigned 16-bit Little-endian"; break;
			case AUDIO_S16LSB: format_str="Signed 16-bit Little-endian"; break;
			case AUDIO_U16MSB: format_str="Unsigned 16-bit Big-endian"; break;
			case AUDIO_S16MSB: format_str="Signed 16-bit Big-endian"; break;
			//case AUDIO_U16SYS: format_str="Unsigned 16-bit System-endian"; break;
			//case AUDIO_S16SYS: format_str="Signed 16-bit System-endian"; break;
		}

		std::cout<<"INFO: Audio Initialised. Settings:"<<std::endl;
		std::cout<<"      Frequency: "<<frequency<<std::endl;
		std::cout<<"      Format: "<<format_str<<std::endl;
		std::cout<<"      Channels: "<<channels<<std::endl;
	}

}

ShivaGUI::AudioManager::~AudioManager()
{
	if( !_loadedChunks.empty() )
	{
		std::cerr<<"WARNING: Likely Memory Leak: Closing AudioManager while AudioClips are still allocated."<<std::endl;
	}
	Mix_CloseAudio();
}
/*
ShivaGUI::AudioController* ShivaGUI::AudioManager::CreateAudioController()
{
	AudioController *controller = new AudioController(this);
	return controller;
}
*/
ShivaGUI::AudioClip* ShivaGUI::AudioManager::GetSample(std::string filename)
{
	Mix_Chunk *chunk = GetChunk(filename);

	AudioClip *clip = new AudioClip(this,chunk,filename,false);

	return clip;
}

void ShivaGUI::AudioManager::FreeSample(std::string filename)
{
	FreeChunk(filename);
}

Mix_Chunk* ShivaGUI::AudioManager::GetChunk(std::string filename)
{
	if( !_audioCapable )
		return NULL;

	// Does it exist in _loadedChunks?
	if( _loadedChunks.find(filename) != _loadedChunks.end() )
	{
		_sampleRefCount[filename] += 1;
		return _loadedChunks[filename];
	}
	else
	{
		Mix_Chunk *chunk = Mix_LoadWAV(filename.c_str());
		if( chunk != NULL )
		{
			_loadedChunks[filename] = chunk;
			_sampleRefCount[filename] = 1;
			return chunk;
		}
		else
		{
			std::cerr<<"WARNING: AudioManager could not load audio file: "<<filename<<std::endl;
			std::cerr<<"         SDL Error: "<<Mix_GetError()<<std::endl;
			return NULL;
		}
	}
}

void ShivaGUI::AudioManager::FreeChunk(std::string filename)
{
	if( !_audioCapable )
		return;

	int numReferences = (--_sampleRefCount[filename]);
	if( numReferences <= 0 )
	{
		Mix_FreeChunk( _loadedChunks[filename] );
		_loadedChunks.erase(filename);
		_sampleRefCount.erase(filename);
	}
}
