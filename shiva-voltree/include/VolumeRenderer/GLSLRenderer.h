///-----------------------------------------------------------------------------------------------
/// \file GLSLRenderer.h
/// \brief Volume tree renderer class
/// \author Leigh McLoughlin, Michelle Wu
/// \date Jan 10, 2013
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef GLSLRENDERER_H_
#define GLSLRENDERER_H_

#include <GL/glew.h>
#include <GL/glu.h>
#include <sstream>

#include "VolumeRenderer/Camera.h"
#include "VolumeTree/VolumeTree.h"
#include "VolumeRenderer/Shader.h"

class GLSLRenderer
{
public:

	//----------------------------------------------------------------------------------
	/// \brief Ctor
	/// \param [in] _width
	/// \param [in] _height
	//----------------------------------------------------------------------------------
	GLSLRenderer( unsigned int _width, unsigned int _height );
	//----------------------------------------------------------------------------------
	/// \brief Dtor
	//----------------------------------------------------------------------------------
	~GLSLRenderer();
	//----------------------------------------------------------------------------------
	/// \brief Set width and height of window
	/// \param [in] _width
	/// \param [in] _height
	//----------------------------------------------------------------------------------
	void SetWindowDimensions( const float &_width, const float &_height );
	//----------------------------------------------------------------------------------
	/// \brief Set volume tree
	/// \param [in] _tree
	//----------------------------------------------------------------------------------
	void SetTree( VolumeTree::Tree* _tree );
	//----------------------------------------------------------------------------------
	/// \brief Function to initialise vertex and fragment shaders
	/// \param [in] _vsFile Vertex shader file
	/// \param [in] _fsPartFile Fragment shader file
	//----------------------------------------------------------------------------------
	bool Initialise( std::string _vsFile, std::string _fsPartFile );
	//----------------------------------------------------------------------------------
	/// \brief Function that recompiles the fragment shader for the new tree. 
	/// Must be called if the tree has changed
	//----------------------------------------------------------------------------------
	bool RebuildTree();
	//----------------------------------------------------------------------------------
	/// \brief Draw method
	//----------------------------------------------------------------------------------
	void Draw();
	//----------------------------------------------------------------------------------
	void Draw( unsigned int _context );
	//----------------------------------------------------------------------------------
	/// \brief Update method
	/// \param [in] _deltaTs Time step
	//----------------------------------------------------------------------------------
	void Update( float _deltaTs );
	//----------------------------------------------------------------------------------
	/// \brief Set step sizes
	/// \param [in] _stepsize
	//----------------------------------------------------------------------------------
	void SetStepsize( const float &_stepsize );
	//----------------------------------------------------------------------------------
	/// \brief Set the object colour (values expected between 0 and 1)
	/// \param [in] _r Red component
	/// \param [in] _g Green component
	/// \param [in] _b Blue component
	//----------------------------------------------------------------------------------
	void SetObjectColour( const float &_r, const float &_g, const float &_b ) { m_objColourR = _r; m_objColourG = _g; m_objColourB = _b; }
	//----------------------------------------------------------------------------------
	
	// TRANSFORMATION-RELATED FUNCTIONS
	
	//----------------------------------------------------------------------------------
	/// \brief Reset world rotation
	//----------------------------------------------------------------------------------
	void ResetWorldRotation();
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
	/// \brief Set world angular velocity in degrees
	/// \param [in] _rotX
	/// \param [in] _rotY
	/// \param [in] _rotZ
	//----------------------------------------------------------------------------------
	void SetWorldAngularVelocityDegs( const float &_rotX, const float &_rotY, const float &_rotZ );
	//----------------------------------------------------------------------------------
	/// \brief Set world angular velocity in radians
	/// \param [in] _rotX
	/// \param [in] _rotY
	/// \param [in] _rotZ
	//----------------------------------------------------------------------------------
	void SetWorldAngularVelocityRads( const float &_rotX, const float &_rotY, const float &_rotZ );
	//----------------------------------------------------------------------------------
	/// \brief Give it mouse position (in window coord space) and view params,
	/// will give you an origin and direction in 3D (world space)
	/// \param [in] _mouseX Mouse x-coord
	/// \param [in] _mouseY Mouse y-coord
	/// \param [in] _viewOriginX
	/// \param [in] _viewOriginY
	/// \param [in] _viewWidth
	/// \param [in] _viewHeight
	/// \param [out] _originX
	/// \param [out] _originY
	/// \param [out] _originZ
	/// \param [out] _dirX
	/// \param [out] _dirY
	/// \param [out] _dirZ
	//----------------------------------------------------------------------------------
	void Unproject(	float _mouseX, float _mouseY, int _viewOriginX, int _viewOriginY, int _viewWidth, int _viewHeight,
					float &_originX, float &_originY, float &_originZ, float &_dirX, float &_dirY, float &_dirZ );
	//----------------------------------------------------------------------------------
	/// \brief Zoom camera
	/// \param [in] _value Amount of zoom
	//----------------------------------------------------------------------------------
	void ZoomCamera( const float &_value );
	//----------------------------------------------------------------------------------
	/// \brief Set camera angle (fov)
	/// \param [in] _angleDegrees
	//----------------------------------------------------------------------------------
	void SetCameraAngle( const float &_angleDegrees );
	//----------------------------------------------------------------------------------
	/// \brief Returns the distance from the camera to the origin
	//----------------------------------------------------------------------------------
	float GetCameraDistance();
	//----------------------------------------------------------------------------------

	// CACHING

	//----------------------------------------------------------------------------------
	// The renderer just provides the mechanisms to allow you to use caches
	// It allows you to query how much memory is available etc
	// But the decision on what should be cached, and how many caches etc, is down to the CachingPolicy
	//----------------------------------------------------------------------------------
	/// \brief After you've decided how many caches you want to use, call this function to create them
	/// Also acts to clear all caches that already exist
	/// \param [in] _nChaches
	//----------------------------------------------------------------------------------
	void ReserveCaches( unsigned int _nCaches );
	//----------------------------------------------------------------------------------
	/// \brief Returns the number of reserved caches
	/// \return m_maxCaches
	//----------------------------------------------------------------------------------
	unsigned int GetNumReservedCaches() const { return m_maxCaches; }
	//----------------------------------------------------------------------------------
	/// \brief Returns the optimal total number of voxels that should be used for caching
	/// This is used when working out the resolution of the caches
	//----------------------------------------------------------------------------------
	unsigned int GetMaxCachingVoxels();
	//----------------------------------------------------------------------------------
	/// \brief Fill cache
	/// \param [in] _cacheID Cache ID
	/// \param [in] _data
	/// \param [in] _sizeX
	/// \param [in] _sizeY
	/// \param [in] _sizeZ
	//----------------------------------------------------------------------------------
	void FillCache( unsigned int _cacheID, float* _data, unsigned int _sizeX, unsigned int _sizeY, unsigned int _sizeZ );
	//----------------------------------------------------------------------------------
	/// \brief Free cache selected using _cacheID input
	/// \param [in] _cacheID Cache ID
	//----------------------------------------------------------------------------------
	void FreeCache( unsigned int _cacheID );
	//----------------------------------------------------------------------------------

	// PARAMETERS

	//----------------------------------------------------------------------------------
	// Instead of rebuilding the tree after parameter changes, you can retrieve parameters
	// TODO: Clean me! If I'd realised how much junk would be in here, I'd have maybe put it in another class...
	//----------------------------------------------------------------------------------
	/// \brief Paramaters type. If this enum is changed the functions immediately below MUST be updated
	//----------------------------------------------------------------------------------
	enum ParameterType
	{
		FLOAT,
		VEC2,
		VEC3,
		VEC4,
		MAT3,
		MAT4
	};
	//----------------------------------------------------------------------------------
	/// \brief Get parameter type size
	/// \return 6
	//----------------------------------------------------------------------------------
	static unsigned int GetParameterTypeSize() { return 6; }
	//----------------------------------------------------------------------------------
	/// \brief Get parameter type index
	/// \param [in] _value Parameter
	//----------------------------------------------------------------------------------
	static unsigned int GetParameterTypeIndex( ParameterType _value ) { return ( unsigned int )_value; }
	//----------------------------------------------------------------------------------
	/// \brief Get parameter type data size
	/// \param [in] _value Parameter
	//----------------------------------------------------------------------------------
	static unsigned int GetParameterTypeDatasize( ParameterType _value );
	//----------------------------------------------------------------------------------
	/// \brief Generates an ID
	/// Will return 0 if size is exceeded
	/// Note that our ID approach is to assume a uint has enough IDs without keeping track of freed ones
	/// This means over 4billion IDs so we're probably ok
	/// \param [in] _type Parameter type
	//----------------------------------------------------------------------------------
	unsigned int NewParameter( ParameterType _type );
	//----------------------------------------------------------------------------------
	/// \brief Delete parameter
	/// \param [in] _paramID Parameter ID
	//----------------------------------------------------------------------------------
	void DeleteParameter( unsigned int _paramID );
	//----------------------------------------------------------------------------------
	/// \brief Set parameter data. Data size MUST correspond with declared parameter type
	/// Makes internal copy of data
	/// \param [in] _paramID Parameter ID
	/// \param [in] _data Data
	//----------------------------------------------------------------------------------
	void SetParameter( unsigned int _paramID, float* _data );
	//----------------------------------------------------------------------------------
	/// \brief Returns the string that tree Nodes should use in their GLSL strings to use the parameter
	/// \param [in] _paramID Parameter ID
	//----------------------------------------------------------------------------------
	std::string GetParameterString( unsigned int _paramID );
	//----------------------------------------------------------------------------------
	/// \brief Must be called when parameters change and before shader is compiled
	/// It populates the m_paramUniformData with copies of all the parameter data
	/// This is then bound to the uniforms internally with BindParametersToGL()
	//----------------------------------------------------------------------------------
	void BuildParameterUniformLists();
	//----------------------------------------------------------------------------------
	/// \brief Use to test whether the tree will need rebuilding or can just make do with changing parameters
	//----------------------------------------------------------------------------------
	bool ParametersStillAvailable();
	//----------------------------------------------------------------------------------

protected:

	//----------------------------------------------------------------------------------
	/// \brief Window width
	//----------------------------------------------------------------------------------
	unsigned int m_windowWidth;
	//----------------------------------------------------------------------------------
	/// \brief Window height
	//----------------------------------------------------------------------------------
	unsigned int m_windowHeight;
	//----------------------------------------------------------------------------------
	// NOTE: this is done like this to match the SHIVA SDFView
	//----------------------------------------------------------------------------------
	/// \brief ModelView matrix
	//----------------------------------------------------------------------------------
	cml::matrix44f_c m_modelViewMatrix;
	//----------------------------------------------------------------------------------
	/// \brief Inverse ModelView matrix
	//----------------------------------------------------------------------------------
	cml::matrix44f_c m_invModelViewMatrix;
	//----------------------------------------------------------------------------------
	/// \brief Model matrix
	//----------------------------------------------------------------------------------
	cml::matrix44f_c m_modelMatrix;
	//----------------------------------------------------------------------------------
	/// \brief The camera
	//----------------------------------------------------------------------------------
	Camera* m_cam;
	//----------------------------------------------------------------------------------
	/// \brief The target
	//----------------------------------------------------------------------------------
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
	/// \brief Angular velocity about x-axis
	//----------------------------------------------------------------------------------
	float m_angularVelX;
	//----------------------------------------------------------------------------------
	/// \brief Angular velocity about y-axis
	//----------------------------------------------------------------------------------
	float m_angularVelY;
	//----------------------------------------------------------------------------------
	/// \brief Angular velocity about z-axis
	//----------------------------------------------------------------------------------
	float m_angularVelZ;
	//----------------------------------------------------------------------------------
	/// \brief Function tree
	//----------------------------------------------------------------------------------
	VolumeTree::Tree* m_functionTree;
	//----------------------------------------------------------------------------------
	/// \brief Fragment colour
	//----------------------------------------------------------------------------------
	int m_fragColour;
	//----------------------------------------------------------------------------------
	/// \brief Step size
	//----------------------------------------------------------------------------------
	float m_stepSize;
	//----------------------------------------------------------------------------------
	/// \brief Delta
	//----------------------------------------------------------------------------------
	float m_gradDelta;
	//----------------------------------------------------------------------------------
	/// \brief R component of object colour
	//----------------------------------------------------------------------------------
	float m_objColourR;
	//----------------------------------------------------------------------------------
	/// \brief G component of object colour
	//----------------------------------------------------------------------------------
	float m_objColourG;
	//----------------------------------------------------------------------------------
	/// \brief B component of object colour
	//----------------------------------------------------------------------------------
	float m_objColourB;
	//----------------------------------------------------------------------------------
	/// \brief Max number of caches
	//----------------------------------------------------------------------------------
	unsigned int m_maxCaches;
	//----------------------------------------------------------------------------------
	/// \brief Array storing OpenGL texture IDs for the volume textures used to store caches
	/// The index is used externally as the 'cache ID'
	/// It is also used as the texture target for binding
	//----------------------------------------------------------------------------------
	int *m_glCacheTexIDs;
	//----------------------------------------------------------------------------------
	/// \brief Max number of params raw. Based on capabilities of graphics card
	//----------------------------------------------------------------------------------
	unsigned int m_maxParametersRaw;
	//----------------------------------------------------------------------------------
	/// \brief This is always the ID of the last assigned param
	//----------------------------------------------------------------------------------
	unsigned int m_paramIDCount;
	//----------------------------------------------------------------------------------
	/// \brief Shader program
	//----------------------------------------------------------------------------------	
	Shader* m_shader;
	//----------------------------------------------------------------------------------
	/// \brief Vertex shader
	//----------------------------------------------------------------------------------
	std::string m_vertShaderString;
	//----------------------------------------------------------------------------------
	/// \brief This is the first part of the fragment shader, which doesn't change even if the tree changes
	//----------------------------------------------------------------------------------
	std::string m_fragShaderString;
	//----------------------------------------------------------------------------------
	/// \brief Cube VAO
	//----------------------------------------------------------------------------------	
	GLuint m_cubeVAO;
	//----------------------------------------------------------------------------------	

	// TREE PARAMETER CLASS
	
	//----------------------------------------------------------------------------------
	class TreeParameter
	{
	public:

		//----------------------------------------------------------------------------------
		/// \brief Ctor
		/// \param [in] _type Parameter type
		/// \param [in] _id Parameter ID
		//----------------------------------------------------------------------------------
		TreeParameter( ParameterType _type, unsigned int _id ) { m_type = _type; m_ID = _id; m_data = NULL; m_shaderUniformOffset = -1; }
		//----------------------------------------------------------------------------------
		/// \brief Dtor
		//----------------------------------------------------------------------------------
		~TreeParameter() {/*delete [] _data;*/}
		//----------------------------------------------------------------------------------
		/// \brief Set data
		/// \param [in] _dataIn
		//----------------------------------------------------------------------------------
		void SetData( float* _dataIn );
		//----------------------------------------------------------------------------------
		/// \brief Parameter type
		//----------------------------------------------------------------------------------
		ParameterType m_type;
		//----------------------------------------------------------------------------------
		/// \brief Parameter ID
		//----------------------------------------------------------------------------------
		unsigned int m_ID;
		//----------------------------------------------------------------------------------
		/// \brief Parameter data
		//----------------------------------------------------------------------------------
		float *m_data;
		//----------------------------------------------------------------------------------
		/// \brief This is the array index for the uniform declaration within the shader
		//----------------------------------------------------------------------------------
		int m_shaderUniformOffset;
		//----------------------------------------------------------------------------------
	};

	//----------------------------------------------------------------------------------
	/// \brief Parameters list
	//----------------------------------------------------------------------------------
	std::vector< TreeParameter > m_paramList;
	//----------------------------------------------------------------------------------
	/// \brief Stores how many entries is in each list
	//----------------------------------------------------------------------------------
	unsigned int *m_paramUniformListLengths;
	//----------------------------------------------------------------------------------
	/// \brief Stores the actual data
	//----------------------------------------------------------------------------------
	float** m_paramUniformData;
	//----------------------------------------------------------------------------------
	/// \brief Returns iterator from the parameters list
	/// \param [in] _id Parameter ID
	//----------------------------------------------------------------------------------
	std::vector< TreeParameter >::iterator GetParameter( unsigned int _id );
	//----------------------------------------------------------------------------------
	/// \brief Returns iterator from the parameters list
	/// \param [in] _id Parameter ID
	/// \param [in] _position
	//----------------------------------------------------------------------------------
	std::vector< TreeParameter >::iterator GetParameter( unsigned int _id, unsigned int &_position );
	//----------------------------------------------------------------------------------
	/// \brief Returns the parameter position (count)
	//----------------------------------------------------------------------------------
	unsigned int GetParameterUniformPos( std::vector<TreeParameter>::iterator _param );
	//----------------------------------------------------------------------------------
	/// \brief Returns string of parameter uniform type
	/// \param [in] _type Parameter type
	/// \param [in] _showDataType
	//----------------------------------------------------------------------------------
	std::string GetParameterUniformTypeStr( ParameterType _type, bool _showDataType );
	//----------------------------------------------------------------------------------
	/// \brief Get number of used raw parameters
	//----------------------------------------------------------------------------------
	unsigned int GetNumUsedRawParameters();
	//----------------------------------------------------------------------------------
	/// \brief Returns a string that should be put at the start of the shader
	/// This declares a set of uniform arrays
	//----------------------------------------------------------------------------------
	std::string GetParameterGLSLDeclaration();
	//----------------------------------------------------------------------------------
	/// \brief Binds the uniform arrays to the shader
	//----------------------------------------------------------------------------------
	void BindParametersToGL();
	//----------------------------------------------------------------------------------
	/// \brief Calculates new position for the camera and initiates the movement to this position
	//----------------------------------------------------------------------------------
	void RefreshCameraPos();
	//----------------------------------------------------------------------------------
	/// \brief Initialise cube vbos and vao
	//----------------------------------------------------------------------------------
	void InitialiseCube();
	//----------------------------------------------------------------------------------
	/// \brief Initialise vertex and fragment shaders
	/// \param [in] _vsFile
	/// \param [in] _fsPartFile
	//----------------------------------------------------------------------------------
	bool InitialiseShaders( std::string _vsFile, std::string _fsPartFile );
	//----------------------------------------------------------------------------------
	/// \brief Load matrices to shader
	//----------------------------------------------------------------------------------
	void LoadMatricesToShader();
	//----------------------------------------------------------------------------------

};

#endif /* GLSLRENDERER_H_ */