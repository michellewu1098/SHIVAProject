//#ifndef MYHEADER_H
//#define MYHEADER_H
// The above is called include guards (https://en.wikipedia.org/wiki/Include_guard)
// and are used to protect the header file from being included
// by the same source file twice

#ifndef __SHIVA_GUISYSTEM_SHARED__
#define __SHIVA_GUISYSTEM_SHARED__


// Define a namespace
namespace ShivaGUI
{
    // Define a class
    class SharedSettings
    {
    public:
		//----------------------------------------------------------------------------------
		/// \brief Ctor
		//----------------------------------------------------------------------------------
		SharedSettings();
		//----------------------------------------------------------------------------------
		/// \brief Dtor
		//----------------------------------------------------------------------------------
		~SharedSettings();


		void ShivaGUI::SharedSettings::SetEyegazePaused( bool _paused);

		bool ShivaGUI::SharedSettings::IsEyegazePaused();
	};
}

#endif
