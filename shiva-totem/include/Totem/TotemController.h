/*
 * TotemController.h
 *
 *  Created on: Jun 7, 2013
 *      Author: leigh
 */

#ifndef TOTEMCONTROLLER_H_
#define TOTEMCONTROLLER_H_

#include "VolumeRenderer/SpringyVec3.h"
#include "VolumeTree/VolumeTree.h"
#include "VolumeTree/Nodes/CSG.h"
#include "VolumeTree/Nodes/TransformNode.h"
#include "VolumeTree/Leaves/VolCacheNode.h"
#include "VolumeTree/Leaves/CylinderNode.h"
#include "Totem/TotemObject.h"
#include "Totem/TotemOperation.h"

#include <map>
#include <list>

namespace Totem
{
	/// Contains the VolumeTree and provides access to all totempole operations
	/// Note on terminology: am calling the things you stack 'objects' and the things you do to the stack 'operators' (even if operators are collections of objects)
	class Controller
	{
	public:
		static Controller* Init();
		static Controller* GetInstance() {return _instance;}
		static void UnInit();

		VolumeTree::Node* GetNodeTree();

		// Object loading
			void SetNumPrimitives(unsigned int value);
			void SetPrimitiveNode(unsigned int ID, VolumeTree::Node*);
			VolumeTree::Node* GetPrimitiveNode(unsigned int ID);

			/// For loading caches of the objects even if they are not currently in use
			//void PrecacheObject(std::string filename);

		// Object manipulation
			//void AddObjectToTop(std::string filename);
			void AddObjectToTop(unsigned int primID);
			void AddObjectNodeToTop(VolumeTree::Node*);
			void SelectTopObject();
			void SelectObjectAbove();
			void SelectObjectBelow();
			void ReorderSelectedObject(bool moveUp);
			void MoveSelectedObject(float x, float y, float z);
			void ResetMoveSelected();
			void DeleteSelectedObject();
			void DeleteAll();
			/// Draws a bounding box around the currently selected object, default is true
			void ShowSelection(bool);

			Totem::Object* GetSelected() {return _selectedObject;}

			/// Will select the first object who's bounding box intersects with the line (if any)
			bool SelectIntersectingObject(float originX, float originY, float originZ, float dirX, float dirY, float dirZ);

		// Global Operations
			// Probably best to have a generic operation class and just store a stack of them here
			// How will local operations be dealt with?

			void AddOperation(Operation*);
			void RemoveLastOperation();

			void AdjustBlend(float value) {_blendAmount+=value;}
			void SetBlend(float value) {_blendAmount=value;}
			float GetBlend() {return _blendAmount;}

	protected:
		Controller();
		~Controller();
		
		static Controller *_instance;

		void RebuildPole();

		Totem::Object *_objectRoot, *_selectedObject;

		unsigned int _numPrimitives;
		VolumeTree::Node **_primitives;

		VolumeTree::CSGNode *_poleBaseNode;
		VolumeTree::CylinderNode *_poleNode;
		VolumeTree::TransformNode *_poleTransformNode;

		bool _showSelection;

		/// I suspect we will need to traverse this in either direction
		std::list<Operation*> _operations;

		// TODO
		//std::map<std::string,VolumeTree::VolCacheNode*> _objectMap;

		float _blendAmount;
	};

}

#endif
