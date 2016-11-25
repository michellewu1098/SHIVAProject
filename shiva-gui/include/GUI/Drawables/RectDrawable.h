//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef __SHIVA_RESOURCESYSTEM_RECTDRAWABLE__
#define __SHIVA_RESOURCESYSTEM_RECTDRAWABLE__
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#include <string>

//////////////////////////////////////////////////////////////////////////
#include "Drawable.h"

//////////////////////////////////////////////////////////////////////////

namespace ShivaGUI
{
	/// This is a simple drawable that will draw a solid rectangle with a border
	class RectDrawable : public Drawable
	{
	public:
		RectDrawable();
		virtual ~RectDrawable();

		/// For setting up the Drawable from xml
		virtual void Inflate(TiXmlElement*,ResourceManager*);

		virtual void Draw();

		void SetFillColour(float r, float g, float b, float a) {_fillR=r;_fillG=g;_fillB=b;_fillA=a;}
		void SetBorderColour(float r, float g, float b, float a) {_borderR=r;_borderG=g;_borderB=b;_borderA=a;}

	protected:

		float _fillR, _fillG, _fillB, _fillA;
		float _borderR, _borderG, _borderB, _borderA;


		virtual void OnSetBounds(float left, float top, float right, float bottom, unsigned int gravity);

	};
}

//////////////////////////////////////////////////////////////////////////
#endif
