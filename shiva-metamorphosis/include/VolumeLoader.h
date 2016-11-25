
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef __SHIVA_VOLUME_LOADER__
#define __SHIVA_VOLUME_LOADER__
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#include <cml/cml.h>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <vector>

//////////////////////////////////////////////////////////////////////////
#include "GUI/Views/View.h"
#include "Utility/GPUProgram.h"
#include "Utility/GPUVariable.h"
#include "GLVolumeTexture.h"
//#include <sdf/signed_distance_field_from_mesh.hpp>

//////////////////////////////////////////////////////////////////////////

/// Asynchronous volume object loader
/// Give it a job, it gives you a 'ticket'
/// Come back regularly to check if the ticket is ready
class VolumeLoader
{
public:

	VolumeLoader();
	~VolumeLoader();

	/// Returns true if loading, false once finished all jobs (i.e. idle)
	bool GetIsProcessing() {return _processing;}


	/// Returns a 'ticket'
	unsigned int AddJobLoad(std::string filename, unsigned int depth = 128, unsigned int width = 128, unsigned int height = 128);


	bool IsJobReady( unsigned int ticket );


	/// Loads a ready volume into a given OpenGL texture ID
	void GetJobToGL(unsigned int ticket, unsigned int glTexID);

	/// Loads a ready volume's bounding box info
	void GetJobBounds(unsigned int ticket, cml::vector3f &boundMin, cml::vector3f &boundMax);

/*
	/// Will have no effect if already processing
	void SetFilename(std::string filename);
	std::string GetFilename() {return _fileToLoad;}



	void LoadToGL(unsigned int glTexID);
	void LoadBounds( cml::vector3f &boundMin, cml::vector3f &boundMax);
*/


	/// Required for boost::thread
	/// This is where the loading and processing will take place
	void ThreadProcess();


protected:

	class JobData
	{
	public:
		JobData(unsigned int ticket, std::string filename, unsigned int depth, unsigned int width, unsigned int height);
		~JobData();

		bool operator== (const unsigned int rhs) {return _ticket == rhs;}

		unsigned int _ticket;
		std::string _filename;
		unsigned int _depth, _width, _height;
		float *_data;
		cml::vector3f _boundMin, _boundMax;
	};

	JobData* GetFinishedJobData(unsigned int ticket);

	unsigned int IssueNewTicket();

	/// Call this when issuing a new job, just to make sure the loading thread is going
	void ThreadLauncher();


	unsigned int _topTicket;

	bool _processing;

	std::vector<JobData*> _waitingQueue, _finishedQueue;
	JobData *_activeJob;
	boost::mutex _waitingQueueMtx, _finishedQueueMtx;


	boost::thread  *_dataLoadThread;


/*
	std::string _fileToLoad;

	unsigned int _depth, _width, _height;


	float *_data;
	
	cml::vector3f _boundMin, _boundMax;
*/
};

//////////////////////////////////////////////////////////////////////////
#endif
