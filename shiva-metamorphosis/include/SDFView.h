#ifndef __SHIVA_GUISYSTEM_SDFVIEW__
#define __SHIVA_GUISYSTEM_SDFVIEW__

#include <GL/glew.h>
#include <GL/glu.h>
#include <cml/cml.h>
#include <boost/thread/thread.hpp>
#include <cmath>
#include <iostream>
#include <fstream>

#include "GUI/Views/View.h"
#include "Utility/GPUProgram.h"
#include "Utility/GPUVariable.h"
#include "GUI/Drawables/Drawable.h"
#include "VolumeLoader.h"
#include "VolumeRenderer/Camera.h"


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
	//----------------------------------------------------------------------------------
	/// \brief Set blend parameter
	/// \param [in] _value
	//----------------------------------------------------------------------------------
	void SetBlendParam( const float &_value ) { m_blendValue = _value; }
	//----------------------------------------------------------------------------------
	/// \brief Set rotation speed
	/// \param [in] _value
	//----------------------------------------------------------------------------------
	void SetRotationSpeed( const float &_value ) { m_rotationSpeed = _value; }
	//----------------------------------------------------------------------------------
	/// \brief Add world rotation offset in degrees
	/// \param [in] _rotX
	/// \param [in] _rotY
	/// \param [in] _rotZ
	//----------------------------------------------------------------------------------
	void AddWorldRotationOffsetDegs( const float &_rotX, const float &_rotY, const float &_rotZ );
	//----------------------------------------------------------------------------------
	/// \brief Add world rotation offset in radians
	/// \param [in] _rotX
	/// \param [in] _rotY
	/// \param [in] _rotZ
	//----------------------------------------------------------------------------------
	void AddWorldRotationOffsetRads( const float &_rotX, const float &_rotY, const float &_rotZ );
	//----------------------------------------------------------------------------------
	void AddRotationOffsetDegs(float rotX, float rotY, float rotZ);
	void AddRotationOffsetRads(float rotX, float rotY, float rotZ);

	void SetColour( const float &_r, const float &_g, const float &_b ) { m_colourR = _r; m_colourG = _g; m_colourB = _b; m_colourSet = 1.0f; }

	/// Should load either model A or model B from file and return true if successful, false if failed (and print out an error report to std::cerr)
	bool LoadVolumeFromFile(std::string filename, bool objectB, ShivaGUI::GUIController *guiController);

	/// Should save complete metamorphosis to file and return true if successful, false if failed (and print out an error report to std::cerr)
	bool SaveVolumeToFile(std::string filename);

	void SetPauseRotation( bool _value ) { m_pauseRotation = _value; }

	void SetQuality( float value );

	void BuildVBOs();

	void LoadMatricesToShader();
	void ThreadReturnVolumeFromFile( float *data, unsigned int dataSize, bool objectB );

protected:

	// Bounds for the View's place in the GUI System
	//----------------------------------------------------------------------------------
	int m_boundsLeft;
	//----------------------------------------------------------------------------------
	int m_boundsTop;
	//----------------------------------------------------------------------------------
	int m_boundsRight;
	//----------------------------------------------------------------------------------
	int m_boundsBottom;
	//----------------------------------------------------------------------------------
	int m_windowWidth;
	//----------------------------------------------------------------------------------
	int m_windowHeight;
	//----------------------------------------------------------------------------------
	/// Viewing matrices and associated variables
	cml::matrix44f_c _projectionMatrix, m_modelViewMatrix;
	//----------------------------------------------------------------------------------
	/// \brief Inverse ModelView matrix
	//----------------------------------------------------------------------------------
	cml::matrix44f_c m_invModelViewMatrix;
	//----------------------------------------------------------------------------------
	/// \brief Model matrix
	//----------------------------------------------------------------------------------
	cml::matrix44f_c m_modelMatrix;
	//----------------------------------------------------------------------------------
	cml::vector3f _cameraPosition, _targetPosition;
	SpringyVec3 m_target;
	//----------------------------------------------------------------------------------
	/// \brief Representation of x-axis
	//----------------------------------------------------------------------------------
	cml::vector4f m_axisX;
	//----------------------------------------------------------------------------------
	/// \brief Representation of y-axis
	//----------------------------------------------------------------------------------
	cml::vector4f m_axisY;
	//----------------------------------------------------------------------------------
	/// \brief Representaiton of z-axis
	//----------------------------------------------------------------------------------
	cml::vector4f m_axisZ;
	//----------------------------------------------------------------------------------
	// Local rotations in radians about the major coordinate axes
	//----------------------------------------------------------------------------------
	/// \brief Local rotation about the x-axis
	//----------------------------------------------------------------------------------
	float m_localRotX;
	//----------------------------------------------------------------------------------
	/// \brief Local rotation about the y-axis
	//----------------------------------------------------------------------------------
	float m_localRotY;
	//----------------------------------------------------------------------------------
	/// \brief Local rotation about the z-axis
	//----------------------------------------------------------------------------------
	float m_localRotZ;
	//----------------------------------------------------------------------------------
	/// \brief World rotation about the x-axis
	//----------------------------------------------------------------------------------
	float m_worldRotX;
	//----------------------------------------------------------------------------------
	/// \brief World rotation about the y-axis
	//----------------------------------------------------------------------------------
	float m_worldRotY;
	//----------------------------------------------------------------------------------
	/// \brief World rotation about the z-axis
	//----------------------------------------------------------------------------------
	float m_worldRotZ;
	//----------------------------------------------------------------------------------
	/// \brief Camera angle (fov)
	//----------------------------------------------------------------------------------
	float m_camAngle;
	//----------------------------------------------------------------------------------
	/// \brief Near plane
	//----------------------------------------------------------------------------------
	float m_camNearPlane;
	//----------------------------------------------------------------------------------
	/// \brief Far plane
	//----------------------------------------------------------------------------------
	float m_camFarPlane;
	//----------------------------------------------------------------------------------
	/// \brief For temporarily pausing the rotation
	//----------------------------------------------------------------------------------
	bool m_pauseRotation;
	//----------------------------------------------------------------------------------
	float m_colourR, m_colourG, m_colourB, m_colourSet;


	/// OpenGL Vertex Buffer Object IDs for a cube
	//----------------------------------------------------------------------------------
	GLuint m_cubeVAO;
	//----------------------------------------------------------------------------------
	unsigned int _cubeVertexVBO, _cubeIndexVBO;
	//----------------------------------------------------------------------------------
	/// Shaders - not too sure what it will do, so feel free to rename
	Utility::GPUProgram *m_SDFShader;

	Utility::GPUVariable *_vertSize, *_vertPos;
	//----------------------------------------------------------------------------------
	/// \brief Blend value
	//----------------------------------------------------------------------------------
	float m_blendValue;
	//----------------------------------------------------------------------------------
	/// \brief Parameter values
	//----------------------------------------------------------------------------------
	cml::vector4f m_parameterValues;
	//----------------------------------------------------------------------------------
	/// \brief Speed of rotation
	//----------------------------------------------------------------------------------
	float m_rotationSpeed;
	//----------------------------------------------------------------------------------
	unsigned int m_volATicket, m_volBTicket;
	VolumeLoader *m_dataLoader;
	


	unsigned int m_texVolA, m_texVolB;
	//----------------------------------------------------------------------------------
	/// \brief Object A filename
	//----------------------------------------------------------------------------------
	std::string m_objectAfilename;
	//----------------------------------------------------------------------------------
	/// \brief Object B filename
	//----------------------------------------------------------------------------------
	std::string m_objectBfilename;
	//----------------------------------------------------------------------------------
	ShivaGUI::Drawable *_loadingDrawable;
	//----------------------------------------------------------------------------------
	Camera* m_cam;
	//----------------------------------------------------------------------------------

private :

	bool _load_volume_from_loader(VolumeLoader *, unsigned int glTexID);

	enum constants { number_of_models = 2 };
	cml::vector3f m_boundMin[number_of_models];
	cml::vector3f m_boundMax[number_of_models];

};

#endif
