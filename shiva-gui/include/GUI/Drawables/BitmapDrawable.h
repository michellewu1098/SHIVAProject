//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef __SHIVA_RESOURCESYSTEM_IMAGEDRAWABLE__
#define __SHIVA_RESOURCESYSTEM_IMAGEDRAWABLE__
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#include <string>

//////////////////////////////////////////////////////////////////////////
#include "Drawable.h"
//#include "Utility/GPUProgram.h"

//////////////////////////////////////////////////////////////////////////

namespace ShivaGUI
{
	class BitmapDrawable : public Drawable
	{
	public:
		BitmapDrawable();
		BitmapDrawable(unsigned int OpenGLTexID);
		virtual ~BitmapDrawable();

		void SetTexID(unsigned int value);

		/// For setting up the Drawable from xml
		virtual void Inflate(TiXmlElement*,ResourceManager*);

		virtual void Draw();

		virtual int GetNativeWidth() {return _texWidth;}
		virtual int GetNativeHeight() {return _texHeight;}

		/// If set, the image will be scaled to fill the bounds
		/// Otherwise, it will display the image at its actual size
		/// The default value is false
		void SetScaleup(bool value) {_scaleUp = value;}
		void SetScaleKeepAspectRatio(bool value) {_keepAspectRatio = value;}

	protected:
		unsigned int _texID;
		int _texWidth, _texHeight;

		/// If scaleUp is set to true and the bounds are smaller than the image size, it will scale it up
		/// otherwise, it will display the image at its actual size
		bool _scaleUp;

		/// Only used if _scaleUp is true
		bool _keepAspectRatio;

		virtual void OnSetBounds(float left, float top, float right, float bottom, unsigned int gravity);


		//Utility::GPUProgram *_testProgram;


	};
}

//////////////////////////////////////////////////////////////////////////
#endif
