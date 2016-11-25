/*
 * ParameterManager.h
 *
 *  Created on: Oct 29, 2013
 *      Author: leigh
 */

#ifndef PARAMETERMANAGER_H_
#define PARAMETERMANAGER_H_

#include "VolumeTree/VolumeTree.h"

namespace VolumeTree
{

	/// Simple structure for storing parameter connection data
	/// 
	class Parameter
	{
	public:

	protected:
	};


	/// The idea of a parameter is that it's linked to a node in the tree and can change a value
	class ParameterManager
	{
	public:
		ParameterManager();

		/// Initialises parameters from vol file and links them to the tree
		/// The tree is assumed to be that loaded from the file
		/// Calls ReadParameters and then LinkParameters
		/// Returns the number of successfully linked parameters, or -1 for error
		int LoadParameters( VolumeTree *tree, std::string filename );


			/// Reads the file and extracts parameters
			/// Does not link them to a tree
			/// Returns the number of read parameters, or -1 for error
			int ReadParameters( std::string filename );

			/// Links the parameters to a tree
			/// Attempts to tell the node that it should use rendering parameters instead of tree rebuilds
			/// Returns the number of successfully linked parameters, or -1 for error
			int LinkParameters( VolumeTree *tree );

		Parameter* GetParameters(int &out_numParams);

	protected:

		VolumeTree *_tree;
	};
}

#endif
