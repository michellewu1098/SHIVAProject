
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef __SHIVA_GUISYSTEM_BUNDLE__
#define __SHIVA_GUISYSTEM_BUNDLE__
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#include <vector>
#include <map>
#include <string>

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////

namespace ShivaGUI
{

	/// TODO: templated methods for this class
	class Bundle
	{
	public:
		Bundle();
		~Bundle();

		bool ContainsKey(std::string key);

		/*
		void PutBoolean(std::string key, bool value);
		bool GetBoolean(std::string key);
		void PutBooleanArray(std::string key, bool *value);
		bool* GetBooleanArray(std::string key);
		*/
		void PutInteger(std::string key, int value);
		int GetInteger(std::string key);
		//int* GetIntegerArray(std::string key);

		void PutFloat(std::string key, float value);
		float GetFloat(std::string key);

		void PutString(std::string key, std::string value);
		std::string GetString(std::string key);
		/*
		float* GetFloatArray(std::string key);


		Bundle* GetBundle(std::string key);
		*/

	protected:

		std::map<std::string,int> _integers;

		std::map<std::string,float> _floats;

		std::map<std::string,std::string > _strings;

	};

}

//////////////////////////////////////////////////////////////////////////
#endif
