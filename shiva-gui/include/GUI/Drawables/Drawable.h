//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef __SHIVA_RESOURCESYSTEM_DRAWABLE__
#define __SHIVA_RESOURCESYSTEM_DRAWABLE__
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
#include "ResourceManager.h"
#include "GUI/Views/View.h"

//////////////////////////////////////////////////////////////////////////

namespace ShivaGUI
{
	class Drawable
	{
	public:

		Drawable();
		virtual ~Drawable();

		/// Name used to load Drawable
		void SetFilename(std::string value) {_filename=value;}
		std::string GetFilename(){return _filename;}

		/// For setting up the Drawable from xml
		virtual void Inflate(TiXmlElement*,ResourceManager*) = 0;

		virtual void Draw() = 0;

		void SetBounds(float left, float top, float right, float bottom, unsigned int gravity = Definitions::CENTRE );
		void GetBounds(float &left, float &top, float &right, float &bottom) {left=_boundsLeft;top=_boundsTop;right=_boundsRight;bottom=_boundsBottom;}

		/// This is used during Layout to figure out the bounds of what is normally a visually 'contained' Drawable, such as an image within a button
		/// Base class sets these as outer bound values
		/// Sub-Drawable classes which have proper provision for containers should change this behaviour
		virtual void GetContentBounds(float &left, float &top, float &right, float &bottom);

		/// For retrieving the actual pixel size of the Drawable
		virtual int GetNativeWidth() {return 0;}
		virtual int GetNativeHeight() {return 0;}

		/// Some Drawables may have special size requirements if they are supposed to contain another Drawable
		/// These functions allow the native sizes to be retrieved on the condition that it contains another item
		virtual int GetNativeWidthFromContent(int contentWidth) {return contentWidth;}
		virtual int GetNativeHeightFromContent(int contentHeight) {return contentHeight;}

	protected:
		std::string _filename;

		float _boundsLeft, _boundsRight, _boundsTop, _boundsBottom;

		virtual void OnSetBounds(float left, float top, float right, float bottom, unsigned int gravity) {}
	};
}

//////////////////////////////////////////////////////////////////////////
#endif
