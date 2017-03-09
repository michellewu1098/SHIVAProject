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
		/// \param [in] _mainNodeIn This corresponds to the primitive node
		/// \param [in] _mainTransformIn This is the transform node parent to primitive node
		//----------------------------------------------------------------------------------
		Object( VolumeTree::Node* _mainNodeIn, VolumeTree::TransformNode* _mainTransformIn );
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
		void SetChild( Object *_value ) { m_child = _value; RecalcOffsets(); }
		//----------------------------------------------------------------------------------
		/// \brief Set parent and recalculate offsets
		/// \param [in] value Parent node
		//----------------------------------------------------------------------------------
		void SetParent( Object *_value ) { m_parent = _value; RecalcOffsets(); }
		//----------------------------------------------------------------------------------
		/// \brief Retrieve child of Totem::Object
		/// \return _child
		//----------------------------------------------------------------------------------
		Object* GetChild() const { return m_child; }
		//----------------------------------------------------------------------------------
		/// \brief Retrieve parent of Totem::Object
		/// \return _parent
		//----------------------------------------------------------------------------------
		Object* GetParent() const { return m_parent; }
		//----------------------------------------------------------------------------------
		/// \brief If shifting up this object switches places with its parent
		/// If shifting down, this object switches places with its child
		/// \param [in] _up
		/// \param [in] _swapOffsets
		//----------------------------------------------------------------------------------
		void ShiftOrder( bool _up, bool _swapOffsets = true );
		//----------------------------------------------------------------------------------
		/// \brief Useful if you've been shifting objects around
		//----------------------------------------------------------------------------------
		Object* GetRoot();
		//----------------------------------------------------------------------------------
		/// \brief Return node tree
		/// \param [in] blendAmount Amount of blending to be applied globally to model
		//----------------------------------------------------------------------------------
		VolumeTree::Node* GetNodeTree( float _blendAmount );
		//----------------------------------------------------------------------------------
		/// \brief Draw bounding box?
		/// \param [in] value
		//----------------------------------------------------------------------------------
		void SetDrawBBox( bool _value );
		//----------------------------------------------------------------------------------
		// The translation-offsets are translations relative to the object's main position in the totem stack
		//----------------------------------------------------------------------------------
		/// \brief Add offset
		/// \param [in] _x
		/// \param [in] _y
		/// \param [in] _z
		/// \param [in] _checkOrder
		//----------------------------------------------------------------------------------
		void AddTranslationOffset( float _x, float _y, float _z, bool _checkOrder = true );
		//----------------------------------------------------------------------------------
		/// \brief Set offset
		/// \param [in] _x
		/// \param [in] _y
		/// \param [in] _z
		/// \param [in] _checkOrder
		//----------------------------------------------------------------------------------
		void SetTranslationOffset( float _x, float _y, float _z, bool _checkOrder = false );
		//----------------------------------------------------------------------------------
		/// \brief Retrieve offset amount
		/// \param [out] _x
		/// \param [out] _y
		/// \param [out] _z
		//----------------------------------------------------------------------------------
		void GetTranslationOffset( float &_x, float &_y, float &_z ) { _x = m_offsetX; _y = m_offsetY; _z = m_offsetZ; }
		//----------------------------------------------------------------------------------
		// Rotations are in radians
		//----------------------------------------------------------------------------------
		/// \brief Set rotation (radians)
		/// \param [in] _x
		/// \param [in] _y
		/// \param [in] _z
		//----------------------------------------------------------------------------------
		void SetRotation( float _x, float _y, float _z ) { m_rx = _x; m_ry = _y; m_rz = _z; UpdateTransform(); }
		//----------------------------------------------------------------------------------
		/// \brief Add rotation (radians)
		/// \param [in] _x
		/// \param [in] _y
		/// \param [in] _z
		//----------------------------------------------------------------------------------
		void AddRotation( float _x, float _y, float _z ) { m_rx += _x; m_ry += _y; m_rz += _z; UpdateTransform(); }
		//----------------------------------------------------------------------------------
		/// \brief Set scaling factor
		/// \param [in] _x
		/// \param [in] _y
		/// \param [in] _z
		//----------------------------------------------------------------------------------
		void SetScale( float _x, float _y, float _z ) { m_sx = _x; m_sy = _y; m_sz = _z; UpdateTransform(); }
		//----------------------------------------------------------------------------------
		/// \brief Add scaling factor
		/// \param [in] _x
		/// \param [in] _y
		/// \param [in] _z
		//----------------------------------------------------------------------------------
		void AddScale( float _x, float _y, float _z ) { m_sx += _x; m_sy += _y; m_sz += _z; UpdateTransform(); }
		//----------------------------------------------------------------------------------
		/// \brief Get scaling factor along x
		/// \return m_sx
		//----------------------------------------------------------------------------------
		float GetScaleX() { return m_sx; }
		//----------------------------------------------------------------------------------
		/// \brief Get scaling factor along y
		/// \return m_sy
		//----------------------------------------------------------------------------------
		float GetScaleY() { return m_sy; }
		//----------------------------------------------------------------------------------
		/// \brief Get scaling factor along z
		/// \return m_sz
		//----------------------------------------------------------------------------------
		float GetScaleZ() { return m_sz; }
		//----------------------------------------------------------------------------------
		/// \brief Retrieve translations
		/// \param [out] _x Amount of translation alongside x-axis
		/// \param [out] _y Amount of translation alongside y-axis
		/// \param [out] _z Amount of translation alongside z-axis
		//----------------------------------------------------------------------------------
		void GetTranslation( float &_x, float &_y, float &_z ) { _x = m_tx; _y = m_ty; _z = m_tz; }
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
		/// \param [in] _selection
		/// \param [in] _originX
		/// \param [in] _originY
		/// \param [in] _originZ
		/// \param [in] _dirX
		/// \param [in] _dirY
		/// \param [in] _dirZ
		//----------------------------------------------------------------------------------
		float SelectIntersectingObject( Totem::Object **_selection, float _originX, float _originY, float _originZ, float _dirX, float _dirY, float _dirZ );
		//----------------------------------------------------------------------------------

	protected:

		//----------------------------------------------------------------------------------
		/// \brief Child object
		//----------------------------------------------------------------------------------
		Totem::Object *m_child;
		//----------------------------------------------------------------------------------
		/// \brief Parent object
		//----------------------------------------------------------------------------------
		Totem::Object *m_parent;
		//----------------------------------------------------------------------------------
		/// \brief Node that actually contains the object volume
		VolumeTree::Node *m_mainNode;
		//----------------------------------------------------------------------------------
		/// \brief Transformation node
		//----------------------------------------------------------------------------------
		VolumeTree::TransformNode *m_mainTransform;
		//----------------------------------------------------------------------------------
		/// \brief Translation offset along x-axis
		//----------------------------------------------------------------------------------
		float m_offsetX;
		//----------------------------------------------------------------------------------
		/// \brief Translation offset along y-axis
		//----------------------------------------------------------------------------------
		float m_offsetY;
		//----------------------------------------------------------------------------------
		/// \brief Translation offset along z-axis
		//----------------------------------------------------------------------------------
		float m_offsetZ;
		//----------------------------------------------------------------------------------
		/// \brief Translation along x
		//----------------------------------------------------------------------------------
		float m_tx;
		//----------------------------------------------------------------------------------
		/// \brief Translation along y
		//----------------------------------------------------------------------------------
		float m_ty;
		//----------------------------------------------------------------------------------
		/// \brief Translation along z
		//----------------------------------------------------------------------------------
		float m_tz;
		//----------------------------------------------------------------------------------
		/// \brief Rotation along x
		//----------------------------------------------------------------------------------
		float m_rx;
		//----------------------------------------------------------------------------------
		/// \brief Rotation along y
		//----------------------------------------------------------------------------------
		float m_ry;
		//----------------------------------------------------------------------------------
		/// \brief Rotation along z
		//----------------------------------------------------------------------------------
		float m_rz;
		//----------------------------------------------------------------------------------
		/// \brief Scaling along x
		//----------------------------------------------------------------------------------
		float m_sx;
		//----------------------------------------------------------------------------------
		/// \brief Scaling along y
		//----------------------------------------------------------------------------------
		float m_sy;
		//----------------------------------------------------------------------------------
		/// \brief Scaling along z
		//----------------------------------------------------------------------------------
		float m_sz;
		//----------------------------------------------------------------------------------
		/// \brief Update main transformation matrix
		//----------------------------------------------------------------------------------
		void UpdateTransform();
		//----------------------------------------------------------------------------------
	};
}

#endif
