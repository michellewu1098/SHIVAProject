//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef __SHIVA_RESOURCESYSTEM_STATELISTDRAWABLE__
#define __SHIVA_RESOURCESYSTEM_STATELISTDRAWABLE__
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#include <vector>
#include <utility>

//////////////////////////////////////////////////////////////////////////
#include "GUI/Drawables/Drawable.h"
#include "Utility/tinyxml.h"

//////////////////////////////////////////////////////////////////////////

namespace ShivaGUI
{
	class StateListDrawable : public Drawable
	{
	public:

		enum States
		{
			Active = 1,
			Disabled = 2,
			HasFocus = 4,
			Pressed = 8,
			ToggleActive	// For something that has a toggle-state for on/off that shows different to Active
		};

		StateListDrawable();
		virtual ~StateListDrawable();

		/// For setting up the Drawable from xml
		virtual void Inflate(TiXmlElement*,ResourceManager*);

		void SetCurrentState(int value) {_currentState = value;}
		int GetCurrentState() {return _currentState;}

		/// Assigns a drawable to a state, or set of states
		/// Note that state = 0 is the default, which will be used if no drawables match the current state
		void AddStateDrawable( Drawable*, int states );

		Drawable* GetCurrentDrawable();
		Drawable* GetDefaultDrawable() {return _defaultDrawable;}

		virtual void Draw();

		virtual void GetContentBounds(float &left, float &top, float &right, float &bottom);

		virtual int GetNativeWidth();
		virtual int GetNativeHeight();

		virtual int GetNativeWidthFromContent(int contentWidth);
		virtual int GetNativeHeightFromContent(int contentHeight);

	protected:
		std::vector<std::pair<Drawable*,int> > _stateDrawables;
		Drawable *_defaultDrawable;

		int _currentState;

		virtual void OnSetBounds(float left, float top, float right, float bottom, unsigned int gravity);

		/// For parsing a single drawable and getting the states for it
		void ParseItem(TiXmlElement*,ResourceManager*);
	};
}

//////////////////////////////////////////////////////////////////////////
#endif
