
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef __SHIVA_GUISYSTEM_SHAREDPREFERENCES__
#define __SHIVA_GUISYSTEM_SHAREDPREFERENCES__
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#include <string>

//////////////////////////////////////////////////////////////////////////
#include "Utility/tinyxml.h"

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////


namespace ShivaGUI
{

	class SharedPreferences
	{
	public:
		SharedPreferences(std::string file);
		~SharedPreferences();

		/// Writes changes to the preferences file
		void Save();

		bool Contains(std::string key);

		bool GetBoolean(std::string key, bool defaultValue);
		float GetFloat(std::string key, float defaultValue);
		int GetInt(std::string key, int defaultValue);
		std::string GetString(std::string key, std::string defaultValue);

		void SetBoolean(std::string key, bool value);
		void SetFloat(std::string key, float value);
		void SetInt(std::string key, int value);
		void SetString(std::string key, std::string value);


	protected:

		TiXmlDocument _prefsDoc;

		TiXmlNode* GetPrefNode(std::string key);

		template <class T> T GetValue(std::string key, std::string typeName, T defaultValue);
		template <class T> void SetValue(std::string key, std::string typeName, T value);

	};

}

//////////////////////////////////////////////////////////////////////////
#endif
