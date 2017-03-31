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
#include "VolumeTree/Leaves/VolCacheNode.h"
#include "Totem/TotemObject.h"
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
		static Controller* GetInstance() { return m_instance; }
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
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		void SetNumPrimitives( unsigned int _value );
		//----------------------------------------------------------------------------------
		/// \brief Set primitive node
		/// \param [in] _ID
		/// \param [in] _primNode
		//----------------------------------------------------------------------------------
		void SetPrimitiveNode( unsigned int _ID, VolumeTree::Node* _primNode );
		//----------------------------------------------------------------------------------
		/// \brief Get primitive node
		/// \param [in] _ID
		//----------------------------------------------------------------------------------
		VolumeTree::Node* GetPrimitiveNode( unsigned int _ID );
		//----------------------------------------------------------------------------------
		// For loading caches of the objects even if they are not currently in use
		//void PrecacheObject(std::string filename);
		//----------------------------------------------------------------------------------
		// Object manipulation
		//----------------------------------------------------------------------------------
		//void AddObjectToTop(std::string filename);
		//----------------------------------------------------------------------------------
		/// \brief Add object to top of stack of objects on pole
		/// \param [in] _primID
		//----------------------------------------------------------------------------------
		void AddObjectToTop( unsigned int _primID, unsigned int _nGUIControllers );
		//----------------------------------------------------------------------------------
		/// \brief Add object node to top of stack
		/// \param [in] _nodeIn
		//----------------------------------------------------------------------------------
		void AddObjectNodeToTop( VolumeTree::Node* _nodeIn, unsigned int _nGUIControllers );
		//----------------------------------------------------------------------------------
		/// \brief Function to build up totem from loaded model
		/// \param [in] _treeIn Imported tree stored in std::queue
		//----------------------------------------------------------------------------------
		void LoadModel( std::queue< VolumeTree::Node* > _treeIn, unsigned int _nGUIControllers );
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
		/// \param [in] _moveUp
		//----------------------------------------------------------------------------------
		void ReorderSelectedObject( bool _moveUp );
		//----------------------------------------------------------------------------------
		/// \brief Move selected object
		/// \param [in] x
		/// \param [in] y
		/// \param [in] z
		//----------------------------------------------------------------------------------
		void MoveSelectedObject( float _x, float _y, float _z );
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
		Totem::Object* GetSelected() { return m_selectedObject; }
		//----------------------------------------------------------------------------------
		/// \brief Will select the first object who's bounding box intersects with the line (if any)
		//----------------------------------------------------------------------------------
		bool SelectIntersectingObject( float _originX, float _originY, float _originZ, float _dirX, float _dirY, float _dirZ );
		//----------------------------------------------------------------------------------
		// Global Operations
		// Probably best to have a generic operation class and just store a stack of them here
		// How will local operations be dealt with?
		//----------------------------------------------------------------------------------
		/// \brief Add operation
		/// \param [in] _inOp
		//----------------------------------------------------------------------------------
		void AddOperation( Operation* _inOp );
		//----------------------------------------------------------------------------------
		/// \brief Remove latest operation
		//----------------------------------------------------------------------------------
		void RemoveLastOperation();
		//----------------------------------------------------------------------------------
		/// \brief Adjust blending amount
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		void AdjustBlend( float _value ) { m_blendAmount += _value; }
		//----------------------------------------------------------------------------------
		/// \brief Set blending amount
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		void SetBlend( float _value ) { m_blendAmount = _value; }
		//----------------------------------------------------------------------------------
		/// \brief Get blending amount
		/// \return m_blendAmount
		//----------------------------------------------------------------------------------
		float GetBlend() const { return m_blendAmount; }
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
		static Controller *m_instance;
		//----------------------------------------------------------------------------------
		/// \brief Function to rebuild pole: 1. If there's no totem pole, it rebuilds it from zero
		/// 2. If we add an object to the pole, we lengthen it and translate it
		/// 3. If there are no objects on the pole, we remove all global operations too
		//----------------------------------------------------------------------------------
		void RebuildPole();
		//----------------------------------------------------------------------------------
		/// \brief Root object
		//----------------------------------------------------------------------------------
		Totem::Object *m_objectRoot;
		//----------------------------------------------------------------------------------
		/// \brief Selected object
		//----------------------------------------------------------------------------------
		Totem::Object *m_selectedObject;
		//----------------------------------------------------------------------------------
		/// \brief Number of primitives
		//----------------------------------------------------------------------------------
		unsigned int m_numPrimitives;
		//----------------------------------------------------------------------------------
		/// \brief Primitives
		//----------------------------------------------------------------------------------
		VolumeTree::Node **m_primitives;
		//----------------------------------------------------------------------------------
		/// \brief Node for pole base
		//----------------------------------------------------------------------------------
		VolumeTree::CSGNode *m_poleBaseNode;
		//----------------------------------------------------------------------------------
		/// \brief Node for pole (cylinder)
		//----------------------------------------------------------------------------------
		VolumeTree::CylinderNode *m_poleNode;
		//----------------------------------------------------------------------------------
		/// \brief Node for pole transform
		//----------------------------------------------------------------------------------
		VolumeTree::TransformNode *m_poleTransformNode;
		//----------------------------------------------------------------------------------
		/// \brief Flag used to show selection of object
		//----------------------------------------------------------------------------------
		bool m_showSelection;
		//----------------------------------------------------------------------------------
		// I suspect we will need to traverse this in either direction
		//----------------------------------------------------------------------------------
		/// \brief List of operations
		//----------------------------------------------------------------------------------
		std::list< Operation* > m_operations;
		//----------------------------------------------------------------------------------
		// TODO
		//std::map<std::string,VolumeTree::VolCacheNode*> _objectMap;
		//----------------------------------------------------------------------------------
		/// \brief Blend amount
		//----------------------------------------------------------------------------------
		float m_blendAmount;
		//----------------------------------------------------------------------------------

	};
}

#endif
