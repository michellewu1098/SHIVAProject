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
			/// \param [in] drillSize
			//----------------------------------------------------------------------------------
			Drill( float drillSize );
			//----------------------------------------------------------------------------------
			/// Default dtor. Does not delete its main node
			//----------------------------------------------------------------------------------
			~Drill();
			//----------------------------------------------------------------------------------
			/// \brief This function must add the operation's nodes to the childNode subtree
			/// \param [in] childNode
			/// \return _drillOpNode
			//----------------------------------------------------------------------------------
			virtual VolumeTree::Node* GetNodeTree( VolumeTree::Node *childNode );
			//----------------------------------------------------------------------------------
			/// \brief Set paramters for drill
			/// \param [in] originX
			/// \param [in] originY
			/// \param [in] originZ
			/// \param [in] dirX
			/// \param [in] dirY
			/// \param [in] dirZ
			/// \param [in] length
			//----------------------------------------------------------------------------------
			void SetDrill( float originX, float originY, float originZ, float dirX, float dirY, float dirZ, float length );
			//----------------------------------------------------------------------------------

		protected:

			//----------------------------------------------------------------------------------
			VolumeTree::CSGNode *_drillOpNode;
			//----------------------------------------------------------------------------------
			VolumeTree::CylinderNode *_drillShapeNode;
			//----------------------------------------------------------------------------------
			VolumeTree::TransformNode *_drillTranslateNode;
			//----------------------------------------------------------------------------------
			VolumeTree::TransformNode *_drillRotateNode;
			//----------------------------------------------------------------------------------
		};
	}
}

#endif
