/*
 * CSG.h
 *
 *  Created on: Jun 4, 2013
 *      Author: leigh
 */

#ifndef TRANSLATENODE_H_
#define TRANSLATENODE_H_

#include <cml/cml.h>
#include "VolumeTree/Node.h"

namespace VolumeTree
{
	/// Transformations are split up to save on data being sent to the shader
	class TransformNode : public Node
	{
	public:
		TransformNode(bool useParams = false);
		TransformNode(Node *child, bool useParams = false);
		virtual ~TransformNode();
		
		virtual std::string GetNodeType(){return "TransformNode";}

		cml::matrix44f_c& GetTransformMatrix() {return _transformMatrix;}

		void SetTranslate(float x, float y, float z);
		void AddTranslate(float x, float y, float z);
		void GetTranslate(float &x, float &y, float &z);

		/// Builds a rotation matrix that rotates a vector(0,0,1) to the specified direction
		void SetRotate(float dirX, float dirY, float dirZ);

		void SetScale( float value ) {SetScale(value,value,value);}
		void SetScale(float x, float y, float z);

		
		void SetTransformParams(float tx, float ty, float tz, //translate, 0 by default
								float rx, float ry, float rz, //rotate as euler angles in radians, 0 by default
								float sx, float sy, float sz, //scale, 1 by default
								float refrx, float refry, float refrz, //reference point for rotation, 0 by default
								float refsx, float refsy, float refsz); //reference point for scale, 0 by default

		void GetTransformParams(float &tx, float &ty, float &tz, //translate, 0 by default
								float &rx, float &ry, float &rz, //rotate as euler angles in radians, 0 by default
								float &sx, float &sy, float &sz, //scale, 1 by default
								float &refrx, float &refry, float &refrz, //reference point for rotation, 0 by default
								float &refsx, float &refsy, float &refsz); //reference point for scale, 0 by default



		/// Returns true if more than one type is true, indicating it uses a full matrix
		bool GetTransformTypes(bool &translation, bool &rotation, bool &scale);


		/// Samples the function at a specific point
		float GetFunctionValue(float x, float y, float z);

		/// Returns a GLSL-compatible string for the function
		std::string GetFunctionGLSLString(bool callCache, std::string samplePosStr);

		void SetChild(Node *child){ _child = child;}
		
		virtual Node* GetFirstChild();
		virtual Node* GetNextChild(Node *previousChild);
		
		virtual unsigned int GetNodeCost();

		virtual void GetBounds(float *minX,float *maxX, float *minY,float *maxY, float *minZ,float *maxZ);

		/// Sets all transformations to zero (identity matrix)
		void Reset();

	protected:
		Node *_child;
		
		bool _useParams;
		/// I was hoping to avoid this, but it needs to be able to free up the params when it gets deleted
		GLSLRenderer *_paramRenderer;
		unsigned int _translationParam;
		std::string _translationParamString;
		unsigned int _matrixParam;
		std::string _matrixParamString;
		/// Node is expected to register any parameters it wants to use
		virtual void OnBuildParameters( GLSLRenderer *renderer );
		/// Node is expected to tell the renderer the values of its parameters
		virtual void OnUpdateParameters( GLSLRenderer *renderer );

		bool _applyTranslate, _applyRotate, _applyScale;

		cml::matrix44f_c _transformMatrix;

		float	_tx, _ty, _tz, //translate, 0 by default
				_rx, _ry, _rz, //rotate as euler angles in radians, 0 by default
				_sx,  _sy,  _sz, //scale, 1 by default
				_refrx,  _refry,  _refrz, //reference point for rotation, 0 by default
				_refsx,  _refsy,  _refsz; //reference point for scale, 0 by default
	};

}


#endif /* CSG_H_ */
