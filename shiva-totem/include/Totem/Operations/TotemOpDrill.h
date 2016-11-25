/*
 * TotemOpDrill.h
 *
 *  Created on: Sep 18, 2013
 *      Author: leigh
 */

#ifndef TOTEMOPDRILL_H_
#define TOTEMOPDRILL_H_

#include "Totem/TotemOperation.h"
#include "VolumeTree/Leaves/CylinderNode.h"
#include "VolumeTree/Nodes/CSG.h"

namespace Totem
{
	namespace Operations
	{
		/// Base Operation class
		/// These provide a sub-tree that modify the main tree in some way
		/// Operations can be either local (specific to an Object on the totem) or global and applied to the entire totem
		class Drill : public Totem::Operation
		{
		public:
			Drill( float drillSize );
			// Does not delete its main node
			~Drill();

			/// This function must add the operation's nodes to the childNode subtree
			virtual VolumeTree::Node* GetNodeTree(VolumeTree::Node *childNode);

			void SetDrill(float originX,float originY,float originZ, float dirX,float dirY,float dirZ, float length);

			//void SetDrawBBox(bool);

		protected:

			VolumeTree::CSGNode *_drillOpNode;
			VolumeTree::CylinderNode *_drillShapeNode;
			VolumeTree::TransformNode *_drillTranslateNode, *_drillRotateNode;
		};
	}
}

#endif
