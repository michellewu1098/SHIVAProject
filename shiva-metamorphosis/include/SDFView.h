
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef __SHIVA_GUISYSTEM_SDFVIEW__
#define __SHIVA_GUISYSTEM_SDFVIEW__
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#include <cml/cml.h>
#include <boost/thread/thread.hpp>

//////////////////////////////////////////////////////////////////////////
#include "GUI/Views/View.h"
#include "Utility/GPUProgram.h"
#include "Utility/GPUVariable.h"
//#include "GLVolumeTexture.h"
#include "GUI/Drawables/Drawable.h"
//#include "vol_metamorph.h"

#include "VolumeLoader.h"

//////////////////////////////////////////////////////////////////////////

/// This is for displaying 3D objects
class SDFView : public ShivaGUI::View
{
public:
	static ShivaGUI::View* Factory() {return new SDFView();}

	SDFView();
	virtual ~SDFView();

	/// Gives the size of the View
	/// If this View has children, it is expected to work out the size and location of these and call Layout() on them too
	virtual void Layout(int left, int top, int right, int bottom, int windowWidth, int windowHeight);

	/// For setting the View's attributes from xml
	virtual void Inflate(TiXmlElement*,ShivaGUI::ResourceManager*, std::string themePrefix = "", bool rootNode = false);

	virtual void Update(float deltaTs, ShivaGUI::GUIController *guiController);

	virtual void Draw();

	/// Themes allow settings to be set for all Views of a given type
	/// The string returned from this function is the string that identifies the type in the theme xml file
	/// It is literally prefixed to normal inflation attributes
	virtual std::string GetThemePrefix() {return "SDFView_";}

	// This View does not accept focus

	/// Input event given to View, expected to filter down hierarchy
	/// \return false if event is not absorbed (e.g. did not hit button etc)
	virtual bool HandleEvent(ShivaGUI::InternalEvent*);


	void SetBlendParam(float value) {_blendValue=value;}

	void SetRotationSpeed(float value) {_rotationSpeed=value;}

	void AddWorldRotationOffsetDegs(float rotX, float rotY, float rotZ);
	void AddWorldRotationOffsetRads(float rotX, float rotY, float rotZ);

	void AddRotationOffsetDegs(float rotX, float rotY, float rotZ);
	void AddRotationOffsetRads(float rotX, float rotY, float rotZ);

	void SetColour(float R, float G, float B) {_colourR=R;_colourG=G;_colourB=B;_colourSet=1.0f;}

	/// Should load either model A or model B from file and return true if successful, false if failed (and print out an error report to std::cerr)
	bool LoadVolumeFromFile(std::string filename, bool objectB, ShivaGUI::GUIController *guiController);

	/// Should save complete metamorphosis to file and return true if successful, false if failed (and print out an error report to std::cerr)
	bool SaveVolumeToFile(std::string filename);

	void SetPauseRotation( bool value ) {_pauseRotation = value;}

	void SetQuality( float value );



	void ThreadReturnVolumeFromFile( float *data, unsigned int dataSize, bool objectB );

protected:

	/// Bounds for the View's place in the GUI System
	int _boundsLeft, _boundsTop, _boundsRight, _boundsBottom, _windowWidth, _windowHeight;

	/// Viewing matrices and associated variables
	cml::matrix44f_c _projectionMatrix, _modelviewMatrix, _invModelview, _worldMatrix;
	cml::vector3f _cameraPosition, _targetPosition;
	cml::vector4f _axisX, _axisY, _axisZ;
	/// Local rotations in radians about the major coordinate axes
	float _rotationX, _rotationY, _rotationZ;
	float _rotationWorldX, _rotationWorldY, _rotationWorldZ;
	float _cameraAngle, _cameraNearPlane, _cameraFarPlane;

	/// For temporarily pausing the rotation
	bool _pauseRotation;

	float _colourR, _colourG, _colourB, _colourSet;


	/// OpenGL Vertex Buffer Object IDs for a cube
	unsigned int _cubeVertexVBO, _cubeIndexVBO;

	/// Shaders - not too sure what it will do, so feel free to rename
	Utility::GPUProgram *_sdfProgram;

	Utility::GPUVariable *_projMatrixUniform, *_mvMatrixUniform, *_invViewMatrixUniform;
	Utility::GPUVariable *_vertSize, *_vertPos;

	float _blendValue;
	Utility::GPUVariable *_fragboundmin, *_fragboundmax, *_blendUniform;
	Utility::GPUVariable *_a0Uniform;
	Utility::GPUVariable *_a1Uniform;
	Utility::GPUVariable *_a2Uniform;
	Utility::GPUVariable *_a3Uniform;
	Utility::GPUVariable *_clipdistUniform;
	Utility::GPUVariable *_timegapUniform ;
	Utility::GPUVariable *_texture0, *_texture1;
	Utility::GPUVariable *_colourUniform;
	cml::vector4f _parameterValues;

	float _rotationSpeed;


//	bool _loadingDataA, _loadingDataB;
//	bool _dataAReady, _dataBReady;
//	VolumeLoader *_dataALoader, *_dataBLoader;
//	boost::thread  *_dataALoadThread, *_dataBLoadThread;

	
	unsigned int _volATicket, _volBTicket;
	VolumeLoader *_dataLoader;
	


	unsigned int _texVolA, _texVolB;

	std::string _objectAfilename, _objectBfilename;

	ShivaGUI::Drawable *_loadingDrawable;

private :
	bool _load_volume_from_loader(VolumeLoader *, unsigned int glTexID);

	enum constants { number_of_models = 2 };
	//sdf::signed_distance_field_from_mesh<float> _models[number_of_models];
//	GLVolumeTexture _volume_data[number_of_models];
	cml::vector3f _bound_min[number_of_models],_bound_max[number_of_models];

};

//////////////////////////////////////////////////////////////////////////
#endif
