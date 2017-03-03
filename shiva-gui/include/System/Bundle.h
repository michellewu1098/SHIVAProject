///-----------------------------------------------------------------------------------------------
/// \file Bundle.h
/// \brief Bundle class
/// \author Leigh McLoughlin
/// \todo Templated methods for this class
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_GUISYSTEM_BUNDLE__
#define __SHIVA_GUISYSTEM_BUNDLE__

#include <vector>
#include <map>
#include <string>

namespace ShivaGUI
{
	class Bundle
	{
	public:

		//----------------------------------------------------------------------------------
		/// \brief Ctor
		//----------------------------------------------------------------------------------
		Bundle();
		//----------------------------------------------------------------------------------
		/// \brief Dtor
		//----------------------------------------------------------------------------------
		~Bundle();
		//----------------------------------------------------------------------------------
		/// \brief Used to check if it contains specific key
		/// \param [in] _key
		//----------------------------------------------------------------------------------
		bool ContainsKey( std::string _key );
		//----------------------------------------------------------------------------------
		/// \brief Insert integer
		/// \param [in] _key
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		void PutInteger( std::string _key, int _value );
		//----------------------------------------------------------------------------------
		/// \brief Return integer associated with key
		/// \param [in] _key
		//----------------------------------------------------------------------------------
		int GetInteger( std::string _key );
		//----------------------------------------------------------------------------------
		/// \brief Insert float 
		/// \param [in] _key
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		void PutFloat( std::string _key, float _value );
		//----------------------------------------------------------------------------------
		/// \brief Returns float associated with key
		/// \param [in] _key
		//----------------------------------------------------------------------------------
		float GetFloat( std::string _key );
		//----------------------------------------------------------------------------------
		/// \brief Insert string 
		/// \param [in] _key 
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		void PutString( std::string _key, std::string _value );
		//----------------------------------------------------------------------------------
		/// \brief Returns string associated with key
		/// \param [in] _key
		//----------------------------------------------------------------------------------
		std::string GetString( std::string _key );
		//----------------------------------------------------------------------------------

	protected:

		//----------------------------------------------------------------------------------
		/// \brief Map of key to int
		//----------------------------------------------------------------------------------
		std::map< std::string, int > m_integers;
		//----------------------------------------------------------------------------------
		/// \brief Map of key to float
		//----------------------------------------------------------------------------------
		std::map< std::string, float > m_floats;
		//----------------------------------------------------------------------------------
		/// \brief Map of key to string
		//----------------------------------------------------------------------------------
		std::map< std::string, std::string > m_strings;
		//----------------------------------------------------------------------------------

	};

}

#endif
