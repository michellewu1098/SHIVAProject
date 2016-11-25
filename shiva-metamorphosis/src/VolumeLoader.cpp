
#include "VolumeLoader.h"
#include "vol_metamorph.h"

#include <iostream>
#include <algorithm>

// Just temporary so I can use SDL_Delay() for testing
#include <SDL.h>
#include <GL/GLee.h>
#include <GL/glu.h>

#define DEFAULT_RESOLUTION 128

VolumeLoader::VolumeLoader()
{
	_processing = false;

	_topTicket = 1;

	_activeJob = NULL;

	_dataLoadThread = NULL;
	
}

VolumeLoader::~VolumeLoader()
{
	if( _dataLoadThread != NULL )
		_dataLoadThread->join();

}


unsigned int VolumeLoader::AddJobLoad(std::string filename, unsigned int depth, unsigned int width, unsigned int height)
{
	unsigned int currentTicket = IssueNewTicket();
	JobData *currentJob = new JobData(currentTicket, filename,depth,width,height);

	_waitingQueueMtx.lock();
		_waitingQueue.push_back(currentJob);
	_waitingQueueMtx.unlock();

	ThreadLauncher();

	return currentTicket;
}



bool VolumeLoader::IsJobReady( unsigned int ticket )
{
	return GetFinishedJobData(ticket) != NULL;
}

void VolumeLoader::GetJobToGL(unsigned int ticket, unsigned int glTexID)
{
	JobData *currentJob = GetFinishedJobData(ticket);
	if( currentJob != NULL )
	{
		glBindTexture(GL_TEXTURE_3D, glTexID);

		// Clamp to edges
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		// Trilinear filtering
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage3D(GL_TEXTURE_3D, 0, GL_ALPHA32F_ARB, currentJob->_depth, currentJob->_height, currentJob->_width, 0, GL_ALPHA, GL_FLOAT, (GLvoid *) currentJob->_data);
	}
}


void VolumeLoader::GetJobBounds(unsigned int ticket, cml::vector3f &boundMin, cml::vector3f &boundMax)
{
	JobData *currentJob = GetFinishedJobData(ticket);
	if( currentJob != NULL )
	{
		boundMin[0] = -1.2f;
		boundMin[1] = -1.2f;
		boundMin[2] = -1.2f;
		boundMax[0] =  1.2f;
		boundMax[1] =  1.2f;
		boundMax[2] =  1.2f;
		/*
		boundMin[0] = currentJob->_boundMin[0];
		boundMin[1] = currentJob->_boundMin[1];
		boundMin[2] = currentJob->_boundMin[2];
		boundMax[0] = currentJob->_boundMax[0];
		boundMax[1] = currentJob->_boundMax[1];
		boundMax[2] = currentJob->_boundMax[2];
		*/
	}
}


VolumeLoader::JobData* VolumeLoader::GetFinishedJobData(unsigned int ticket)
{
	_finishedQueueMtx.lock();

		JobData *foundData = NULL;
		for( std::vector<JobData*>::iterator it = _finishedQueue.begin(); (it != _finishedQueue.end()) && (foundData == NULL); ++it )
		{
			if( (*it)->_ticket == ticket )
			{
				foundData = (*it);
			}
		}

	_finishedQueueMtx.unlock();

	return foundData;
}


void VolumeLoader::ThreadLauncher()
{
	// There is data in the queue, we need to get the thread going to process it
	// If thread is already running, it will simply find hte next job, so do nothing

	if( !_processing )
	{
		if( _dataLoadThread != NULL )
		{
			delete _dataLoadThread;
		}
		_processing = true;
		_dataLoadThread = new boost::thread( boost::bind(&VolumeLoader::ThreadProcess, this) );//new boost::thread(boost::ref(this));
	}

}


void VolumeLoader::ThreadProcess()
{
	_processing = true;
	std::cout<<"INFO: VolumeLoader Processing"<<std::endl;


	_waitingQueueMtx.lock();
	do
	{
		_activeJob = _waitingQueue.front();
		_waitingQueue.erase( _waitingQueue.begin() );

		_waitingQueueMtx.unlock();


		std::cout<<"INFO: Attempting to load vol file: "<<_activeJob->_filename<<": "<<_activeJob->_width<<"x"<<_activeJob->_height<<"x"<<_activeJob->_depth<<std::endl;


		float* bbox = NULL;
		open_params tmp;
		tmp.m_fit_to_box = true;
		tmp.m_calculate_bbox = true;
		tmp.m_fill_array = true;
		tmp.m_exact_bbox = true;

		if (!openVol(_activeJob->_filename.c_str(), _activeJob->_depth, _activeJob->_width, _activeJob->_height, tmp, &bbox, &(_activeJob->_data)))
		{
			std::cout<<"WARNING: Failed to load .vol file: "<<_activeJob->_filename<<std::endl;
		}
		else
		{
			
			std::cout<<"INFO: Unpacked texture "<<_activeJob->_filename<<": "<<_activeJob->_width<<"x"<<_activeJob->_height<<"x"<<_activeJob->_depth<<" in bounds (";
			std::cout<<bbox[0]<<","<<bbox[1]<<","<<bbox[2]<<")<("<<bbox[3]<<","<<bbox[4]<<","<<bbox[5]<<")"<<std::endl;

			_activeJob->_boundMin[0]=bbox[0];
			_activeJob->_boundMin[1]=bbox[1];
			_activeJob->_boundMin[2]=bbox[2];
			_activeJob->_boundMax[0]=bbox[3];
			_activeJob->_boundMax[1]=bbox[4];
			_activeJob->_boundMax[2]=bbox[5];
		}

		freePt(&bbox);

		_finishedQueueMtx.lock();
			_finishedQueue.push_back(_activeJob);
		_finishedQueueMtx.unlock();


		_activeJob = NULL;

		_waitingQueueMtx.lock();
	}
	while( !_waitingQueue.empty() );

	_processing = false;
	
	_waitingQueueMtx.unlock();
}



unsigned int VolumeLoader::IssueNewTicket()
{
	return _topTicket++;
}



VolumeLoader::JobData::JobData(unsigned int ticket, std::string filename, unsigned int depth, unsigned int width, unsigned int height)
{
	_ticket = ticket;
	_filename = filename;
	_depth = depth;
	_width = width;
	_height = height;
	_data = NULL;
}

VolumeLoader::JobData::~JobData()
{
	freePt(&_data);
}





#if 0


void VolumeLoader::operator()()
{
	_processing = true;
	std::cout<<"VolumeLoader Processing"<<std::endl;

	_depth = DEFAULT_RESOLUTION;
	_width = DEFAULT_RESOLUTION;
	_height = DEFAULT_RESOLUTION;
	float* bbox = NULL;
	if (!openVol(_fileToLoad.c_str(), _depth, _width, _height, &bbox, &_data))
	{
		std::cout<<"Failed to load .vol file"<<std::endl;
		return;
	}
	
	std::cout<<"INFO: Unpacked texture "<<_fileToLoad<<": "<<_width<<"x"<<_height<<"x"<<_depth<<" in bounds (";
	std::cout<<bbox[0]<<","<<bbox[1]<<","<<bbox[2]<<")<("<<bbox[3]<<","<<bbox[4]<<","<<bbox[5]<<")"<<std::endl;

	_boundMin[0]=bbox[0];
	_boundMin[1]=bbox[1];
	_boundMin[2]=bbox[2];
	_boundMax[0]=bbox[3];
	_boundMax[1]=bbox[4];
	_boundMax[2]=bbox[5];

	freePt(&bbox);
	_processing = false;
}

void VolumeLoader::LoadToGL(unsigned int glTexID)
{
	glBindTexture(GL_TEXTURE_3D, glTexID);

	// Clamp to edges
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	// Trilinear filtering
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage3D(GL_TEXTURE_3D, 0, GL_ALPHA32F_ARB, _depth, _height, _width, 0, GL_ALPHA, GL_FLOAT, (GLvoid *) _data);

	// Can now delete _data
	freePt(&_data);
	_data = NULL;
}

void VolumeLoader::LoadBounds( cml::vector3f &boundMin, cml::vector3f &boundMax)
{
	boundMin[0] = _boundMin[0];
	boundMin[1] = _boundMin[1];
	boundMin[2] = _boundMin[2];
	boundMax[0] = _boundMax[0];
	boundMax[1] = _boundMax[1];
	boundMax[2] = _boundMax[2];
}

#endif
