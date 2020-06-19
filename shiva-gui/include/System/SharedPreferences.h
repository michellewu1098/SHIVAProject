///-----------------------------------------------------------------------------------------------
/// \file SharedPreferences.h
/// \brief Shared preferences
/// \author Leigh McLoughlin
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_GUISYSTEM_SHAREDPREFERENCES__
#define __SHIVA_GUISYSTEM_SHAREDPREFERENCES__

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

#include "Utility/tinyxml.h"


namespace ShivaGUI
{
	class SharedPreferences
	{
	public:

		//----------------------------------------------------------------------------------
		/// \brief Ctor
		/// \param [in] _file
		//----------------------------------------------------------------------------------
		SharedPreferences( std::string _file );
		//----------------------------------------------------------------------------------
		/// \brief Dtor
		//----------------------------------------------------------------------------------
		~SharedPreferences();
		//----------------------------------------------------------------------------------
		/// \brief Writes changes to the preferences file
		//----------------------------------------------------------------------------------
		void Save();
		//----------------------------------------------------------------------------------
		/// \brief Returns true if it contains node with key value
		/// \param [in] _key
		//----------------------------------------------------------------------------------
		bool Contains( std::string _key );
		//----------------------------------------------------------------------------------
		/// \brief Get boolean value associated with key
		/// \param [in] _key
		/// \param [in] _defaultValue Boolean default value
		//----------------------------------------------------------------------------------
		bool GetBoolean( std::string _key, bool _defaultValue );
		//----------------------------------------------------------------------------------
		/// \brief Get float value associated with key
		/// \param [in] _key
		/// \param [in] _defaultValue Float default value
		//----------------------------------------------------------------------------------
		float GetFloat( std::string _key, float _defaultValue );
		//----------------------------------------------------------------------------------
		/// \brief Get int value associated with key
		/// \param [in] _key
		/// \param [in] _defaultValue Integer default value
		//----------------------------------------------------------------------------------
		int GetInt(std::string _key, int _defaultValue );
		//----------------------------------------------------------------------------------
		/// \brief Get string value associated with key
		/// \param [in] _key
		/// \param [in] _defaultValue String default value
		//----------------------------------------------------------------------------------
		std::string GetString( std::string _key, std::string _defaultValue );
		//----------------------------------------------------------------------------------
		/// \brief Set boolean value for key
		/// \param [in] _key
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		void SetBoolean( std::string _key, bool _value );
		//----------------------------------------------------------------------------------
		/// \brief Set float value for key
		/// \param [in] _key
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		void SetFloat( std::string _key, float _value );
		//----------------------------------------------------------------------------------
		/// \brief Set integer value for key
		/// \param [in] _key
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		void SetInt( std::string _key, int _value);
		//----------------------------------------------------------------------------------
		/// \brief Set string value for key
		/// \param [in] _key
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		void SetString( std::string _key, std::string _value );
		//----------------------------------------------------------------------------------


		// Set and Get the RGB colours of the model
		//----------------------------------------------------------------------------------
		/// \Set R component of totem colour
		//----------------------------------------------------------------------------------
		void SetTotColourR(float &_r);
		//----------------------------------------------------------------------------------
		/// \Get R component of totem colour
		//----------------------------------------------------------------------------------
		float GetTotColourR();
		//----------------------------------------------------------------------------------
		/// \Set G component of totem colour
		//----------------------------------------------------------------------------------
		void SetTotColourG(float &_g);
		//----------------------------------------------------------------------------------
		/// \Get G component of totem colour
		//----------------------------------------------------------------------------------
		float GetTotColourG();
		//----------------------------------------------------------------------------------
		/// \Set B component of totem colour
		//----------------------------------------------------------------------------------
		void SetTotColourB(float &_b);
		//----------------------------------------------------------------------------------
		/// \Get B component of totem colour
		//----------------------------------------------------------------------------------
		float GetTotColourB();
	
		//----------------------------------------------------------------------------------
		/// \Set file name and path of the current model file
		//----------------------------------------------------------------------------------
		void SetFullFilename(std::string _filename);
		//----------------------------------------------------------------------------------
		/// \Get file name and path of the current model file		
		//----------------------------------------------------------------------------------
		std::string GetFullFilename();

	protected:

		//----------------------------------------------------------------------------------
		/// \brief Preferences document
		//----------------------------------------------------------------------------------
		TiXmlDocument m_prefsDoc;
		//----------------------------------------------------------------------------------
		/// \brief Get preferences node
		/// \param [in] _key
		//----------------------------------------------------------------------------------
		TiXmlNode* GetPrefNode( std::string _key );
		//----------------------------------------------------------------------------------
		/// \brief Get value
		/// \param [in] _key
		/// \param [in] _typeName
		/// \param [in] _defaultValue
		//----------------------------------------------------------------------------------
		template <class T> T GetValue( std::string _key, std::string _typeName, T _defaultValue );
		//----------------------------------------------------------------------------------
		/// \brief Set value
		/// \param [in] _key
		/// \param [in] _typeName
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		template <class T> void SetValue( std::string _key, std::string _typeName, T _value );
		//----------------------------------------------------------------------------------
	};
}

#endif
