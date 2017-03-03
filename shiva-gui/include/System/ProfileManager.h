///-----------------------------------------------------------------------------------------------
/// \file ProfileManager.h
/// \brief Responsible for user profiles
/// Entities should query this for current state on start-up
/// Queries are assumed to be expensive, so try not to make them too often
/// If the current profile is changed at some point, a PROFILE_CHANGE event should be issued
/// \author Leigh McLoughlin
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_GUI_SYSTEM_PROFILEMANAGER__
#define __SHIVA_GUI_SYSTEM_PROFILEMANAGER__

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include "System/SharedPreferences.h"


namespace ShivaGUI
{
	class ProfileManager
	{
	public:

		//----------------------------------------------------------------------------------
		/// \brief Ctor
		/// \param [in] _authoringProgramName Human-readable name of the program
		/// \param [in] _programOptionsDir Directory for storing program-specific profile data
		//----------------------------------------------------------------------------------
		ProfileManager( std::string _authoringProgramName, std::string _programOptionsDir );
		//----------------------------------------------------------------------------------
		/// \brief Dtor
		//----------------------------------------------------------------------------------
		~ProfileManager();
		//----------------------------------------------------------------------------------

		// FILE MANAGEMENT

		//----------------------------------------------------------------------------------
		/// \brief Profiles are stored in a directory, which may be shared among programs
		/// \param [in] _profileDirectory
		//----------------------------------------------------------------------------------
		bool SetDirectory( std::string _profileDirectory );
		//----------------------------------------------------------------------------------
		/// \brief Returns profile directory
		//----------------------------------------------------------------------------------
		std::string GetDirectory();
		//----------------------------------------------------------------------------------
		/// \brief Clears all settings and loads them from a specific profile
		/// \param [in] _profileName
		//----------------------------------------------------------------------------------
		bool Load( std::string _profileName );
		//----------------------------------------------------------------------------------
		/// \brief Saves current settings to profile
		/// \param [in] _fileName
		//----------------------------------------------------------------------------------
		bool Save( std::string _fileName );
		//----------------------------------------------------------------------------------
		/// \brief For copying a profile. The profile does not have to be loaded
		/// \param [in] _profileName
		/// \param [in] _newProfileName
		//----------------------------------------------------------------------------------
		bool Copy( std::string _profileName, std::string _newProfileName );
		//----------------------------------------------------------------------------------
		/// \brief For setting the internal name of the profile
		/// This would normally be a person's name, but could be a description of the profile
		//----------------------------------------------------------------------------------
		void SetName();
		//----------------------------------------------------------------------------------
		/// \brief Returns the current directory for storing program-specific data
		//----------------------------------------------------------------------------------
		std::string GetCurrentOptionsDir();
		//----------------------------------------------------------------------------------
		/// \brief Returns the current profile filename
		//----------------------------------------------------------------------------------
		std::string GetCurrentProfileFileName();
		//----------------------------------------------------------------------------------

		// OPTION ACCESS

		//----------------------------------------------------------------------------------
		/// \brief Provides access to program-specific data stored in the current profile
		/// SharedPrefs are stored in: _profilesDir / _profileName / _prefsDir / prefs.xml
		/// Note that this will be deleted if another profile is loaded
		//----------------------------------------------------------------------------------
		SharedPreferences* GetProgSpecificOptions();
		//----------------------------------------------------------------------------------
		/// \brief Checks whether the profile contains a given option key at this level
		/// Multiple entries of the same option key may exist, so this will return the number of option entries
		/// \param [in] _key
		//----------------------------------------------------------------------------------
		int ContainsOption( std::string _key );
		//----------------------------------------------------------------------------------
		/// \brief For hierarchical options, this enters a new sub-node
		/// Subsequent calls to Get*() or Set*() will operate on child nodes within this node
		/// You should exit the node once you're finished
		/// Returns true on success, false if it could not find the node in question
		/// In the case that there are multiple nodes of the same name, you must use the index to specify which to enter
		/// Indices are simply the order that the options will appear in the xml file
		/// \param [in] _key
		/// \param [in] _index
		//----------------------------------------------------------------------------------
		bool EnterOptionNode( std::string _key, int _index = 0 );
		//----------------------------------------------------------------------------------
		/// \brief Leaves a sub-node in a hierarchical option, going up the tree to the parent
		/// You cannot go higher than the options level, e.g. you cannot go to the parent and view the header
		//----------------------------------------------------------------------------------
		void ExitOptionNode();
		//----------------------------------------------------------------------------------
		/// \brief For iterating through options
		//----------------------------------------------------------------------------------
		void InitIterator();
		//----------------------------------------------------------------------------------
		/// \brief Returns the key of the option and increments the iterator
		/// \return "END" and index as -1 when finished
		/// \param [in] _index
		//----------------------------------------------------------------------------------
		std::string GetNextKey( int &_index );
		//----------------------------------------------------------------------------------
		/// \brief Get boolean value
		/// \param [in] _key
		/// \param [in] _defaultValue
		/// \param [in] _index
		//----------------------------------------------------------------------------------
		bool GetBoolean( std::string _key, bool _defaultValue, int _index = 0 );
		//----------------------------------------------------------------------------------
		/// \brief Get float value
		/// \param [in] _key
		/// \param [in] _defaultValue
		/// \param [in] _index
		//----------------------------------------------------------------------------------
		float GetFloat( std::string _key, float _defaultValue, int _index = 0 );
		//----------------------------------------------------------------------------------
		/// \brief Get integer value
		/// \param [in] _key
		/// \param [in] _defaultValue
		/// \param [in] _index
		//----------------------------------------------------------------------------------
		int GetInt( std::string _key, int _defaultValue, int _index = 0 );
		//----------------------------------------------------------------------------------
		/// \brief Get string value
		/// \param [in] _key
		/// \param [in] _defaultValue
		/// \param [in] _index
		//----------------------------------------------------------------------------------
		std::string GetString( std::string _key, std::string _defaultValue, int _index = 0 );
		//----------------------------------------------------------------------------------
		/// \brief Set boolean value
		/// \param [in] _key
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		void SetBoolean( std::string _key, bool _value );
		//----------------------------------------------------------------------------------
		/// \brief Set float value
		/// \param [in] _key
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		void SetFloat( std::string _key, float _value );
		//----------------------------------------------------------------------------------
		/// \brief Set integer value
		/// \param [in] _key
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		void SetInt( std::string _key, int _value );
		//----------------------------------------------------------------------------------
		/// \brief Set string value
		/// \param [in] _key
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		void SetString( std::string _key, std::string _value );
		//----------------------------------------------------------------------------------

	protected:

		//----------------------------------------------------------------------------------
		/// \brief Directory that contains the profiles
		//----------------------------------------------------------------------------------
		boost::filesystem::path m_profilesDir;
		//----------------------------------------------------------------------------------
		/// \brief Direct link to the root node that contains all the options
		//----------------------------------------------------------------------------------
		TiXmlElement *m_optionsNode;
		//----------------------------------------------------------------------------------
		/// \brief The current options node
		//----------------------------------------------------------------------------------
		TiXmlElement *m_currentOptionsNode;
		//----------------------------------------------------------------------------------
		/// \brief Name text assigned to the profile
		//----------------------------------------------------------------------------------
		std::string m_name;
		//----------------------------------------------------------------------------------
		/// \brief Name of the program that will be used to author any profiles
		//----------------------------------------------------------------------------------
		std::string m_author;
		//----------------------------------------------------------------------------------
		/// \brief Name of the profile root
		/// Main profile data is stored in this name + .xml within the _profilesDir
		/// SharedPrefs are stored in: _profilesDir / _profileName / _prefsDir / prefs.xml
		//----------------------------------------------------------------------------------
		std::string m_profileName;
		//----------------------------------------------------------------------------------
		/// \brief Directory name of the program
		/// SharedPrefs are stored in: _profilesDir / _profileName / _prefsDir / prefs.xml
		//----------------------------------------------------------------------------------
		std::string m_prefsDir;
		//----------------------------------------------------------------------------------
		/// \brief Base tinyXML document for the profile
		//----------------------------------------------------------------------------------
		TiXmlDocument m_profileDoc;
		//----------------------------------------------------------------------------------
		/// \brief These are the program-specific options within the profile
		/// Changing the profile invalidates this
		//----------------------------------------------------------------------------------
		SharedPreferences *m_preferences;
		//----------------------------------------------------------------------------------
		/// \brief Utility function for updating the header of the profile
		/// If no header exists, it will create one
		//----------------------------------------------------------------------------------
		bool UpdateHeader();
		//----------------------------------------------------------------------------------
		/// \brief Utility function for retrieving a child node with a specific value
		/// Will return the 'index'th matching element
		/// Note that the value is checked case-insensitively
		/// Returns NULL if it is not found
		/// \param [in] _parent
		/// \param [in] _value
		/// \param [in] _index
		//----------------------------------------------------------------------------------
		TiXmlElement* GetSubElement( TiXmlElement *_parent, std::string _value, int _index = 0 );
		//----------------------------------------------------------------------------------
		/// \brief Utility function to retrieve a text node of an element
		/// Returns NULL if not found
		//----------------------------------------------------------------------------------
		TiXmlText* GetSubText( TiXmlElement *_parent );
		//----------------------------------------------------------------------------------
		/// \brief Utility function to add a sub-element with text
		/// If the sub-element already exists, it will replace the text
		/// Returns false on failure
		/// \param [in] _parent 
		/// \param [in] _value
		/// \param [in] _text
		//----------------------------------------------------------------------------------
		void AddSubElementText( TiXmlElement *_parent, std::string _value, std::string _text );
		//----------------------------------------------------------------------------------
		/// \brief For retrieving an option from the current option node
		/// \param [in] _key
		/// \param [in] _defaultValue
		/// \param [in] _index
		//----------------------------------------------------------------------------------
		template < class T > T GetOption( std::string _key, T _defaultValue, int _index = 0 );
		//----------------------------------------------------------------------------------
		/// \brief For setting an option in the current option node
		/// \param [in] _key
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		template <class T> void SetOption( std::string _key, T _value );
		//----------------------------------------------------------------------------------
		/// \brief Iterator for cycling through options
		//----------------------------------------------------------------------------------
		TiXmlNode *m_optionIterator;
		//----------------------------------------------------------------------------------

	};
}

#endif



