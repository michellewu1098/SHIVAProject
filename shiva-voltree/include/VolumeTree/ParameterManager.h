///-----------------------------------------------------------------------------------------------
/// \file ParameterManager.h
/// \brief Simple structure for storing parameter connection data
/// \author Leigh McLoughlin
/// \date Oct 29, 2013
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef PARAMETERMANAGER_H_
#define PARAMETERMANAGER_H_

#include "VolumeTree/VolumeTree.h"

namespace VolumeTree
{
	class Parameter
	{
	public:

	protected:
	};


	// The idea of a parameter is that it's linked to a node in the tree and can change a value
	
	class ParameterManager
	{
	public:

		//----------------------------------------------------------------------------------
		/// \brief Ctor
		//----------------------------------------------------------------------------------
		ParameterManager();
		//----------------------------------------------------------------------------------
		/// \brief Initialises parameters from vol file and links them to the tree
		/// The tree is assumed to be that loaded from the file
		/// Calls ReadParameters and then LinkParameters
		/// Returns the number of successfully linked parameters, or -1 for error
		/// \param [in] _tree
		/// \param [in] _filename
		//----------------------------------------------------------------------------------
		int LoadParameters( VolumeTree::Tree *_tree, std::string _filename );
		//----------------------------------------------------------------------------------
		/// \brief Reads the file and extracts parameters
		/// Does not link them to a tree
		/// Returns the number of read parameters, or -1 for error
		/// \param [in] _filename
		//----------------------------------------------------------------------------------
		int ReadParameters( std::string _filename );
		//----------------------------------------------------------------------------------
		/// \brief Links the parameters to a tree
		/// Attempts to tell the node that it should use rendering parameters instead of tree rebuilds
		/// Returns the number of successfully linked parameters, or -1 for error
		/// \param [in] _tree
		//----------------------------------------------------------------------------------
		int LinkParameters( VolumeTree::Tree *_tree );
		//----------------------------------------------------------------------------------
		/// \brief Get parameters
		/// \param [out] _numParams
		//----------------------------------------------------------------------------------
		Parameter* GetParameters( int &_numParams );
		//----------------------------------------------------------------------------------

	protected:

		//----------------------------------------------------------------------------------
		/// \brief Volume tree
		//----------------------------------------------------------------------------------
		VolumeTree::Tree *_tree;
		//----------------------------------------------------------------------------------

	};
}

#endif
