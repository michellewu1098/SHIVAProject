
#include "VolumeTree/CachingPolicies/CachingPolicy.h"
#include "VolumeRenderer/GLSLRenderer.h"

//----------------------------------------------------------------------------------

VolumeTree::CachingPolicy::CachingPolicy()
{ }

//----------------------------------------------------------------------------------

VolumeTree::CachingPolicy::~CachingPolicy()
{ }

//----------------------------------------------------------------------------------

void VolumeTree::CachingPolicy::Process( Node *_rootNode, GLSLRenderer *_renderer )
{
	// We are only caching nodes which say they require it

	// Find out how many nodes say they need caching
	unsigned int numCacheRequires = _rootNode->GetSubtreeNumCacheRequires( true );
#ifdef _DEBUG
	std::cout << "INFO: Processing " << numCacheRequires << " cache requests" << std::endl;
#endif

	// TODO: If the tree requires a large number of caches we will need to cache more parent nodes to combine the requests

	// TODO: If the tree requires a medium number of caches but this is more caches than the renderer is currently supporting, we need to reserve more within the renderer
	
	// The combined volumes of all nodes requiring caches
	float totalCacheVolume = _rootNode->GetSubtreeCacheVolume();

	unsigned int maxTotalVoxels = _renderer->GetMaxCachingVoxels();

	float cacheDensity = ( ( float )maxTotalVoxels) / totalCacheVolume;

	// Maximum number of voxels for any one node:
	unsigned int maxNodeVoxels = 128 * 128 * 128;

	// Go through tree and apply caches:
	TreeTraverse( _rootNode, _renderer, cacheDensity, maxNodeVoxels, 0 );
}

//----------------------------------------------------------------------------------

unsigned int VolumeTree::CachingPolicy::TreeTraverse( Node *_currentNode, GLSLRenderer *_renderer, float _cacheDensity, unsigned int _maxNodeVoxels, unsigned int _currentCacheID )
{

	if( _currentNode->GetRequiresCache() )
	{
		// First, see if it is already using a cache
		if( _currentNode->GetUseCache() )
		{
			// This allows for nodes appearing multiple times in the tree
			// It also allows for more dynamic tree changes (I hope)
			// TODO: see if the cache resolution it's using is good enough or if it needs to be remade at a different resolution
			// TODO: need to pass down whether a parent node is cached, to potentially free caches that aren't required
		}
		else
		{
			float nodeVolume = _currentNode->GetSubtreeCacheVolume();

			unsigned int numVoxels = ( unsigned int )( nodeVolume * _cacheDensity );
			if( numVoxels > _maxNodeVoxels )
			{
				numVoxels = _maxNodeVoxels;
			}
			float nodeCacheDensity = pow( ( ( float )numVoxels ) / nodeVolume, 1.0f / 3.0f );

			float boundsX, boundsY, boundsZ;
			_currentNode->GetBoundSizes(&boundsX,&boundsY,&boundsZ);

			unsigned int cacheResX = ( unsigned int )( boundsX * nodeCacheDensity );
			unsigned int cacheResY = ( unsigned int )( boundsY * nodeCacheDensity );
			unsigned int cacheResZ = ( unsigned int )( boundsZ * nodeCacheDensity );

			_currentNode->SetUseCache( true, _currentCacheID, cacheResX, cacheResY, cacheResZ );
			++_currentCacheID;
		}
	}
	
	// We now continue traversing and tell all nodes their cache settings, even those which are below a caching node
	// This is to help nodes that need to load their data to know the resolution they should load at
	// Otherwise these nodes will be empty of data when the caches are actually built
	// (we are not actually building caches yet, just telling nodes the settings for when caches are built)
	// (nodes that load their data will do so at this point though)

	{
		for( Node *currentChild = _currentNode->GetFirstChild(); currentChild != NULL; currentChild = _currentNode->GetNextChild( currentChild ) )
		{
			_currentCacheID = TreeTraverse( currentChild, _renderer, _cacheDensity, _maxNodeVoxels, _currentCacheID );
		}
	}
	return _currentCacheID;
}

//----------------------------------------------------------------------------------