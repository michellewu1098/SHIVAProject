/*
 * TotemOperation.h
 *
 *  Created on: Sep 18, 2013
 *      Author: leigh
 */

#ifndef TOTEMOPERATION_H_
#define TOTEMOPERATION_H_

#include "VolumeTree/VolumeTree.h"
#include "VolumeTree/Nodes/TransformNode.h"

namespace Totem
{
	/// Base Operation class
	/// These provide a sub-tree that modify the main tree in some way
	/// Operations can be either local (specific to an Object on the totem) or global and applied to the entire totem
	class Operation
	{
	public:
		Operation();
		// Does not delete its main node
		~Operation();

		/// This function must add the operation's nodes to the childNode subtree
		virtual VolumeTree::Node* GetNodeTree(VolumeTree::Node *childNode) = 0;

		//void SetDrawBBox(bool);

	protected:

	};
}

#endif
