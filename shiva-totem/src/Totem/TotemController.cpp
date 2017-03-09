#include "Totem/TotemController.h"

//----------------------------------------------------------------------------------
// Initialise statics

Totem::Controller* Totem::Controller::m_instance = NULL;

//----------------------------------------------------------------------------------

Totem::Controller* Totem::Controller::Init()
{
	m_instance = new Totem::Controller();
	return m_instance;
}

//----------------------------------------------------------------------------------

void Totem::Controller::UnInit()
{
	delete m_instance;
}

//----------------------------------------------------------------------------------

Totem::Controller::Controller()
{
	m_objectRoot = m_selectedObject = NULL;
	m_primitives = NULL;

	m_poleBaseNode = NULL;
	m_poleNode = NULL;
	m_poleTransformNode = NULL;

	m_showSelection = true;

	m_blendAmount = 0.1f;

	RebuildPole();
}

//----------------------------------------------------------------------------------

Totem::Controller::~Controller()
{
	delete m_objectRoot;
}

//----------------------------------------------------------------------------------

VolumeTree::Node* Totem::Controller::GetNodeTree()
{
	if( m_objectRoot != NULL )
	{
		VolumeTree::Node *treeRoot = m_objectRoot->GetNodeTree( m_blendAmount );

		if( !m_operations.empty() )
		{
			for( std::list< Operation* >::reverse_iterator it = m_operations.rbegin(); it != m_operations.rend(); ++it )
			{
				treeRoot = ( *it )->GetNodeTree( treeRoot );
			}
		}

		// TODO: clean up this memory leak (the CSG node is never deleted)
		return new VolumeTree::CSGNode( treeRoot, m_poleBaseNode );
		//return new VolumeTree::CSGNode( _objectRoot->GetNodeTree(), _poleBaseNode );
	}
	return m_poleBaseNode;
}

//----------------------------------------------------------------------------------

void Totem::Controller::SetNumPrimitives( unsigned int _value )
{
	if( m_primitives != NULL )
	{
		for( unsigned int i = 0; i < _value; i++ )
		{
			delete m_primitives[ i ];
		}
		delete [] m_primitives;
		m_primitives = NULL;
	}
	if( _value > 0 )
	{
		m_primitives = new VolumeTree::Node*[ _value ];
		for( unsigned int i = 0; i < _value; i++ )
		{
			m_primitives[ i ] = NULL;
		}
	}
	m_numPrimitives = _value;
}

//----------------------------------------------------------------------------------

void Totem::Controller::SetPrimitiveNode( unsigned int _ID, VolumeTree::Node *_primNode )
{
	if( ( _ID < m_numPrimitives ) && ( m_primitives != NULL ) )
	{
		if( m_primitives[ _ID ] != NULL )
		{
			delete m_primitives[ _ID ];
		}
		m_primitives[ _ID ] = _primNode;
	}
}

//----------------------------------------------------------------------------------

VolumeTree::Node* Totem::Controller::GetPrimitiveNode( unsigned int _ID )
{
	if( ( _ID < m_numPrimitives ) && ( m_primitives != NULL ) )
	{
		return m_primitives[ _ID ];
	}
	return NULL;
}

//----------------------------------------------------------------------------------

void Totem::Controller::AddObjectToTop( unsigned int _primID )
{
	AddObjectNodeToTop( GetPrimitiveNode( _primID ) );
}

//----------------------------------------------------------------------------------

void Totem::Controller::AddObjectNodeToTop( VolumeTree::Node *_nodeIn )
{
	if( _nodeIn != NULL )
	{
		Totem::Object *newTopObj = new Totem::Object( _nodeIn );
		if( m_objectRoot != NULL )
		{
			float offsetZ = 0.0f, junk;
			m_objectRoot->GetTranslationOffset( junk, junk, offsetZ );
			//float baseZ = _objectRoot->GetBaseOffset();
			//float offsetZ = baseZ + (_objectRoot->GetBBoxZ() * 0.5f);
			newTopObj->SetChild( m_objectRoot );
			m_objectRoot->SetParent( newTopObj );
			newTopObj->AddTranslationOffset( 0.0f, 0.0f, offsetZ, false );
		}
		m_objectRoot = newTopObj;
	}
	RebuildPole();
}

//----------------------------------------------------------------------------------

void Totem::Controller::loadModel( std::queue< VolumeTree::Node* > _treeIn )
{
	// There is probably a better way to do this, but it works so..

	VolumeTree::Node* node;
	std::stack< Totem::Object* > objStack; 

	while( !_treeIn.empty() )
	{
		 node = _treeIn.front();
		 _treeIn.pop();

		 if( node != NULL )
		 {
			 std::string nodeType = node->GetNodeType();

			 if( nodeType != "CSGNode" )
			 {
				 if( nodeType != "TransformNode" && nodeType != "BlendCSGNode" ) // If node is a primitive
				 {
					 if( nodeType == "CylinderNode" )
					 {
						 VolumeTree::CylinderNode* cylinderNode = dynamic_cast< VolumeTree::CylinderNode * >( node );
						 if( cylinderNode->isPole() )
						 {
							 break;
						 }
					 }

					 VolumeTree::Node* transformNode = _treeIn.front(); // Retrieve transform node for primitive
					 _treeIn.pop();

					 if( transformNode->GetNodeType() == "TransformNode" )
					 {
						 Totem::Object *newObj = new Totem::Object( node, dynamic_cast< VolumeTree::TransformNode * >( transformNode ) );
						 objStack.push( newObj );
					 }
				 }
				 else if( nodeType == "BlendCSGNode" )
				 {
					 // Need to retrieve blending amount if Pump Operation is used
					 VolumeTree::BlendCSGNode* blendNode = dynamic_cast< VolumeTree::BlendCSGNode * >( node );
					 float junk;
					 blendNode->GetBlendParams( m_blendAmount, junk, junk );
				 }
			 }
			 else
			 {
				 // This takes care of the global drilling operation
				 VolumeTree::CSGNode *op = dynamic_cast< VolumeTree::CSGNode * >( node );
				 if( op != NULL && op->GetCSGType() == VolumeTree::CSGNode::CSG_SUBTRACTION )
				 {
					 // Because we know that each drilling operation is made up of a CylinderNode and two TransformNodes we can do this
					 std::vector< VolumeTree::Node* > drillNodes( 3 ) ;
					 for( unsigned int i = 0; i < 3; i++ )
					 {
						drillNodes[ i ] = _treeIn.front();
						_treeIn.pop();
					 }

					 Totem::Operations::Drill* drillOp = new  Totem::Operations::Drill( dynamic_cast< VolumeTree::CylinderNode* >( drillNodes[ 0 ] ),
																						dynamic_cast< VolumeTree::TransformNode* >( drillNodes[ 1 ] ),
																						dynamic_cast< VolumeTree::TransformNode* >( drillNodes[ 2 ] ) );
					 m_operations.push_front( drillOp );
			     }
				else
				{
					// If it's CSG::UNION, stop while loop here because *following reasoning* model 
					// objects are on the left branch of the tree with root CSG::UNION
					break;
				}
			 }
		 }
	}

	// Now we order the objects on Totem Pole 
	while( !objStack.empty() )
	{
		Totem::Object *obj = objStack.top();
		objStack.pop();
		
		// Get the object translation on z (position on stack) 
		float tz = 0.0f, junk;
		obj->GetTranslation( junk, junk, tz );

		if( m_objectRoot != NULL )
		{
			obj->SetChild( m_objectRoot );
			m_objectRoot->SetParent( obj );
		}
		
		m_objectRoot = obj;
		
		// Recalculate offsetZ: problem is that when RecalcOffsets() is called, the stacked position is calculated as if the objects where stacked one on
		// top of each other, this obviously led to weird results when loading the models with translations
		float offsetZ = -tz - m_objectRoot->GetStackedPosition();
		m_objectRoot->AddTranslationOffset( 0.0f, 0.0f, offsetZ, false );
	}

	RebuildPole();
}

//----------------------------------------------------------------------------------

void Totem::Controller::SelectTopObject()
{
	if( m_selectedObject != NULL )
	{
		m_selectedObject->SetDrawBBox( false );
	}
	m_selectedObject = m_objectRoot;
	if( m_selectedObject != NULL )
	{
		m_selectedObject->SetDrawBBox( m_showSelection );
	}
}

//----------------------------------------------------------------------------------

void Totem::Controller::SelectObjectAbove()
{
	if( m_selectedObject != NULL )
	{
		Totem::Object *newSelection = m_selectedObject->GetParent();
		if( newSelection != NULL )
		{
			m_selectedObject->SetDrawBBox( false );
			m_selectedObject = newSelection;
			m_selectedObject->SetDrawBBox( m_showSelection );
		}
	}
}

//----------------------------------------------------------------------------------

void Totem::Controller::SelectObjectBelow()
{
	if( m_selectedObject != NULL )
	{
		Totem::Object *newSelection = m_selectedObject->GetChild();
		if( newSelection != NULL )
		{
			m_selectedObject->SetDrawBBox( false );
			m_selectedObject = newSelection;
			m_selectedObject->SetDrawBBox( m_showSelection );
		}
	}
}

//----------------------------------------------------------------------------------

void Totem::Controller::ReorderSelectedObject( bool _moveUp )
{
	if( m_selectedObject != NULL )
	{
		m_selectedObject->ShiftOrder( _moveUp );
		m_objectRoot = m_selectedObject->GetRoot();
		m_objectRoot->RecalcOffsets();
	}
}

//----------------------------------------------------------------------------------

void Totem::Controller::MoveSelectedObject( float _x, float _y, float _z )
{
	if( m_selectedObject != NULL )
	{
		m_selectedObject->AddTranslationOffset( _x, _y, _z );
		m_objectRoot = m_selectedObject->GetRoot();
	}
}

//----------------------------------------------------------------------------------

void Totem::Controller::ResetMoveSelected()
{
	if( m_selectedObject != NULL )
	{
		m_selectedObject->SetTranslationOffset( 0.0f, 0.0f, 0.0f );
		m_objectRoot = m_selectedObject->GetRoot();
	}
}

//----------------------------------------------------------------------------------

void Totem::Controller::DeleteSelectedObject()
{
	if( m_selectedObject != NULL )
	{
		Totem::Object *victim = m_selectedObject;
		// Remove from tree
		Totem::Object *child = m_selectedObject->GetChild();
		Totem::Object *parent = m_selectedObject->GetParent();
		victim->SetChild( NULL ); // Otherwise it will try to delete the next node
		m_selectedObject = NULL;
		if( child != NULL )
		{
			child->SetParent( parent );
			m_selectedObject = child;
		}
		if( parent != NULL )
		{
			parent->SetChild( child );
			m_selectedObject = parent;
		}

		// Highlight our new selected object and make sure the root is correct
		if( m_selectedObject != NULL )
		{
			m_selectedObject->SetDrawBBox( m_showSelection );
			m_objectRoot = m_selectedObject->GetRoot();
		}
		else
		{
			m_objectRoot = NULL;
		}

		delete victim;
	}

	if( m_objectRoot != NULL )
	{
		m_objectRoot->RecalcOffsets();
	}
	RebuildPole();
}

//----------------------------------------------------------------------------------

void Totem::Controller::DeleteAll()
{
	if( m_objectRoot != NULL )
	{
		// This will recursively delete entire tree
		delete m_objectRoot;
	}
	if( m_operations.size() > 0 )
	{
		for( std::list< Operation* >::iterator it = m_operations.begin(); it != m_operations.end(); ++it )
		{
			delete ( *it );
		}
	}
	m_operations.clear();
	m_objectRoot = NULL;
	m_selectedObject = NULL;
	RebuildPole();
}

//----------------------------------------------------------------------------------

void Totem::Controller::ShowSelection( bool _value )
{
	m_showSelection = _value;
	if( m_selectedObject != NULL )
	{
		m_selectedObject->SetDrawBBox( m_showSelection );
	}
}

//----------------------------------------------------------------------------------

bool Totem::Controller::SelectIntersectingObject( float _originX, float _originY, float _originZ, float _dirX, float _dirY, float _dirZ )
{
	if( m_objectRoot != NULL )
	{
		Totem::Object *selection = NULL;
		// This function will choose the intersecting object nearest the origin
		m_objectRoot->SelectIntersectingObject( &selection, _originX, _originY, _originZ, _dirX, _dirY, _dirZ );

		if( selection != NULL )
		{
			m_selectedObject->SetDrawBBox( false );
			m_selectedObject = selection;
			m_selectedObject->SetDrawBBox( m_showSelection );
			return true;
		}
	}
	return false;
}

//----------------------------------------------------------------------------------

void Totem::Controller::AddOperation( Totem::Operation *_inOp )
{
	// Operations are added/removed from the *front* of the list
	if( _inOp != NULL )
		m_operations.push_front( _inOp );
}

//----------------------------------------------------------------------------------

void Totem::Controller::RemoveLastOperation()
{
	if( !m_operations.empty() )
	{
		Totem::Operation *lastOp = m_operations.front();
		delete lastOp;
		m_operations.pop_front();
	}
}

//----------------------------------------------------------------------------------

void Totem::Controller::RebuildPole()
{
	if( m_poleBaseNode == NULL )
	{
		m_poleNode = new VolumeTree::CylinderNode( 1.0f, 0.05f, 0.05f );
		m_poleNode->SetPole( true );
		m_poleTransformNode = new VolumeTree::TransformNode( m_poleNode );
		m_poleTransformNode->SetTranslate( 0.0f, 0.0f, 0.5f );
		VolumeTree::CylinderNode* poleBase = new VolumeTree::CylinderNode( 0.1f, 0.5f, 0.5f );
		poleBase->SetPole( true );
		VolumeTree::TransformNode *baseTransform = new VolumeTree::TransformNode( poleBase );
		baseTransform->SetTranslate( 0.0f, 0.0f, -0.05f );
		m_poleBaseNode = new VolumeTree::CSGNode( m_poleTransformNode, baseTransform );
	}

	if( m_objectRoot != NULL )
	{
		m_objectRoot->RecalcOffsets();
		float topZ = m_objectRoot->GetBaseOffset();
		m_poleNode->SetLength( topZ + 0.5f );
		m_poleTransformNode->SetTranslate( 0.0f, 0.0f, ( topZ + 0.5f ) * 0.5f );
	}
	else
	{
		// There are no objects on the pole, so remove all global operations too
		if( m_operations.size() > 0 )
		{
			for( std::list< Operation* >::iterator it = m_operations.begin(); it != m_operations.end(); ++it )
			{
				delete ( *it );
			}
		}
		m_operations.clear();
		m_poleTransformNode->SetTranslate( 0.0f, 0.0f, 0.5f );
		m_poleNode->SetLength( 1.0f );
	}
}

//----------------------------------------------------------------------------------