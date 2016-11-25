/*
 * GLSLRenderer.h
 *
 *  Created on: Jan 10, 2013
 *      Author: leigh
 */

#ifndef GLSLRENDERER_H_
#define GLSLRENDERER_H_

#include <cml/cml.h>

#include "VolumeRenderer/SpringyVec3.h"

#include "VolumeTree/VolumeTree.h"

class GLSLRenderer
{
public:

	GLSLRenderer(unsigned int width, unsigned int height);
	~GLSLRenderer();

	void SetWindowDimensions(float width, float height);


	void SetTree(VolumeTree::Tree *);

	bool Initialise(std::string vertFile, std::string fragFilePart);

	/// You must call this if the tree has changed
	/// It recompiles the fragment shader for the new tree
	bool RebuildTree();

	void Draw();

	void Update(float deltaTs);

	void SetStepsize(float stepsize);

	/// Values expected between 0 and 1
	void SetObjectColour(float R, float G, float B) {_objectColourR = R; _objectColourG = G; _objectColourB = B;}


	// Transformation-related

		void ResetWorldRotation();

		void AddWorldRotationOffsetDegs(float rotX, float rotY, float rotZ);
		void AddWorldRotationOffsetRads(float rotX, float rotY, float rotZ);

		void SetWorldAngularVelocityDegs(float rotX, float rotY, float rotZ);
		void SetWorldAngularVelocityRads(float rotX, float rotY, float rotZ);

		/// Give it mouse position (in window coord space) and view params,
		/// will give you an origin and direction in 3D (world space)
		void Unproject(	float mouseX,float mouseY,
						int viewOriginX, int viewOriginY, int viewWidth, int viewHeight,
						float &originX,float &originY,float &originZ,
						float &dirX,float &dirY, float &dirZ);

		void ZoomCamera( float value );

		void SetCameraAngle( float angleDeg );

		/// Returns the distance from the camera to the origin
		float GetCameraDistance();


	// Caching
	// The renderer just provides the mechanisms to allow you to use caches
	// It allows you to query how much memory is available etc
	// But the decision on what should be cached, and how many caches etc, is down to the CachingPolicy

		//void GetCacheSize(float boundsX, float boundsY, float boundsZ, unsigned int *resX, unsigned int *resY, unsigned int *resZ);

		/// After you've decided how many caches you want to use, call this function to create them
		/// Also acts to clear all caches that already exist
		void ReserveCaches(unsigned int numCaches);
		unsigned int GetNumReservedCaches() {return _maxCaches;}

		/// Returns the optimal total number of voxels that should be used for caching
		/// This is used when working out the resolution of the caches
		unsigned int GetMaxCachingVoxels();

		/// Returns a cacheID
		/// Use the cacheID for the GLSL function string appended to the word "CacheTexture"
		void FillCache(unsigned int cacheID, float *data, unsigned int sizeX, unsigned int sizeY, unsigned int sizeZ);
		void FreeCache(unsigned int cacheID);


	// Parameters
	// Instead of rebuilding the tree after parameter changes, you can retrieve parameters
	// TODO: Clean me! If I'd realised how much junk would be in here, I'd have maybe put it in another class...

		/// If this enum is changed the functions immediately below MUST be updated
		enum ParameterType
		{
			FLOAT,
			VEC2,
			VEC3,
			VEC4,
			MAT3,
			MAT4
		};
		static unsigned int GetParameterTypeSize() {return 6;}
		static unsigned int GetParameterTypeIndex(ParameterType value) {return (unsigned int) value;}
		static unsigned int GetParameterTypeDatasize( ParameterType value );

		/// Generates an ID
		/// Will return 0 if size is exceeded
		/// Note that our ID approach is to assume a uint has enough IDs without keeping track of freed ones
		/// This means over 4billion IDs so we're probably ok
		unsigned int NewParameter(ParameterType type);

		void DeleteParameter(unsigned int paramID);

		/// Data size MUST correspond with declared parameter type
		/// Makes internal copy of data
		void SetParameter(unsigned int paramID, float *data);

		/// Returns the string that tree Nodes should use in their GLSL strings to use the parameter
		std::string GetParameterString(unsigned int paramID);

		/// Must be called when parameters change and before shader is compiled
		/// It populates the _parameterUniformData with copies of all the parameter data
		/// This is then bound to the uniforms internally with BindParametersToGL()
		void BuildParameterUniformLists();

		/// Use to test whether the tree will need rebuilding or can just make do with changing parameters
		bool ParametersStillAvailable();

protected:

	unsigned int _windowWidth, _windowHeight;

	/// NOTE: this is done like this to match the SHIVA SDFView
	/// Viewing matrices and associated variables
	cml::matrix44f_c _projectionMatrix, _modelviewMatrix, _invModelview, _worldMatrix;
	//cml::vector3f _cameraPosition, _targetPosition;
	SpringyVec3 _cameraPosition, _targetPosition;
	cml::vector4f _axisX, _axisY, _axisZ;
	/// Local rotations in radians about the major coordinate axes
	float _rotationX, _rotationY, _rotationZ;
	float _rotationWorldX, _rotationWorldY, _rotationWorldZ;
	float _cameraAngle, _cameraNearPlane, _cameraFarPlane;

	float _angularVelocityX, _angularVelocityY, _angularVelocityZ;

	float _temp1X,_temp1Y,_temp1Z, _temp2X,_temp2Y,_temp2Z;

	VolumeTree::Tree *_functionTree;

	/// OpenGL Vertex Buffer Object IDs for a cube
	unsigned int _cubeVertexVBO, _cubeIndexVBO;

	unsigned int _GPUProgram;
	unsigned int _fragShader, _vertShader;

	/// This is the first part of the fragment shader, which doesn't change even if the tree changes
	std::string _fragShaderString;

	int _projMatrixUniform, _mvMatrixUniform, _invViewMatrixUniform, _vertSize, _vertPos;
	int _fragboundmin, _fragboundmax;
	int _fragStepsize, _fragGradDelta;
	int _fragCache0, _fragCache1, _fragCache2, _fragCache3;
	int _fragColour;
	float _stepsize, _gradDelta;

	float _objectColourR, _objectColourG, _objectColourB;

	float _cameraX, _cameraY, _cameraZ;


	unsigned int _maxCaches;
	/// Array storing OpenGL texture IDs for the volume textures used to store caches
	/// The index is used externally as the 'cache ID'
	/// It is also used as the texture target for binding
	int *_glCacheTexIDs;


	/// Based on capabilities of graphics card
	unsigned int _maxParametersRaw;
	/// This is always the ID of the last assigned param
	unsigned int _paramIDcount;

	class TreeParameter
	{
	public:
		TreeParameter(ParameterType ty, unsigned int id) {_type = ty;_ID = id;_data = NULL; _shaderUniformOffset = -1;}
		~TreeParameter() {/*delete [] _data;*/}
		void SetData(float *dataIn);
		ParameterType _type;
		unsigned int _ID;
		float *_data;

		/// This is the array index for the uniform declaration within the shader
		int _shaderUniformOffset;
	};
	std::vector<TreeParameter> _paramList;

	/// Stores how many entries is in each list
	unsigned int *_parameterUniformListLengths;
	/// Stores the actual data
	float **_parameterUniformData;

	// Returns iterator from the _paramList
	std::vector<TreeParameter>::iterator GetParameter(unsigned int id);
	std::vector<TreeParameter>::iterator GetParameter(unsigned int id, unsigned int &position);

	unsigned int GetParameterUniformPos(std::vector<TreeParameter>::iterator param);
	std::string GetParameterUniformTypeStr( ParameterType type, bool showDataType );

	unsigned int GetNumUsedRawParameters();

	/// Returns a string that should be put at the start of the shader
	/// This declares a set of uniform arrays
	std::string GetParameterGLSLDeclaration();
	/// Binds the uniform arrays to the shader
	void BindParametersToGL();

	/// Calculates new position for the camera and initiates the movement to this position
	void RefreshCameraPos();

	void InitialiseCube();
	bool InitialiseShaders(std::string vertFile, std::string fragFilePart);
	bool CompileShader(unsigned int);
};

#endif /* GLSLRENDERER_H_ */
