#include "VolumeTree/Leaves/VolCacheNode.h"

#include "vol_metamorph.h"
#include "vol_totem.h"

//----------------------------------------------------------------------------------

VolumeTree::VolCacheNode::VolCacheNode()
{
	m_requiresCache = true;
	m_boundsMinX = m_boundsMaxX = m_boundsMinY = m_boundsMaxY = m_boundsMinZ = m_boundsMaxZ = 0.0f;
	m_cachedFunction = NULL;
	m_volCacheNode = NULL;
}

//----------------------------------------------------------------------------------

VolumeTree::VolCacheNode::VolCacheNode( totemio::CacheNode *_nodeIn )
{
	m_volCacheNode = _nodeIn;
	m_requiresCache = true;
	m_boundsMinX = m_boundsMaxX = m_boundsMinY = m_boundsMaxY = m_boundsMinZ = m_boundsMaxZ = 0.0f;
	m_cachedFunction = NULL;

	// Just get bounds for now
	if( m_volCacheNode != NULL )
	{
		float *bounds = NULL; //new float[27];

		bool result = m_volCacheNode->getBoundingBox( &bounds );
		if( result )
		{
			// For now enlarge bbox by 10%
			// TODO: ensure negative shell around contents
			float lengthX = bounds[ 3 ] - bounds[ 0 ];
			float lengthY = bounds[ 4 ] - bounds[ 1 ];
			float lengthZ = bounds[ 5 ] - bounds[ 2 ];

			m_boundsMinX = bounds[ 0 ] - lengthX * 0.1f;
			m_boundsMaxX = bounds[ 3 ] + lengthX * 0.1f;
			m_boundsMinY = bounds[ 1 ] - lengthY * 0.1f;
			m_boundsMaxY = bounds[ 4 ] + lengthY * 0.1f;
			m_boundsMinZ = bounds[ 2 ] - lengthZ * 0.1f;
			m_boundsMaxZ = bounds[ 5 ] + lengthZ * 0.1f;

			totemio::freePointer( &bounds );
		}
	}
}

//----------------------------------------------------------------------------------

VolumeTree::VolCacheNode::VolCacheNode( std::string _filename )
{
	m_volCacheNode = NULL;
	m_requiresCache = true;
	m_filename = _filename;
	m_boundsMinX = m_boundsMaxX = m_boundsMinY = m_boundsMaxY = m_boundsMinZ = m_boundsMaxZ = 0.0f;
	m_cachedFunction = NULL;
	// Just get bounds for now

	float *bounds = NULL;
	float *data = NULL; //new float[27];

	open_params params;
	params.m_fit_to_box = false;
	params.m_calculate_bbox = true;
	params.m_fill_array = false;
	params.m_exact_bbox = false;
	
	if( !openVol( _filename.c_str(), 3,3,3, params, &bounds, &data ) )
	{
		std::cerr << "WARNING: Could not open VOL file: " << _filename << std::endl;
	}
	else
	{
		m_boundsMinX = bounds[ 0 ];
		m_boundsMaxX = bounds[ 3 ];
		m_boundsMinY = bounds[ 1 ];
		m_boundsMaxY = bounds[ 4 ];
		m_boundsMinZ = bounds[ 2 ];
		m_boundsMaxZ = bounds[ 5 ];
	}

	//delete [] bounds;
	//delete [] data;
	freePt( &bounds );
	freePt( &data );
}

//----------------------------------------------------------------------------------

VolumeTree::VolCacheNode::~VolCacheNode()
{
	if( m_volCacheNode != NULL )
	{
		totemio::freeTree( m_volCacheNode );
		totemio::freePointer( &m_cachedFunction );
	}
	else if( m_cachedFunction != NULL )
	{
		freePt( &m_cachedFunction );
	}
}

//----------------------------------------------------------------------------------

float VolumeTree::VolCacheNode::GetFunctionValue( float _x, float _y, float _z )
{
	// TODO: use cached function
	// Transform coords
	_x = ( _x + m_cacheOffsetX ) * m_cacheScaleX + 0.5f;
	_y = ( _y + m_cacheOffsetY ) * m_cacheScaleY + 0.5f;
	_z = ( _z + m_cacheOffsetZ ) * m_cacheScaleZ + 0.5f;

	// TODO: tri-lerp
	// For now, just use nearest filtering:

	_x = ( std::max )( _x, 0.0f );
	_y = ( std::max )( _y, 0.0f );
	_z = ( std::max )( _z, 0.0f );

	_x = ( std::min )( _x, 1.0f );
	_y = ( std::min )( _y, 1.0f );
	_z = ( std::min )( _z, 1.0f );

	_x = ( _x * ( float )m_cacheResX );
	_y = ( _y * ( float )m_cacheResY );
	_z = ( _z * ( float )m_cacheResZ );

	float scrap;
	_x = modf( _x, &scrap ) < 0.5 ? floor( _x ) : ceil( _x );
	_y = modf( _y, &scrap ) < 0.5 ? floor( _y ) : ceil( _y );
	_z = modf( _z, &scrap ) < 0.5 ? floor( _z ) : ceil( _z );

	return SampleCacheFunction( ( unsigned int )_x, ( unsigned int )_y,( unsigned int )_z );
}

//----------------------------------------------------------------------------------

std::string VolumeTree::VolCacheNode::GetFunctionGLSLString( bool _callCache, std::string _samplePosStr )
{
	// Should never get here, this node can only be cached
	std::cerr << "WARNING: GetFunctionGLSLString() called on VolCacheNode" << std::endl;
	return "-1.0";
}

//----------------------------------------------------------------------------------

void VolumeTree::VolCacheNode::GetBounds( float *_minX, float *_maxX, float *_minY, float *_maxY, float *_minZ, float *_maxZ )
{
	*_minX = m_boundsMinX;
	*_maxX = m_boundsMaxX;
	*_minY = m_boundsMinY;
	*_maxY = m_boundsMaxY;
	*_minZ = m_boundsMinZ;
	*_maxZ = m_boundsMaxZ;
}

//----------------------------------------------------------------------------------

void VolumeTree::VolCacheNode::SetUseCache( bool _useCache, unsigned int _cacheID, unsigned int _cacheResX, unsigned int _cacheResY, unsigned int _cacheResZ )
{
	Node::SetUseCache( _useCache, _cacheID, _cacheResX, _cacheResY, _cacheResZ );
	if( _useCache )
	{
		if( m_volCacheNode )
		{
			#ifdef _DEBUG
			std::cout << "INFO: VolCacheNode::SetUseCache resX: " << _cacheResX << " resY: " << _cacheResY << " resZ: " << _cacheResZ << std::endl;
			#endif
			float *bbox = new float[ 6 ];
			bbox[ 0 ] = m_boundsMinX;
			bbox[ 1 ] = m_boundsMinY;
			bbox[ 2 ] = m_boundsMinZ;
			bbox[ 3 ] = m_boundsMaxX;
			bbox[ 4 ] = m_boundsMaxY;
			bbox[ 5 ] = m_boundsMaxZ;
			if( !m_volCacheNode->generateCache( m_cacheResX, m_cacheResY, m_cacheResZ, bbox, &m_cachedFunction ) )
			{
				std::cerr << "WARNING: Could not build cache from cache node: " << m_volCacheNode->getID() << std::endl;
			}
		}
		else if( !m_filename.empty() )
		{

			float *bounds = NULL;
			m_cachedFunction = NULL;

			open_params params;
			params.m_fit_to_box = false;
			params.m_calculate_bbox = false;
			params.m_fill_array = true;
			params.m_exact_bbox = false;

			std::cout << "INFO: Loading VOL file: " << m_filename << std::endl;
			if( !openVol( m_filename.c_str(), m_cacheResX, m_cacheResY, m_cacheResZ, params, &bounds, &m_cachedFunction ) )
			{
				std::cerr << "WARNING: Could not build cache from VOL file: " << m_filename << std::endl;
			}
			freePt( &bounds );
		}
	}
}

//----------------------------------------------------------------------------------

void VolumeTree::VolCacheNode::PopulateCacheData( float **_data, float _startX, float _startY, float _startZ, float _stepX, float _stepY, float _stepZ )
{
	if( m_cachedFunction != NULL )
	{
		for( unsigned int i = 0; i < m_cacheResX * m_cacheResY * m_cacheResZ; i++ )
		{
			( *_data)[ i ] = -m_cachedFunction[ i ];
		}
	}
	/*
	float *bounds = NULL;
	_cachedFunction = NULL;
	if( !openVol(_filename.c_str(), _cacheResX,_cacheResY,_cacheResZ, false, &bounds, &_cachedFunction ) )
	{
		std::cerr<<"WARNING: Could not build cache from VOL file: "<<_filename<<std::endl;
	}
	else
	{
		for( unsigned int i = 0; i < _cacheResX*_cacheResY*_cacheResZ; i++ )
		{
			(*data)[i] = -_cachedFunction[i];
		}
	}
	freePt(&bounds);
	*/

	// Unfortunately we need to keep the cache in case a parent node needs caching, in which case we need to be able to sample the function
}

//----------------------------------------------------------------------------------

float VolumeTree::VolCacheNode::SampleCacheFunction( unsigned int _x, unsigned int _y, unsigned int _z )
{
	if( m_cachedFunction != NULL && ( _x < m_cacheResX ) && ( _y < m_cacheResY ) && ( _z < m_cacheResZ ) )
	{
		return -m_cachedFunction[ _x + _y * m_cacheResX + _z * m_cacheResX * m_cacheResY ];
	}
	return -1.0f;
}

//----------------------------------------------------------------------------------