#include "VolumeTree/Nodes/CSG.h"

//----------------------------------------------------------------------------------

VolumeTree::CSGNode::CSGNode()
{
	m_childA = m_childB = NULL;
	m_CSGType = CSG_UNION;
}

//----------------------------------------------------------------------------------

VolumeTree::CSGNode::CSGNode( Node *_childA, Node *_childB )
{
	m_childA = _childA;
	m_childB = _childB;
	m_CSGType = CSG_UNION;
}

//----------------------------------------------------------------------------------

VolumeTree::CSGNode::~CSGNode()
{
	//delete m_childA;
	//delete m_childB;
}

//----------------------------------------------------------------------------------

float VolumeTree::CSGNode::GetFunctionValue( float _x, float _y, float _z )
{
	//return std::max< float >( m_childA->GetFunctionValue( _x, _y, _z ), m_childB->GetFunctionValue( _x, _y, _z ) ); // fmax( m_childA->GetFunctionValue( _x, _y, _z ), m_childB->GetFunctionValue( _x, _y, _z ) );

	// TODO: CSG types other than union!!!

	// R-intersection: f1+f2-sqrt(f1^2+f2^2)
	float f1 = m_childA->GetFunctionValue( _x, _y, _z );
	float f2 = m_childB->GetFunctionValue( _x, _y, _z );
	return f1 + f2 + sqrt( pow( f1, 2 ) + pow( f2, 2 ) );

	// TODO: functions for other operation types
}

//----------------------------------------------------------------------------------

std::string VolumeTree::CSGNode::GetFunctionGLSLString( bool _callCache, std::string _samplePosStr )
{
	if( m_childA != NULL && m_childB != NULL )
	{
		// Just Union for this test
		std::stringstream functionString;
		/*if( callCache )
		{
			functionString<<"CSG_Union("<<_childA->GetCachedFunctionGLSLString(samplePosStr)<<","<<_childB->GetCachedFunctionGLSLString(samplePosStr)<<")";
		}
		else */

		if( m_CSGType == CSG_UNION )
		{
			functionString << "CSG_Union(";
		}
		else if( m_CSGType == CSG_SUBTRACTION )
		{
			functionString << "CSG_Subtract(";
		}
		else if( m_CSGType == CSG_INTERSECTION )
		{
			functionString << "CSG_Intersect(";
		}

		if( _callCache )
		{
			functionString << m_childA->GetCachedFunctionGLSLString( _samplePosStr ) << "," << m_childB->GetCachedFunctionGLSLString( _samplePosStr ) << ")";
		}
		else
		{
			functionString << m_childA->GetFunctionGLSLString( false, _samplePosStr ) << "," << m_childB->GetFunctionGLSLString( false, _samplePosStr ) << ")";
		}

		/*
		if( _CSGType == CSG_UNION )
		{
			functionString<<"CSG_Union("<<_childA->GetFunctionGLSLString(false,samplePosStr)<<","<<_childB->GetFunctionGLSLString(false,samplePosStr)<<")";
		}
		else if( _CSGType == CSG_SUBTRACTION )
		{
			functionString<<"CSG_Subtract("<<_childA->GetFunctionGLSLString(false,samplePosStr)<<","<<_childB->GetFunctionGLSLString(false,samplePosStr)<<")";
		}
		else if( _CSGType == CSG_INTERSECTION )
		{
			functionString<<"CSG_Intersect("<<_childA->GetFunctionGLSLString(false,samplePosStr)<<","<<_childB->GetFunctionGLSLString(false,samplePosStr)<<")";
		}
		*/

		return functionString.str();
	}
	else
	{
		std::cerr << "WARNING: CSGNode has one or more invalid children" << std::endl;
		return "-1";
	}
}

//----------------------------------------------------------------------------------

VolumeTree::Node* VolumeTree::CSGNode::GetFirstChild()
{
	if( m_childA != NULL )
	{
		return m_childA;
	}
	else
		return m_childB;
}

//----------------------------------------------------------------------------------

VolumeTree::Node* VolumeTree::CSGNode::GetNextChild( Node *_previousChild )
{
	if( _previousChild == NULL )
	{
		return GetFirstChild();
	}
	else if( _previousChild == m_childA )
	{
		return m_childB;
	}
	return NULL;
}

//----------------------------------------------------------------------------------

void VolumeTree::CSGNode::GetBounds( float *_minX, float *_maxX, float *_minY, float *_maxY, float *_minZ, float *_maxZ )
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