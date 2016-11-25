//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef __SHIVA_RESOURCESYSTEM_LAYEREDIMAGEDRAWABLE__
#define __SHIVA_RESOURCESYSTEM_LAYEREDIMAGEDRAWABLE__
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#include <string>

//////////////////////////////////////////////////////////////////////////
#include "Drawable.h"
#include "Utility/GPUProgram.h"
#include "Utility/GPUVariable.h"

//////////////////////////////////////////////////////////////////////////

namespace ShivaGUI
{
	/// This drawable takes a number of bitmaps and replaces each channel with a colour
	/// The idea is to provide a mechanism for customising things like button colours
	class LayeredImageDrawable : public Drawable
	{
	public:
		LayeredImageDrawable();
		virtual ~LayeredImageDrawable();

		/// Each layerGroup consists of 4 layers, corresponding to RGBA for each texture
		void SetTexID(unsigned int value, unsigned int layerGroup);
		void SetLayerColour(std::string colour, unsigned int group, unsigned int layer);
		void SetLayerColour(unsigned int colour, unsigned int group, unsigned int layer);

		/// For setting up the Drawable from xml
		virtual void Inflate(TiXmlElement*,ResourceManager*);

		virtual void Draw();

		void GetContentBounds(float &left, float &top, float &right, float &bottom);

		virtual int GetNativeWidth() {return _texWidth;}
		virtual int GetNativeHeight() {return _texHeight;}

		virtual int GetNativeWidthFromContent(int contentWidth);
		virtual int GetNativeHeightFromContent(int contentHeight);

		/// If set, the image will be scaled to fill the bounds
		/// Otherwise, it will display the image at its actual size
		/// The default value is false
		void SetScaleup(bool value) {_scaleUp = value;}

	protected:


		class LayerGroup
		{
		public:
			LayerGroup();
			~LayerGroup();

			void Init(unsigned int groupNumber, unsigned int GLSLProgID);

			void SetGLTexID( unsigned int texID );

			void SetLayerColour(unsigned int colour, unsigned int layer);

			void Bind();
			void Unbind();
		protected:
			unsigned int _groupNumber;
			unsigned int _texID;
			int *_colourUniforms;
			int _textureUniform;
			float *_layerColours;

		};


		/// Uses bound values to update vertex and texture VBOs
		/// Requires VBO objects to exist
		void BuildVBOs();

		unsigned int _maxLayerGroups;

		LayerGroup *_layerGroups;


		/*
		unsigned int *_texID;
		/// Replacement colours for each layer
		int *_layerColours;

//		Utility::GPUVariable *_boundsLeftUniform, *_boundsRightUniform, *_boundsBottomUniform, *_boundsTopUniform;
		unsigned int *_layerColourUniforms;
		*/

		unsigned int _squareVertexVBO, _squareTexcoordVBO, _squareIndexVBO;
		Utility::GPUProgram *_layerProgram;

		/// These are taken from the first layer group, we assume all layers are the same size
		int _texWidth, _texHeight;


		/// If scaleUp is set to true and the bounds are smaller than the image size, it will scale it up
		/// otherwise, it will display the image at its actual size
		/// Default is true
		bool _scaleUp;

		bool _keepAspectRatio;

		/// Whether the Drawable should act as a 9patch when scaled up
		/// Default is false
		bool _isNinePatch;

		/// Centre bounds associated with nine-patch
		float _centreLeftProp, _centreRightProp, _centreTopProp, _centreBottomProp;
		float _centreLeftBounds, _centreRightBounds, _centreTopBounds, _centreBottomBounds;
		
		/// Content size in proportions of image size
		float _contentLeftProp, _contentRightProp, _contentTopProp, _contentBottomProp;
		/// Actual content pixel positions
		float _contentLeftBounds, _contentRightBounds, _contentTopBounds, _contentBottomBounds;

		virtual void OnSetBounds(float left, float top, float right, float bottom, unsigned int gravity);

	};
}

//////////////////////////////////////////////////////////////////////////
#endif
