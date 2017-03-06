///-----------------------------------------------------------------------------------------------
/// \file CustomLayoutParams.h
/// \brief Views use these for storing info about how the it wants to be arranged inside the Layout
/// \author Leigh McLoughlin
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_GUISYSTEM_CUSTOMLAYOUTPARAMS__
#define __SHIVA_GUISYSTEM_CUSTOMLAYOUTPARAMS__

#include "GUI/Views/ViewGroups/LayoutParams.h"
#include "Utility/tinyxml.h"


namespace ShivaGUI
{
	class CustomLayoutParams : public LayoutParams
	{
	public:

		//----------------------------------------------------------------------------------
		/// \brief Ctor
		//----------------------------------------------------------------------------------
		CustomLayoutParams();
		//----------------------------------------------------------------------------------
		/// \brief For saving the LayoutParam's attributes to xml
		/// \param [in] _xmlElement
		/// \param [in] _resources
		//----------------------------------------------------------------------------------
		virtual void Deflate( TiXmlElement* _xmlElement, ResourceManager *_resources );
		//----------------------------------------------------------------------------------
		// Use the base class's GetWidth/HeightPercent() for the width and height of the View
		//----------------------------------------------------------------------------------
		/// \brief Set centre x-coord. These positions are a proportion of the parent layout and must lie between 0 and 1
		//----------------------------------------------------------------------------------
		void SetCentreX( float _value ) { m_centreX = norm( _value ); }
		//----------------------------------------------------------------------------------
		/// \brief Set centre y-coord. These positions are a proportion of the parent layout and must lie between 0 and 1
		//----------------------------------------------------------------------------------
		void SetCentreY( float _value ) { m_centreY = norm( _value ); }
		//----------------------------------------------------------------------------------
		/// \brief Get centre x-coord
		/// \return m_centreX
		//----------------------------------------------------------------------------------
		float GetCentreX() { return m_centreX; }
		//----------------------------------------------------------------------------------
		/// \brief Get centre y-coord
		/// \return m_centreY
		//----------------------------------------------------------------------------------
		float GetCentreY() { return m_centreY; }
		//----------------------------------------------------------------------------------

	protected:

		//----------------------------------------------------------------------------------
		/// \brief Centre x-coord
		//----------------------------------------------------------------------------------
		float m_centreX;
		//----------------------------------------------------------------------------------
		/// \brief Centre y-coord
		//----------------------------------------------------------------------------------
		float m_centreY;
		//----------------------------------------------------------------------------------
		/// \brief Normalise
		//----------------------------------------------------------------------------------
		float norm( float );
		//----------------------------------------------------------------------------------

	};
}

#endif
