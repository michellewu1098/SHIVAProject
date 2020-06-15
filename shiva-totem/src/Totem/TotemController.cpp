#include "Totem/TotemController.h"
#include "System/SharedPreferences.h"
#include "GUIManager.h"

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

//	ShivaGUI::SharedPreferences *prefs = GetGUIManager()->GetProgSpecificOptions();

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
	//delete m_objectRoot;

	if( m_objectRoot != NULL )
	{
		delete m_objectRoot;
	}

	m_objectRoot = NULL;

	if( m_operations.size() > 0 )
	{
		for( std::list< Operation* >::iterator it = m_operations.begin(); it != m_operations.end(); ++it )
		{
			delete ( *it );
		}
	}
	m_operations.clear();

	if( m_poleNode != NULL ) { delete m_poleNode; } 
	if( m_poleTransformNode != NULL ) { delete m_poleTransformNode; } 
	if( m_poleBaseNode != NULL ) { delete m_poleBaseNode; }
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

		// Doesn't seem to work!!
		if (_primNode->GetNodeType() == "ConeNode")
		{
			_primNode->SetPrimKind("Cone");
		}
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

void Totem::Controller::AddObjectToTop( unsigned int _primID, unsigned int _nGUIControllers  )
{
	AddObjectNodeToTop( _primID, GetPrimitiveNode( _primID ), _nGUIControllers );
}

//----------------------------------------------------------------------------------

void Totem::Controller::AddObjectNodeToTop( int _primID, VolumeTree::Node *_nodeIn, unsigned int _nGUIControllers  )
{
	if( _nodeIn != NULL )
	{
		Totem::Object *newTopObj = new Totem::Object( _nodeIn, _nGUIControllers );
		if( m_objectRoot != NULL )
		{
			float offsetZ = 0.0f, junk;
			m_objectRoot->GetTranslationOffset( junk, junk, offsetZ );
			//float baseZ = _objectRoot->GetBaseOffset();
			//float offsetZ = baseZ + (_objectRoot->GetBBoxZ() * 0.5f);
			newTopObj->SetChild( m_objectRoot );
//			newTopObj->
			m_objectRoot->SetParent( newTopObj );
			newTopObj->AddTranslationOffset( 0.0f, 0.0f, offsetZ, false );
		}
		m_objectRoot = newTopObj;
		// Set type of primitive identifier
		newTopObj->SetPrimTypeID(_primID);
	}
	RebuildPole();
}

//----------------------------------------------------------------------------------

void Totem::Controller::ReAddObjectToTop( Totem::Object* _obj )
{
	if( _obj != NULL )
	{
		if( m_objectRoot != NULL )
		{
			_obj->SetChild( m_objectRoot );
			m_objectRoot->SetParent( _obj );
		}
		m_objectRoot = _obj;
	}	
	RebuildPole();
}

//----------------------------------------------------------------------------------

void Totem::Controller::LoadModel( std::queue< VolumeTree::Node* > _treeIn, unsigned int _nGUIControllers )
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

//						 m_selectedObject->GetPrimitiveID();

						 if( cylinderNode->isPole() )
						 {
							 break;
						 }
					 }

					 VolumeTree::Node* transformNode = _treeIn.front(); // Retrieve transform node for primitive
					 _treeIn.pop();

					 if( transformNode->GetNodeType() == "TransformNode" )
					 {
						 Totem::Object *newObj = new Totem::Object( node, dynamic_cast< VolumeTree::TransformNode * >( transformNode ), _nGUIControllers );
										 
						 std::string primKind = node->GetPrimKind();
						 int primTypeID = node->GetPrimTypeID();

						 //newObj->GetPrimTypeID();

						 //	 					 if( nodeType == "SphereNode" )

						  newObj->SetPrimTypeID(primTypeID);

/*
	 					 if( primKind == "Sphere" )
						 {
							 newObj->SetPrimTypeID(0);
						 }
	 					 if( primKind == "Cone" )
						 {
							 newObj->SetPrimTypeID(1);
						 }
	 					 if( primKind == "Cylinder" )
						 {
							 newObj->SetPrimTypeID(2);
						 }
	 					 if( primKind == "Cube" )
						 {
							 newObj->SetPrimTypeID(3);
						 }
	 					 if( primKind == "Cuboid" )
						 {
							 newObj->SetPrimTypeID(4);
						 }
						 */

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

					 Totem::Operations::Drill* drillOp = new Totem::Operations::Drill(  dynamic_cast< VolumeTree::CylinderNode* >( drillNodes[ 0 ] ),
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
//			std::string xc = obj->GetPrimitiveID();
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

bool AxisChanged(float axisValue)
{
	if (axisValue < 0.0f || axisValue > 0.0f)
		return true;
	else
		return false;
}

//----------------------------------------------------------------------------------

bool Totem::Controller::RebuildRequired(char changedAxis, float translationVal, float selectedObjectNewAxisVal, float selectedObjectAxisValB4)
{
	// Find out if the pole, base and objects need to be rebuilt because of the current move.
	// (This helps to avoid the 'long pole' and 'distant view' issues that existed previously)
	// A rebuild is triggered if an object is, or becomes, the furthest away from all other objects either:
	//   - x axis: Leftmost / Rightmost
	//	 - y axis: Closest / Furthest
	//   - z axis: Highest / Lowest

	// The algortihm for this is:
	// - Iterate through the complete list of totem primitives/objects
	//		- For the axis that has changed, compare the selected object with the current object
	// If the selected object is, or was, the furthest on either end of the axis, rebuild the pole

	// NOTE: This approach uses a single central point (middle) of the objects for comparison.  This could be far more complex and allow for the dimensions of objects,
	//       perhaps using an object's bounding box.  It has been done this way as time was short.

	float mSelectedObjectXB4 = 0.0f;
	float mSelectedObjectYB4 = 0.0f;
	float mSelectedObjectZB4 = 0.0f;

	bool endOfList = false;
	bool rebuildPole = false;

	float mCurrentObjectAxisVal = 0.0f;

	float mCurrentObjectX = 0.0f;
	float mCurrentObjectY = 0.0f;
	float mCurrentObjectZ = 0.0f;

	bool mostNegB4 = true;
	bool mostPosB4 = true;
	bool mostNegNow = true;
	bool mostPosNow = true;

	float epsilon = 0.1f; // Used to see if 2 floating point numbers are roughly 'equal' (allowing for floating point vageries)
	
	m_objectRoot = m_selectedObject->GetRoot();

	m_currentObject = m_objectRoot; // Start at the root

	// Cycle through all objects comparing with the axis of the selected object that has changed i.e. x, y or z
	while(!endOfList)
	{
		if (m_currentObject != NULL)  // This identifies when the end of the list has been reached
		{	
			if (m_currentObject != m_selectedObject) // Do NOT make a comparison of the selected object with itself!
			{						
				// Get the x, y and z co-ords of current object
				m_currentObject->GetTranslation(mCurrentObjectX, mCurrentObjectY, mCurrentObjectZ);

				// Identify which axis has changed
				switch (changedAxis)
				{
					case 'x':
						mCurrentObjectAxisVal =  mCurrentObjectX; // ROUNDF(mCurrentObjectX, 100); // Truncate to 2DP to avoid rounding issues
						break;

					case 'y':
						mCurrentObjectAxisVal = mCurrentObjectY; // ROUNDF(mCurrentObjectY, 100); // Truncate to 2DP to avoid rounding issues
						break;

					case 'z':
						mCurrentObjectAxisVal = mCurrentObjectZ; // ROUNDF(mCurrentObjectZ, 100); // Truncate to 2DP to avoid rounding issues
						break;
				}


				// *** Movement along axis - -ve and +ve ***
				//
				// Example: x axis (left/right):
				//
				//					-----
				//					|	|
				//					|	|
				//					|	|
				//			-ve		|	|	+ve
				//					|	|	
				//					|	|
				//					|	|
				//			---------------------
				//			|					|
				//			---------------------
				
				// Is translation -ve?
				if (translationVal < 0.0f)
				{	
					// Selected object has moved NEGATIVELY along the current axis

					// IS the selected object NOW the MOST NEGATIVE on the axis (or WAS and STILL IS)? - if so, a rebuild will be needed.
					// WAS the selected object the MOST POSITIVE on the axis and NOW IS NOT? - if so, a rebuild will be needed.

					// Check if the selected object IS NOW the MOST NEGATIVE of all the objects on the axis
					if ((mCurrentObjectAxisVal <= selectedObjectNewAxisVal) || (fabs(mCurrentObjectAxisVal - selectedObjectNewAxisVal) < epsilon))
						// The selected object IS NOT the MOST NEGATIVE of all the objects on the axis
						mostNegNow = false;
						
					// Check if the selected object WAS the MOST NEGATIVE of all the objects on the axis before
					if ((mCurrentObjectAxisVal <= selectedObjectAxisValB4) || (fabs(mCurrentObjectAxisVal - selectedObjectAxisValB4) < epsilon))
						// The selected object WAS NOT the MOST NEGATIVE of all the objects on the axis before
						mostNegB4 = false;

					// Check if the selected object IS NOW the MOST POSITIVE of all the objects on the axis
					if ((mCurrentObjectAxisVal >= selectedObjectNewAxisVal) || (fabs(mCurrentObjectAxisVal - selectedObjectNewAxisVal) < epsilon))
						// The selected object IS NOT the MOST POSITIVE of all the objects on the axis
						mostPosNow = false;

					// Check if the selected object WAS the MOST POSITIVE of all the objects on the axis
					if ((mCurrentObjectAxisVal >= selectedObjectAxisValB4) || (fabs(mCurrentObjectAxisVal - selectedObjectAxisValB4) < epsilon))
						// The selected object WAS NOT the MOST POSITIVE of all the objects on the axis before
						mostPosB4 = false;
				}


				// Is translation +ve?
				if (translationVal > 0.0f)
				{
					// Selected object has moved POSITIVELY along the current axis

					// IS the selected object NOW the MOST POSITIVE (or WAS and STILL IS)? - if so, a rebuild will be needed.
					// WAS the selected object the MOST NEGATIVE and NOW IS NOT? - if so, a rebuild will be needed.

					// Check if the selected object IS NOW the MOST POSITIVE of all the objects on the axis
					if ((mCurrentObjectAxisVal >= selectedObjectNewAxisVal) || (fabs(mCurrentObjectAxisVal - selectedObjectNewAxisVal) < epsilon))
						// The selected object IS NOT the MOST POSITIVE of all the objects on the axis
						mostPosNow = false;

					// Check if the selected object WAS the MOST POSITIVE of all the objects on the axis
					if ((mCurrentObjectAxisVal >= selectedObjectAxisValB4) || (fabs(mCurrentObjectAxisVal - selectedObjectAxisValB4) < epsilon))
 						// The selected object WAS NOT the MOST POSITIVE of all the objects on the axis
						mostPosB4 = false;

					// Check if the selected object IS NOW the MOST NEGATIVE of all the objects on the axis
					if ((mCurrentObjectAxisVal <= selectedObjectNewAxisVal) || (fabs(mCurrentObjectAxisVal - selectedObjectNewAxisVal) < epsilon))
 						// The selected object IS NOT the MOST NEGATIVE of all the objects on the axis
						mostNegNow = false;

					// Check if the selected object WAS the MOST NEGATIVE of all the objects on the axis
					if ((mCurrentObjectAxisVal <= selectedObjectAxisValB4) || (fabs(mCurrentObjectAxisVal - selectedObjectAxisValB4) < epsilon))
 						// The selected object WAS NOT the MOST NEGATIVE of all the objects on the axis
						mostNegB4 = false;
				}
			}
			// Get next object
			m_currentObject = m_currentObject->GetChild();
		}
		else
		{
			endOfList = true;
		}			
	}	

	// Rebuilding is only required if an object has moved along an axis and is now:
	// - the furthest object on either end of that axis
	// - or if the furthest object on an axis has now moved nearer to the others
	if (mostNegNow || mostPosNow ||
		(mostPosB4 && !mostPosNow) || (mostNegB4 && !mostNegNow))
		return true;
	else
		return false;
}

//----------------------------------------------------------------------------------
	
void Totem::Controller::MoveSelectedObject( float _x, float _y, float _z )
{
	if( m_selectedObject != NULL )
	{
		// Find out if the pole and base (and objects) need to be rebuilt because of the current move/translation.
		// (This helps to refocus the model to fit the space available and avoids the 'long pole' and 'distant view' issues that existed previously).
		// NOTE: For efficiency reasons, a rebuild is only performed when needed rather than every time a move occurs.
		//		 Rebuilding is 'computationally expensive'/has a performance impact, and so should only be done when necessary.
		// A rebuild is triggered if an object becomes, or was the furthest away from all other objects either:
		//   - x axis: Leftmost / Rightmost
		//	 - y axis: Closest / Furthest
		//   - z axis: Highest / Lowest


		// The algortihm for this is:
		// - Iterate through the list of totem primitives
		// - If the moved primitive is or was the leftmost/rightmost, furthest/closest, highest/lowest then rebuild the pole

		float mSelectedObjectXB4 = 0.0f;
		float mSelectedObjectYB4 = 0.0f;
		float mSelectedObjectZB4 = 0.0f;

		float mSelectedObjectX = 0.0f;
		float mSelectedObjectY = 0.0f;
		float mSelectedObjectZ = 0.0f;

		float mCurrentObjectX = 0.0f;
		float mCurrentObjectY = 0.0f;
		float mCurrentObjectZ = 0.0f;

		bool endOfList = false;
		bool rebuildPole = false;

		bool leftmostB4 = true;
		bool rightmostB4 = true;
		bool closestB4 = true;
		bool furthestB4 = true;
		bool highestB4 = true;
		bool lowestB4 = true;

		bool leftmostNow = true;
		bool rightmostNow = true;
		bool closestNow = true;
		bool furthestNow = true;
		bool highestNow = true;
		bool lowestNow = true;

		bool xChanged = false;
		bool yChanged = false;
		bool zChanged = false;


		// Get x, y and z co-ords of the selected object before the translation is applied
		m_selectedObject->GetTranslation(mSelectedObjectXB4, mSelectedObjectYB4, mSelectedObjectZB4);

		// Apply the translation
		m_selectedObject->AddTranslationOffset( _x, _y, _z );

		// Get x, y and z co-ords of the selected object after translation
		m_selectedObject->GetTranslation(mSelectedObjectX, mSelectedObjectY, mSelectedObjectZ);

		bool rebuild = false;

		if (AxisChanged(_x))
			rebuild = RebuildRequired('x', _x, mSelectedObjectX, mSelectedObjectXB4);
		else
			if (AxisChanged(_y))
				rebuild = RebuildRequired('y', _y, mSelectedObjectY, mSelectedObjectYB4);
			else
				if (AxisChanged(_z))
					rebuild = RebuildRequired('z', _z, mSelectedObjectZ, mSelectedObjectZB4);

		if (rebuild == true)
			RebuildPole();	// This will cause the pole to resize, avoiding the 'long pole' problem that existed previously.
	}
}

//----------------------------------------------------------------------------------

void Totem::Controller::GetSelectedObjectOffset( float &_x, float &_y, float &_z )
{
	if( m_selectedObject != NULL )
	{
		m_selectedObject->GetTranslationOffset( _x, _y, _z );
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

		//delete victim;
	}

	if( m_objectRoot != NULL )
	{
		m_objectRoot->RecalcOffsets();
	}
	RebuildPole();
}

//----------------------------------------------------------------------------------

void Totem::Controller::InsertObject( Totem::Object* _obj )
{
	if( _obj != NULL )
	{
		Totem::Object *child = _obj->GetPrevChild();
		Totem::Object *parent = _obj->GetParent();

		if( child != NULL )
		{
			child->SetParent( _obj );
			_obj->SetChild( child );
		}

		_obj->SetPrevChild( NULL );

		if( parent != NULL )
		{
			parent->SetChild( _obj );
		}

		if( m_selectedObject != NULL )
		{
			m_selectedObject->SetDrawBBox( false );
			m_selectedObject = _obj;
			m_selectedObject->SetDrawBBox( m_showSelection );
			m_objectRoot = m_selectedObject->GetRoot();
		}
		else 
		{
			m_objectRoot = NULL;
		}
	}

	if( m_objectRoot != NULL )
	{
		m_objectRoot->RecalcOffsets();
	}
	RebuildPole();
}

//----------------------------------------------------------------------------------

Totem::Object* Totem::Controller::DeleteSelectedObj()
{
	Totem::Object *victim = NULL;

	if( m_selectedObject != NULL )
	{
		// Remove from tree
		victim = m_selectedObject;

		Totem::Object *child = m_selectedObject->GetChild(); // Identify the child object
		Totem::Object *parentOfSelectedObj = m_selectedObject->GetParent(); // Identify the parent object

		// April 2020 Code added to prevent objects 'above' a selected object, i.e. parents of the object, from falling when the 
		// item is deleted.  The original behaviour caused complex models to collapse if a lower object was deleted
		float selObjTransx;
		float selObjTransy;
		float selObjTransz;

		float selObjOffsetx;
		float selObjOffsety;
		float selObjOffsetz;

		// Get the vertical position and the (vertical) offset of the object being deleted
		m_selectedObject->GetTranslation(selObjTransx, selObjTransy, selObjTransz);
		m_selectedObject->GetTranslationOffset(selObjOffsetx, selObjOffsety, selObjOffsetz);

		// Algorithm
		// ---------
		// Go through all of the parents, grandparents (i.e. the objects 'above' the one being deleted) and so on of the object being 
		// deleted and increase their vertical offsets to prevent them falling when the item is deleted
		// 1. Start with the the parent of the deleted object
		// 2. Increment the vertical offset of the parent
		// 3. Find the parent of the item in 2. and continue until no more parents

		float currParentTransx;
		float currParentTransy;
		float currParentTransz;

		float currParentOffsetx;
		float currParentOffsety;
		float currParentOffsetz;

		Totem::Object *currParent;
		Totem::Object *prevParent;


#ifdef _DEBUG
		// Show translations and offsets of all primitives on the pole
		currParent = m_objectRoot;

		std::cout << "\nBEFORE delete\n" << std::endl;
		while (currParent != NULL)
		{
			currParent->GetTranslation(currParentTransx, currParentTransy, currParentTransz);
			currParent->GetTranslationOffset(currParentOffsetx, currParentOffsety, currParentOffsetz);

			std::cout << "currParent ->GetPrimTypeID(): " << currParent ->GetPrimTypeID() << ",  currParentTransz: " << currParentTransz << ",  currParentOffsetz: " << currParentOffsetz << std::endl;

			currParent = currParent->GetChild();
		}
		std::cout << "\n" << std::endl;
#endif

		// Start with the object 'above'/is the parent of the one being deleted
		if (parentOfSelectedObj != NULL)
		{
			prevParent = m_selectedObject;
			currParent = prevParent->GetParent();
		
			while (currParent != NULL)
			{
				currParent->GetTranslationOffset(currParentOffsetx, currParentOffsety, currParentOffsetz);

				currParent->SetTranslationOffset(currParentOffsetx, currParentOffsety, currParentOffsetz + 0.5f);

				prevParent = currParent;
				currParent = currParent->GetParent();
			}
		}		
			
#ifdef _DEBUG
		// Show translations and offsets of all primitives on the pole
		currParent = m_objectRoot;

		std::cout << "\nAFTER delete #1\n" << std::endl;
		while (currParent != NULL)
		{
			currParent->GetTranslation(currParentTransx, currParentTransy, currParentTransz);
			currParent->GetTranslationOffset(currParentOffsetx, currParentOffsety, currParentOffsetz);

			std::cout << "currParent ->GetPrimTypeID(): " << currParent ->GetPrimTypeID() << ",  currParentTransz: " << currParentTransz << ",  currParentOffsetz: " << currParentOffsetz << std::endl;

			currParent = currParent->GetChild();
		}
		std::cout << "\n" << std::endl;
#endif
		
		victim->SetPrevChild( child );
		victim->SetChild( NULL ); // Otherwise it will try to delete the next node
		m_selectedObject = NULL;
		if( child != NULL )
		{
			child->SetParent( parentOfSelectedObj );
			m_selectedObject = child;
		}
		if( parentOfSelectedObj != NULL )
		{
			parentOfSelectedObj->SetChild( child );
			m_selectedObject = parentOfSelectedObj;
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
	}

	if( m_objectRoot != NULL )
	{
		m_objectRoot->RecalcOffsets();
	}
	
	RebuildPole();
		
	return victim;	
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
		//delete lastOp;
		m_operations.pop_front();
	}
}



//static bool _showBase = true;
static bool showBase = true;

void SetShowBase(bool _value) { showBase = _value; };

bool GetShowBase() { return showBase; };


//static bool _showPole = true;
static bool showPole = true;

void SetShowPole(bool _value) { showPole = _value; };

bool GetShowPole() { return showPole; };


//----------------------------------------------------------------------------------


void Totem::Controller::ShowHidePoleAndBase()
{
	// This runs when the pole and base are first built and then each time a primitive is added
	bool showPole = GetShowPole();// = true;
	bool showPoleBase = GetShowBase();// = true;


	// Toggle order:
	//	1. Default: Show Pole and base
	//	2. Hide pole
	//	3. Hide base (and pole)
	//	4. Hide both

	if (showPole && showPoleBase)
		// Both are showing, so toggle to pole hidden
		SetShowPole(false);
	else if (showPoleBase && !showPole)
		// Base is showing but pole isn't so toggle to base hiden too
		SetShowBase(!showPoleBase);
	else if (!showPole && !showPoleBase)
		{
			// Base and pole both hidden, so now toggle to showing both
			SetShowBase(true);
			SetShowPole(true);
		}
}


void Totem::Controller::RebuildPole()
{
	
	// This runs when the pole and base are first built and then each time a primitive is added

/*
	bool showPole = GetShowPole();// = true;
	bool showPoleBase = GetShowBase();// = true;


	// Toggle order:
	//	1. Default: Show Pole and base
	//	2. Hide pole
	//	3. Hide base (and pole)
	//	4. Hide both

	if (showPole && showPoleBase)
		// Both are showing, so toggle to pole hidden
		SetShowPole(false);
	else if (showPoleBase && !showPole)
		// Base is showing but pole isn't so toggle to base hiden too
		SetShowBase(!showPoleBase);
	else if (!showPole && !showPoleBase)
		{
			// Base and pole both hidden, so now toggle to showing both
			SetShowBase(true);
			SetShowPole(true);
		}
		*/

/*
	if( m_poleBaseNode == NULL)
	{
		// Upright pole
		m_poleNode = new VolumeTree::CylinderNode( 1.0f, 0.05f, 0.05f );
		m_poleNode->SetPole( true );

		// Hide pole (make thin)
		bool hidePole = true;
//		GetShowPole();

		if (hidePole)
		  m_poleNode->SetRadius( 0.01f, 0.01f );///baseDiameter);
		else
		  m_poleNode->SetRadius( 0.05f, 0.05f );///baseDiameter);

		m_poleTransformNode = new VolumeTree::TransformNode( m_poleNode );
		m_poleTransformNode->SetTranslate( 0.0f, 0.0f, 0.5f );


		// Base
		VolumeTree::CylinderNode* poleBase = new VolumeTree::CylinderNode( 0.1f, 0.5f, 0.5f );
		//poleBase->SetPole( true );
		poleBase->SetBasePole( true );

//		float baseDiameter = poleBase->GetRadiusY();

		VolumeTree::TransformNode *baseTransform = new VolumeTree::TransformNode( poleBase );
		baseTransform->SetTranslate( 0.0f, 0.0f, -0.05f );
		//VolumeTree::TransformNode baseTransform = VolumeTree::TransformNode( poleBase );
		//baseTransform.SetTranslate( 0.0f, 0.0f, -0.05f );
		m_poleBaseNode = new VolumeTree::CSGNode( m_poleTransformNode, baseTransform );
	}
	*/

	// Show/Hide pole (make fat/thin)
	m_poleNode = new VolumeTree::CylinderNode( 1.0f, 0.05f, 0.05f );
	bool showPole= GetShowPole();
	if (showPole)
	{
//		m_poleNode = new VolumeTree::CylinderNode( 1.0f, 0.05f, 0.05f );
		m_poleNode->SetRadius( 0.05f, 0.05f );
//		m_poleNode->SetPole( true );
	}
	else
	{
//		m_poleNode = new VolumeTree::CylinderNode( 1.0f, 0.001f, 0.001f );
		m_poleNode->SetRadius( 0.001f, 0.001f );

//		m_poleNode->SetPole( true );
//		m_poleNode->SetRadius( 0.01, 0.01 );///base Diameter);
	}
	m_poleNode->SetPole( true );
	m_poleTransformNode = new VolumeTree::TransformNode( m_poleNode );


	//	m_poleTransformNode->SetTranslate( 0.0f, 0.0f, 0.5f );
	
	// Show/Hide pole (make fat/thin)
	bool showPoleBase = GetShowBase();
	VolumeTree::CylinderNode* poleBase = new VolumeTree::CylinderNode( 0.1f, 0.5f, 0.5f );
	if (showPoleBase)
	{
		poleBase->SetRadius( 0.5f, 0.5f );
	}
	else
	{
		poleBase->SetRadius( 0.001f, 0.001f );
	}

	poleBase->SetBasePole( true );

//		float baseDiameter = poleBase->GetRadiusY();

	VolumeTree::TransformNode *baseTransform = new VolumeTree::TransformNode( poleBase );
	baseTransform->SetTranslate( 0.0f, 0.0f, -0.05f );
		//VolumeTree::TransformNode baseTransform = VolumeTree::TransformNode( poleBase );
		//baseTransform.SetTranslate( 0.0f, 0.0f, -0.05f );
	m_poleBaseNode = new VolumeTree::CSGNode( m_poleTransformNode, baseTransform );



	if( m_objectRoot != NULL )
	{
		m_objectRoot->RecalcOffsets();
		float x, y, z;
		VolumeTree::Node *treeRoot = m_objectRoot->GetNodeTree( m_blendAmount );
		treeRoot->GetBoundSizes(&x, &y, &z);
		float topZ = z;
		//float topZ = m_objectRoot->GetBaseOffset(); //Oleg: this was fixed to prevent pole growing even if the model is relatively small in height 
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