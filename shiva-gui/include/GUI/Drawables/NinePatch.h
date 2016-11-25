//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef __SHIVA_RESOURCESYSTEM_NINEPATCH__
#define __SHIVA_RESOURCESYSTEM_NINEPATCH__
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
#include "GUI/Drawables/Drawable.h"

//////////////////////////////////////////////////////////////////////////

namespace ShivaGUI
{
	class NinePatch : public Drawable
	{
	public:

		NinePatch();
		virtual ~NinePatch();

		void SetTexID(unsigned int value);

		/// For setting up the Drawable from xml
		virtual void Inflate(TiXmlElement*,ResourceManager*);

		virtual void Draw();

		virtual void GetContentBounds(float &left, float &top, float &right, float &bottom);

		virtual int GetNativeWidth() {return _texWidth;}
		virtual int GetNativeHeight() {return _texHeight;}

		virtual int GetNativeWidthFromContent(int contentWidth);
		virtual int GetNativeHeightFromContent(int contentHeight);

	protected:

		virtual void OnSetBounds(float left, float top, float right, float bottom, unsigned int gravity);


		//
		float _centreLeftProp, _centreRightProp, _centreTopProp, _centreBottomProp;
		float _centreLeftBounds, _centreRightBounds, _centreTopBounds, _centreBottomBounds;

		/// Content size in proportions of image size
		float _contentLeftProp, _contentRightProp, _contentTopProp, _contentBottomProp;
		/// Actual content pixel positions
		float _contentLeftBounds, _contentRightBounds, _contentTopBounds, _contentBottomBounds;

		unsigned int _texID;
		int _texWidth, _texHeight;

		bool _fixedX, _fixedY;
	};
}

//////////////////////////////////////////////////////////////////////////
#endif
