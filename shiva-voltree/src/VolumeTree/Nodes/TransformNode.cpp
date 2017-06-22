
#include "VolumeTree/Nodes/TransformNode.h"
#include "VolumeRenderer/GLSLRenderer.h"

//----------------------------------------------------------------------------------

VolumeTree::TransformNode::TransformNode( bool _useParams )
{
	m_useParams = _useParams;
	m_child = NULL;
	Reset();
	m_matrixParam = 0;
	m_translationParam = 0;
	m_paramRenderer = NULL;
	#ifdef _DEBUG
	std::cout << "INFO: Creating TransformNode" << std::endl;
	#endif
}

//----------------------------------------------------------------------------------

VolumeTree::TransformNode::TransformNode( Node *_child, bool _useParams )
{
	m_useParams = _useParams;
	m_child = _child;
	Reset();
	m_matrixParam = 0;
	m_translationParam = 0;
	m_paramRenderer = NULL;
	#ifdef _DEBUG
	std::cout << "INFO: Creating TransformNode" << std::endl;
	#endif
}

//----------------------------------------------------------------------------------

VolumeTree::TransformNode::~TransformNode()
{
	if( m_paramRenderer != NULL )
	{
		m_paramRenderer->DeleteParameter( m_matrixParam );
		m_paramRenderer->DeleteParameter( m_translationParam );
	}

	#ifdef _DEBUG
	std::cout << "INFO: Deleting TransformNode" << std::endl;
	#endif
}

//----------------------------------------------------------------------------------

void VolumeTree::TransformNode::SetTranslate( float _x, float _y, float _z )
{
	m_applyTranslate = true;
	//std::cout << "INFO: VolumeTree::TransformNode::SetTranslate: " << _x << " " << _y << " " << _z << std::endl;
	cml::matrix_set_translation( m_transformMatrix, -_x, -_y, -_z );
	m_tx = -_x;
	m_ty = -_y;
	m_tz = -_z;
}

//----------------------------------------------------------------------------------
void VolumeTree::TransformNode::AddTranslate( float _x, float _y, float _z )
{
	m_applyTranslate = true;
	//std::cout<<"INFO: VolumeTree::TransformNode::SetTranslate: "<<x<<" "<<y<<" "<<z<<std::endl;
	cml::matrix44f_c temp;
	temp.identity();
	cml::matrix_set_translation( temp, -_x, -_y, -_z );
	m_transformMatrix = m_transformMatrix * temp;
	m_tx -= _x;
	m_ty -= _y;
	m_tz -= _z;
}

//----------------------------------------------------------------------------------

void VolumeTree::TransformNode::GetTranslate( float &_x, float &_y, float &_z )
{
	float tX, tY, tZ;
	cml::matrix_get_translation( m_transformMatrix, tX, tY, tZ );
	_x = -tX;
	_y = -tY;
	_z = -tZ;
}

//----------------------------------------------------------------------------------

void VolumeTree::TransformNode::SetRotate( float _dirX, float _dirY, float _dirZ )
{
	m_applyRotate = true;
	cml::matrix44f_c temp;
	temp.identity();
	cml::matrix_rotation_vec_to_vec( temp, cml::vector3f( 0.0f, 0.0f, 1.0f ), cml::vector3f( _dirX, _dirY,-_dirZ ), true );
	m_transformMatrix = m_transformMatrix * temp;

	float angle0 = 0.0f, angle1 = 0.0f, angle2 = 0.0f;
	cml::matrix_to_euler( temp, angle0, angle1, angle2, cml::euler_order_yzx );
	m_rx = angle2;
	m_ry = angle0;
	m_rz = angle1;
}

//----------------------------------------------------------------------------------

void VolumeTree::TransformNode::SetScale( float _x, float _y, float _z )
{
	m_applyScale = true;
	m_sx = _x; m_sy = _y; m_sz = _z;
	cml::matrix_scale( m_transformMatrix, _x, _y, _z );
}

//----------------------------------------------------------------------------------

void VolumeTree::TransformNode::SetRotation( float _rx, float _ry, float _rz )
{
	m_rx = -_rx; m_ry = -_ry; m_rz = -_rz;
}

//----------------------------------------------------------------------------------

void VolumeTree::TransformNode::SetTransformParams(	float _tx, float _ty, float _tz, //translate, 0 by default
													float _rx, float _ry, float _rz, //rotate as euler angles in radians, 0 by default
													float _sx, float _sy, float _sz, //scale, 1 by default
													float _refrx, float _refry, float _refrz, //reference point for rotation, 0 by default
													float _refsx, float _refsy, float _refsz ) //reference point for scale, 0 by default
{
	Reset();
	
	m_tx = -_tx; m_ty = -_ty; m_tz = -_tz;
	m_rx = -_rx; m_ry = -_ry; m_rz = -_rz;
	m_sx = _sx; m_sy = _sy; m_sz = _sz;
	m_refrx = _refrx; m_refry = _refry; m_refrz = _refrz;
	m_refsx = _refsx; m_refsy = _refsy; m_refsz = _refsz;

#ifdef _DEBUG

	//std::cout << "INFO: TransformNode::SetTransformParams() rx: " << _rx << " ry: " << _ry << " rz: " << _rz << std::endl;
	//std::cout << "INFO:                                     tx: " << _tx << " ty: " << _ty << " tz: " << _tz << std::endl;
	//std::cout << "INFO:                                     sx: " << _sx << " sy: " << _sy << " sz: " << _sz << std::endl;

#endif
	
	cml::matrix44f_c preScaleTrans;
	preScaleTrans.identity();
	cml::matrix_set_translation( preScaleTrans, -_refsx, -_refsy, -_refsz );

	cml::matrix44f_c scaleMat;
	scaleMat.identity();
	cml::matrix_scale( scaleMat, 1.0f / _sx, 1.0f / _sy, 1.0f / _sz );

	cml::matrix44f_c postScaleTrans;
	postScaleTrans.identity();
	cml::matrix_set_translation( postScaleTrans, _refsx, _refsy, _refsz );


	cml::matrix44f_c preRotTrans;
	preRotTrans.identity();
	cml::matrix_set_translation( preRotTrans, -_refrx, -_refry, -_refrz );

	cml::matrix44f_c rotationMat;
	rotationMat.identity();
	cml::matrix_rotation_euler( rotationMat, -_ry, -_rz, -_rx, cml::euler_order_yzx );

	cml::matrix44f_c postRotTrans;
	postRotTrans.identity();
	cml::matrix_set_translation( postRotTrans, _refrx, _refry, _refrz );


	cml::matrix44f_c translateMat;
	translateMat.identity();
	cml::matrix_set_translation( translateMat, -_tx, -_ty, -_tz );

	m_transformMatrix = postScaleTrans * scaleMat * preScaleTrans * postRotTrans * rotationMat * preRotTrans  * translateMat * m_transformMatrix;

	m_applyRotate = m_applyTranslate = m_applyScale = true;
}

//----------------------------------------------------------------------------------

void VolumeTree::TransformNode::GetTransformParams(	float &_tx, float &_ty, float &_tz, //translate, 0 by default
													float &_rx, float &_ry, float &_rz, //rotate as euler angles in radians, 0 by default
													float &_sx, float &_sy, float &_sz, //scale, 1 by default
													float &_refrx, float &_refry, float &_refrz, //reference point for rotation, 0 by default
													float &_refsx, float &_refsy, float &_refsz ) //reference point for scale, 0 by default
{
	_tx = m_tx; _ty = m_ty; _tz = m_tz;
	_rx = m_rx; _ry = m_ry; _rz = m_rz;
	_sx = m_sx; _sy = m_sy; _sz = m_sz;
	_refrx = m_refrx; _refry = m_refry; _refrz = m_refrz;
	_refsx = m_refsx; _refsy = m_refsy; _refsz = m_refsz;
}

//----------------------------------------------------------------------------------

bool VolumeTree::TransformNode::GetTransformTypes( bool &_translation, bool &_rotation, bool &_scale )
{
	_translation = m_applyTranslate;
	_rotation = m_applyRotate;
	_scale = m_applyScale;
	return ( ( int )m_applyTranslate + ( int )m_applyRotate + ( int )m_applyScale ) > 1;
}

//----------------------------------------------------------------------------------

float VolumeTree::TransformNode::GetFunctionValue( float _x, float _y, float _z )
{
	// transform the sample position then send it to the child

	cml::vector4f coords( _x, _y, _z, 1 );
	cml::matrix44f_c inverseTransform = cml::inverse( m_transformMatrix );
	coords = inverseTransform * coords;

	if( m_child != NULL )
	{
		return m_child->GetFunctionValue( coords.data()[ 0 ], coords.data()[ 1 ], coords.data()[ 2 ] );
	}
	return -1.0f;
}

//----------------------------------------------------------------------------------

std::string VolumeTree::TransformNode::GetFunctionGLSLString( bool _callCache, std::string _samplePosStr )
{
	if( m_child != NULL)
	{
		if( m_applyTranslate && !m_applyRotate && !m_applyScale )
		{
			if( m_translationParamString.empty() )
			{
				// Need to wrap the 'samplePosition' in samplePosStr with a Translate function
				std::stringstream functionString;
				cml::vector3f translate = cml::matrix_get_translation( m_transformMatrix );

				functionString << "Translate(" << _samplePosStr << ",vec3(" << translate.data()[ 0 ] << "," << translate.data()[ 1 ] << "," << translate.data()[ 2 ] << "))";
				_samplePosStr = functionString.str();
			}
			else
			{
				std::stringstream functionString;

				functionString << "Translate(" << _samplePosStr << "," << m_translationParamString << ")";
				_samplePosStr = functionString.str();
			}
		}
		else
		{
			// This DOES NOT need an inverse matrix at this point
			//cml::matrix44f_c tempMat = _transformMatrix;
			//tempMat = cml::inverse(tempMat);
			std::stringstream functionString;
			functionString << "Transform(" << _samplePosStr << ",";
			if( m_matrixParamString.empty() )
			{
				functionString << "mat4(";
				for( unsigned int i = 0; i < 16; i++ )
				{
					functionString << m_transformMatrix.data()[ i ];
					if( i != 15 )
						functionString << ",";
				}
				functionString << "))";
			}
			else
			{
				functionString << m_matrixParamString;
				functionString << ")";
			}
			_samplePosStr = functionString.str();
		}

		if( _callCache )
		{
			return m_child->GetCachedFunctionGLSLString( _samplePosStr );
		}
		else
		{
			return m_child->GetFunctionGLSLString( false, _samplePosStr );
		}
	}
	else
	{
		std::cerr << "WARNING: TransformNode has invalid child" << std::endl;
		return "-1";
	}
}

//----------------------------------------------------------------------------------

VolumeTree::Node* VolumeTree::TransformNode::GetFirstChild()
{
	return m_child;
}

//----------------------------------------------------------------------------------

VolumeTree::Node* VolumeTree::TransformNode::GetNextChild( Node *_previousChild )
{
	if( _previousChild == NULL )
		return m_child;
	return NULL;
}

//----------------------------------------------------------------------------------

unsigned int VolumeTree::TransformNode::GetNodeCost()
{
	unsigned int cost = 0;
	if( m_applyTranslate ) cost += 1;
	if( m_applyRotate ) cost += 3;
	if( m_applyScale ) cost += 1;
	return cost;
}

//----------------------------------------------------------------------------------

void VolumeTree::TransformNode::GetBounds( float *_minX, float *_maxX, float *_minY, float *_maxY, float *_minZ, float *_maxZ )
{
	if( m_child != NULL )
	{
		// Get child bounds then transform them
		
		float AminX, AmaxX, AminY, AmaxY, AminZ, AmaxZ;
		m_child->GetBounds( &AminX, &AmaxX, &AminY, &AmaxY, &AminZ, &AmaxZ );

		//cml::vector4f minvals(AminX,AminY,AminZ,1);
		//cml::vector4f maxvals(AmaxX,AmaxY,AmaxZ,1);

		// Points from all 8 corners
		cml::vector4f point0( AminX, AminY, AminZ, 1.0f );
		cml::vector4f point1( AminX, AminY, AmaxZ, 1.0f );
		cml::vector4f point2( AminX, AmaxY, AminZ, 1.0f );
		cml::vector4f point3( AminX, AmaxY, AmaxZ, 1.0f );
		
		cml::vector4f point4( AmaxX, AminY, AminZ, 1.0f );
		cml::vector4f point5( AmaxX, AminY, AmaxZ, 1.0f );
		cml::vector4f point6( AmaxX, AmaxY, AminZ, 1.0f );
		cml::vector4f point7( AmaxX, AmaxY, AmaxZ, 1.0f );

		// Transform all points
		cml::matrix44f_c inverseTransform = cml::inverse( m_transformMatrix );
		point0 = inverseTransform * point0;
		point1 = inverseTransform * point1;
		point2 = inverseTransform * point2;
		point3 = inverseTransform * point3;
		point4 = inverseTransform * point4;
		point5 = inverseTransform * point5;
		point6 = inverseTransform * point6;
		point7 = inverseTransform * point7;

		// Work out new min/max
		float minvals[ 3 ];
		float maxvals[ 3 ];
		for( unsigned int i = 0; i < 3; i++ )
		{
			float min0 = std::min< float >( std::min< float >( point0[ i ], point1[ i ] ), std::min< float >( point2[ i ], point3[ i ] ) );
			float min1 = std::min< float >( std::min< float >( point4[ i ], point5[ i ] ), std::min< float >( point6[ i ], point7[ i ] ) );
			minvals[ i ] = std::min< float >( min0, min1 );

			float max0 = std::max< float >( std::max< float >( point0[ i ], point1[ i ] ), std::max< float >( point2[ i ], point3[ i ] ) );
			float max1 = std::max< float >( std::max< float >( point4[ i ], point5[ i ] ), std::max< float >( point6[ i ], point7[ i ] ) );
			maxvals[ i ] = std::max< float >( max0, max1 );
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

		*_minX = minvals[ 0 ];
		*_maxX = maxvals[ 0 ];
		*_minY = minvals[ 1 ];
		*_maxY = maxvals[ 1 ];
		*_minZ = minvals[ 2 ];
		*_maxZ = maxvals[ 2 ];
		
	}
	else
	{
		// Results undefined
		*_minX = 0.0f;
		*_maxX = 0.0f;
		*_minY = 0.0f;
		*_maxY = 0.0f;
		*_minZ = 0.0f;
		*_maxZ = 0.0f;
	}
}

//----------------------------------------------------------------------------------

void VolumeTree::TransformNode::Reset()
{
	m_tx = m_ty = m_tz = 0.0f; //translate, 0 by default
	m_rx = m_ry = m_rz = 0.0f; //rotate as euler angles in radians, 0 by default
	m_sx = m_sy = m_sz = 1.0f; //scale, 1 by default
	m_refrx = m_refry = m_refrz = 0.0f; //reference point for rotation, 0 by default
	m_refsx = m_refsy = m_refsz = 0.0f; //reference point for scale, 0 by default

	m_applyTranslate = m_applyRotate = m_applyScale = false;
	m_transformMatrix.identity();
}

//----------------------------------------------------------------------------------

void VolumeTree::TransformNode::OnBuildParameters( GLSLRenderer *_renderer )
{
	m_paramRenderer = _renderer;
	if( m_useParams && ( m_matrixParam == 0) )
	{
		if( m_applyTranslate && !m_applyRotate && !m_applyScale )
		{
			m_translationParam = _renderer->NewParameter( GLSLRenderer::VEC3 );
			#ifdef _DEBUG
			std::cout << "INFO: TransformNode retrieved param for translation: " << m_translationParam << std::endl;
			#endif
		}
		else if( m_applyTranslate || m_applyRotate || m_applyScale )
		{
			m_matrixParam = _renderer->NewParameter( GLSLRenderer::MAT4 );
			#ifdef _DEBUG
			std::cout << "INFO: TransformNode retrieved param for matrix: " << m_matrixParam << std::endl;
			#endif
		}
	}
}

//----------------------------------------------------------------------------------

void VolumeTree::TransformNode::OnUpdateParameters( GLSLRenderer *_renderer )
{
	if( m_useParams )
	{
		// Unfortunately this needs updating regularly, since we don't know if nodes with lower ID's have been removed or not
		if( m_matrixParam > 0 )
		{
			_renderer->SetParameter( m_matrixParam, m_transformMatrix.data() );
			m_matrixParamString = _renderer->GetParameterString( m_matrixParam );
			#ifdef _DEBUG
			//std::cout << "INFO: TransformNode retrieved string for matrix, ID: " << m_matrixParam << " string: " << m_matrixParamString << std::endl;
			#endif
		}
		else
		{
			m_matrixParamString.clear();
		}
		if( m_translationParam > 0 )
		{
			float *data = new float[ 3 ];
			data[ 0 ] = m_tx;
			data[ 1 ] = m_ty;
			data[ 2 ] = m_tz;
			_renderer->SetParameter( m_translationParam, data );
			m_translationParamString = _renderer->GetParameterString( m_translationParam );
			#ifdef _DEBUG
			//std::cout << "INFO: TransformNode retrieved string for translation, ID: " << m_translationParam << " string: " << m_translationParamString << std::endl;
			#endif
		}
		else
		{
			m_translationParamString.clear();
		}
	}
}

//----------------------------------------------------------------------------------