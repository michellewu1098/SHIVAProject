///-----------------------------------------------------------------------------------------------
/// \file TotemController.h
/// \brief Contains the VolumeTree and provides access to all totempole operations
/// \author Leigh McLoughlin, Michelle Wu
/// \date Jun 7, 2013
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef TOTEMCONTROLLER_H_
#define TOTEMCONTROLLER_H_

#include <map>
#include <list>

#include "VolumeRenderer/SpringyVec3.h"
#include "VolumeTree/VolumeTree.h"
#include "VolumeTree/Nodes/CSG.h"
#include "VolumeTree/Nodes/TransformNode.h"
#include "VolumeTree/Leaves/VolCacheNode.h"
#include "VolumeTree/Leaves/CylinderNode.h"
#include "Totem/TotemObject.h"
#include "Totem/TotemOperation.h"
#include "Totem/Operations/TotemOpDrill.h"

namespace Totem
{ 
	//Note on terminology: am calling the things you stack 'objects' and the things you do to the stack 'operators' (even if operators are collections of objects)
	
	class Controller
	{
	public:

		//----------------------------------------------------------------------------------
		static Controller* Init();
		//----------------------------------------------------------------------------------
		static Controller* GetInstance() { return _instance; }
		//----------------------------------------------------------------------------------
		static void UnInit();
		//----------------------------------------------------------------------------------
		/// \brief Get node tree
		//----------------------------------------------------------------------------------
		VolumeTree::Node* GetNodeTree();
		//----------------------------------------------------------------------------------
		// Object loading
		//----------------------------------------------------------------------------------
		/// \brief Set number of primitives
		/// \param [in] value
		//----------------------------------------------------------------------------------
		void SetNumPrimitives( unsigned int value );
		//----------------------------------------------------------------------------------
		/// \brief Set primitive node
		/// \param [in] ID
		/// \param [in] primNode
		//----------------------------------------------------------------------------------
		void SetPrimitiveNode( unsigned int ID, VolumeTree::Node* );
		//----------------------------------------------------------------------------------
		/// \brief Get primitive node
		/// \param [in] ID
		//----------------------------------------------------------------------------------
		VolumeTree::Node* GetPrimitiveNode( unsigned int ID );
		//----------------------------------------------------------------------------------
		// For loading caches of the objects even if they are not currently in use
		//void PrecacheObject(std::string filename);
		//----------------------------------------------------------------------------------
		// Object manipulation
		//----------------------------------------------------------------------------------
		//void AddObjectToTop(std::string filename);
		//----------------------------------------------------------------------------------
		/// \brief Add object to top of stack of objects on pole
		/// \param [in] primID
		//----------------------------------------------------------------------------------
		void AddObjectToTop( unsigned int primID );
		//----------------------------------------------------------------------------------
		/// \brief Add object node to top of stack
		/// \param [in] nodeIn
		//----------------------------------------------------------------------------------
		void AddObjectNodeToTop( VolumeTree::Node* );
		//----------------------------------------------------------------------------------
		/// \brief Function to build up totem from loaded model
		/// \param [in] treeIn Imported tree stored in std::queue
		//----------------------------------------------------------------------------------
		void loadModel( std::queue< VolumeTree::Node* > treeIn );
		//----------------------------------------------------------------------------------
		/// \brief Select object at top of stack
		//----------------------------------------------------------------------------------
		void SelectTopObject();
		//----------------------------------------------------------------------------------
		/// \brief Select object above
		//----------------------------------------------------------------------------------
		void SelectObjectAbove();
		//----------------------------------------------------------------------------------
		/// \brief Select object below
		//----------------------------------------------------------------------------------
		void SelectObjectBelow();
		//----------------------------------------------------------------------------------
		/// \brief Reorder selected object
		/// \param [in] moveUp
		//----------------------------------------------------------------------------------
		void ReorderSelectedObject( bool moveUp );
		//----------------------------------------------------------------------------------
		/// \brief Move selected object
		/// \param [in] x
		/// \param [in] y
		/// \param [in] z
		//----------------------------------------------------------------------------------
		void MoveSelectedObject( float x, float y, float z );
		//----------------------------------------------------------------------------------
		/// \brief Reset selected object
		//----------------------------------------------------------------------------------
		void ResetMoveSelected();
		//----------------------------------------------------------------------------------
		/// \brief Delete selected object
		//----------------------------------------------------------------------------------
		void DeleteSelectedObject();
		//----------------------------------------------------------------------------------
		/// \brief Delete everything and rebuild pole
		//----------------------------------------------------------------------------------
		void DeleteAll();
		//----------------------------------------------------------------------------------
		/// \brief Draws a bounding box around the currently selected object, default is true
		//----------------------------------------------------------------------------------
		void ShowSelection( bool );
		//----------------------------------------------------------------------------------
		/// \brief Get selected object
		/// \return _selectedObject
		//----------------------------------------------------------------------------------
		Totem::Object* GetSelected() { return _selectedObject; }
		//----------------------------------------------------------------------------------
		/// \brief Will select the first object who's bounding box intersects with the line (if any)
		//----------------------------------------------------------------------------------
		bool SelectIntersectingObject( float originX, float originY, float originZ, float dirX, float dirY, float dirZ );
		//----------------------------------------------------------------------------------
		// Global Operations
		// Probably best to have a generic operation class and just store a stack of them here
		// How will local operations be dealt with?
		//----------------------------------------------------------------------------------
		/// \brief Add operation
		/// \param [in] inOp
		//----------------------------------------------------------------------------------
		void AddOperation( Operation* );
		//----------------------------------------------------------------------------------
		/// \brief Remove latest operation
		//----------------------------------------------------------------------------------
		void RemoveLastOperation();
		//----------------------------------------------------------------------------------
		/// \brief Adjust blending amount
		/// \param [in] value
		//----------------------------------------------------------------------------------
		void AdjustBlend( float value ) { _blendAmount += value; }
		//----------------------------------------------------------------------------------
		/// \brief Set blending amount
		/// \param [in] value
		//----------------------------------------------------------------------------------
		void SetBlend( float value ) { _blendAmount = value; }
		//----------------------------------------------------------------------------------
		/// \brief Get blending amount
		/// \return _blendAmount
		//----------------------------------------------------------------------------------
		float GetBlend() const { return _blendAmount; }
		//----------------------------------------------------------------------------------

	protected:

		//----------------------------------------------------------------------------------
		/// \brief Default ctor
		//----------------------------------------------------------------------------------
		Controller();
		//----------------------------------------------------------------------------------
		/// \brief Dtor
		//----------------------------------------------------------------------------------
		~Controller();
		//----------------------------------------------------------------------------------
		/// \brief Controller instance
		//----------------------------------------------------------------------------------
		static Controller *_instance;
		//----------------------------------------------------------------------------------
		/// \brief Function to rebuild pole: 1. If there's no totem pole, it rebuilds it from zero
		/// 2. If we add an object to the pole, we lengthen it and translate it
		/// 3. If there are no objects on the pole, we remove all global operations too
		//----------------------------------------------------------------------------------
		void RebuildPole();
		//----------------------------------------------------------------------------------
		/// \brief Root object
		//----------------------------------------------------------------------------------
		Totem::Object *_objectRoot;
		//----------------------------------------------------------------------------------
		/// \brief Selected object
		//----------------------------------------------------------------------------------
		Totem::Object *_selectedObject;
		//----------------------------------------------------------------------------------
		/// \brief Number of primitives
		//----------------------------------------------------------------------------------
		unsigned int _numPrimitives;
		//----------------------------------------------------------------------------------
		/// \brief Primitives
		//----------------------------------------------------------------------------------
		VolumeTree::Node **_primitives;
		//----------------------------------------------------------------------------------
		/// \brief Node for pole base
		//----------------------------------------------------------------------------------
		VolumeTree::CSGNode *_poleBaseNode;
		//----------------------------------------------------------------------------------
		/// \brief Node for pole (cylinder)
		//----------------------------------------------------------------------------------
		VolumeTree::CylinderNode *_poleNode;
		//----------------------------------------------------------------------------------
		/// \brief Node for pole transform
		//----------------------------------------------------------------------------------
		VolumeTree::TransformNode *_poleTransformNode;
		//----------------------------------------------------------------------------------
		/// \brief Flag used to show selection of object
		//----------------------------------------------------------------------------------
		bool _showSelection;
		//----------------------------------------------------------------------------------
		// I suspect we will need to traverse this in either direction
		//----------------------------------------------------------------------------------
		/// \brief List of operations
		//----------------------------------------------------------------------------------
		std::list< Operation* > _operations;
		//----------------------------------------------------------------------------------
		// TODO
		//std::map<std::string,VolumeTree::VolCacheNode*> _objectMap;
		//----------------------------------------------------------------------------------
		/// \brief Blend amount
		//----------------------------------------------------------------------------------
		float _blendAmount;
		//----------------------------------------------------------------------------------

	};
}

#endif
