
#include "VolumeLoader.h"
#include "vol_metamorph.h"

#define DEFAULT_RESOLUTION 128

//----------------------------------------------------------------------------------

VolumeLoader::VolumeLoader()
{
	m_processing = false;
	m_topTicket = 1;
	m_activeJob = NULL;
	m_dataLoadThread = NULL;
}

//----------------------------------------------------------------------------------

VolumeLoader::~VolumeLoader()
{
	if( m_dataLoadThread != NULL )
		m_dataLoadThread->join();
}

//----------------------------------------------------------------------------------

unsigned int VolumeLoader::AddJobLoad( std::string _filename, unsigned int _depth, unsigned int _width, unsigned int _height )
{
	unsigned int currentTicket = IssueNewTicket();
	JobData *currentJob = new JobData( currentTicket, _filename, _depth, _width, _height );

	m_waitingQueueMtx.lock();
		m_waitingQueue.push_back(currentJob);
	m_waitingQueueMtx.unlock();

	ThreadLauncher();

	return currentTicket;
}

//----------------------------------------------------------------------------------

bool VolumeLoader::IsJobReady( unsigned int _ticket )
{
	return GetFinishedJobData( _ticket ) != NULL;
}

//----------------------------------------------------------------------------------

void VolumeLoader::GetJobToGL( unsigned int _ticket, unsigned int _glTexID )
{
	JobData *currentJob = GetFinishedJobData( _ticket );
	if( currentJob != NULL )
	{
		glBindTexture( GL_TEXTURE_3D, _glTexID );

		// Clamp to edges
		glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP );
		glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP );
		glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP );
		// Trilinear filtering
		glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

		glTexImage3D( GL_TEXTURE_3D, 0, GL_ALPHA32F_ARB, currentJob->m_depth, currentJob->m_height, currentJob->m_width, 0, GL_ALPHA, GL_FLOAT, ( GLvoid * )currentJob->m_data );
	}
}

//----------------------------------------------------------------------------------

void VolumeLoader::GetJobBounds( unsigned int _ticket, cml::vector3f &_boundMin, cml::vector3f &_boundMax )
{
	JobData *currentJob = GetFinishedJobData( _ticket );
	if( currentJob != NULL )
	{
		_boundMin[ 0 ] = -1.2f;
		_boundMin[ 1 ] = -1.2f;
		_boundMin[ 2 ] = -1.2f;
		_boundMax[ 0 ] =  1.2f;
		_boundMax[ 1 ] =  1.2f;
		_boundMax[ 2 ] =  1.2f;
	}
}

//----------------------------------------------------------------------------------

VolumeLoader::JobData* VolumeLoader::GetFinishedJobData( unsigned int _ticket )
{
	m_finishedQueueMtx.lock();

		JobData *foundData = NULL;
		for( std::vector< JobData* >::iterator it = m_finishedQueue.begin(); ( it != m_finishedQueue.end() ) && ( foundData == NULL ); ++it )
		{
			if( ( *it )->m_ticket == _ticket )
			{
				foundData = ( *it );
			}
		}

	m_finishedQueueMtx.unlock();

	return foundData;
}

//----------------------------------------------------------------------------------

void VolumeLoader::ThreadLauncher()
{
	// There is data in the queue, we need to get the thread going to process it
	// If thread is already running, it will simply find hte next job, so do nothing

	if( !m_processing )
	{
		if( m_dataLoadThread != NULL )
		{
			delete m_dataLoadThread;
		}
		m_processing = true;
		m_dataLoadThread = new boost::thread( boost::bind( &VolumeLoader::ThreadProcess, this ) );
	}
}

//----------------------------------------------------------------------------------

void VolumeLoader::ThreadProcess()
{
	m_processing = true;
	std::cout << "INFO: VolumeLoader Processing" << std::endl;

	m_waitingQueueMtx.lock();
	
	do
	{
		m_activeJob = m_waitingQueue.front();
		m_waitingQueue.erase( m_waitingQueue.begin() );

		m_waitingQueueMtx.unlock();

		std::cout << "INFO: Attempting to load vol file: " << m_activeJob->m_filename << ": " << m_activeJob->m_width << "x" << m_activeJob->m_height << "x" << m_activeJob->m_depth << std::endl;

		float* bbox = NULL;
		open_params tmp;
		tmp.m_fit_to_box = true;
		tmp.m_calculate_bbox = true;
		tmp.m_fill_array = true;
		tmp.m_exact_bbox = true;

		if ( !openVol( m_activeJob->m_filename.c_str(), m_activeJob->m_depth, m_activeJob->m_width, m_activeJob->m_height, tmp, &bbox, &( m_activeJob->m_data ) ) )
		{
			std::cout << "WARNING: Failed to load .vol file: " << m_activeJob->m_filename << std::endl;
		}
		else
		{
			std::cout << "INFO: Unpacked texture " << m_activeJob->m_filename << ": " << m_activeJob->m_width << "x" << m_activeJob->m_height << "x" << m_activeJob->m_depth << " in bounds (";
			std::cout << bbox[ 0 ] << "," << bbox[ 1 ] << "," << bbox[ 2 ] << ")<(" << bbox[ 3 ] << "," << bbox[ 4 ] << "," << bbox[ 5 ] << ")" << std::endl;

			m_activeJob->m_boundMin[ 0 ] = bbox[ 0 ];
			m_activeJob->m_boundMin[ 1 ] = bbox[ 1 ];
			m_activeJob->m_boundMin[ 2 ] = bbox[ 2 ];
			m_activeJob->m_boundMax[ 0 ] = bbox[ 3 ];
			m_activeJob->m_boundMax[ 1 ] = bbox[ 4 ];
			m_activeJob->m_boundMax[ 2 ] = bbox[ 5 ];
		}

		freePt( &bbox );

		m_finishedQueueMtx.lock();
			m_finishedQueue.push_back( m_activeJob );
		m_finishedQueueMtx.unlock();


		m_activeJob = NULL;

		m_waitingQueueMtx.lock();
	}
	while( !m_waitingQueue.empty() );

	m_processing = false;
	
	m_waitingQueueMtx.unlock();
}

//----------------------------------------------------------------------------------

unsigned int VolumeLoader::IssueNewTicket()
{
	return m_topTicket++;
}

//----------------------------------------------------------------------------------

VolumeLoader::JobData::JobData( unsigned int _ticket, std::string _filename, unsigned int _depth, unsigned int _width, unsigned int _height )
{
	m_ticket = _ticket;
	m_filename = _filename;
	m_depth = _depth;
	m_width = _width;
	m_height = _height;
	m_data = NULL;
}

//----------------------------------------------------------------------------------

VolumeLoader::JobData::~JobData()
{
	freePt( &m_data );
}

//----------------------------------------------------------------------------------

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
