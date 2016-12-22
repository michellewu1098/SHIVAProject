///-----------------------------------------------------------------------------------------------
/// \file StateListDrawable.h
/// \brief Contains a number of other Drawables, one for each 'state': these are used for things like buttons, which need to display different
/// Drawables depending on what state they're in (e.g. pressed or inactive etc.)
/// \author Leigh McLoughlin
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_RESOURCESYSTEM_STATELISTDRAWABLE__
#define __SHIVA_RESOURCESYSTEM_STATELISTDRAWABLE__

#include <vector>
#include <utility>
#include <string>
#include <iostream>

#include "GUI/Drawables/Drawable.h"
#include "Utility/tinyxml.h"


namespace ShivaGUI
{
	class StateListDrawable : public Drawable
	{
	public:

		//----------------------------------------------------------------------------------
		/// \brief List of possible states
		//----------------------------------------------------------------------------------
		enum States
		{
			Active = 1,
			Disabled = 2,
			HasFocus = 4,
			Pressed = 8,
			ToggleActive	// For something that has a toggle-state for on/off that shows different to Active
		};
		//----------------------------------------------------------------------------------
		/// \brief Ctor
		//----------------------------------------------------------------------------------
		StateListDrawable();
		//----------------------------------------------------------------------------------
		/// \brief Dtor
		//----------------------------------------------------------------------------------
		virtual ~StateListDrawable();
		//----------------------------------------------------------------------------------
		/// \brief For setting up the Drawable from xml
		/// \param [in] xmlElement
		/// \param [in] resources
		//----------------------------------------------------------------------------------
		virtual void Inflate( TiXmlElement*, ResourceManager* );
		//----------------------------------------------------------------------------------
		/// \brief Set current state
		/// \param [in] value
		//----------------------------------------------------------------------------------
		void SetCurrentState( int value ) { _currentState = value; }
		//----------------------------------------------------------------------------------
		/// \brief Get current state
		/// \return _currentState
		//----------------------------------------------------------------------------------
		int GetCurrentState() { return _currentState; }
		//----------------------------------------------------------------------------------
		/// \brief Assign a drawable to a state, or set of states. Note that state = 0 is the default, which will be 
		/// used if no drawables match the current state
		/// \param [in] drawable
		/// \param [in] states
		//----------------------------------------------------------------------------------
		void AddStateDrawable( Drawable*, int states );
		//----------------------------------------------------------------------------------
		/// \brief Get current drawable
		//----------------------------------------------------------------------------------
		Drawable* GetCurrentDrawable();
		//----------------------------------------------------------------------------------
		/// \brief Get default drawable
		/// \return _defaultDrawable
		//----------------------------------------------------------------------------------
		Drawable* GetDefaultDrawable() { return _defaultDrawable; }
		//----------------------------------------------------------------------------------
		/// \brief Draw method
		//----------------------------------------------------------------------------------
		virtual void Draw();
		//----------------------------------------------------------------------------------
		/// \brief Get content bounds
		/// \param [out] left
		/// \param [out] top
		/// \param [out] right
		/// \param [out] bottom
		//----------------------------------------------------------------------------------
		virtual void GetContentBounds( float &left, float &top, float &right, float &bottom );
		//----------------------------------------------------------------------------------
		// For retrieving the actual pixel size of Drawable
		//----------------------------------------------------------------------------------
		/// \brief Get current Drawable width
		//----------------------------------------------------------------------------------
		virtual int GetNativeWidth();
		//----------------------------------------------------------------------------------
		/// \brief Get current Drawable height
		//----------------------------------------------------------------------------------
		virtual int GetNativeHeight();
		//----------------------------------------------------------------------------------
		/// \brief Get native width of current Drawable
		/// \param [in] contentWidth
		//----------------------------------------------------------------------------------
		virtual int GetNativeWidthFromContent( int contentWidth );
		//----------------------------------------------------------------------------------
		/// \brief Get native height of current Drawable
		/// \param [in] contentHeight
		//----------------------------------------------------------------------------------
		virtual int GetNativeHeightFromContent( int contentHeight );
		//----------------------------------------------------------------------------------

	protected:

		//----------------------------------------------------------------------------------
		/// \brief Container for states of Drawables
		//----------------------------------------------------------------------------------
		std::vector< std::pair< Drawable*, int > > _stateDrawables;
		//----------------------------------------------------------------------------------
		/// \brief Default Drawable
		//----------------------------------------------------------------------------------
		Drawable *_defaultDrawable;
		//----------------------------------------------------------------------------------
		/// \brief Current state
		//----------------------------------------------------------------------------------
		int _currentState;
		//----------------------------------------------------------------------------------
		/// \brief Set bounds
		/// \param [in] left
		/// \param [in] top
		/// \param [in] right
		/// \param [in] bottom
		/// \param [in] gravity
		//----------------------------------------------------------------------------------
		virtual void OnSetBounds( float left, float top, float right, float bottom, unsigned int gravity );
		//----------------------------------------------------------------------------------
		/// \brief For parsing a single drawable and getting the states for it
		/// \param [in] xmlElement
		/// \param [in] resources
		//----------------------------------------------------------------------------------
		void ParseItem( TiXmlElement*, ResourceManager* );
		//----------------------------------------------------------------------------------
	};
}

#endif
