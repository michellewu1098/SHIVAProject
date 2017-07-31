///-----------------------------------------------------------------------------------------------
/// \file Drawable.h
/// \brief Base class for drawable object
/// \author Leigh McLoughlin
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_RESOURCESYSTEM_DRAWABLE__
#define __SHIVA_RESOURCESYSTEM_DRAWABLE__

#include "ResourceManager.h"

namespace ShivaGUI
{
	class Drawable
	{
	public:

		//----------------------------------------------------------------------------------
		///\ brief Ctor
		//----------------------------------------------------------------------------------
		Drawable();
		//----------------------------------------------------------------------------------
		/// \brief Dtor
		//----------------------------------------------------------------------------------
		virtual ~Drawable();
		//----------------------------------------------------------------------------------
		/// \brief Set fileName used to load Drawable
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		void SetFilename( std::string _value ) { m_filename = _value; }
		//----------------------------------------------------------------------------------
		/// \brief Get filename
		/// \return m_filename
		//----------------------------------------------------------------------------------
		std::string GetFilename() { return m_filename; }
		//----------------------------------------------------------------------------------
		/// \brief For setting up the Drawable from xml
		//----------------------------------------------------------------------------------
		virtual void Inflate( TiXmlElement*, ResourceManager* ) = 0;
		//----------------------------------------------------------------------------------
		/// \brief Draw method
		//----------------------------------------------------------------------------------
		virtual void Draw() = 0;
		//----------------------------------------------------------------------------------
		/// \brief Set bounds
		/// \param [in] _left 
		/// \param [in] _top
		/// \param [in] _right
		/// \param [in] _bottom
		/// \param [in] _gravity
		//----------------------------------------------------------------------------------
		void SetBounds( const float &_left, const float &_top, const float &_right, const float &_bottom, const unsigned int &_gravity = Definitions::CENTRE );
		//----------------------------------------------------------------------------------
		/// \brief Get bounds
		/// \param [out] _left
		/// \param [out] _top
		/// \param [out] _right
		/// \param [out] _bottom
		//----------------------------------------------------------------------------------
		void GetBounds( float &_left, float &_top, float &_right, float &_bottom ) { _left = m_boundsLeft; _top = m_boundsTop; _right = m_boundsRight; _bottom = m_boundsBottom; }
		//----------------------------------------------------------------------------------
		/// \brief This is used during Layout to figure out the bounds of what is normally a visually 'contained' Drawable, such as an image within a button
		/// Base class sets these as outer bound values
		/// Sub-Drawable classes which have proper provision for containers should change this behaviour
		//----------------------------------------------------------------------------------
		virtual void GetContentBounds( float &_left, float &_top, float &_right, float &_bottom );
		//----------------------------------------------------------------------------------
		// For retrieving the actual pixel size of the Drawable
		//----------------------------------------------------------------------------------
		/// \brief Retrieve width of drawable
		//----------------------------------------------------------------------------------
		virtual int GetNativeWidth() { return 0; }
		//----------------------------------------------------------------------------------
		/// \brief Retrieve height of drawable
		//----------------------------------------------------------------------------------
		virtual int GetNativeHeight() { return 0; }
		//----------------------------------------------------------------------------------
		// Some Drawables may have special size requirements if they are supposed to contain another Drawable
		// These functions allow the native sizes to be retrieved on the condition that it contains another item
		//----------------------------------------------------------------------------------
		/// \brief Get width from content
		/// \param [in] _contentWidth
		//----------------------------------------------------------------------------------
		virtual int GetNativeWidthFromContent( int _contentWidth ) { return _contentWidth; }
		//----------------------------------------------------------------------------------
		/// \brief Get height from content
		/// \param [in] _contentHeight
		//----------------------------------------------------------------------------------
		virtual int GetNativeHeightFromContent( int _contentHeight ) { return _contentHeight; }
		//----------------------------------------------------------------------------------
		/// \brief Load projection and modelview matrices to shader
		//----------------------------------------------------------------------------------
		void LoadMatricesToShader( GLuint _shaderID, cml::matrix44f_c _proj, cml::matrix44f_c _mv );
		//----------------------------------------------------------------------------------

	protected:

		//----------------------------------------------------------------------------------
		/// \brief Filename
		//----------------------------------------------------------------------------------
		std::string m_filename;
		//----------------------------------------------------------------------------------
		/// \brief Left boundary
		//----------------------------------------------------------------------------------
		float m_boundsLeft;
		//----------------------------------------------------------------------------------
		/// \brief Right boundary
		//----------------------------------------------------------------------------------
		float m_boundsRight;
		//----------------------------------------------------------------------------------
		/// \brief Top boundary
		//----------------------------------------------------------------------------------
		float m_boundsTop;
		//----------------------------------------------------------------------------------
		/// \brief Bottom boundary
		//----------------------------------------------------------------------------------
		float m_boundsBottom;
		//----------------------------------------------------------------------------------
		/// \brief Set bounds
		/// \param [in] _left 
		/// \param [in] _top
		/// \param [in] _right
		/// \param [in] _bottom
		/// \param [in] _gravity
		//----------------------------------------------------------------------------------
		virtual void OnSetBounds( float _left, float _top, float _right, float _bottom, unsigned int _gravity ) {}
		//----------------------------------------------------------------------------------
		/// \brief Projection matrix
		//----------------------------------------------------------------------------------
		cml::matrix44f_c m_projMat;
		//----------------------------------------------------------------------------------
		/// \brief Modelview matrix
		//----------------------------------------------------------------------------------
		cml::matrix44f_c m_mvMat;
		//----------------------------------------------------------------------------------
	};
}

#endif // __SHIVA_RESOURCESYSTEM_DRAWABLE__
