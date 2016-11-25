
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef __SHIVA_GUISYSTEM_LAYOUTPARAMS__
#define __SHIVA_GUISYSTEM_LAYOUTPARAMS__
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////

class TiXmlElement;

//////////////////////////////////////////////////////////////////////////

namespace ShivaGUI
{
	class ResourceManager;

	/// Views use these for storing info about how the it wants to be arranged inside the Layout
	class LayoutParams
	{
	public:

		enum FillType
		{
			FILL_PARENT,
			WRAP_CONTENT,
			PERCENTAGE_PARENT	// A percentage of the parent's size
		};

		LayoutParams();
		LayoutParams(float width, float height);
		LayoutParams( FillType width, FillType height );
		virtual ~LayoutParams();

		/// For saving the LayoutParam's attributes to xml
		virtual void Deflate(TiXmlElement*,ResourceManager *resources);


		/// Specifies that the basic layout size is given in percentages
		bool GetSizeUsesPercent() {return _usesPercent;}

		/// Specifies that the basic layout size is given with a special const that matches the parent in some way
		bool GetSizeUsesConst() {return _usesConst;}

		void SetHeightPercent(float value) {_heightPercent=value; _usesPercent = true; _usesConst = false;}
		void SetWidthPercent(float value) {_widthPercent=value; _usesPercent = true; _usesConst = false;}
		float GetHeightPercent() {return _heightPercent;}
		float GetWidthPercent() {return _widthPercent;}

		void SetHeightConst( FillType value ) {_heightConst=value;}
		void SetWidthConst( FillType value ) {_widthConst=value;}
		FillType GetHeightConst() {return _heightConst;}
		FillType GetWidthConst() {return _widthConst;}

		int GetPaddingLeft()   {return _paddingLeft;}
		int GetPaddingRight()  {return _paddingRight;}
		int GetPaddingTop()    {return _paddingTop;}
		int GetPaddingBottom() {return _paddingBottom;}

		void SetPadding(int left, int right, int bottom, int top) {_paddingLeft=left;_paddingRight=right;_paddingBottom=bottom;_paddingTop=top;}
		void SetPaddingLeft(int value)  {_paddingLeft=value;}
		void SetPaddingRight(int value) {_paddingRight=value;}
		void SetPaddingTop(int value)    {_paddingTop=value;}
		void SetPaddingBottom(int value)  {_paddingBottom=value;}

		/// The pixel sizes requested when using WRAP_CONTENT
		void SetWrapWidth(int value) {_wrapWidth=value;}
		void SetWrapHeight(int value) {_wrapHeight=value;}
		int GetWrapWidth(){return _wrapWidth;}
		int GetWrapHeight(){return _wrapHeight;}

	protected:
		/// These are percentages
		float _heightPercent, _widthPercent;

		FillType _heightConst, _widthConst;

		bool _usesPercent, _usesConst;

		/// Inset padding, in pixel sizes
		int _paddingLeft, _paddingRight, _paddingTop, _paddingBottom;

		int _wrapWidth, _wrapHeight;

	};

}

//////////////////////////////////////////////////////////////////////////
#endif
