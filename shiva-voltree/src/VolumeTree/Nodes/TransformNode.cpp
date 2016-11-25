
#include "VolumeTree/Nodes/TransformNode.h"

#include "VolumeRenderer/GLSLRenderer.h"

VolumeTree::TransformNode::TransformNode( bool useParams)
{
	_useParams = useParams;
	_child = NULL;
	Reset();
	_matrixParam = 0;
	_translationParam = 0;
	_paramRenderer = NULL;
	#ifdef _DEBUG
	std::cout<<"INFO: Creating TransformNode"<<std::endl;
	#endif
}

VolumeTree::TransformNode::TransformNode(Node *child, bool useParams)
{
	_useParams = useParams;
	_child = child;
	Reset();
	_matrixParam = 0;
	_translationParam = 0;
	_paramRenderer = NULL;
	#ifdef _DEBUG
	std::cout<<"INFO: Creating TransformNode"<<std::endl;
	#endif
}

VolumeTree::TransformNode::~TransformNode()
{
	if( _paramRenderer != NULL )
	{
		_paramRenderer->DeleteParameter(_matrixParam);
		_paramRenderer->DeleteParameter(_translationParam);
	}
	#ifdef _DEBUG
	std::cout<<"INFO: Deleting TransformNode"<<std::endl;
	#endif
}

void VolumeTree::TransformNode::SetTranslate(float x, float y, float z)
{
	_applyTranslate = true;
	//std::cout<<"INFO: VolumeTree::TransformNode::SetTranslate: "<<x<<" "<<y<<" "<<z<<std::endl;
	cml::matrix_set_translation(_transformMatrix,-x,-y,-z);
	_tx = -x;
	_ty = -y;
	_tz = -z;
}

void VolumeTree::TransformNode::AddTranslate(float x, float y, float z)
{
	_applyTranslate = true;
	//std::cout<<"INFO: VolumeTree::TransformNode::SetTranslate: "<<x<<" "<<y<<" "<<z<<std::endl;
	cml::matrix44f_c temp;
	temp.identity();
	cml::matrix_set_translation(temp,-x,-y,-z);
	_transformMatrix = _transformMatrix*temp;
	_tx -= x;
	_ty -= y;
	_tz -= z;
}

void VolumeTree::TransformNode::GetTranslate(float &x, float &y, float &z)
{
	float tX, tY, tZ;
	cml::matrix_get_translation(_transformMatrix,tX,tY,tZ);
	x = -tX;
	y = -tY;
	z = -tZ;
}

void VolumeTree::TransformNode::SetRotate(float dirX, float dirY, float dirZ)
{
	_applyRotate = true;
	cml::matrix44f_c temp;
	temp.identity();
	cml::matrix_rotation_vec_to_vec(temp, cml::vector3f(0.0f,0.0f,1.0f), cml::vector3f(dirX,dirY,-dirZ),true);
	_transformMatrix = _transformMatrix*temp;

	float angle0 = 0.0f, angle1 = 0.0f, angle2 = 0.0f;
	cml::matrix_to_euler(temp,angle0,angle1,angle2,cml::EulerOrder::euler_order_yzx);
	_rx = angle2;
	_ry = angle0;
	_rz = angle1;
}

void VolumeTree::TransformNode::SetScale(float x, float y, float z)
{
	_applyScale = true;
	_sx = x; _sy = y; _sz = z;
	cml::matrix_scale(_transformMatrix,x,y,z);
}

void VolumeTree::TransformNode::SetTransformParams(	float tx, float ty, float tz, //translate, 0 by default
													float rx, float ry, float rz, //rotate as euler angles in radians, 0 by default
													float sx, float sy, float sz, //scale, 1 by default
													float refrx, float refry, float refrz, //reference point for rotation, 0 by default
													float refsx, float refsy, float refsz) //reference point for scale, 0 by default
{
	Reset();
	
	_tx = -tx; _ty = -ty; _tz = -tz;
	_rx = -rx; _ry = -ry; _rz = -rz;
	_sx = sx; _sy = sy; _sz = sz;
	_refrx = refrx; _refry = refry; _refrz = refrz;
	_refsx = refsx; _refsy = refsy; _refsz = refsz;

#ifdef _DEBUG

	std::cout<<"INFO: TransformNode::SetTransformParams() rx: "<<rx<<" ry: "<<ry<<" rz: "<<rz<<std::endl;
	std::cout<<"INFO:                                     tx: "<<tx<<" ty: "<<ty<<" tz: "<<tz<<std::endl;
	std::cout<<"INFO:                                     sx: "<<sx<<" sy: "<<sy<<" sz: "<<sz<<std::endl;

#endif



	cml::matrix44f_c preScaleTrans;
	preScaleTrans.identity();
	cml::matrix_set_translation(preScaleTrans,-refsx,-refsy,-refsz);

	cml::matrix44f_c scaleMat;
	scaleMat.identity();
	cml::matrix_scale(scaleMat,1.0f/sx,1.0f/sy,1.0f/sz);

	cml::matrix44f_c postScaleTrans;
	postScaleTrans.identity();
	cml::matrix_set_translation(postScaleTrans,refsx,refsy,refsz);


	cml::matrix44f_c preRotTrans;
	preRotTrans.identity();
	cml::matrix_set_translation(preRotTrans,-refrx,-refry,-refrz);

	cml::matrix44f_c rotationMat;
	rotationMat.identity();
	cml::matrix_rotation_euler(rotationMat, -ry, -rz, -rx, cml::EulerOrder::euler_order_yzx);

	cml::matrix44f_c postRotTrans;
	postRotTrans.identity();
	cml::matrix_set_translation(postRotTrans,refrx,refry,refrz);


	cml::matrix44f_c translateMat;
	translateMat.identity();
	cml::matrix_set_translation(translateMat,-tx,-ty,-tz);

	_transformMatrix = postScaleTrans * scaleMat * preScaleTrans * postRotTrans * rotationMat * preRotTrans  * translateMat * _transformMatrix;

	_applyRotate = _applyTranslate = _applyScale = true;
}


void VolumeTree::TransformNode::GetTransformParams(	float &tx, float &ty, float &tz, //translate, 0 by default
													float &rx, float &ry, float &rz, //rotate as euler angles in radians, 0 by default
													float &sx, float &sy, float &sz, //scale, 1 by default
													float &refrx, float &refry, float &refrz, //reference point for rotation, 0 by default
													float &refsx, float &refsy, float &refsz) //reference point for scale, 0 by default
{
	tx = _tx; ty = _ty; tz = _tz;
	rx = _rx; ry = _ry; rz = _rz;
	sx = _sx; sy = _sy; sz = _sz;
	refrx = _refrx; refry = _refry; refrz = _refrz;
	refsx = _refsx; refsy = _refsy; refsz = _refsz;
}

bool VolumeTree::TransformNode::GetTransformTypes(bool &translation, bool &rotation, bool &scale)
{
	translation=_applyTranslate;
	rotation=_applyRotate;
	scale=_applyScale;
	return ((int)_applyTranslate + (int)_applyRotate + (int)_applyScale) > 1;
}

float VolumeTree::TransformNode::GetFunctionValue(float x, float y, float z)
{
	// transform the sample position then send it to the child

	cml::vector4f coords(x,y,z,1);
	cml::matrix44f_c inverseTransform = cml::inverse(_transformMatrix);
	coords = inverseTransform * coords;

	if( _child != NULL )
	{
		return _child->GetFunctionValue(coords.data()[0],coords.data()[1],coords.data()[2]);
	}
	return -1.0f;
}

std::string VolumeTree::TransformNode::GetFunctionGLSLString(bool callCache, std::string samplePosStr)
{
	if( _child != NULL)
	{
		if( _applyTranslate && !_applyRotate && !_applyScale )
		{
			if( _translationParamString.empty() )
			{
				// Need to wrap the 'samplePosition' in samplePosStr with a Translate function
				std::stringstream functionString;
				cml::vector3f translate = cml::matrix_get_translation(_transformMatrix);

				functionString<<"Translate("<<samplePosStr<<",vec3("<<translate.data()[0]<<","<<translate.data()[1]<<","<<translate.data()[2]<<"))";
				samplePosStr = functionString.str();
			}
			else
			{
				std::stringstream functionString;

				functionString<<"Translate("<<samplePosStr<<","<<_translationParamString<<")";
				samplePosStr = functionString.str();
			}
		}
		else
		{
			// This DOES NOT need an inverse matrix at this point
			//cml::matrix44f_c tempMat = _transformMatrix;
			//tempMat = cml::inverse(tempMat);
			std::stringstream functionString;
			functionString<<"Transform("<<samplePosStr<<",";
			if( _matrixParamString.empty() )
			{
				functionString<<"mat4(";
				for( unsigned int i = 0; i < 16; i++ )
				{
					functionString<<_transformMatrix.data()[i];
					if( i != 15 )
						functionString<<",";
				}
				functionString<<"))";
			}
			else
			{
				functionString<<_matrixParamString;
				functionString<<")";
			}
			samplePosStr = functionString.str();
		}

		if( callCache )
		{
			return _child->GetCachedFunctionGLSLString(samplePosStr);
		}
		else
		{
			return _child->GetFunctionGLSLString(false,samplePosStr);
		}
	}
	else
	{
		std::cerr<<"WARNING: TransformNode has invalid child"<<std::endl;
		return "-1";
	}
}


VolumeTree::Node* VolumeTree::TransformNode::GetFirstChild()
{
	return _child;
}

VolumeTree::Node* VolumeTree::TransformNode::GetNextChild(Node *previousChild)
{
	if( previousChild == NULL )
		return _child;
	return NULL;
}

unsigned int VolumeTree::TransformNode::GetNodeCost()
{
	unsigned int cost = 0;
	if( _applyTranslate ) cost += 1;
	if( _applyRotate ) cost += 3;
	if( _applyScale ) cost += 1;
	return cost;
}

void VolumeTree::TransformNode::GetBounds(float *minX,float *maxX, float *minY,float *maxY, float *minZ,float *maxZ)
{
	if( _child != NULL )
	{
		// Get child bounds then transform them
		
		float AminX, AmaxX, AminY, AmaxY, AminZ, AmaxZ;
		_child->GetBounds(&AminX, &AmaxX, &AminY, &AmaxY, &AminZ, &AmaxZ);

		//cml::vector4f minvals(AminX,AminY,AminZ,1);
		//cml::vector4f maxvals(AmaxX,AmaxY,AmaxZ,1);

		// Points from all 8 corners
		cml::vector4f point0(AminX,AminY,AminZ,1.0f);
		cml::vector4f point1(AminX,AminY,AmaxZ,1.0f);
		cml::vector4f point2(AminX,AmaxY,AminZ,1.0f);
		cml::vector4f point3(AminX,AmaxY,AmaxZ,1.0f);
		
		cml::vector4f point4(AmaxX,AminY,AminZ,1.0f);
		cml::vector4f point5(AmaxX,AminY,AmaxZ,1.0f);
		cml::vector4f point6(AmaxX,AmaxY,AminZ,1.0f);
		cml::vector4f point7(AmaxX,AmaxY,AmaxZ,1.0f);

		// Transform all points
		cml::matrix44f_c inverseTransform = cml::inverse(_transformMatrix);
		point0 = inverseTransform * point0;
		point1 = inverseTransform * point1;
		point2 = inverseTransform * point2;
		point3 = inverseTransform * point3;
		point4 = inverseTransform * point4;
		point5 = inverseTransform * point5;
		point6 = inverseTransform * point6;
		point7 = inverseTransform * point7;

		// Work out new min/max
		float minvals[3];
		float maxvals[3];
		for( unsigned int i = 0; i < 3; i++ )
		{
			float min0 = std::min<float>( std::min<float>(point0[i],point1[i]), std::min<float>(point2[i],point3[i]) );
			float min1 = std::min<float>( std::min<float>(point4[i],point5[i]), std::min<float>(point6[i],point7[i]) );
			minvals[i] = std::min<float>(min0,min1);

			float max0 = std::max<float>( std::max<float>(point0[i],point1[i]), std::max<float>(point2[i],point3[i]) );
			float max1 = std::max<float>( std::max<float>(point4[i],point5[i]), std::max<float>(point6[i],point7[i]) );
			maxvals[i] = std::max<float>(max0,max1);
		};

/*
		//cml::vector4f translate = cml::vector4f(cml::matrix_get_translation(_transformMatrix),1.0f);
		//cml::matrix44f_c inverseTransform = _transformMatrix;
		//cml::matrix_set_translation(inverseTransform,0.0f,0.0f,0.0f);
		cml::matrix44f_c inverseTransform;
		inverseTransform.identity();
		inverseTransform = cml::inverse(_transformMatrix);
		//cml::matrix_set_translation(inverseTransform, -translate[0],-translate[1],-translate[2] );
		//cml::matrix44f_c inverseTransform = cml::inverse(_transformMatrix);
		minvals = inverseTransform * minvals;
		maxvals = inverseTransform * maxvals;
		//minvals -= translate;
		//maxvals -= translate;
*/

		*minX = minvals[0];
		*maxX = maxvals[0];
		*minY = minvals[1];
		*maxY = maxvals[1];
		*minZ = minvals[2];
		*maxZ = maxvals[2];
		
	}
	else
	{
		// Results undefined
		*minX = 0.0f;
		*maxX = 0.0f;
		*minY = 0.0f;
		*maxY = 0.0f;
		*minZ = 0.0f;
		*maxZ = 0.0f;
	}

}

void VolumeTree::TransformNode::Reset()
{
	_tx = _ty = _tz = 0.0f; //translate, 0 by default
	_rx = _ry = _rz = 0.0f; //rotate as euler angles in radians, 0 by default
	_sx = _sy = _sz = 1.0f; //scale, 1 by default
	_refrx = _refry = _refrz = 0.0f; //reference point for rotation, 0 by default
	_refsx = _refsy = _refsz = 0.0f; //reference point for scale, 0 by default

	_applyTranslate = _applyRotate = _applyScale = false;
	_transformMatrix.identity();
}

void VolumeTree::TransformNode::OnBuildParameters( GLSLRenderer *renderer )
{
	_paramRenderer = renderer;
	if( _useParams && (_matrixParam == 0) )
	{
		if( _applyTranslate && !_applyRotate && !_applyScale )
		{
			_translationParam = renderer->NewParameter( GLSLRenderer::ParameterType::VEC3 );
			#ifdef _DEBUG
			std::cout<<"INFO: TransformNode retrieved param for translation: "<<_translationParam<<std::endl;
			#endif
		}
		else if( _applyTranslate || _applyRotate || _applyScale )
		{
			_matrixParam = renderer->NewParameter( GLSLRenderer::ParameterType::MAT4 );
			#ifdef _DEBUG
			std::cout<<"INFO: TransformNode retrieved param for matrix: "<<_matrixParam<<std::endl;
			#endif
		}
	}
}

void VolumeTree::TransformNode::OnUpdateParameters( GLSLRenderer *renderer )
{
	if( _useParams )
	{
		// Unfortunately this needs updating regularly, since we don't know if nodes with lower ID's have been removed or not
		if( _matrixParam > 0 )
		{
			renderer->SetParameter( _matrixParam, _transformMatrix.data() );
			_matrixParamString = renderer->GetParameterString( _matrixParam );
			#ifdef _DEBUG
			//std::cout<<"INFO: TransformNode retrieved string for matrix, ID: "<<_matrixParam<<" string: "<<_matrixParamString<<std::endl;
			#endif
		}
		else
		{
			_matrixParamString.clear();
		}
		if( _translationParam > 0 )
		{
			float *data = new float[3];
			data[0] = _tx;
			data[1] = _ty;
			data[2] = _tz;
			renderer->SetParameter( _translationParam, data );
			_translationParamString = renderer->GetParameterString( _translationParam );
			#ifdef _DEBUG
			//std::cout<<"INFO: TransformNode retrieved string for translation, ID: "<<_translationParam<<" string: "<<_translationParamString<<std::endl;
			#endif
		}
		else
		{
			_translationParamString.clear();
		}
	}
}
