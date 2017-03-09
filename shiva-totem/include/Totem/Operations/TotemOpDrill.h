///-----------------------------------------------------------------------------------------------
/// \file TotemOpDrill.h
/// \brief Base Operation class. These provide a sub-tree that modify the main tree in some way
/// Operations can be either local (specific to an Object on the totem) or global and applied to the entire totem
/// \author Leigh McLoughlin
/// \date Sep 18, 2013
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef TOTEMOPDRILL_H_
#define TOTEMOPDRILL_H_

#include "Totem/TotemOperation.h"
#include "VolumeTree/Leaves/CylinderNode.h"
#include "VolumeTree/Nodes/CSG.h"

namespace Totem
{
	namespace Operations
	{

		class Drill : public Totem::Operation
		{
		public:

			//----------------------------------------------------------------------------------
			/// \brief Ctor
			/// \param [in] _drillSize
			//----------------------------------------------------------------------------------
			Drill( float _drillSize );
			//----------------------------------------------------------------------------------
			/// \brief Ctor
			//----------------------------------------------------------------------------------
			Drill( VolumeTree::CylinderNode* _shape, VolumeTree::TransformNode* _rotate, VolumeTree::TransformNode* _translate );
			//----------------------------------------------------------------------------------
			/// Default dtor. Does not delete its main node
			//----------------------------------------------------------------------------------
			~Drill();
			//----------------------------------------------------------------------------------
			/// \brief This function must add the operation's nodes to the childNode subtree
			/// \param [in] _childNode
			/// \return m_drillOpNode
			//----------------------------------------------------------------------------------
			virtual VolumeTree::Node* GetNodeTree( VolumeTree::Node *_childNode );
			//----------------------------------------------------------------------------------
			/// \brief Set paramters for drill
			/// \param [in] _originX
			/// \param [in] _originY
			/// \param [in] _originZ
			/// \param [in] _dirX
			/// \param [in] _dirY
			/// \param [in] _dirZ
			/// \param [in] _length
			//----------------------------------------------------------------------------------
			void SetDrill( float _originX, float _originY, float _originZ, float _dirX, float _dirY, float _dirZ, float _length );
			//----------------------------------------------------------------------------------

		protected:

			//----------------------------------------------------------------------------------
			/// \brief Drilling operation node
			//----------------------------------------------------------------------------------
			VolumeTree::CSGNode *m_drillOpNode;
			//----------------------------------------------------------------------------------
			/// \brief Drill shape node
			//----------------------------------------------------------------------------------
			VolumeTree::CylinderNode *m_drillShapeNode;
			//----------------------------------------------------------------------------------
			/// \brief Drill translation node
			//----------------------------------------------------------------------------------
			VolumeTree::TransformNode *m_drillTranslateNode;
			//----------------------------------------------------------------------------------
			/// \brief Drill rotation node
			//----------------------------------------------------------------------------------
			VolumeTree::TransformNode *m_drillRotateNode;
			//----------------------------------------------------------------------------------
		};
	}
}

#endif
