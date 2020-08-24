#include "System/SharedSettings.h"


ShivaGUI::SharedSettings::SharedSettings()
{ }


ShivaGUI::SharedSettings::~SharedSettings()
{ }


//----------------------------------------------------------------------------------

static	bool m_eyegazePaused = false;


void ShivaGUI::SharedSettings::SetEyegazePaused( bool _paused)
{
	m_eyegazePaused = _paused; 
}

bool ShivaGUI::SharedSettings::IsEyegazePaused()
{
	return m_eyegazePaused; 
}
