/*
 * TotemController.h
 *
 *  Created on: Jun 7, 2013
 *      Author: leigh
 */

#ifndef TOTEMOBJECT_H_
#define TOTEMOBJECT_H_

#include "VolumeTree/VolumeTree.h"
#include "VolumeTree/Nodes/TransformNode.h"

namespace Totem
{
	/// This is a wrapper for the main objects in the totempole
	/// It is also a doubly-linked list
	class Object
	{
	public:
		Object(VolumeTree::Node*);
		// Does not delete its main node
		~Object();

		void SetChild(Object *value) {_child = value;RecalcOffsets();}
		void SetParent(Object *value) {_parent = value;RecalcOffsets();}
		Object* GetChild() {return _child;}
		Object* GetParent() {return _parent;}

		/// If shifting up this object switches places with its parent
		/// If shifting down, this object switches places with its child
		void ShiftOrder(bool up, bool swapOffsets = true);
		/// Useful if you've been shifting objects around
		Object* GetRoot();

		VolumeTree::Node* GetNodeTree(float blendAmount);

		void SetDrawBBox(bool);

		// The translation-offsets are translations relative to the object's main position in the totem stack
		void AddTranslationOffset(float x, float y, float z, bool checkOrder = true);
		void SetTranslationOffset(float x, float y, float z, bool checkOrder = false);
		void GetTranslationOffset(float &x, float &y, float &z) {x=_offsetX;y=_offsetY;z=_offsetZ;}

		// Rotations are in radians
		void SetRotation(float x, float y, float z) {_rx=x;_ry=y;_rz=z;UpdateTransform();}
		void AddRotation(float x, float y, float z) {_rx+=x;_ry+=y;_rz+=z;UpdateTransform();}
		void SetScale(float x, float y, float z) {_sx=x;_sy=y;_sz=z;UpdateTransform();}
		void AddScale(float x, float y, float z) {_sx+=x;_sy+=y;_sz+=z;UpdateTransform();}
		float GetScaleX() {return _sx;}
		float GetScaleY() {return _sy;}
		float GetScaleZ() {return _sz;}
		void GetTranslation(float &x, float &y, float &z) {x=_tx;y=_ty;z=_tz;}

		/// Returns the height of the object bounding box
		float GetBBoxZ();


		/// This should return the Z value that is the top of the bounding box of the object
		float GetBaseOffset();
		void RecalcOffsets();
		/// This is the basic position for the object
		/// Its actual position is this (in Z, with 0 in X,Y) plus the XYZ offset value
		float GetStackedPosition();

		/// Performs a self-intersection with the line and its bounding box and its children
		/// If a child object has a closer hit, it sets this as the selection param
		/// Return value is a distance from the origin to the bounding box of the object
		/// If there is no intersection, selection is NULL and return value has no meaning
		float SelectIntersectingObject(Totem::Object **selection, float originX, float originY, float originZ, float dirX, float dirY, float dirZ);

	protected:

		Totem::Object *_child, *_parent;

		/// Node that actually contains the object volume
		VolumeTree::Node *_mainNode;
		VolumeTree::TransformNode *_mainTransform;

		float _offsetX,_offsetY,_offsetZ;
		float _tx,_ty,_tz, _rx,_ry,_rz, _sx,_sy,_sz;

		void UpdateTransform();
	};
}

#endif
