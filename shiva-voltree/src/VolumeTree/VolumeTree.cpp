/*
 * VolumeTree.cpp
 *
 *  Created on: Jan 9, 2013
 *      Author: leigh
 */

#include "VolumeTree/VolumeTree.h"
#include "VolumeTree/Leaves/ConeNode.h"
#include "VolumeTree/Leaves/CubeNode.h"
#include "VolumeTree/Leaves/CylinderNode.h"
#include "VolumeTree/Leaves/SphereNode.h"
#include "VolumeTree/Leaves/TorusNode.h"
#include "VolumeTree/Leaves/VolCacheNode.h"
#include "VolumeTree/Nodes/BlendCSG.h"
#include "VolumeTree/Nodes/CSG.h"
#include "VolumeTree/Nodes/TransformNode.h"

#include <iostream>
#include <sstream>
#include <boost/filesystem.hpp>

VolumeTree::Tree::Tree()
{
	_rootNode = NULL;
	
	_bboxExtents = new float[3];
	_bboxCentre = new float[3];
	_bboxBoundsMin = new float[3];
	_bboxBoundsMax = new float[3];
	_exportID = 0;
}

VolumeTree::Tree::Tree(Node *rootNode)
{
	_rootNode = rootNode;
	
	_bboxExtents = new float[3];
	_bboxCentre = new float[3];
	_bboxBoundsMin = new float[3];
	_bboxBoundsMax = new float[3];
	_exportID = 0;
}

VolumeTree::Tree::~Tree()
{
	// TODO: currently we need to leave the _rootNode because it is probably used externally :S

	delete [] _bboxExtents;
	delete [] _bboxCentre;
	delete [] _bboxBoundsMin;
	delete [] _bboxBoundsMax;
}


bool VolumeTree::Tree::Load(std::string filename)
{
	if( filename.empty() )
		return false;

	if( !boost::filesystem::is_regular_file( filename ) )
	{
		std::cerr<<"WARNING: VolumeTree::Tree cannot find Vol file to load: "<<filename<<std::endl;
		return false;
	}

	std::cout<<"INFO: VolumeTree::Tree Loading from Vol file: "<<filename<<std::endl;

	
	// Build up tree
	totemio::TotemNode *ioNode = NULL;

	bool retVal = openVol(filename.c_str(), &ioNode);

	if( retVal && ioNode != NULL )
	{
		Node *newTree = BuildImportNode(ioNode);
		if( newTree != NULL )
		{
			if( _rootNode != NULL )
			{
				delete _rootNode;
			}
			_rootNode = newTree;
			return true;
		}
		else
		{
			std::cerr<<"ERROR: BuildImportNode failed to give valid root node from loaded Vol file"<<std::endl;
			return false;
		}
	}
	else
	{
		std::cerr<<"ERROR: Failed to load Vol file: "<<filename<<std::endl;
		return false;
	}

	return false;
}

bool VolumeTree::Tree::Save(std::string filename)
{

	if( _rootNode == NULL )
	{
		return false;
	}

	std::cout<<"INFO: VolumeTree::Tree Saving to Vol file: "<<filename<<std::endl;

	// Build up tree
	totemio::TotemNode *exportRootNode = BuildExportNode(_rootNode);

	// Export tree
	bool retVal = saveVol(filename.c_str(), exportRootNode );

	//delete exportRootNode;
	// TODO: freeing the tree causes a crash
	//totemio::freeTree( exportRootNode );

	return retVal;
}

void VolumeTree::Tree::CalcBoundingBox()
{
	if( _rootNode != NULL )
	{
		_rootNode->GetBounds(&(_bboxBoundsMin[0]),&(_bboxBoundsMax[0]), &(_bboxBoundsMin[1]),&(_bboxBoundsMax[1]), &(_bboxBoundsMin[2]),&(_bboxBoundsMax[2]));
		_rootNode->GetBoundSizes( &(_bboxExtents[0]),&(_bboxExtents[1]),&(_bboxExtents[2]) );

		for( unsigned int i = 0; i < 3; i++ )
		{
			_bboxCentre[i] = _bboxBoundsMin[i] + (0.5f*_bboxExtents[i]);
		}
	}
	else
	{
		for( unsigned int i = 0; i < 3; i++ )
		{
			_bboxCentre[i] = _bboxBoundsMin[i] = _bboxBoundsMax[i] = _bboxExtents[i] = 0.0f;
		}
	}
}

void VolumeTree::Tree::GetBoundingBox( float *minvals, float *maxvals)
{
	for(unsigned int i = 0; i < 3; i++ )
	{
		minvals[i] = _bboxBoundsMin[i];
		maxvals[i] = _bboxBoundsMax[i];
	}
}

void VolumeTree::Tree::GetBoundingBoxSize( float *boxSize)
{
	for(unsigned int i = 0; i < 3; i++ )
	{
		boxSize[i] = _bboxExtents[i];
	}
}

void VolumeTree::Tree::GetBoundingBoxCentre( float *boxCentre)
{
	for(unsigned int i = 0; i < 3; i++ )
	{
		boxCentre[i] = _bboxCentre[i];
	}
}

float VolumeTree::Tree::GetBoundingBoxMaxDim()
{
	float bboxMaxDim = _bboxExtents[0];
	for( unsigned int i = 1; i < 3; i++ )
	{
		if( _bboxExtents[i] > bboxMaxDim )
		{
			bboxMaxDim = _bboxExtents[i];
		}
	}
	return bboxMaxDim;
}

void VolumeTree::Tree::DrawBBoxes()
{
	if( _rootNode != NULL )
	{
		_rootNode->DrawBBoxes();
	}
}

float VolumeTree::Tree::GetFunctionValue(float x, float y, float z)
{
	if( _rootNode != NULL )
		return _rootNode->GetFunctionValue(x,y,z);
	return 0.0f;
}

std::string VolumeTree::Tree::GetCachedFunctionGLSLString()
{
	std::stringstream functionString;
	functionString<<"float GetField(vec3 samplePosition) { return ";

	if( _rootNode != NULL )
	{
		functionString<<_rootNode->GetCachedFunctionGLSLString("samplePosition");
	}
	else
	{
		functionString<<"0.0f";
	}
	functionString<<";}";

	return functionString.str();
}

std::string VolumeTree::Tree::GetFunctionGLSLString()
{
	std::stringstream functionString;
	functionString<<"float GetField(vec3 samplePosition) { return ";

	if( _rootNode != NULL )
	{
		functionString<<_rootNode->GetFunctionGLSLString(false,"samplePosition");
	}
	else
	{
		functionString<<"0";
	}
	functionString<<";}";

	return functionString.str();
}

void VolumeTree::Tree::BuildCaches(CachingPolicy *policy, GLSLRenderer *renderer)
{
	if( _rootNode != NULL )
	{
		if( policy != NULL )
		{
			/// The policy decides which nodes should be cached
			policy->Process(_rootNode,renderer);
		}
		/// This will build the actual caches
		_rootNode->BuildCaches(renderer);
	}
}


int VolumeTree::Tree::GetNumNodes()
{
	if( _rootNode != NULL )
	{
		int count = 0, depth = 0;
		Node *result = _rootNode->RecursiveGetNode(-1, count, depth);
		#ifdef _DEBUG
			std::cout<<"INFO: VolumeTree::Tree::GetNumNodes(): "<<count<<" nodes"<<std::endl;
		#endif
		return count + 1;
	}
	return 0;
}

VolumeTree::Node* VolumeTree::Tree::GetNode(int index)
{
	if( _rootNode != NULL )
	{
		int count = 0, depth = 0;
		Node *result = _rootNode->RecursiveGetNode(index, count, depth);
		if( result == NULL )
		{
			std::cerr<<"WARNING: VolumeTree::Tree::GetNode("<<index<<") not found!"<<std::endl;
		}
		return result;
	}
	return NULL;
}

int VolumeTree::Tree::GetNodeDepth(int index)
{
	if( _rootNode != NULL )
	{
		int count = 0, depth = 0;
		Node *result = _rootNode->RecursiveGetNode(index, count, depth);
		return depth;
	}
	return 0;
}

void VolumeTree::Tree::UpdateParameters( GLSLRenderer *renderer )
{
	if( _rootNode != NULL )
	{
		_rootNode->UpdateParameters( renderer );
	}
}

VolumeTree::Node* VolumeTree::Tree::BuildImportNode(totemio::TotemNode *currentNode)
{
	if( currentNode == NULL )
	{
		std::cerr<<"ERROR: VolumeTree::Tree::BuildImportNode given NULL root, results undefined"<<std::endl;
		return NULL;
	}

	// Create corresponding internal node and fill with data
	VolumeTree::Node *currentImport = NULL;
	if( currentNode->getTypeID() == CONE_NODE_ID )
	{
		totemio::ConeNode *currentTyped = dynamic_cast<totemio::ConeNode *>(currentNode);

		if( currentTyped != NULL )
		{
			float radius, height;
			currentTyped->getConeParams(radius, height);
			VolumeTree::ConeNode *currentImportTyped = new VolumeTree::ConeNode(height,radius);

			currentImport = (VolumeTree::Node*) currentImportTyped;
		}
		else
		{
			std::cerr<<"ERROR: VolumeTree::Tree::BuildImportNode node ID type does not match class type: cone, results undefined"<<std::endl;
			return NULL;
		}
	}
	else if( currentNode->getTypeID() == BOX_NODE_ID )
	{
		totemio::BoxNode *currentTyped = dynamic_cast<totemio::BoxNode *>(currentNode);

		if( currentTyped != NULL )
		{
			float dimx, dimy, dimz;
			currentTyped->getBoxParams(dimx, dimy, dimz);
			VolumeTree::CubeNode *currentImportTyped = new VolumeTree::CubeNode(dimx,dimy,dimz);

			currentImport = (VolumeTree::Node*) currentImportTyped;
		}
		else
		{
			std::cerr<<"ERROR: VolumeTree::Tree::BuildImportNode node ID type does not match class type: cube (box), results undefined"<<std::endl;
			return NULL;
		}
	}
	else if( currentNode->getTypeID() == CYLINDER_NODE_ID )
	{
		totemio::CylinderNode *currentTyped = dynamic_cast<totemio::CylinderNode *>(currentNode);

		if( currentTyped != NULL )
		{
			float radiusX, radiusY, height;
			currentTyped->getCylinderParams(radiusX, radiusY, height);
			VolumeTree::CylinderNode *currentImportTyped = new VolumeTree::CylinderNode(height,radiusX, radiusY);

			currentImport = (VolumeTree::Node*) currentImportTyped;
		}
		else
		{
			std::cerr<<"ERROR: VolumeTree::Tree::BuildImportNode node ID type does not match class type: cylinder, results undefined"<<std::endl;
			return NULL;
		}
	}
	else if( currentNode->getTypeID() == ELLIPSOID_NODE_ID )
	{
		totemio::EllipsoidNode *currentTyped = dynamic_cast<totemio::EllipsoidNode *>(currentNode);

		if( currentTyped != NULL )
		{
			float dimx, dimy, dimz;
			currentTyped->getEllipsoidParams(dimx, dimy, dimz);
			VolumeTree::SphereNode *currentImportTyped = new VolumeTree::SphereNode(dimx, dimy, dimz);

			currentImport = (VolumeTree::Node*) currentImportTyped;
		}
		else
		{
			std::cerr<<"ERROR: VolumeTree::Tree::BuildImportNode node ID type does not match class type: sphere (ellipsoid), results undefined"<<std::endl;
			return NULL;
		}
	}
	else if( currentNode->getTypeID() == TORUS_NODE_ID )
	{
		totemio::TorusNode *currentTyped = dynamic_cast<totemio::TorusNode *>(currentNode);

		if( currentTyped != NULL )
		{
			float radius_big, radius_small;
			currentTyped->getTorusParams(radius_big, radius_small);
			VolumeTree::TorusNode *currentImportTyped = new VolumeTree::TorusNode(radius_big,radius_small);

			currentImport = (VolumeTree::Node*) currentImportTyped;
		}
		else
		{
			std::cerr<<"ERROR: VolumeTree::Tree::BuildImportNode node ID type does not match class type: torus, results undefined"<<std::endl;
			return NULL;
		}
	}
	else if( currentNode->getTypeID() == CACHE_NODE_ID )
	{
		totemio::CacheNode *currentTyped = dynamic_cast<totemio::CacheNode *>(currentNode);

		if( currentTyped != NULL )
		{
			VolumeTree::VolCacheNode *currentImportTyped = new VolumeTree::VolCacheNode(currentTyped);
			
			currentImport = (VolumeTree::Node*) currentImportTyped;
		}
		else
		{
			std::cerr<<"ERROR: VolumeTree::Tree::BuildImportNode node ID type does not match class type: cache node, results undefined"<<std::endl;
			return NULL;
		}
	}
	else if( (currentNode->getTypeID() == CSG_NODE_ID) || (currentNode->getTypeID() == BLENDCSG_NODE_ID) )
	{
		totemio::CSGNode *currentTyped = dynamic_cast<totemio::CSGNode *>(currentNode);

		if( currentTyped != NULL )
		{
			VolumeTree::CSGNode *currentImportTyped;
			
			if( currentNode->getTypeID() == BLENDCSG_NODE_ID )
			{
				totemio::BlendCSGNode *blendCurrentTyped = dynamic_cast<totemio::BlendCSGNode *>(currentNode);
				VolumeTree::BlendCSGNode *blendCurrentImportTyped = new VolumeTree::BlendCSGNode();

				if( blendCurrentTyped != NULL )
				{
					float a0, a1, a2;
					blendCurrentTyped->getBlendParams(a0, a1, a2);
					blendCurrentImportTyped->SetBlendParams(a0, a1, a2);

					currentImportTyped = (VolumeTree::CSGNode*) blendCurrentImportTyped;
				}
				else
				{
					std::cerr<<"ERROR: VolumeTree::Tree::BuildImportNode node ID type does not match class type: BlendCSG, results undefined"<<std::endl;
					return NULL;
				}
			}
			else
			{
				currentImportTyped = new VolumeTree::CSGNode();	
			}

			VolumeTree::CSGNode::CSGType csgImportType;
			int csgType;
			currentTyped->getCSGType( csgType );

			if( csgType == totemio::CSGNode::CSG_TYPE_UNION )
				csgImportType = VolumeTree::CSGNode::CSG_UNION;
			else if( csgType == totemio::CSGNode::CSG_TYPE_INTERSECTION )
				csgImportType = VolumeTree::CSGNode::CSG_INTERSECTION;
			else if( csgType == totemio::CSGNode::CSG_TYPE_SUBTRACTION )
				csgImportType = VolumeTree::CSGNode::CSG_SUBTRACTION;
			currentImportTyped->SetCSGType(csgImportType);

			currentImport = (VolumeTree::Node*) currentImportTyped;

			// Recurse children
			unsigned int numChildren = currentNode->getChildrenSize();
			if( numChildren == 2 )
			{
				currentImportTyped->SetChildA( BuildImportNode( currentNode->getChild(0) ) );
				currentImportTyped->SetChildB( BuildImportNode( currentNode->getChild(1) ) );
			}
			else
			{
				std::cerr<<"WARNING: VolumeTree::Tree::BuildImportNode CSG node without two child nodes, attempting to continue but results are undefined"<<std::endl;
			}
		}
		else
		{
			std::cerr<<"ERROR: VolumeTree::Tree::BuildImportNode node ID type does not match class type: CSG, results undefined"<<std::endl;
			return NULL;
		}
	}
	else if( currentNode->getTypeID() == TRANSFORM_NODE_ID )
	{
		totemio::TransformNode *currentTyped = dynamic_cast<totemio::TransformNode *>(currentNode);

		if( currentTyped != NULL )
		{
			float tx = 0.0f, ty = 0.0f, tz = 0.0f, //translate, 0 by default
			rx = 0.0f, ry = 0.0f, rz = 0.0f, //rotate as euler angles in radians, 0 by default
			sx = 1.0f, sy = 1.0f, sz = 1.0f, //scale, 1 by default
			refrx = 0.0f, refry = 0.0f, refrz = 0.0f, //reference point for rotation, 0 by default
			refsx = 0.0f, refsy = 0.0f, refsz = 0.0f; //reference point for scale, 0 by default

			currentTyped->getTransformParams(tx, ty, tz, //translate, 0 by default
											rx, ry, rz, //rotate as euler angles in radians, 0 by default
											sx, sy, sz, //scale, 1 by default
											refrx, refry, refrz, //reference point for rotation, 0 by default
											refsx, refsy, refsz); //reference point for scale, 0 by default

			VolumeTree::TransformNode *currentImportTyped = new VolumeTree::TransformNode();

			// TODO: complete this (need to complete the transform class first)
			//currentImportTyped->SetTranslate(tx,ty,tz);
			
			currentImportTyped->SetTransformParams(tx, ty, tz, //translate, 0 by default
											rx, ry, rz, //rotate as euler angles in radians, 0 by default
											sx, sy, sz, //scale, 1 by default
											refrx, refry, refrz, //reference point for rotation, 0 by default
											refsx, refsy, refsz); //reference point for scale, 0 by default

			currentImport = (VolumeTree::Node*) currentImportTyped;

			// Recursively add child
			if( currentNode->getChildrenSize() == 1 )
			{
				currentImportTyped->SetChild( BuildImportNode( currentNode->getChild(0) ) );
			}
			else
			{
				std::cerr<<"WARNING: VolumeTree::Tree::BuildImportNode Transform node without a child node, attempting to continue but results are undefined"<<std::endl;
			}
		}
		else
		{
			std::cerr<<"ERROR: VolumeTree::Tree::BuildImportNode node ID type does not match class type: transform node, results undefined"<<std::endl;
			return NULL;
		}
	}

	
	// In case we have an unknown node type, don't know what to do
	if( currentImport == NULL )
	{
		std::cerr<<"ERROR: VolumeTree::Tree::BuildImportNode could not resolve current node, results undefined"<<std::endl;
		return NULL;
	}

	return currentImport;
}

totemio::TotemNode* VolumeTree::Tree::BuildExportNode(Node *currentNode)
{
	if( currentNode == NULL )
	{
		std::cerr<<"ERROR: VolumeTree::Tree::BuildExportNode given NULL root, results undefined"<<std::endl;
		return NULL;
	}

	// Create corresponding totemio node and fill with data
	totemio::TotemNode *currentExport = NULL;
	std::string nodeTypeStr = currentNode->GetNodeType();
	if( nodeTypeStr == "ConeNode" )
	{
		VolumeTree::ConeNode *currentTyped = dynamic_cast<VolumeTree::ConeNode *>(currentNode);

		if( currentTyped != NULL )
		{
			totemio::ConeNode *currentExportTyped = new totemio::ConeNode( GetExportNodeID( currentNode ).c_str() );
			currentExportTyped->setConeParams(currentTyped->GetRadius(),currentTyped->GetLength());
			currentExport = (totemio::TotemNode*) currentExportTyped;
		}
		else
		{
			std::cerr<<"ERROR: VolumeTree::Tree::BuildExportNode node string type does not match class type: "<<nodeTypeStr<<", results undefined"<<std::endl;
			return NULL;
		}
	}
	else if( nodeTypeStr == "CubeNode" )
	{
		VolumeTree::CubeNode *currentTyped = dynamic_cast<VolumeTree::CubeNode *>(currentNode);

		if( currentTyped != NULL )
		{
			totemio::BoxNode *currentExportTyped = new totemio::BoxNode( GetExportNodeID( currentNode ).c_str() );
			currentExportTyped->setBoxParams(currentTyped->GetLengthX(),currentTyped->GetLengthY(),currentTyped->GetLengthZ());
			currentExport = (totemio::TotemNode*) currentExportTyped;
		}
		else
		{
			std::cerr<<"ERROR: VolumeTree::Tree::BuildExportNode node string type does not match class type: "<<nodeTypeStr<<", results undefined"<<std::endl;
			return NULL;
		}
	}
	else if( nodeTypeStr == "CylinderNode" )
	{
		VolumeTree::CylinderNode *currentTyped = dynamic_cast<VolumeTree::CylinderNode *>(currentNode);

		if( currentTyped != NULL )
		{
			totemio::CylinderNode *currentExportTyped = new totemio::CylinderNode( GetExportNodeID( currentNode ).c_str() );
			currentExportTyped->setCylinderParams(currentTyped->GetRadiusX(),currentTyped->GetRadiusY(),currentTyped->GetLength());
			currentExport = (totemio::TotemNode*) currentExportTyped;
		}
		else
		{
			std::cerr<<"ERROR: VolumeTree::Tree::BuildExportNode node string type does not match class type: "<<nodeTypeStr<<", results undefined"<<std::endl;
			return NULL;
		}
	}
	else if( nodeTypeStr == "SphereNode" )
	{
		VolumeTree::SphereNode *currentTyped = dynamic_cast<VolumeTree::SphereNode *>(currentNode);

		if( currentTyped != NULL )
		{
			totemio::EllipsoidNode *currentExportTyped = new totemio::EllipsoidNode( GetExportNodeID( currentNode ).c_str() );
			currentExportTyped->setEllipsoidParams(currentTyped->GetRadiusX(),currentTyped->GetRadiusY(),currentTyped->GetRadiusZ());
			currentExport = (totemio::TotemNode*) currentExportTyped;
		}
		else
		{
			std::cerr<<"ERROR: VolumeTree::Tree::BuildExportNode node string type does not match class type: "<<nodeTypeStr<<", results undefined"<<std::endl;
			return NULL;
		}
	}
	else if( nodeTypeStr == "TorusNode" )
	{
		VolumeTree::TorusNode *currentTyped = dynamic_cast<VolumeTree::TorusNode *>(currentNode);

		if( currentTyped != NULL )
		{
			totemio::TorusNode *currentExportTyped = new totemio::TorusNode( GetExportNodeID( currentNode ).c_str() );
			currentExportTyped->setTorusParams(currentTyped->GetSweepradius(),currentTyped->GetCircleRadius());
			currentExport = (totemio::TotemNode*) currentExportTyped;
		}
		else
		{
			std::cerr<<"ERROR: VolumeTree::Tree::BuildExportNode node string type does not match class type: "<<nodeTypeStr<<", results undefined"<<std::endl;
			return NULL;
		}
	}
	else if( nodeTypeStr == "BlendCSGNode" )
	{
		VolumeTree::BlendCSGNode *currentTyped = dynamic_cast<VolumeTree::BlendCSGNode *>(currentNode);

		if( currentTyped != NULL )
		{
			totemio::BlendCSGNode *currentExportTyped = new totemio::BlendCSGNode( GetExportNodeID( currentNode ).c_str() );

			float a0,a1,a2;
			currentTyped->GetBlendParams(a0,a1,a2);
			currentExportTyped->setBlendParams(a0,a1,a2);

			CSGNode::CSGType csgType = currentTyped->GetCSGType();
			totemio::CSGNode::CSG_TYPE csgExportType;
			if( csgType == CSGNode::CSG_UNION )
				csgExportType = totemio::CSGNode::CSG_TYPE_UNION;
			else if( csgType == CSGNode::CSG_INTERSECTION )
				csgExportType = totemio::CSGNode::CSG_TYPE_INTERSECTION;
			else if( csgType == CSGNode::CSG_SUBTRACTION )
				csgExportType = totemio::CSGNode::CSG_TYPE_SUBTRACTION;
			currentExportTyped->setCSGType(csgExportType);

			currentExport = (totemio::TotemNode*) currentExportTyped;
		}
		else
		{
			std::cerr<<"ERROR: VolumeTree::Tree::BuildExportNode node string type does not match class type: "<<nodeTypeStr<<", results undefined"<<std::endl;
			return NULL;
		}
	}
	else if( nodeTypeStr == "CSGNode" )
	{
		VolumeTree::CSGNode *currentTyped = dynamic_cast<VolumeTree::CSGNode *>(currentNode);

		if( currentTyped != NULL )
		{
			totemio::CSGNode *currentExportTyped = new totemio::CSGNode( GetExportNodeID( currentNode ).c_str() );

			CSGNode::CSGType csgType = currentTyped->GetCSGType();
			totemio::CSGNode::CSG_TYPE csgExportType;
			if( csgType == CSGNode::CSG_UNION )
				csgExportType = totemio::CSGNode::CSG_TYPE_UNION;
			else if( csgType == CSGNode::CSG_INTERSECTION )
				csgExportType = totemio::CSGNode::CSG_TYPE_INTERSECTION;
			else if( csgType == CSGNode::CSG_SUBTRACTION )
				csgExportType = totemio::CSGNode::CSG_TYPE_SUBTRACTION;
			currentExportTyped->setCSGType(csgExportType);

			currentExport = (totemio::TotemNode*) currentExportTyped;
		}
		else
		{
			std::cerr<<"ERROR: VolumeTree::Tree::BuildExportNode node string type does not match class type: "<<nodeTypeStr<<", results undefined"<<std::endl;
			return NULL;
		}
	}
	else if( nodeTypeStr == "TransformNode" )
	{
		VolumeTree::TransformNode *currentTyped = dynamic_cast<VolumeTree::TransformNode *>(currentNode);

		if( currentTyped != NULL )
		{
			totemio::TransformNode *currentExportTyped = new totemio::TransformNode( GetExportNodeID( currentNode ).c_str() );

			//currentExportTyped->setTransformMatrix( cml::matrix44f_c().identity().data() );

			float tx = 0.0f, ty = 0.0f, tz = 0.0f, //translate, 0 by default
			rx = 0.0f, ry = 0.0f, rz = 0.0f, //rotate as euler angles in radians, 0 by default
			sx = 1.0f, sy = 1.0f, sz = 1.0f, //scale, 1 by default
			refrx = 0.0f, refry = 0.0f, refrz = 0.0f, //reference point for rotation, 0 by default
			refsx = 0.0f, refsy = 0.0f, refsz = 0.0f; //reference point for scale, 0 by default



			currentTyped->GetTransformParams(	tx, ty, tz, //translate, 0 by default
												rx, ry, rz, //rotate as euler angles in radians, 0 by default
												sx, sy, sz, //scale, 1 by default
												refrx, refry, refrz, //reference point for rotation, 0 by default
												refsx, refsy, refsz); //reference point for scale, 0 by default

			/*
			bool translationSet, rotationSet, scaleSet;
			if( currentTyped->GetTransformTypes(translationSet, rotationSet, scaleSet) )
			{
				// Need to send whole matrix
				//currentExportTyped->setTransformMatrix( cml::matrix44f_c( currentTyped->GetTransformMatrix() ).transpose().data() );
			}

			if( translationSet )
			{
				currentTyped->GetTranslate(tx,ty,tz);

				std::cout<<"INFO: ******** Translate node values: "<<tx<<" "<<ty<<" "<<tz<<std::endl;

				// Oleg todo:
				//currentExportTyped->setTranslate(x,y,z);
			}
			if( rotationSet )
			{
				
			}
			if( scaleSet )
			{

			}
			*/

			currentExportTyped->setTransformParams(	-tx, -ty, -tz, //translate, 0 by default
													-rx, -ry, -rz, //rotate as euler angles in radians, 0 by default
													sx, sy, sz, //scale, 1 by default
													refrx, refry, refrz, //reference point for rotation, 0 by default
													refsx, refsy, refsz); //reference point for scale, 0 by default
			

			// Temporary code for debugging
			/*
			{
				float *temp = cml::matrix44f_c( currentTyped->GetTransformMatrix() ).transpose().data();
				std::cout<<"INFO: ******** transform matrix values: ";
				for( unsigned int i = 0; i < 16; i++ )
				{
					std::cout<<" "<<temp[i];
				}
				temp[11] = - temp[11]; // no idea why this needs to be negative!
				std::cout<<std::endl;
				currentExportTyped->setTransformMatrix( temp );
			}
			*/


			currentExport = (totemio::TotemNode*) currentExportTyped;
		}
		else
		{
			std::cerr<<"ERROR: VolumeTree::Tree::BuildExportNode node string type does not match class type: "<<nodeTypeStr<<", results undefined"<<std::endl;
			return NULL;
		}
	}

	// In case we have an unknown node type, don't know what to do
	if( currentExport == NULL )
	{
		std::cerr<<"ERROR: VolumeTree::Tree::BuildExportNode could not resolve current node, results undefined"<<std::endl;
		return NULL;
	}

	// Recurse children
	for( Node *currentChild = currentNode->GetFirstChild(); currentChild != NULL; currentChild = currentNode->GetNextChild(currentChild) )
	{
		totemio::TotemNode *childExportNode = BuildExportNode( currentChild );
		currentExport->addChild(childExportNode);
	}

	return currentExport;
}

std::string VolumeTree::Tree::GetExportNodeID( Node *currentNode )
{
	if( currentNode == NULL || currentNode->GetIDString().empty() )
	{
		std::stringstream IDstream;
		IDstream<<_exportID;
		_exportID++;
		return IDstream.str();
	}
	else
	{
		// TODO: check string is unique!
		return currentNode->GetIDString();
	}
}
