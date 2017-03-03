///-----------------------------------------------------------------------------------------------
/// \file StateListDrawable.h
/// \brief Contains a number of other Drawables, one for each 'state': these are used for things like buttons, which need to display different
/// Drawables depending on what state they're in (e.g. pressed or inactive etc.)
/// \author Leigh McLoughlin
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_RESOURCESYSTEM_STATELISTDRAWABLE__
#define __SHIVA_RESOURCESYSTEM_STATELISTDRAWABLE__

#include <utility>

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
		/// \param [in] _xmlElement
		/// \param [in] _resources
		//----------------------------------------------------------------------------------
		virtual void Inflate( TiXmlElement* _xmlElement, ResourceManager* _resources );
		//----------------------------------------------------------------------------------
		/// \brief Set current state
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		void SetCurrentState( int _value ) { m_currentState = _value; }
		//----------------------------------------------------------------------------------
		/// \brief Get current state
		/// \return m_currentState
		//----------------------------------------------------------------------------------
		int GetCurrentState() { return m_currentState; }
		//----------------------------------------------------------------------------------
		/// \brief Assign a drawable to a state, or set of states. Note that state = 0 is the default, which will be 
		/// used if no drawables match the current state
		/// \param [in] _drawable
		/// \param [in] _states
		//----------------------------------------------------------------------------------
		void AddStateDrawable( Drawable* _drawable, int _states );
		//----------------------------------------------------------------------------------
		/// \brief Get current drawable
		//----------------------------------------------------------------------------------
		Drawable* GetCurrentDrawable();
		//----------------------------------------------------------------------------------
		/// \brief Get default drawable
		/// \return m_defaultDrawable
		//----------------------------------------------------------------------------------
		Drawable* GetDefaultDrawable() { return m_defaultDrawable; }
		//----------------------------------------------------------------------------------
		/// \brief Draw method
		//----------------------------------------------------------------------------------
		virtual void Draw();
		//----------------------------------------------------------------------------------
		/// \brief Get content bounds
		/// \param [out] _left
		/// \param [out] _top
		/// \param [out] _right
		/// \param [out] _bottom
		//----------------------------------------------------------------------------------
		virtual void GetContentBounds( float &_left, float &_top, float &_right, float &_bottom );
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
		/// \param [in] _contentWidth
		//----------------------------------------------------------------------------------
		virtual int GetNativeWidthFromContent( int _contentWidth );
		//----------------------------------------------------------------------------------
		/// \brief Get native height of current Drawable
		/// \param [in] _contentHeight
		//----------------------------------------------------------------------------------
		virtual int GetNativeHeightFromContent( int _contentHeight );
		//----------------------------------------------------------------------------------

	protected:

		//----------------------------------------------------------------------------------
		/// \brief Container for states of Drawables
		//----------------------------------------------------------------------------------
		std::vector< std::pair< Drawable*, int > > m_stateDrawables;
		//----------------------------------------------------------------------------------
		/// \brief Default Drawable
		//----------------------------------------------------------------------------------
		Drawable *m_defaultDrawable;
		//----------------------------------------------------------------------------------
		/// \brief Current state
		//----------------------------------------------------------------------------------
		int m_currentState;
		//----------------------------------------------------------------------------------
		/// \brief Set bounds
		/// \param [in] _left
		/// \param [in] _top
		/// \param [in] _right
		/// \param [in] _bottom
		/// \param [in] _gravity
		//----------------------------------------------------------------------------------
		virtual void OnSetBounds( float _left, float _top, float _right, float _bottom, unsigned int _gravity );
		//----------------------------------------------------------------------------------
		/// \brief For parsing a single drawable and getting the states for it
		/// \param [in] _xmlElement
		/// \param [in] _resources
		//----------------------------------------------------------------------------------
		void ParseItem( TiXmlElement* _xmlElement, ResourceManager* _resources );
		//----------------------------------------------------------------------------------
	};
}

#endif
