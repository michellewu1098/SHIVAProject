#include "Totem/TotemObject.h"

//----------------------------------------------------------------------------------

Totem::Object::Object( VolumeTree::Node *_mainNodeIn, unsigned int _nGUIControllers )
{
	m_mainNode = _mainNodeIn;
	//m_mainNode->HasBoundingBox( true );


	// Apparently only mainTransform needs to be set with a bounding box
	// Using params currently only works if we have one renderer, which we don't (each Activity has a different renderer)
	m_mainTransform = new VolumeTree::TransformNode( false );
	m_mainTransform->SetNumbOfContext( _nGUIControllers );
	m_mainTransform->HasBoundingBox( true );

	m_child = m_parent = NULL;
	m_prevChild = NULL;

	m_offsetX = m_offsetY = m_offsetZ = 0.0f;
	m_tx = m_ty = m_tz = 0.0f;
	m_rx = m_ry = m_rz = 0.0f;
	m_sx = m_sy = m_sz = 1.0f;
}

//----------------------------------------------------------------------------------

Totem::Object::Object( VolumeTree::Node *_mainNodeIn, VolumeTree::TransformNode *_mainTransformIn, unsigned int _nGUIControllers )
{
	m_mainNode = _mainNodeIn;
	//m_mainNode->HasBoundingBox( true );

	// Apparently only mainTransform needs to be set with a bounding box
	m_mainTransform = _mainTransformIn;
	m_mainTransform->SetNumbOfContext( _nGUIControllers );
	m_mainTransform->HasBoundingBox( true );

	m_child = m_parent = NULL;
	m_prevChild = NULL;

	m_tx = m_ty = m_tz = 0.0f;
	m_rx = m_ry = m_rz = 0.0f;
	m_sx = m_sy = m_sz = 1.0f;

	float junk;
	// Retrieve translation, rotation and scale info from imported transform node
	m_mainTransform->GetTransformParams( m_tx, m_ty, m_tz, 
										 m_rx, m_ry, m_rz, 
										 m_sx, m_sy, m_sz, 
										 junk, junk, junk, 
										 junk, junk, junk );

	m_offsetX = -m_tx;
	m_offsetY = -m_ty;
	m_offsetZ = 0.0f;

}

//----------------------------------------------------------------------------------

Totem::Object::~Object()
{
	delete m_child;
	delete m_mainTransform;
	delete m_prevChild;
}

//----------------------------------------------------------------------------------

bool Totem::Object::IsValid()
{
	if( m_mainNode != NULL && m_mainTransform != NULL )
	{
		return true;
	}
	else
	{
		return false; 
	}
}

//----------------------------------------------------------------------------------

void Totem::Object::ShiftOrder( bool _up, bool _swapOffsets )
{
	float stackedPosition = GetStackedPosition();
	if( _up )
	{
		if( m_parent != NULL )
		{
			// Offset of object we are swapping with, which will become our new offset if swapOffsets is true
			float ourNewOffsetX, ourNewOffsetY, ourNewOffsetZ;
			m_parent->GetTranslationOffset( ourNewOffsetX, ourNewOffsetY, ourNewOffsetZ );
			float theirPrevActualPos = 0.0f, junk;
			m_parent->GetTranslation( junk, junk, theirPrevActualPos );
			float theirPrevStackedPos = m_parent->GetStackedPosition();

			Totem::Object *grandparent = m_parent->GetParent();
			// Remove ourselves from list
			m_parent->SetChild( m_child );
			if( m_child != NULL )
			{
				m_child->SetParent( m_parent );
			}

			// Parent becomes child
			m_child = m_parent;
			m_child->SetParent( this );

			// If old parent had a parent, that becomes our new parent
			m_parent = grandparent;
			if( m_parent != NULL )
			{
				m_parent->SetChild( this );
			}

			if( _swapOffsets )
			{
				m_child->SetTranslationOffset( m_offsetX, m_offsetY, m_offsetZ );
				m_offsetX = ourNewOffsetX;
				m_offsetY = ourNewOffsetY;
				m_offsetZ = ourNewOffsetZ;
			}
			else
			{
				float theirNewStackedPos = m_child->GetStackedPosition();
				// The stacked position of the parent will change, we want to use the difference as the new offset
				// Difference between where parent was to where it is now
				float theirNewOffsetZ = theirPrevActualPos - theirNewStackedPos;
				m_child->AddTranslationOffset( 0.0f, 0.0f, theirNewOffsetZ - ourNewOffsetZ, false );

				//std::cout<<"INFO: Order Change: offsetZ = "<<offsetZ<<" _offsetZ = "<<_offsetZ<<std::endl;

				// For our new offset, need to subtract difference in stacked positions of parent
				float ourOffsetChange = theirPrevStackedPos - theirNewStackedPos;
				m_offsetZ -= ourOffsetChange;
			}
		}
	}
	else
	{
		if( m_child != NULL )
		{
			
			// Offset of object we are swapping with, which will become our new offset if swapOffsets is true
			float ourNewOffsetX, ourNewOffsetY, ourNewOffsetZ;
			m_child->GetTranslationOffset( ourNewOffsetX, ourNewOffsetY, ourNewOffsetZ );
			float theirPrevActualPos = 0.0f, junk;
			m_child->GetTranslation( junk, junk, theirPrevActualPos );
			float theirPrevStackedPos = m_child->GetStackedPosition();

			Totem::Object *grandchild = m_child->GetChild();
			// Remove ourselves from list
			m_child->SetParent( m_parent );
			Totem::Object *prevParent = m_parent;
			if( m_parent != NULL )
			{
				m_parent->SetChild( m_child );
			}

			// Child becomes parent
			m_parent = m_child;
			m_child = grandchild; // Put this here because setting children/parents causes rebuild of offsets which requires a valid child
			m_parent->SetChild( this );

			// If old child had a child, that becomes our new child
			if( m_child != NULL )
			{
				m_child->SetParent( this );
			}
			
			if( _swapOffsets )
			{
				m_parent->SetTranslationOffset( m_offsetX, m_offsetY, m_offsetZ );
				m_offsetX = ourNewOffsetX;
				m_offsetY = ourNewOffsetY;
				m_offsetZ = ourNewOffsetZ;
			}
			else
			{
				float theirNewStackedPos = m_parent->GetStackedPosition();
				// The stacked position of the parent will change, we want to use the difference as the new offset
				// Difference between where parent was to where it is now
				float theirNewOffsetZ = theirPrevActualPos - theirNewStackedPos;
				m_parent->AddTranslationOffset( 0.0f, 0.0f, theirNewOffsetZ - ourNewOffsetZ, false );
				
				// If old parent, need to update its position?!
				if( prevParent != NULL )
				{
					//prevParent->AddTranslationOffset(0.0f,0.0f,theirNewOffsetZ,false);
					prevParent->RecalcOffsets();
				}

				//std::cout<<"INFO: Order Change: offsetZ = "<<offsetZ<<" _offsetZ = "<<_offsetZ<<std::endl;

				// For our new offset, need to subtract difference in stacked positions of parent
				float ourOffsetChange = theirNewStackedPos - theirPrevStackedPos;
				m_offsetZ += ourOffsetChange;
			}
		}
	}

	RecalcOffsets();
}

//----------------------------------------------------------------------------------

Totem::Object* Totem::Object::GetRoot()
{
	if( m_parent != NULL )
	{
		return m_parent->GetRoot();
	}
	return this;
}

//----------------------------------------------------------------------------------

VolumeTree::Node* Totem::Object::GetNodeTree( float _blendAmount )
{
	m_mainTransform->SetChild( m_mainNode );

	VolumeTree::Node *rootNode = m_mainTransform;

	if( m_child != NULL )
	{
		// TODO: fix this memory leak
		VolumeTree::BlendCSGNode *blendNode = new VolumeTree::BlendCSGNode( rootNode, m_child->GetNodeTree( _blendAmount ) );
		//VolumeTree::BlendCSGNode blendNode = VolumeTree::BlendCSGNode( rootNode, m_child->GetNodeTree( _blendAmount ) );
		blendNode->SetBlendParams( _blendAmount, 1.0f, 1.0f );
		//blendNode.SetBlendParams( _blendAmount, 1.0f, 1.0f );
		//rootNode = &blendNode;
		rootNode = ( VolumeTree::Node* )blendNode;
	}

	return rootNode;
}

//----------------------------------------------------------------------------------

void Totem::Object::SetDrawBBox( bool _value )
{
	m_mainTransform->SetDrawBBox( _value );
}

//----------------------------------------------------------------------------------

void Totem::Object::SetTranslationOffset( float _x, float _y, float _z, bool _checkOrder )
{
	m_offsetX = _x;
	m_offsetY = _y;
	m_offsetZ = _z;
	AddTranslationOffset( 0.0f, 0.0f, 0.0f, _checkOrder );
}

//----------------------------------------------------------------------------------

void Totem::Object::AddTranslationOffset( float _x, float _y, float _z, bool _checkOrder )
{
	m_offsetX += _x;
	m_offsetY += _y;
	m_offsetZ += _z;

	std::cout << "OffsetX: " << m_offsetX << " OffsetY: " << m_offsetY << " Offset Z: " << m_offsetZ << std::endl;
	
	RecalcOffsets();
	/* // RecalcOffsets() call replaces this part:
	float baseZ = 0.0f;
	if( _child != NULL )
	{
		baseZ += _child->GetBaseOffset();
	}
	
	if( _mainNode != NULL )
	{
		float x,y,z;
		_mainNode->GetBoundSizes(&x,&y,&z);
		baseZ += z*0.5f;
	}

	//_mainTransform->SetTranslate(_offsetX,_offsetY,baseZ+_offsetZ);
	_tx = _offsetX;
	_ty = _offsetY;
	_tz = baseZ + _offsetZ;
	UpdateTransform();
	*/

	// Now see if the translation has changed the effective order of objects on the pole
	// To do this we check to see if its centre has moved past the centre of its child or parent
	float centreZ = m_tz;
	
	if( _checkOrder )
	{
		if( m_child != NULL )
		{
			float tempX, tempY, tempZ;
			m_child->GetTranslation( tempX, tempY, tempZ );
			float childZ = tempZ;
			if( centreZ < childZ )
			{
				std::cout << "INFO: Totem translation offset has changed order, moving item down - ourZ = " << centreZ << " childZ = " << childZ << std::endl;

				ShiftOrder( false, false );
			}
		}
		if( m_parent != NULL )
		{
			float tempX, tempY, tempZ;
			m_parent->GetTranslation( tempX, tempY, tempZ );
			float parentZ = tempZ;
			if( centreZ > parentZ )
			{
				std::cout << "INFO: Totem translation offset has changed order, moving item up - ourZ = " << centreZ << " parentZ = " << parentZ << std::endl;

				ShiftOrder( true, false );

				std::cout << "INFO: totem translate, new tz: " << m_tz << std::endl;
			}
		}
	}
}

//----------------------------------------------------------------------------------

float Totem::Object::GetBBoxZ()
{
	if( m_mainNode != NULL )
	{
		float x, y, z;
		m_mainNode->GetBoundSizes( &x, &y, &z );
		return z;
	}
	return 0.0f;
}

//----------------------------------------------------------------------------------

float Totem::Object::GetBaseOffset()
{
	float total = GetBBoxZ();
	if( m_child != NULL )
	{
		total += m_child->GetBaseOffset();
	}
	return total;
}

//----------------------------------------------------------------------------------

void Totem::Object::RecalcOffsets()
{
	m_tx = m_offsetX;
	m_ty = m_offsetY;
	m_tz = GetStackedPosition() + m_offsetZ;
	UpdateTransform();

	if( m_child != NULL )
	{
		m_child->RecalcOffsets();
	}
}

//----------------------------------------------------------------------------------

float Totem::Object::GetStackedPosition()
{
	float z = 0.0f;
	if( m_child != NULL )
	{
		z += m_child->GetBaseOffset();
	}
	
	if( m_mainNode != NULL )
	{
		float bx, by, bz;
		m_mainNode->GetBoundSizes( &bx, &by, &bz );
		z += bz * 0.5f;
	}
	return z;
}

//----------------------------------------------------------------------------------

float Totem::Object::SelectIntersectingObject( Totem::Object **_selection, float _originX, float _originY, float _originZ, float _dirX, float _dirY, float _dirZ )
{
	if( m_mainNode == NULL || m_mainTransform == NULL )
	{
		*_selection = NULL;
		return -1.0f;
	}

	// Do our collision test
	float ourDist = 0.0f;
	bool ourHit = false;

	// Intersect ray with a box

	//_mainTransform->SetChild(_mainNode);
	float boxminX, boxminY, boxminZ, boxmaxX, boxmaxY, boxmaxZ;
	m_mainTransform->GetBounds( &boxminX, &boxmaxX, &boxminY, &boxmaxY, &boxminZ, &boxmaxZ );

	
	#ifdef _DEBUG
		std::cout << "INFO: Attempting selection, object bounds: minX " << boxminX << " maxX " << boxmaxX << std::endl;
		std::cout << "INFO:                                      minY " << boxminY << " maxY " << boxmaxY << std::endl;
		std::cout << "INFO:                                      minZ " << boxminZ << " maxZ " << boxmaxZ << std::endl;
	#endif

	// http://www.siggraph.org/education/materials/HyperGraph/raytrace/rtinter3.htm
	// bool intersectBox(Ray r, float3 boxmin, float3 boxmax, float *tnear, float *tfar)
	{
		// compute intersection of ray with all six bbox planes
		float invRX = 1.0f / _dirX; float invRY = 1.0f / _dirY; float invRZ = 1.0f / _dirZ;
		float tbotX = invRX * ( boxminX - _originX ); float tbotY = invRY * ( boxminY - _originY ); float tbotZ = invRZ * ( boxminZ - _originZ );
		float ttopX = invRX * ( boxmaxX - _originX ); float ttopY = invRY * ( boxmaxY - _originY ); float ttopZ = invRZ * ( boxmaxZ - _originZ );

		// re-order intersections to find smallest and largest on each axis
		float tminX = ( std::min )( ttopX, tbotX ); float tminY = ( std::min )( ttopY, tbotY ); float tminZ = ( std::min )( ttopZ, tbotZ );
		float tmaxX = ( std::max )( ttopX, tbotX ); float tmaxY = ( std::max )( ttopY, tbotY ); float tmaxZ = ( std::max )( ttopZ, tbotZ );

		// find the largest tmin and the smallest tmax
		float largest_tmin = ( std::max )( ( std::max )( tminX, tminY ), ( std::max )( tminX, tminZ ) );
		float smallest_tmax = ( std::min )( ( std::min )( tmaxX, tmaxY ), ( std::min )( tmaxX, tmaxZ ) );

		ourDist = largest_tmin;
		//*tfar = smallest_tmax;

		ourHit = smallest_tmax > largest_tmin;
	}
	
	#ifdef _DEBUG
		std::cout << "INFO: Attempting selection, object hit result " << ourHit << " distance " << ourDist << std::endl;
	#endif


	// Send instruction to children
	if( m_child != NULL )
	{
		Totem::Object *childSelection = NULL;
		float childDist = m_child->SelectIntersectingObject( &childSelection, _originX, _originY, _originZ, _dirX, _dirY, _dirZ );

		// childSelection will be non-NULL only if an intersection was found
		if( ( childSelection != NULL ) && ( !ourHit || ( childDist < ourDist ) ) )
		{
			*_selection = childSelection;
			return childDist;
		}
	}

	// If the logic gets to here, either there was no child hit or we are closer than our children, or there is no hit at all
	if( ourHit )
	{
		*_selection = this;
		return ourDist;
	}
	else
	{
		*_selection = NULL;
		return -1.0f;
	}
}

//----------------------------------------------------------------------------------

void  Totem::Object::UpdateTransform()
{
	m_mainTransform->SetTransformParams( m_tx, m_ty, m_tz,
										 m_rx, m_ry, m_rz,
										 m_sx, m_sy, m_sz,
										 0.0f, 0.0f, 0.0f,
										 0.0f, 0.0f, 0.0f );
}

//----------------------------------------------------------------------------------