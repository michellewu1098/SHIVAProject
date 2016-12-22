///-----------------------------------------------------------------------------------------------
/// \file TotemOperation.h
/// \brief Base Operation class. These provide a sub-tree that modify the main tree in some way
/// Operations can be either local (specific to an Object on the totem) or global and applied to the entire totem
/// \author Leigh McLoughlin
/// \date Sep 18, 2013
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef TOTEMOPERATION_H_
#define TOTEMOPERATION_H_

#include "VolumeTree/VolumeTree.h"
#include "VolumeTree/Nodes/TransformNode.h"

namespace Totem
{

	class Operation
	{
	public:

		//----------------------------------------------------------------------------------
		/// \brief Default ctor
		//----------------------------------------------------------------------------------
		Operation();
		//----------------------------------------------------------------------------------
		// \brief Default dtor. Does not delete its main node
		//----------------------------------------------------------------------------------
		~Operation();
		//----------------------------------------------------------------------------------
		/// \brief This function must add the operation's nodes to the childNode subtree
		//----------------------------------------------------------------------------------
		virtual VolumeTree::Node* GetNodeTree( VolumeTree::Node *childNode ) = 0;
		//----------------------------------------------------------------------------------

	};
}

#endif
