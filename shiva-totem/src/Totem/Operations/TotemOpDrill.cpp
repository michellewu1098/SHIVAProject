
#include "Totem/Operations/TotemOpDrill.h"

//----------------------------------------------------------------------------------

Totem::Operations::Drill::Drill( float drillSize )
{
	_drillOpNode = new VolumeTree::CSGNode();
	_drillOpNode->SetCSGType( VolumeTree::CSGNode::CSG_SUBTRACTION );

	_drillShapeNode = new VolumeTree::CylinderNode( 20.0f, drillSize, drillSize );
	
	_drillRotateNode = new VolumeTree::TransformNode( _drillShapeNode );
	_drillTranslateNode = new VolumeTree::TransformNode( _drillRotateNode );

	_drillOpNode->SetChildB( _drillTranslateNode );
}

//----------------------------------------------------------------------------------

Totem::Operations::Drill::Drill( VolumeTree::CylinderNode *_shape, VolumeTree::TransformNode *_rotate, VolumeTree::TransformNode *_translate )
{
	_drillOpNode = new VolumeTree::CSGNode();
	_drillOpNode->SetCSGType( VolumeTree::CSGNode::CSG_SUBTRACTION );

	_drillShapeNode = _shape;
	_drillRotateNode = _rotate;
	_drillTranslateNode = _translate;

	_drillOpNode->SetChildB( _drillTranslateNode );
}

//----------------------------------------------------------------------------------

Totem::Operations::Drill::~Drill()
{
	//delete _drillOpNode;
	//delete _drillShapeNode;
	//delete _drillTranslateNode;
	//delete _drillRotateNode;
}

//----------------------------------------------------------------------------------

VolumeTree::Node* Totem::Operations::Drill::GetNodeTree( VolumeTree::Node *childNode )
{
	_drillOpNode->SetChildA( childNode );
	return _drillOpNode;
}

//----------------------------------------------------------------------------------

void Totem::Operations::Drill::SetDrill( float originX, float originY, float originZ, float dirX, float dirY, float dirZ, float length )
{
	// Set the transform
	_drillRotateNode->SetRotate( dirX, dirY, dirZ );
	_drillTranslateNode->SetTranslate( originX, originY, originZ );
}

//----------------------------------------------------------------------------------