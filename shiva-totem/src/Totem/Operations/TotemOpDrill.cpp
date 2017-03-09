#include "Totem/Operations/TotemOpDrill.h"

//----------------------------------------------------------------------------------

Totem::Operations::Drill::Drill( float _drillSize )
{
	m_drillOpNode = new VolumeTree::CSGNode();
	m_drillOpNode->SetCSGType( VolumeTree::CSGNode::CSG_SUBTRACTION );

	m_drillShapeNode = new VolumeTree::CylinderNode( 20.0f, _drillSize, _drillSize );
	
	m_drillRotateNode = new VolumeTree::TransformNode( m_drillShapeNode );
	m_drillTranslateNode = new VolumeTree::TransformNode( m_drillRotateNode );

	m_drillOpNode->SetChildB( m_drillTranslateNode );
}

//----------------------------------------------------------------------------------

Totem::Operations::Drill::Drill( VolumeTree::CylinderNode *_shape, VolumeTree::TransformNode *_rotate, VolumeTree::TransformNode *_translate )
{
	m_drillOpNode = new VolumeTree::CSGNode();
	m_drillOpNode->SetCSGType( VolumeTree::CSGNode::CSG_SUBTRACTION );

	m_drillShapeNode = _shape;
	m_drillRotateNode = _rotate;
	m_drillTranslateNode = _translate;

	m_drillOpNode->SetChildB( m_drillTranslateNode );
}

//----------------------------------------------------------------------------------

Totem::Operations::Drill::~Drill()
{
	//delete m_drillOpNode;
	//delete m_drillShapeNode;
	//delete m_drillTranslateNode;
	//delete m_drillRotateNode;
}

//----------------------------------------------------------------------------------

VolumeTree::Node* Totem::Operations::Drill::GetNodeTree( VolumeTree::Node *_childNode )
{
	m_drillOpNode->SetChildA( _childNode );
	return m_drillOpNode;
}

//----------------------------------------------------------------------------------

void Totem::Operations::Drill::SetDrill( float _originX, float _originY, float _originZ, float _dirX, float _dirY, float _dirZ, float _length )
{
	// Set the transform
	m_drillRotateNode->SetRotate( _dirX, _dirY, _dirZ );
	m_drillTranslateNode->SetTranslate( _originX, _originY, _originZ );
}

//----------------------------------------------------------------------------------