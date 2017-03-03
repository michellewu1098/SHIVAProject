#include "VolumeTree/Nodes/BlendCSG.h"

//----------------------------------------------------------------------------------

VolumeTree::BlendCSGNode::BlendCSGNode()
{
	m_childA = m_childB = NULL;
	m_a0 = 0.1f;
	m_a1 = 1.0f;
	m_a2 = 1.0f;
}

//----------------------------------------------------------------------------------

VolumeTree::BlendCSGNode::BlendCSGNode( Node *_childA, Node *_childB )
{
	m_childA = _childA;
	m_childB = _childB;
	m_a0 = 0.1f;
	m_a1 = 1.0f;
	m_a2 = 1.0f;
}

//----------------------------------------------------------------------------------

VolumeTree::BlendCSGNode::~BlendCSGNode()
{ }

//----------------------------------------------------------------------------------

float VolumeTree::BlendCSGNode::GetFunctionValue( float _x, float _y, float _z )
{
	//return std::max<float>(_childA->GetFunctionValue(x,y,z),_childB->GetFunctionValue(x,y,z));//fmax(_childA->GetFunctionValue(x,y,z),_childB->GetFunctionValue(x,y,z));

	// TODO: CSG types other than union!!!

	// R-intersection: f1+f2-sqrt(f1^2+f2^2)
	float f1 = m_childA->GetFunctionValue( _x, _y, _z );
	float f2 = m_childB->GetFunctionValue( _x, _y, _z );
	float value = f1 + f2 + sqrt( pow( f1, 2 ) + pow( f2, 2 ) );

	value += m_a0 / ( 1.0f + pow( f1 / m_a1, 2 ) + pow( f2 / m_a2, 2 ) );
	return value;
}

//----------------------------------------------------------------------------------

std::string VolumeTree::BlendCSGNode::GetFunctionGLSLString( bool _callCache, std::string _samplePosStr )
{
	if( m_childA != NULL && m_childB != NULL )
	{

		std::stringstream functionString;
		if( m_CSGType == CSG_UNION )
		{
			functionString << "BlendCSG_Union(";
		}
		else if( m_CSGType == CSG_SUBTRACTION )
		{
			functionString << "BlendCSG_Subtract(";
		}
		else if( m_CSGType == CSG_INTERSECTION )
		{
			functionString << "BlendCSG_Intersect(";
		}

		if( _callCache )
		{
			functionString << m_childA->GetCachedFunctionGLSLString( _samplePosStr ) << "," << m_childB->GetCachedFunctionGLSLString( _samplePosStr ) << "," << m_a0 << "," << m_a1 << "," << m_a2 << ")";
		}
		else
		{
			functionString << m_childA->GetFunctionGLSLString( false, _samplePosStr ) << "," << m_childB->GetFunctionGLSLString( false, _samplePosStr ) << "," << m_a0 << "," << m_a1 << "," << m_a2 <<")";
		}

/*
		if( callCache )
		{
			functionString<<"BlendCSG_Union("<<_childA->GetCachedFunctionGLSLString(samplePosStr)<<","<<_childB->GetCachedFunctionGLSLString(samplePosStr)<<","<<_a0<<","<<_a1<<","<<_a2<<")";
		}
		else
		{
			functionString<<"BlendCSG_Union("<<_childA->GetFunctionGLSLString(false,samplePosStr)<<","<<_childB->GetFunctionGLSLString(false,samplePosStr)<<","<<_a0<<","<<_a1<<","<<_a2<<")";
		}
*/
		return functionString.str();
	}
	else
	{
		std::cerr << "WARNING: BlendCSGNode has one or more invalid children" << std::endl;
		return "-1";
	}
}

//----------------------------------------------------------------------------------

void VolumeTree::BlendCSGNode::GetBounds( float *_minX, float *_maxX, float *_minY, float *_maxY, float *_minZ, float *_maxZ )
{
	if( m_childA != NULL && m_childB != NULL )
	{
		// This is the normal case

		if( m_CSGType == CSG_SUBTRACTION )
		{
			m_childA->GetBounds( _minX, _maxX, _minY, _maxY, _minZ, _maxZ );
			//std::cout<<"INFO: ********************** subtraction"<<std::endl;
		}
		else
		{
			// We cannot tell intersection bounds just from the bounding boxes of our children
			// So we will just treat it the same as a union
			// TODO: fix this

			float AminX, AmaxX, AminY, AmaxY, AminZ, AmaxZ;
			float BminX, BmaxX, BminY, BmaxY, BminZ, BmaxZ;
			m_childA->GetBounds( &AminX, &AmaxX, &AminY, &AmaxY, &AminZ, &AmaxZ );
			m_childB->GetBounds( &BminX, &BmaxX, &BminY, &BmaxY, &BminZ, &BmaxZ );

			*_minX = std::min< float >( AminX, BminX );
			*_maxX = std::max< float >( AmaxX, BmaxX );
			*_minY = std::min< float >( AminY, BminY );
			*_maxY = std::max< float >( AmaxY, BmaxY );
			*_minZ = std::min< float >( AminZ, BminZ );
			*_maxZ = std::max< float >( AmaxZ, BmaxZ );
		}

		float lengthX = ( *_maxX ) - ( *_minX );
		float lengthY = ( *_maxY ) - ( *_minY );
		float lengthZ = ( *_maxZ ) - ( *_minZ );

		*_minX -= ( std::max )( 0.0f, m_a0 * lengthX * 0.20f );
		*_maxX += ( std::max )( 0.0f, m_a0 * lengthX * 0.20f );
		*_minY -= ( std::max )( 0.0f, m_a0 * lengthY * 0.20f );
		*_maxY += ( std::max )( 0.0f, m_a0 * lengthY * 0.20f );
		*_minZ -= ( std::max )( 0.0f, m_a0 * lengthZ * 0.20f );
		*_maxZ += ( std::max )( 0.0f, m_a0 * lengthZ * 0.20f );

	}
	else if( m_childA != NULL && m_childB == NULL )
	{
		m_childA->GetBounds( _minX, _maxX, _minY, _maxY, _minZ, _maxZ );
	}
	else if( m_childA == NULL && m_childB != NULL )
	{
		m_childB->GetBounds( _minX, _maxX, _minY, _maxY, _minZ, _maxZ );
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

/*
TODO
void VolumeTree::BlendCSGNode::GetBounds(float *minX,float *maxX, float *minY,float *maxY, float *minZ,float *maxZ)
{
	if( _childA != NULL && _childB != NULL )
	{
		// This is the normal case

		float AminX, AmaxX, AminY, AmaxY, AminZ, AmaxZ;
		float BminX, BmaxX, BminY, BmaxY, BminZ, BmaxZ;
		_childA->GetBounds(&AminX, &AmaxX, &AminY, &AmaxY, &AminZ, &AmaxZ);
		_childB->GetBounds(&BminX, &BmaxX, &BminY, &BmaxY, &BminZ, &BmaxZ);

		*minX = std::min<float>(AminX,BminX);
		*maxX = std::max<float>(AmaxX,BmaxX);
		*minY = std::min<float>(AminY,BminY);
		*maxY = std::max<float>(AmaxY,BmaxY);
		*minZ = std::min<float>(AminZ,BminZ);
		*maxZ = std::max<float>(AmaxZ,BmaxZ);
	}
	else if( _childA != NULL && _childB == NULL )
	{
		_childA->GetBounds(minX,maxX,minY,maxY,minZ,maxZ);
	}
	else if( _childA == NULL && _childB != NULL )
	{
		_childB->GetBounds(minX,maxX,minY,maxY,minZ,maxZ);
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
*/

//----------------------------------------------------------------------------------