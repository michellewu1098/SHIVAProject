///-----------------------------------------------------------------------------------------------
/// \file LayeredImageDrawable.h
/// \brief This drawable takes a number of bitmaps and replaces each channel with a colour
/// The idea is to provide a mechanism for customising things like button colours
/// \author Leigh McLoughlin, Michelle Wu
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_RESOURCESYSTEM_LAYEREDIMAGEDRAWABLE__
#define __SHIVA_RESOURCESYSTEM_LAYEREDIMAGEDRAWABLE__


#include "Drawable.h"
#include "Utility/GPUProgram.h"

namespace ShivaGUI
{
	class LayeredImageDrawable : public Drawable
	{
	public:

		//----------------------------------------------------------------------------------
		/// \brief Ctor
		//----------------------------------------------------------------------------------
		LayeredImageDrawable();
		//----------------------------------------------------------------------------------
		/// \brief Dtor
		//----------------------------------------------------------------------------------
		virtual ~LayeredImageDrawable();
		//----------------------------------------------------------------------------------
		//Each layerGroup consists of 4 layers, corresponding to RGBA for each texture
		/// \brief Set texture id
		/// \param [in] _value Texture id value
		/// \param [in] _layerGroup Number of layer group
		//----------------------------------------------------------------------------------
		void SetTexID( unsigned int _value, unsigned int _layerGroup );
		//----------------------------------------------------------------------------------
		/// \brief Set layer colour
		/// \param [in] _colour
		/// \param [in] _group
		/// \param [in] _layer
		//----------------------------------------------------------------------------------
		void SetLayerColour( std::string _colour, unsigned int _group, unsigned int _layer );
		//----------------------------------------------------------------------------------
		/// \brief Set layer colour
		/// \param [in] _colour
		/// \param [in] _group
		/// \param [in] _layer
		//----------------------------------------------------------------------------------
		void SetLayerColour( unsigned int _colour, unsigned int _group, unsigned int _layer );
		//----------------------------------------------------------------------------------
		/// \brief For setting up the Drawable from xml
		//----------------------------------------------------------------------------------
		virtual void Inflate( TiXmlElement* _xmlElement, ResourceManager* _resources );
		//----------------------------------------------------------------------------------
		/// \brief Draw method
		//----------------------------------------------------------------------------------
		virtual void Draw();
		//----------------------------------------------------------------------------------
		/// \brief Get the content's bounds
		//----------------------------------------------------------------------------------
		void GetContentBounds( float &_left, float &_top, float &_right, float &_bottom );
		//----------------------------------------------------------------------------------
		/// \brief Get native texture width
		/// \return m_texWidth
		//----------------------------------------------------------------------------------
		virtual int GetNativeWidth() { return m_texWidth; }
		//----------------------------------------------------------------------------------
		/// \brief Get native texture height
		/// \return m_texHeight
		//----------------------------------------------------------------------------------
		virtual int GetNativeHeight() { return m_texHeight; }
		//----------------------------------------------------------------------------------
		/// \brief Get native width from content width
		/// \param [in] _contentWidth
		//----------------------------------------------------------------------------------
		virtual int GetNativeWidthFromContent( int _contentWidth );
		//----------------------------------------------------------------------------------
		/// \brief Get native height from content height
		/// \param [in] _contentHeight
		//----------------------------------------------------------------------------------
		virtual int GetNativeHeightFromContent( int _contentHeight );
		//----------------------------------------------------------------------------------
		/// \brief If set, the image will be scaled to fill the bounds
		/// Otherwise, it will display the image at its actual size
		/// The default value is false
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		void SetScaleup( bool _value ) { m_scaleUp = _value; }
		//----------------------------------------------------------------------------------
		/// \brief Add text layer
		/// \param [in] _fontColour Font colour
		//----------------------------------------------------------------------------------
		void AddTextLayer( unsigned int _fontColour );
		//----------------------------------------------------------------------------------
		/// \brief This is set when we're using ImageTextButton widget
		//----------------------------------------------------------------------------------
		void IsRenderingText() { m_usingText = true; }
		//----------------------------------------------------------------------------------

	protected:

		// Class for LayerGroup representation
		class LayerGroup
		{
		public:

			//----------------------------------------------------------------------------------
			/// \brief Ctor
			//----------------------------------------------------------------------------------
			LayerGroup();
			//----------------------------------------------------------------------------------
			/// \brief Dtor
			//----------------------------------------------------------------------------------
			~LayerGroup();
			//----------------------------------------------------------------------------------
			/// \brief Initilise layer group
			/// \param [in] _groupNumber Group number
			/// \param [in] _GLSLProgID Shader's id
			//----------------------------------------------------------------------------------
			void Init( unsigned int _groupNumber, unsigned int _GLSLProgID );
			//----------------------------------------------------------------------------------
			/// \brief Set texture id
			/// \param [in] _texID
			//----------------------------------------------------------------------------------
			void SetGLTexID( unsigned int _texID );
			//----------------------------------------------------------------------------------
			/// \brief Set layer colour
			/// \param [in] _colour
			/// \param [in] _layer
			//----------------------------------------------------------------------------------
			void SetLayerColour( unsigned int _colour, unsigned int _layer );
			//----------------------------------------------------------------------------------
			/// \brief Bind 
			//----------------------------------------------------------------------------------
			void Bind();
			//----------------------------------------------------------------------------------
			/// \brief Unbind
			//----------------------------------------------------------------------------------
			void Unbind();
			//----------------------------------------------------------------------------------

		protected:

			//----------------------------------------------------------------------------------
			/// \brief Layer group number
			//----------------------------------------------------------------------------------
			unsigned int m_groupNumber;
			//----------------------------------------------------------------------------------
			/// \brief Texture id
			//----------------------------------------------------------------------------------
			unsigned int m_texID;
			//----------------------------------------------------------------------------------
			/// \brief Colour uniforms
			//----------------------------------------------------------------------------------
			int *m_colourUniforms;
			//----------------------------------------------------------------------------------
			/// \brief Texture uniform
			//----------------------------------------------------------------------------------
			int m_textureUniform;
			//----------------------------------------------------------------------------------
			/// \brief Layer colours
			//----------------------------------------------------------------------------------
			float *m_layerColours;
			//----------------------------------------------------------------------------------
		};

		//----------------------------------------------------------------------------------
		/// \brief Uses bound values to update vertex and texture VBOs
		/// Requires VBO objects to exist
		//----------------------------------------------------------------------------------
		void BuildVBOs();
		//----------------------------------------------------------------------------------
		/// \brief Maximum number of layer groups
		//----------------------------------------------------------------------------------
		unsigned int m_maxLayerGroups;
		//----------------------------------------------------------------------------------
		/// \brief Layer groups
		//----------------------------------------------------------------------------------
		LayerGroup *m_layerGroups;
		//----------------------------------------------------------------------------------
		/// \brief Object vao
		//----------------------------------------------------------------------------------
		unsigned int m_vao;
		//----------------------------------------------------------------------------------
		/// \brief Layer's shader program
		//----------------------------------------------------------------------------------
		Utility::GPUProgram *m_shader;
		//----------------------------------------------------------------------------------
		// These are taken from the first layer group, we assume all layers are the same size
		//----------------------------------------------------------------------------------
		/// \brief Texture width
		//----------------------------------------------------------------------------------
		int m_texWidth;
		//----------------------------------------------------------------------------------
		/// \brief Texture height
		//----------------------------------------------------------------------------------
		int m_texHeight;
		//----------------------------------------------------------------------------------
		/// \brief If scaleUp is set to true and the bounds are smaller than the image size, it will scale it up
		/// otherwise, it will display the image at its actual size
		/// Default is true
		//----------------------------------------------------------------------------------
		bool m_scaleUp;
		//----------------------------------------------------------------------------------
		/// \brief Flag to choose whether too keep aspect ratio or not
		//----------------------------------------------------------------------------------
		bool m_keepAspectRatio;
		//----------------------------------------------------------------------------------
		/// \brief Whether the Drawable should act as a 9patch when scaled up
		/// Default is false
		//----------------------------------------------------------------------------------
		bool m_isNinePatch;
		//----------------------------------------------------------------------------------
		// Centre bounds associated with nine-patch
		//----------------------------------------------------------------------------------
		/// \brief Centre left size in proportions of image size
		//----------------------------------------------------------------------------------
		float m_centreLeftProp;
		//----------------------------------------------------------------------------------
		/// \brief Centre right size in proportions of image size
		//----------------------------------------------------------------------------------
		float m_centreRightProp;
		//----------------------------------------------------------------------------------
		/// \brief Centre top size in proportions of image size
		//----------------------------------------------------------------------------------
		float m_centreTopProp;
		//----------------------------------------------------------------------------------
		/// \brief Centre bottom size in proportions of image size
		//----------------------------------------------------------------------------------
		float m_centreBottomProp;
		//----------------------------------------------------------------------------------
		/// \brief Centre left boundaries
		//----------------------------------------------------------------------------------
		float m_centreLeftBounds;
		//----------------------------------------------------------------------------------
		/// \brief Centre right boundaries
		//----------------------------------------------------------------------------------
		float m_centreRightBounds;
		//----------------------------------------------------------------------------------
		/// \brief Centre top boundaries
		//----------------------------------------------------------------------------------
		float m_centreTopBounds;
		//----------------------------------------------------------------------------------
		/// \brief Centre bottom boundaries
		//----------------------------------------------------------------------------------
		float m_centreBottomBounds;
		//----------------------------------------------------------------------------------
		// Content size in proportions of image size
		//----------------------------------------------------------------------------------
		/// \brief Content left size in proportions of image size
		//----------------------------------------------------------------------------------
		float m_contentLeftProp;
		//----------------------------------------------------------------------------------
		/// \brief Content right size in proportions of image size
		//----------------------------------------------------------------------------------
		float m_contentRightProp;
		//----------------------------------------------------------------------------------
		/// \brief Content top size in proportions of image size
		//----------------------------------------------------------------------------------
		float m_contentTopProp;
		//----------------------------------------------------------------------------------
		/// \brief Content bottom size in proportions of image size
		//----------------------------------------------------------------------------------
		float m_contentBottomProp;
		//----------------------------------------------------------------------------------
		// Actual content pixel positions
		//----------------------------------------------------------------------------------
		/// \brief Content left boundaries
		//----------------------------------------------------------------------------------
		float m_contentLeftBounds;
		//----------------------------------------------------------------------------------
		/// \brief Content right boundaries
		//----------------------------------------------------------------------------------
		float m_contentRightBounds;
		//----------------------------------------------------------------------------------
		/// \brief Content top boundaries
		//----------------------------------------------------------------------------------
		float m_contentTopBounds;
		//----------------------------------------------------------------------------------
		/// \brief Content bottom boundaries
		//----------------------------------------------------------------------------------
		float m_contentBottomBounds;
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
		/// \brief Used to control the rendering of text below the image icon
		//----------------------------------------------------------------------------------
		bool m_usingText; 
		//----------------------------------------------------------------------------------
		/// \brief Number of layers in a LayeredImageDrawable from xml
		//----------------------------------------------------------------------------------
		unsigned int m_nLayers;
		//----------------------------------------------------------------------------------
		/// \brief Handle to texture representing text
		//----------------------------------------------------------------------------------
		int m_textTextureID;
		//----------------------------------------------------------------------------------

	};
}

#endif
