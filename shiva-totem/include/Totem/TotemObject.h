///-----------------------------------------------------------------------------------------------
/// \file TotemObject.h
/// \brief This is a wrapper for the main objects in the totempole. It is also a doubly-linked list 
/// \author Leigh McLoughlin, Michelle Wu
/// \date Jun 7, 2013
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef TOTEMOBJECT_H_
#define TOTEMOBJECT_H_

#include <algorithm>

#include "VolumeTree/VolumeTree.h"
#include "VolumeTree/Nodes/TransformNode.h"
#include "VolumeTree/Nodes/CSG.h"
#include "VolumeTree/Nodes/BlendCSG.h"

namespace Totem
{
	class Object
	{
	public:

		//----------------------------------------------------------------------------------
		/// \brief Ctor
		/// \param [in] mainNodeIn Primitive node
		//----------------------------------------------------------------------------------
		Object( VolumeTree::Node* );
		//----------------------------------------------------------------------------------
		/// \brief Ctor used when loading a model
		/// \param [in] mainNodeIn This corresponds to the primitive node
		/// \param [in] mainTransformIn This is the transform node parent to primitive node
		//----------------------------------------------------------------------------------
		Object( VolumeTree::Node*, VolumeTree::TransformNode* );
		//----------------------------------------------------------------------------------
		// Does not delete its main node
		//----------------------------------------------------------------------------------
		/// \brief Dtor 
		//----------------------------------------------------------------------------------
		~Object();
		//----------------------------------------------------------------------------------
		/// \brief Set child and recalculate offsets
		/// \param [in] value Child node
		//----------------------------------------------------------------------------------
		void SetChild( Object *value ) { _child = value; RecalcOffsets(); }
		//----------------------------------------------------------------------------------
		/// \brief Set parent and recalculate offsets
		/// \param [in] value Parent node
		//----------------------------------------------------------------------------------
		void SetParent( Object *value ) { _parent = value; RecalcOffsets(); }
		//----------------------------------------------------------------------------------
		/// \brief Retrieve child of Totem::Object
		/// \return _child
		//----------------------------------------------------------------------------------
		Object* GetChild() const { return _child; }
		//----------------------------------------------------------------------------------
		/// \brief Retrieve parent of Totem::Object
		/// \return _parent
		//----------------------------------------------------------------------------------
		Object* GetParent() const { return _parent; }
		//----------------------------------------------------------------------------------
		/// \brief If shifting up this object switches places with its parent
		/// If shifting down, this object switches places with its child
		/// \param [in] up
		/// \param [in] swapOffsets
		//----------------------------------------------------------------------------------
		void ShiftOrder( bool up, bool swapOffsets = true );
		//----------------------------------------------------------------------------------
		/// \brief Useful if you've been shifting objects around
		//----------------------------------------------------------------------------------
		Object* GetRoot();
		//----------------------------------------------------------------------------------
		/// \brief Return node tree
		/// \param [in] blendAmount Amount of blending to be applied globally to model
		//----------------------------------------------------------------------------------
		VolumeTree::Node* GetNodeTree( float blendAmount );
		//----------------------------------------------------------------------------------
		/// \brief Draw bounding box?
		/// \param [in] value
		//----------------------------------------------------------------------------------
		void SetDrawBBox( bool );
		//----------------------------------------------------------------------------------
		// The translation-offsets are translations relative to the object's main position in the totem stack
		//----------------------------------------------------------------------------------
		/// \brief Add offset
		/// \param [in] x
		/// \param [in] y
		/// \param [in] z
		/// \param [in] checkOrder
		//----------------------------------------------------------------------------------
		void AddTranslationOffset( float x, float y, float z, bool checkOrder = true );
		//----------------------------------------------------------------------------------
		/// \brief Set offset
		/// \param [in] x
		/// \param [in] y
		/// \param [in] z
		/// \param [in] checkOrder
		//----------------------------------------------------------------------------------
		void SetTranslationOffset( float x, float y, float z, bool checkOrder = false );
		//----------------------------------------------------------------------------------
		/// \brief Retrieve offset amount
		/// \param [out] x
		/// \param [out] y
		/// \param [out] z
		//----------------------------------------------------------------------------------
		void GetTranslationOffset( float &x, float &y, float &z ) { x = _offsetX; y = _offsetY; z = _offsetZ; }
		//----------------------------------------------------------------------------------
		// Rotations are in radians
		//----------------------------------------------------------------------------------
		/// \brief Set rotation (radians)
		/// \param [in] x
		/// \param [in] y
		/// \param [in] z
		//----------------------------------------------------------------------------------
		void SetRotation( float x, float y, float z ) { _rx = x; _ry = y; _rz = z; UpdateTransform(); }
		//----------------------------------------------------------------------------------
		/// \brief Add rotation (radians)
		/// \param [in] x
		/// \param [in] y
		/// \param [in] z
		//----------------------------------------------------------------------------------
		void AddRotation( float x, float y, float z ) { _rx += x; _ry += y; _rz += z; UpdateTransform(); }
		//----------------------------------------------------------------------------------
		/// \brief Set scaling factor
		/// \param [in] x
		/// \param [in] y
		/// \param [in] z
		//----------------------------------------------------------------------------------
		void SetScale( float x, float y, float z ) { _sx = x; _sy = y; _sz = z; UpdateTransform(); }
		//----------------------------------------------------------------------------------
		/// \brief Add scaling factor
		/// \param [in] x
		/// \param [in] y
		/// \param [in] z
		//----------------------------------------------------------------------------------
		void AddScale( float x, float y, float z ) { _sx += x; _sy += y; _sz += z; UpdateTransform(); }
		//----------------------------------------------------------------------------------
		/// \brief Get scaling factor along x
		/// \return _sx
		//----------------------------------------------------------------------------------
		float GetScaleX() { return _sx; }
		//----------------------------------------------------------------------------------
		/// \brief Get scaling factor along y
		/// \return _sy
		//----------------------------------------------------------------------------------
		float GetScaleY() { return _sy; }
		//----------------------------------------------------------------------------------
		/// \brief Get scaling factor along z
		/// \return _sz
		//----------------------------------------------------------------------------------
		float GetScaleZ() { return _sz; }
		//----------------------------------------------------------------------------------
		/// \brief Retrieve translations
		/// \param [out] x Amount of translation alongside x-axis
		/// \param [out] y Amount of translation alongside y-axis
		/// \param [out] z Amount of translation alongside z-axis
		//----------------------------------------------------------------------------------
		void GetTranslation( float &x, float &y, float &z ) { x = _tx; y = _ty; z = _tz; }
		//----------------------------------------------------------------------------------
		/// \brief Return the height of the object bounding box
		//----------------------------------------------------------------------------------
		float GetBBoxZ();
		//----------------------------------------------------------------------------------
		/// \brief This should return the Z value that is the top of the bounding box of the object
		//----------------------------------------------------------------------------------
		float GetBaseOffset();
		//----------------------------------------------------------------------------------
		/// \brief Recalculate offsets
		//----------------------------------------------------------------------------------
		void RecalcOffsets();
		//----------------------------------------------------------------------------------
		/// \brief This is the basic position for the object. Its actual position is this (in Z, with 0 in X,Y) plus the XYZ offset value
		//----------------------------------------------------------------------------------
		float GetStackedPosition();
		//----------------------------------------------------------------------------------
		/// \brief Performs a self-intersection with the line and its bounding box and its children
		/// If a child object has a closer hit, it sets this as the selection param
		/// Return value is a distance from the origin to the bounding box of the object
		/// If there is no intersection, selection is NULL and return value has no meaning
		/// \param [in] selection
		/// \param [in] originX
		/// \param [in] originY
		/// \param [in] originZ
		/// \param [in] dirX
		/// \param [in] dirY
		/// \param [in] dirZ
		//----------------------------------------------------------------------------------
		float SelectIntersectingObject( Totem::Object **selection, float originX, float originY, float originZ, float dirX, float dirY, float dirZ );
		//----------------------------------------------------------------------------------

	protected:

		//----------------------------------------------------------------------------------
		/// \brief Child object
		//----------------------------------------------------------------------------------
		Totem::Object *_child;
		//----------------------------------------------------------------------------------
		/// \brief Parent object
		//----------------------------------------------------------------------------------
		Totem::Object *_parent;
		//----------------------------------------------------------------------------------
		/// \brief Node that actually contains the object volume
		VolumeTree::Node *_mainNode;
		//----------------------------------------------------------------------------------
		/// \brief Transformation node
		//----------------------------------------------------------------------------------
		VolumeTree::TransformNode *_mainTransform;
		//----------------------------------------------------------------------------------
		/// \brief Translation offset along x-axis
		//----------------------------------------------------------------------------------
		float _offsetX;
		//----------------------------------------------------------------------------------
		/// \brief Translation offset along y-axis
		//----------------------------------------------------------------------------------
		float _offsetY;
		//----------------------------------------------------------------------------------
		/// \brief Translation offset along z-axis
		//----------------------------------------------------------------------------------
		float _offsetZ;
		//----------------------------------------------------------------------------------
		/// \brief Translation along x
		//----------------------------------------------------------------------------------
		float _tx;
		//----------------------------------------------------------------------------------
		/// \brief Translation along y
		//----------------------------------------------------------------------------------
		float _ty;
		//----------------------------------------------------------------------------------
		/// \brief Translation along z
		//----------------------------------------------------------------------------------
		float _tz;
		//----------------------------------------------------------------------------------
		/// \brief Rotation along x
		//----------------------------------------------------------------------------------
		float _rx;
		//----------------------------------------------------------------------------------
		/// \brief Rotation along y
		//----------------------------------------------------------------------------------
		float _ry;
		//----------------------------------------------------------------------------------
		/// \brief Rotation along z
		//----------------------------------------------------------------------------------
		float _rz;
		//----------------------------------------------------------------------------------
		/// \brief Scaling along x
		//----------------------------------------------------------------------------------
		float _sx;
		//----------------------------------------------------------------------------------
		/// \brief Scaling along y
		//----------------------------------------------------------------------------------
		float _sy;
		//----------------------------------------------------------------------------------
		/// \brief Scaling along z
		//----------------------------------------------------------------------------------
		float _sz;
		//----------------------------------------------------------------------------------
		/// \brief Update main transformation matrix
		//----------------------------------------------------------------------------------
		void UpdateTransform();
		//----------------------------------------------------------------------------------
	};
}

#endif
