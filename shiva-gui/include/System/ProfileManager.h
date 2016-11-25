
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef __SHIVA_GUI_SYSTEM_PROFILEMANAGER__
#define __SHIVA_GUI_SYSTEM_PROFILEMANAGER__
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include <string>
#include <boost/filesystem.hpp>

//////////////////////////////////////////////////////////////////////////
#include "Utility/tinyxml.h"
#include "System/SharedPreferences.h"

//////////////////////////////////////////////////////////////////////////

namespace ShivaGUI
{

	/// Responsible for user profiles
	/// Entities should query this for current state on start-up
	/// Queries are assumed to be expensive, so try not to make them too often
	/// If the current profile is changed at some point, a PROFILE_CHANGE event should be issued
	class ProfileManager
	{
	public:

		/// \param authoringProgramName Human-readable name of the program
		/// \param programOptionsDir Directory for storing program-specific profile data
		ProfileManager( std::string authoringProgramName, std::string programOptionsDir );
		~ProfileManager();

		// File Management

			/// Profiles are stored in a directory, which may be shared among programs
			bool SetDirectory( std::string profileDirectory );
			std::string GetDirectory();

			/// Clears all settings and loads them from a specific profile
			bool Load( std::string profileName );

			/// Saves current settings to profile
			bool Save( std::string fileName );

			/// For copying a profile
			/// The profile does not have to be loaded
			bool Copy( std::string profileName, std::string newProfileName );

			/// For setting the internal name of the profile
			/// This would normally be a person's name, but could be a description of the profile
			void SetName();

			/// Returns the current directory for storing program-specific data
			std::string GetCurrentOptionsDir();

			std::string GetCurrentProfileFileName();


		// Option Access

			/// Provides access to program-specific data stored in the current profile
			/// SharedPrefs are stored in: _profilesDir / _profileName / _prefsDir / prefs.xml
			/// Note that this will be deleted if another profile is loaded
			SharedPreferences* GetProgSpecificOptions();

			/// Checks whether the profile contains a given option key at this level
			/// Multiple entries of the same option key may exist, so this will return the number of option entries
			int ContainsOption( std::string key );

			/// For hierarchical options, this enters a new sub-node
			/// Subsequent calls to Get*() or Set*() will operate on child nodes within this node
			/// You should exit the node once you're finished
			/// Returns true on success, false if it could not find the node in question
			/// In the case that there are multiple nodes of the same name, you must use the index to specify which to enter
			/// Indices are simply the order that the options will appear in the xml file
			bool EnterOptionNode( std::string key, int index = 0 );

			/// Leaves a sub-node in a hierarchical option, going up the tree to the parent
			/// You cannot go higher than the options level, e.g. you cannot go to the parent and view the header
			void ExitOptionNode();


			/// For iterating through options
			void InitIterator();
			/// Returns the key of the option and increments the iterator
			/// Returns "END" and index as -1 when finished
			std::string GetNextKey(int &index);


			bool GetBoolean(std::string key, bool defaultValue, int index = 0 );
			float GetFloat(std::string key, float defaultValue, int index = 0 );
			int GetInt(std::string key, int defaultValue, int index = 0 );
			std::string GetString(std::string key, std::string defaultValue, int index = 0 );

			void SetBoolean(std::string key, bool value);
			void SetFloat(std::string key, float value);
			void SetInt(std::string key, int value);
			void SetString(std::string key, std::string value);


	protected:

		/// Directory that contains the profiles
		boost::filesystem::path _profilesDir;

		/// Direct link to the root node that contains all the options
		TiXmlElement *_optionsNode;

		/// The current options node
		TiXmlElement *_currentOptionsNode;

		/// Name text assigned to the profile
		std::string _name;

		/// Name of the program that will be used to author any profiles
		std::string _author;

		/// Name of the profile root
		/// Main profile data is stored in this name + .xml within the _profilesDir
		/// SharedPrefs are stored in: _profilesDir / _profileName / _prefsDir / prefs.xml
		std::string _profileName;

		/// Directory name of the program
		/// SharedPrefs are stored in: _profilesDir / _profileName / _prefsDir / prefs.xml
		std::string _prefsDir;

		/// Base tinyXML document for the profile
		TiXmlDocument _profileDoc;

		/// These are the program-specific options within the profile
		/// Changing the profile invalidates this
		SharedPreferences *_preferences;

		/// Utility function for updating the header of the profile
		/// If no header exists, it will create one
		bool UpdateHeader();

		/// Utility function for retrieving a child node with a specific value
		/// Will return the 'index'th matching element
		/// Note that the value is checked case-insensitively
		/// Returns NULL if it is not found
		TiXmlElement* GetSubElement(TiXmlElement *parent, std::string value, int index = 0);

		/// Utility function to retrieve a text node of an element
		/// Returns NULL if not found
		TiXmlText* GetSubText(TiXmlElement *parent);

		/// Utility function to add a sub-element with text
		/// If the sub-element already exists, it will replace the text
		/// Returns false on failure
		void AddSubElementText(TiXmlElement *parent, std::string value, std::string text);

		/// For retrieving an option from the current option node
		template <class T> T GetOption(std::string key, T defaultValue, int index = 0 );

		/// For setting an option in the current option node
		template <class T> void SetOption(std::string key, T value);

		/// Iterator for cycling through options
		TiXmlNode *_optionIterator;

	};
}

//////////////////////////////////////////////////////////////////////////
#endif



