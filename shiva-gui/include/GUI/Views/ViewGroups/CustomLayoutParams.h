
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef __SHIVA_GUISYSTEM_CUSTOMLAYOUTPARAMS__
#define __SHIVA_GUISYSTEM_CUSTOMLAYOUTPARAMS__
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
#include "GUI/Views/ViewGroups/LayoutParams.h"

//////////////////////////////////////////////////////////////////////////

namespace ShivaGUI
{

	/// Views use these for storing info about how the it wants to be arranged inside the Layout
	class CustomLayoutParams : public LayoutParams
	{
	public:

		CustomLayoutParams();

		/// For saving the LayoutParam's attributes to xml
		virtual void Deflate(TiXmlElement*,ResourceManager *resources);

		// Use the base class's GetWidth/HeightPercent() for the width and height of the View

		// These positions are a proportion of the parent layout and must lie between 0 and 1
		void SetCentreX(float value) {_centreX = norm(value);}
		void SetCentreY(float value) {_centreY = norm(value);}
		float GetCentreX() {return _centreX;}
		float GetCentreY() {return _centreY;}

	protected:
		float _centreX, _centreY;

		float norm(float);
	};

}

//////////////////////////////////////////////////////////////////////////
#endif
