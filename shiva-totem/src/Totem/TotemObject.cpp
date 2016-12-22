#include "Totem/TotemObject.h"

//----------------------------------------------------------------------------------

Totem::Object::Object( VolumeTree::Node *mainNodeIn )
{
	_mainNode = mainNodeIn;
	// Using params currently only works if we have one renderer, which we don't (each Activity has a different renderer)
	_mainTransform = new VolumeTree::TransformNode( false );
	_child = _parent = NULL;

	_offsetX = _offsetY = _offsetZ = 0.0f;
	_tx = _ty = _tz = 0.0f;
	_rx = _ry = _rz = 0.0f;
	_sx = _sy = _sz = 1.0f;
}

//----------------------------------------------------------------------------------

Totem::Object::Object( VolumeTree::Node *mainNodeIn, VolumeTree::TransformNode *mainTransformIn )
{
	_mainNode = mainNodeIn;
	_mainTransform = mainTransformIn;

	_child = _parent = NULL;

	_mainTransform->GetTranslate( _offsetX, _offsetY, _offsetZ );

	_tx = _ty = _tz = 0.0f;
	_rx = _ry = _rz = 0.0f;
	_sx = _sy = _sz = 1.0f;

}

//----------------------------------------------------------------------------------

Totem::Object::~Object()
{
	delete _child;
	delete _mainTransform;
}

//----------------------------------------------------------------------------------

void Totem::Object::ShiftOrder( bool up, bool swapOffsets )
{
	float stackedPosition = GetStackedPosition();
	if( up )
	{
		if( _parent != NULL )
		{
			// Offset of object we are swapping with, which will become our new offset if swapOffsets is true
			float ourNewOffsetX, ourNewOffsetY, ourNewOffsetZ;
			_parent->GetTranslationOffset( ourNewOffsetX, ourNewOffsetY, ourNewOffsetZ );
			float theirPrevActualPos = 0.0f, junk;
			_parent->GetTranslation( junk, junk, theirPrevActualPos );
			float theirPrevStackedPos = _parent->GetStackedPosition();

			Totem::Object *grandparent = _parent->GetParent();
			// Remove ourselves from list
			_parent->SetChild( _child );
			if( _child != NULL )
			{
				_child->SetParent( _parent );
			}

			// Parent becomes child
			_child = _parent;
			_child->SetParent( this );

			// If old parent had a parent, that becomes our new parent
			_parent = grandparent;
			if( _parent != NULL )
			{
				_parent->SetChild( this );
			}

			if( swapOffsets )
			{
				_child->SetTranslationOffset( _offsetX, _offsetY, _offsetZ );
				_offsetX = ourNewOffsetX;
				_offsetY = ourNewOffsetY;
				_offsetZ = ourNewOffsetZ;
			}
			else
			{
				float theirNewStackedPos = _child->GetStackedPosition();
				// The stacked position of the parent will change, we want to use the difference as the new offset
				// Difference between where parent was to where it is now
				float theirNewOffsetZ = theirPrevActualPos - theirNewStackedPos;
				_child->AddTranslationOffset( 0.0f, 0.0f, theirNewOffsetZ - ourNewOffsetZ, false );

				//std::cout<<"INFO: Order Change: offsetZ = "<<offsetZ<<" _offsetZ = "<<_offsetZ<<std::endl;

				// For our new offset, need to subtract difference in stacked positions of parent
				float ourOffsetChange = theirPrevStackedPos - theirNewStackedPos;
				_offsetZ -= ourOffsetChange;
			}
		}
	}
	else
	{
		if( _child != NULL )
		{
			
			// Offset of object we are swapping with, which will become our new offset if swapOffsets is true
			float ourNewOffsetX, ourNewOffsetY, ourNewOffsetZ;
			_child->GetTranslationOffset( ourNewOffsetX, ourNewOffsetY, ourNewOffsetZ );
			float theirPrevActualPos = 0.0f, junk;
			_child->GetTranslation( junk, junk, theirPrevActualPos );
			float theirPrevStackedPos = _child->GetStackedPosition();

			Totem::Object *grandchild = _child->GetChild();
			// Remove ourselves from list
			_child->SetParent( _parent );
			Totem::Object *prevParent = _parent;
			if( _parent != NULL )
			{
				_parent->SetChild( _child );
			}

			// Child becomes parent
			_parent = _child;
			_child = grandchild; // Put this here because setting children/parents causes rebuild of offsets which requires a valid child
			_parent->SetChild( this );

			// If old child had a child, that becomes our new child
			if( _child != NULL )
			{
				_child->SetParent( this );
			}
			
			if( swapOffsets )
			{
				_parent->SetTranslationOffset( _offsetX, _offsetY, _offsetZ );
				_offsetX = ourNewOffsetX;
				_offsetY = ourNewOffsetY;
				_offsetZ = ourNewOffsetZ;
			}
			else
			{
				float theirNewStackedPos = _parent->GetStackedPosition();
				// The stacked position of the parent will change, we want to use the difference as the new offset
				// Difference between where parent was to where it is now
				float theirNewOffsetZ = theirPrevActualPos - theirNewStackedPos;
				_parent->AddTranslationOffset( 0.0f, 0.0f, theirNewOffsetZ - ourNewOffsetZ, false );
				
				// If old parent, need to update its position?!
				if( prevParent != NULL )
				{
					//prevParent->AddTranslationOffset(0.0f,0.0f,theirNewOffsetZ,false);
					prevParent->RecalcOffsets();
				}

				//std::cout<<"INFO: Order Change: offsetZ = "<<offsetZ<<" _offsetZ = "<<_offsetZ<<std::endl;

				// For our new offset, need to subtract difference in stacked positions of parent
				float ourOffsetChange = theirNewStackedPos - theirPrevStackedPos;
				_offsetZ += ourOffsetChange;
			}
		}
	}

	RecalcOffsets();
}

//----------------------------------------------------------------------------------

Totem::Object* Totem::Object::GetRoot()
{
	if( _parent != NULL )
	{
		return _parent->GetRoot();
	}
	return this;
}

//----------------------------------------------------------------------------------

VolumeTree::Node* Totem::Object::GetNodeTree( float blendAmount )
{
	_mainTransform->SetChild( _mainNode );

	VolumeTree::Node *rootNode = _mainTransform;

	if( _child != NULL )
	{
		// TODO: fix this memory leak
		VolumeTree::BlendCSGNode *blendNode = new VolumeTree::BlendCSGNode( rootNode, _child->GetNodeTree( blendAmount ) );
		blendNode->SetBlendParams( blendAmount, 1.0f, 1.0f );
		rootNode = ( VolumeTree::Node* ) blendNode;
	}

	return rootNode;
}

//----------------------------------------------------------------------------------

void Totem::Object::SetDrawBBox( bool value )
{
	_mainTransform->SetDrawBBox( value );
}

//----------------------------------------------------------------------------------

void Totem::Object::SetTranslationOffset( float x, float y, float z, bool checkOrder )
{
	_offsetX = x;
	_offsetY = y;
	_offsetZ = z;
	AddTranslationOffset( 0.0f, 0.0f, 0.0f, checkOrder );
}

//----------------------------------------------------------------------------------

void Totem::Object::AddTranslationOffset( float x, float y, float z, bool checkOrder )
{
	_offsetX += x;
	_offsetY += y;
	_offsetZ += z;
	
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
	float centreZ = _tz;
	
	if( checkOrder )
	{
		if( _child != NULL )
		{
			float tempX, tempY, tempZ;
			_child->GetTranslation( tempX, tempY, tempZ );
			float childZ = tempZ;
			if( centreZ < childZ )
			{
				std::cout << "INFO: Totem translation offset has changed order, moving item down - ourZ = " << centreZ << " childZ = " << childZ << std::endl;

				ShiftOrder( false, false );
			}
		}
		if( _parent != NULL )
		{
			float tempX, tempY, tempZ;
			_parent->GetTranslation( tempX, tempY, tempZ );
			float parentZ = tempZ;
			if( centreZ > parentZ )
			{
				std::cout << "INFO: Totem translation offset has changed order, moving item up - ourZ = " << centreZ << " parentZ = " << parentZ << std::endl;

				ShiftOrder( true, false );

				std::cout << "INFO: totem translate, new tz: " << _tz << std::endl;
			}
		}
	}
}

//----------------------------------------------------------------------------------

float Totem::Object::GetBBoxZ()
{
	if( _mainNode != NULL )
	{
		float x,y,z;
		_mainNode->GetBoundSizes( &x, &y, &z );
		return z;
	}
	return 0.0f;
}

//----------------------------------------------------------------------------------

float Totem::Object::GetBaseOffset()
{
	float total = GetBBoxZ();
	if( _child != NULL )
	{
		total += _child->GetBaseOffset();
	}
	return total;
}

//----------------------------------------------------------------------------------

void Totem::Object::RecalcOffsets()
{
	_tx = _offsetX;
	_ty = _offsetY;
	_tz = GetStackedPosition() + _offsetZ;
	UpdateTransform();

	if( _child != NULL )
	{
		_child->RecalcOffsets();
	}
}

//----------------------------------------------------------------------------------

float Totem::Object::GetStackedPosition()
{
	float z = 0.0f;
	if( _child != NULL )
	{
		z += _child->GetBaseOffset();
	}
	
	if( _mainNode != NULL )
	{
		float bx, by, bz;
		_mainNode->GetBoundSizes( &bx, &by, &bz );
		z += bz * 0.5f;
	}
	return z;
}

//----------------------------------------------------------------------------------

float Totem::Object::SelectIntersectingObject( Totem::Object **selection, float originX, float originY, float originZ, float dirX, float dirY, float dirZ )
{
	if( _mainNode == NULL || _mainTransform == NULL )
	{
		*selection = NULL;
		return -1.0f;
	}

	// Do our collision test
	float ourDist = 0.0f;
	bool ourHit = false;

	// Intersect ray with a box

	//_mainTransform->SetChild(_mainNode);
	float boxminX, boxminY, boxminZ, boxmaxX, boxmaxY, boxmaxZ;
	_mainTransform->GetBounds( &boxminX, &boxmaxX, &boxminY, &boxmaxY, &boxminZ, &boxmaxZ );

	
	#ifdef _DEBUG
		std::cout << "INFO: Attempting selection, object bounds: minX " << boxminX << " maxX " << boxmaxX << std::endl;
		std::cout << "INFO:                                      minY " << boxminY << " maxY " << boxmaxY << std::endl;
		std::cout << "INFO:                                      minZ " << boxminZ << " maxZ " << boxmaxZ << std::endl;
	#endif

	// http://www.siggraph.org/education/materials/HyperGraph/raytrace/rtinter3.htm
	// bool intersectBox(Ray r, float3 boxmin, float3 boxmax, float *tnear, float *tfar)
	{
		// compute intersection of ray with all six bbox planes
		float invRX = 1.0f / dirX; float invRY = 1.0f / dirY; float invRZ = 1.0f / dirZ;
		float tbotX = invRX * ( boxminX - originX ); float tbotY = invRY * ( boxminY - originY ); float tbotZ = invRZ * ( boxminZ - originZ );
		float ttopX = invRX * ( boxmaxX - originX ); float ttopY = invRY * ( boxmaxY - originY ); float ttopZ = invRZ * ( boxmaxZ - originZ );

		// re-order intersections to find smallest and largest on each axis
		float tminX = std::min( ttopX, tbotX ); float tminY = std::min( ttopY, tbotY ); float tminZ = std::min( ttopZ, tbotZ );
		float tmaxX = std::max( ttopX, tbotX ); float tmaxY = std::max( ttopY, tbotY ); float tmaxZ = std::max( ttopZ, tbotZ );

		// find the largest tmin and the smallest tmax
		float largest_tmin = std::max( std::max( tminX, tminY ), std::max( tminX, tminZ ) );
		float smallest_tmax = std::min( std::min( tmaxX, tmaxY ), std::min( tmaxX, tmaxZ ) );

		ourDist = largest_tmin;
		//*tfar = smallest_tmax;

		ourHit = smallest_tmax > largest_tmin;
	}
	
	#ifdef _DEBUG
		std::cout << "INFO: Attempting selection, object hit result " << ourHit << " distance " << ourDist << std::endl;
	#endif


	// Send instruction to children
	if( _child != NULL )
	{
		Totem::Object *childSelection = NULL;
		float childDist = _child->SelectIntersectingObject( &childSelection, originX, originY, originZ, dirX, dirY, dirZ );

		// childSelection will be non-NULL only if an intersection was found
		if( ( childSelection != NULL ) && ( !ourHit || ( childDist < ourDist ) ) )
		{
			*selection = childSelection;
			return childDist;
		}
	}

	// If the logic gets to here, either there was no child hit or we are closer than our children, or there is no hit at all
	if( ourHit )
	{
		*selection = this;
		return ourDist;
	}
	else
	{
		*selection = NULL;
		return -1.0f;
	}
}

//----------------------------------------------------------------------------------

void  Totem::Object::UpdateTransform()
{

	_mainTransform->SetTransformParams(	_tx,   _ty,  _tz,
										_rx,   _ry,  _rz,
										_sx,   _sy,  _sz,
										0.0f, 0.0f, 0.0f,
										0.0f, 0.0f, 0.0f );
}

//----------------------------------------------------------------------------------