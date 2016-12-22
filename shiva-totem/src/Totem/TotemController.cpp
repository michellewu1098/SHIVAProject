
#include "Totem/TotemController.h"

//----------------------------------------------------------------------------------
// Initialise statics

Totem::Controller* Totem::Controller::_instance = NULL;

//----------------------------------------------------------------------------------

Totem::Controller* Totem::Controller::Init()
{
	_instance = new Totem::Controller();
	return _instance;
}

//----------------------------------------------------------------------------------

void Totem::Controller::UnInit()
{
	delete _instance;
}

//----------------------------------------------------------------------------------

Totem::Controller::Controller()
{
	_objectRoot = _selectedObject = NULL;
	_primitives = NULL;

	_poleBaseNode = NULL;
	_poleNode = NULL;
	_poleTransformNode = NULL;

	_showSelection = true;

	_blendAmount = 0.1f;

	RebuildPole();
}

//----------------------------------------------------------------------------------

Totem::Controller::~Controller()
{
	delete _objectRoot;
}

//----------------------------------------------------------------------------------

VolumeTree::Node* Totem::Controller::GetNodeTree()
{
	if( _objectRoot != NULL )
	{
		VolumeTree::Node *treeRoot = _objectRoot->GetNodeTree( _blendAmount );

		if( !_operations.empty() )
		{
			for( std::list< Operation* >::reverse_iterator it = _operations.rbegin(); it != _operations.rend(); ++it )
			{
				treeRoot = ( *it )->GetNodeTree( treeRoot );
			}
		}

		// TODO: clean up this memory leak (the CSG node is never deleted)
		return new VolumeTree::CSGNode( treeRoot, _poleBaseNode );
		//return new VolumeTree::CSGNode(_objectRoot->GetNodeTree(),_poleBaseNode);
	}
	return _poleBaseNode;
}

//----------------------------------------------------------------------------------

void Totem::Controller::SetNumPrimitives( unsigned int value )
{
	if( _primitives != NULL )
	{
		for( unsigned int i = 0; i < value; i++ )
		{
			delete _primitives[ i ];
		}
		delete [] _primitives;
		_primitives = NULL;
	}
	if( value > 0 )
	{
		_primitives = new VolumeTree::Node*[ value ];
		for( unsigned int i = 0; i < value; i++ )
		{
			_primitives[ i ] = NULL;
		}
	}
	_numPrimitives = value;
}

//----------------------------------------------------------------------------------

void Totem::Controller::SetPrimitiveNode( unsigned int ID, VolumeTree::Node *primNode )
{
	if( ( ID < _numPrimitives ) && ( _primitives != NULL ) )
	{
		if( _primitives[ ID ] != NULL )
		{
			delete _primitives[ ID ];
		}
		_primitives[ ID ] = primNode;
	}
}

//----------------------------------------------------------------------------------

VolumeTree::Node* Totem::Controller::GetPrimitiveNode( unsigned int ID )
{
	if( (ID < _numPrimitives) && (_primitives != NULL) )
	{
		return _primitives[ ID ];
	}
	return NULL;
}

//----------------------------------------------------------------------------------

void Totem::Controller::AddObjectToTop( unsigned int primID )
{
	AddObjectNodeToTop( GetPrimitiveNode( primID ) );
}

//----------------------------------------------------------------------------------

void Totem::Controller::AddObjectNodeToTop( VolumeTree::Node *nodeIn )
{
	if( nodeIn != NULL )
	{
		Totem::Object *newTopObj = new Totem::Object( nodeIn );
		if( _objectRoot != NULL )
		{
			float offsetZ = 0.0f, junk;
			_objectRoot->GetTranslationOffset( junk, junk, offsetZ );
			//float baseZ = _objectRoot->GetBaseOffset();
			//float offsetZ = baseZ + (_objectRoot->GetBBoxZ() * 0.5f);
			newTopObj->SetChild( _objectRoot );
			_objectRoot->SetParent( newTopObj );
			newTopObj->AddTranslationOffset( 0.0f, 0.0f, offsetZ, false );
		}
		_objectRoot = newTopObj;
	}
	RebuildPole();
}

//----------------------------------------------------------------------------------

void Totem::Controller::loadModel( VolumeTree::Node *nodeIn )
{

}

//----------------------------------------------------------------------------------

void Totem::Controller::SelectTopObject()
{
	if( _selectedObject != NULL )
	{
		_selectedObject->SetDrawBBox( false );
	}
	_selectedObject = _objectRoot;
	if( _selectedObject != NULL )
	{
		_selectedObject->SetDrawBBox( _showSelection );
	}
}

//----------------------------------------------------------------------------------

void Totem::Controller::SelectObjectAbove()
{
	if( _selectedObject != NULL )
	{
		Totem::Object *newSelection = _selectedObject->GetParent();
		if( newSelection != NULL )
		{
			_selectedObject->SetDrawBBox( false );
			_selectedObject = newSelection;
			_selectedObject->SetDrawBBox( _showSelection );
		}
	}
}

//----------------------------------------------------------------------------------

void Totem::Controller::SelectObjectBelow()
{
	if( _selectedObject != NULL )
	{
		Totem::Object *newSelection = _selectedObject->GetChild();
		if( newSelection != NULL )
		{
			_selectedObject->SetDrawBBox( false );
			_selectedObject = newSelection;
			_selectedObject->SetDrawBBox( _showSelection );
		}
	}
}

//----------------------------------------------------------------------------------

void Totem::Controller::ReorderSelectedObject( bool moveUp )
{
	if( _selectedObject != NULL )
	{
		_selectedObject->ShiftOrder( moveUp );
		_objectRoot = _selectedObject->GetRoot();
		_objectRoot->RecalcOffsets();
	}
}

//----------------------------------------------------------------------------------

void Totem::Controller::MoveSelectedObject( float x, float y, float z )
{
	if( _selectedObject != NULL )
	{
		_selectedObject->AddTranslationOffset( x, y, z );
		_objectRoot = _selectedObject->GetRoot();
	}
}

//----------------------------------------------------------------------------------

void Totem::Controller::ResetMoveSelected()
{
	if( _selectedObject != NULL )
	{
		_selectedObject->SetTranslationOffset( 0.0f, 0.0f, 0.0f );
		_objectRoot = _selectedObject->GetRoot();
	}
}

//----------------------------------------------------------------------------------

void Totem::Controller::DeleteSelectedObject()
{
	if( _selectedObject != NULL )
	{
		Totem::Object *victim = _selectedObject;
		// Remove from tree
		Totem::Object *child = _selectedObject->GetChild();
		Totem::Object *parent = _selectedObject->GetParent();
		victim->SetChild( NULL ); // Otherwise it will try to delete the next node
		_selectedObject = NULL;
		if( child != NULL )
		{
			child->SetParent( parent );
			_selectedObject = child;
		}
		if( parent != NULL )
		{
			parent->SetChild( child );
			_selectedObject = parent;
		}

		// Highlight our new selected object and make sure the root is correct
		if( _selectedObject != NULL )
		{
			_selectedObject->SetDrawBBox( _showSelection );
			_objectRoot = _selectedObject->GetRoot();
		}
		else
		{
			_objectRoot = NULL;
		}

		delete victim;
	}

	if( _objectRoot != NULL )
	{
		_objectRoot->RecalcOffsets();
	}
	RebuildPole();
}

//----------------------------------------------------------------------------------

void Totem::Controller::Clear()
{
	if( _objectRoot != NULL )
	{
		delete _objectRoot;
	}

	if( _operations.size() > 0 )
	{
		for( std::list< Operation* >::iterator it = _operations.begin(); it != _operations.end(); ++it )
		{
			delete ( *it );
		}
	}

	_operations.clear();
	_objectRoot = NULL;
	_selectedObject = NULL;
}

//----------------------------------------------------------------------------------

void Totem::Controller::DeleteAll()
{
	if( _objectRoot != NULL )
	{
		// This will recursively delete entire tree
		delete _objectRoot;
	}
	if( _operations.size() > 0 )
	{
		for( std::list<Operation*>::iterator it = _operations.begin(); it != _operations.end(); ++it )
		{
			delete ( *it );
		}
	}
	_operations.clear();
	_objectRoot = NULL;
	_selectedObject = NULL;
	RebuildPole();
}

//----------------------------------------------------------------------------------

void Totem::Controller::ShowSelection( bool value )
{
	_showSelection = value;
	if( _selectedObject != NULL )
	{
		_selectedObject->SetDrawBBox( _showSelection );
	}
}

//----------------------------------------------------------------------------------

bool Totem::Controller::SelectIntersectingObject( float originX, float originY, float originZ, float dirX, float dirY, float dirZ )
{
	if( _objectRoot != NULL )
	{
		Totem::Object *selection = NULL;
		// This function will choose the intersecting object nearest the origin
		_objectRoot->SelectIntersectingObject( &selection, originX, originY, originZ, dirX, dirY, dirZ );

		if( selection != NULL )
		{
			_selectedObject->SetDrawBBox( false );
			_selectedObject = selection;
			_selectedObject->SetDrawBBox( _showSelection );
			return true;
		}
	}
	return false;
}

//----------------------------------------------------------------------------------

void Totem::Controller::AddOperation( Totem::Operation *inOp )
{
	// Operations are added/removed from the *front* of the list
	if( inOp != NULL )
		_operations.push_front( inOp );
}

//----------------------------------------------------------------------------------

void Totem::Controller::RemoveLastOperation()
{
	if( !_operations.empty() )
	{
		Totem::Operation *lastOp = _operations.front();
		delete lastOp;
		_operations.pop_front();
	}
}

//----------------------------------------------------------------------------------

void Totem::Controller::RebuildPole()
{
	if( _poleBaseNode == NULL )
	{
		_poleNode = new VolumeTree::CylinderNode( 1.0f, 0.05f, 0.05f );
		_poleTransformNode = new VolumeTree::TransformNode( _poleNode );
		_poleTransformNode->SetTranslate( 0.0f, 0.0f, 0.5f );
		VolumeTree::TransformNode *baseTransform = new VolumeTree::TransformNode( new VolumeTree::CylinderNode( 0.1f, 0.5f, 0.5f ) );
		baseTransform->SetTranslate( 0.0f, 0.0f, -0.05f );
		_poleBaseNode = new VolumeTree::CSGNode( _poleTransformNode, baseTransform );
	}

	if( _objectRoot != NULL )
	{
		_objectRoot->RecalcOffsets();
		float topZ = _objectRoot->GetBaseOffset();
		_poleNode->SetLength( topZ + 0.5f );
		_poleTransformNode->SetTranslate( 0.0f, 0.0f, ( topZ + 0.5f ) * 0.5f );
	}
	else
	{
		// There are no objects on the pole, so remove all global operations too
		if( _operations.size() > 0 )
		{
			for( std::list< Operation* >::iterator it = _operations.begin(); it != _operations.end(); ++it )
			{
				delete ( *it );
			}
		}
		_operations.clear();
		_poleTransformNode->SetTranslate( 0.0f, 0.0f, 0.5f );
		_poleNode->SetLength( 1.0f );
	}
}

//----------------------------------------------------------------------------------