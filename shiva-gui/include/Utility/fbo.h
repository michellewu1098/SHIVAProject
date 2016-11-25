
#ifndef _fbo__
#define _fbo__

#include <GL/GLee.h>

#include <map>

/// The Frame Buffer Object namespace
/// For information on fbo's, try: 
/// The fbo extension documentation at: http://oss.sgi.com/projects/ogl-sample/registry/EXT/framebuffer_object.txt
/// The NVIDIA OpenGL Framebuffer Object Extension presentation from GDC 2005 by Simon Green
namespace FrameBufferObjects
{
	/// Forward declarations
	class attachment;
	class config;
	class FBO;


		/// The attachment points for frame buffer objects
	enum slot {
		FBO_NULL_ATTACHMENT		= 0x00000,
		FBO_COLOUR_ATTACHMENT0		= 0x00001,
		FBO_COLOUR_ATTACHMENT1		= 0x00002,
		FBO_COLOUR_ATTACHMENT2		= 0x00004,
		FBO_COLOUR_ATTACHMENT3		= 0x00008,
		FBO_COLOUR_ATTACHMENT4		= 0x00010,
		FBO_COLOUR_ATTACHMENT5		= 0x00020,
		FBO_COLOUR_ATTACHMENT6		= 0x00040,
		FBO_COLOUR_ATTACHMENT7		= 0x00080,
		FBO_COLOUR_ATTACHMENT8		= 0x00100,
		FBO_COLOUR_ATTACHMENT9		= 0x00200,
		FBO_COLOUR_ATTACHMENT10		= 0x00400,
		FBO_COLOUR_ATTACHMENT11		= 0x00800,
		FBO_COLOUR_ATTACHMENT12		= 0x01000,
		FBO_COLOUR_ATTACHMENT13		= 0x02000,
		FBO_COLOUR_ATTACHMENT14		= 0x04000,
		FBO_COLOUR_ATTACHMENT15		= 0x08000,
		FBO_DEPTH_ATTACHMENT		= 0x10000,
		FBO_STENCIL_ATTACHMENT		= 0x20000
		};


		/// The base attachment class
	class attachment
	{
	public:
			/// Constructor
		attachment() : is_bound(false),  current_slot(FBO_NULL_ATTACHMENT), current_config(NULL) {}

			/// Destructor
			/// Must unbind the attachment
		virtual ~attachment() {unbind();};

			/// For binding the attachment to a given slot
			/// Generic binding function, that internally calls the actual virtual bind functions, since the logic of this function is the same regardless of the actual bind method
		void attach(config*, slot);

	protected:

			/// This function contains the actual OpenGL binding method
			/// \note When this function is called, it is guaranteed (to be assumed) that current_slot will be set to the slot to bind to
		virtual void bind() {};

			/// This function contains the actual OpenGL unbinding method
		virtual void unbind() {};


			/// Whether the attachment is currently bound
		bool is_bound;

			/// The slot that the attachment is currently bound to
		slot current_slot;

			/// The configuration the attachment is currently attached in
			/// This information is only used to determine if the attachment is being bound twice within the same configuration
		config* current_config;
	};






		/// A texture attachment
		/// \note This is a base class because the functions are different depending on the texture type
	class tex_attachment : public attachment
	{
	public:

		enum tex_format
		{
			FBO_TEXTURE_RECTANGLE 	= GL_TEXTURE_RECTANGLE_ARB,
			FBO_TEXTURE_2D 		= GL_TEXTURE_2D,
			FBO_TEXTURE_3D 		= GL_TEXTURE_3D
		};

			/// Constructor takes the texture format and the texture ID
			/// Textures make sense outside of frame buffer objects, so their construction is left external
		tex_attachment(tex_format,unsigned int);

		virtual ~tex_attachment();

	protected:

			/// Actually binds the texture attachment
		virtual void bind();

			/// Actually unbinds the texture attachment
		virtual void unbind();

		unsigned int tex_ID;

		tex_format target_format;
	};


		/// A 2D texture attachment
	class tex_attachment_2D : public tex_attachment
	{
	public:

			/// \note Must generate the texture beforehand using openGL
		tex_attachment_2D(tex_format,unsigned int texture_ID);

		//~tex_attachment_2D();

	protected:

			/// Actually binds the texture attachment
		void bind();

			/// Actually unbinds the texture attachment
		void unbind();
	};



		/// A 3D texture attachment
	class tex_attachment_3D : public tex_attachment
	{
	public:

			/// \note Must generate the texture beforehand using openGL
		tex_attachment_3D(unsigned int texture_ID);

		//~tex_attachment_2D();

			/// 3D textures only get bound one 2D slice at a time
		void set_zoffset(unsigned int in) {zoffset = in;}

		void force_zoffset(unsigned int in) {set_zoffset(in); bind();}

	protected:

			/// Actually binds the texture attachment
		void bind();

			/// Actually unbinds the texture attachment
		void unbind();

		unsigned int zoffset;
	};








		/// The render buffer attachment class
		/// Render buffer attachments cannot be read from
		/// They are still extremely useful, e.g. you want to render objects to texture, and want depth testing, but will not be using the depth channel afterwards - just use a render buffer
	class buf_attachment : public attachment
	{
	public:

			/// The format that buffers can be
		enum buf_format
		{

				// From: base GL PixelFormat
			FBO_RGB			= GL_RGB,
			FBO_RGBA		= GL_RGBA,
			FBO_LUMINANCE		= GL_LUMINANCE,
			FBO_LUMINANCE_ALPHA	= GL_LUMINANCE_ALPHA,

			// From: base GL PixelInternalFormat
			FBO_R3_G3_B2		= GL_R3_G3_B2,
			FBO_ALPHA4		= GL_ALPHA4,
			FBO_ALPHA8		= GL_ALPHA8,
			FBO_ALPHA12		= GL_ALPHA12,
			FBO_ALPHA16		= GL_ALPHA16,
			FBO_LUMINANCE4		= GL_LUMINANCE4,
			FBO_LUMINANCE8		= GL_LUMINANCE8,
			FBO_LUMINANCE12		= GL_LUMINANCE12,
			FBO_LUMINANCE16		= GL_LUMINANCE16,
			FBO_LUMINANCE4_ALPHA4	= GL_LUMINANCE4_ALPHA4,
			FBO_LUMINANCE6_ALPHA2	= GL_LUMINANCE6_ALPHA2,
			FBO_LUMINANCE8_ALPHA8	= GL_LUMINANCE8_ALPHA8,
			FBO_LUMINANCE12_ALPHA4	= GL_LUMINANCE12_ALPHA4,
			FBO_LUMINANCE12_ALPHA12	= GL_LUMINANCE12_ALPHA12,
			FBO_LUMINANCE16_ALPHA16	= GL_LUMINANCE16_ALPHA16,
			FBO_INTENSITY		= GL_INTENSITY,
			FBO_INTENSITY4		= GL_INTENSITY4,
			FBO_INTENSITY8		= GL_INTENSITY8,
			FBO_INTENSITY12		= GL_INTENSITY12,
			FBO_INTENSITY16		= GL_INTENSITY16,
			FBO_RGB4		= GL_RGB4,
			FBO_RGB5		= GL_RGB5,
			FBO_RGB8		= GL_RGB8,
			FBO_RGB10		= GL_RGB10,
			FBO_RGB12		= GL_RGB12,
			FBO_RGB16		= GL_RGB16,
			FBO_RGBA2		= GL_RGBA2,
			FBO_RGBA4		= GL_RGBA4,
			FBO_RGB5_A1		= GL_RGB5_A1,
			FBO_RGBA8		= GL_RGBA8,
			FBO_RGB10_A2		= GL_RGB10_A2,
			FBO_RGBA12		= GL_RGBA12,
			FBO_RGBA16		= GL_RGBA16,

			// From: GL_ARB_texture_float
/*				= GL_TEXTURE_RED_TYPE_ARB,
				= GL_TEXTURE_GREEN_TYPE_ARB,
				= GL_TEXTURE_BLUE_TYPE_ARB,
				= GL_TEXTURE_ALPHA_TYPE_ARB,
				= GL_TEXTURE_LUMINANCE_TYPE_ARB,
				= GL_TEXTURE_INTENSITY_TYPE_ARB,
				= GL_TEXTURE_DEPTH_TYPE_ARB,
				= GL_UNSIGNED_NORMALIZED_ARB,
*/
			FBO_RGBA32F		= GL_RGBA32F_ARB,
			FBO_RGB32F		= GL_RGB32F_ARB,
			FBO_ALPHA32F		= GL_ALPHA32F_ARB,
			FBO_INTENSITY32F	= GL_INTENSITY32F_ARB,
			FBO_LUMINANCE32F	= GL_LUMINANCE32F_ARB,
			FBO_LUMINANCE_ALPHA32F	= GL_LUMINANCE_ALPHA32F_ARB,
			FBO_RGBA16F		= GL_RGBA16F_ARB,
			FBO_RGB16F		= GL_RGB16F_ARB,
			FBO_ALPHA16F		= GL_ALPHA16F_ARB,
			FBO_INTENSITY16F	= GL_INTENSITY16F_ARB,
			FBO_LUMINANCE16F	= GL_LUMINANCE16F_ARB,
			FBO_LUMINANCE_ALPHA16F	= GL_LUMINANCE_ALPHA16F_ARB,

			// From: GL_EXT_texture
				// These appear to be the same as the base GL PixelInternalFormat
/*
				= GL_ALPHA4_EXT,
				= GL_ALPHA8_EXT,
				= GL_ALPHA12_EXT,
				= GL_ALPHA16_EXT,
				= GL_LUMINANCE4_EXT,
				= GL_LUMINANCE8_EXT,
				= GL_LUMINANCE12_EXT,
				= GL_LUMINANCE16_EXT,
				= GL_LUMINANCE4_ALPHA4_EXT,
				= GL_LUMINANCE6_ALPHA2_EXT,
				= GL_LUMINANCE8_ALPHA8_EXT,
				= GL_LUMINANCE12_ALPHA4_EXT,
				= GL_LUMINANCE12_ALPHA12_EXT,
				= GL_LUMINANCE16_ALPHA16_EXT,
				= GL_INTENSITY_EXT,
				= GL_INTENSITY4_EXT,
				= GL_INTENSITY8_EXT,
				= GL_INTENSITY12_EXT,
				= GL_INTENSITY16_EXT,
				= GL_RGB2_EXT,
				= GL_RGB4_EXT,
				= GL_RGB5_EXT,
				= GL_RGB8_EXT,
				= GL_RGB10_EXT,
				= GL_RGB12_EXT,
				= GL_RGB16_EXT,
				= GL_RGBA2_EXT,
				= GL_RGBA4_EXT,
				= GL_RGB5_A1_EXT,
				= GL_RGBA8_EXT,
				= GL_RGB10_A2_EXT,
				= GL_RGBA12_EXT,
				= GL_RGBA16_EXT,
*/

			// These are FBO specific
			FBO_STENCIL_INDEX1	= GL_STENCIL_INDEX1_EXT,
			FBO_STENCIL_INDEX4	= GL_STENCIL_INDEX4_EXT,
			FBO_STENCIL_INDEX8	= GL_STENCIL_INDEX8_EXT,
			FBO_STENCIL_INDEX16	= GL_STENCIL_INDEX16_EXT,

			FBO_DEPTH		= GL_DEPTH_COMPONENT

		};

			/// Constructor takes the format, width and height of the buffer attachment
			/// Buffer attachments do not make sense outside of frame buffer objects, so their construction is internal
		buf_attachment(buf_format,unsigned int size_w, unsigned int size_h );

			/// The destructor will destroy the attachment
		~buf_attachment();


	protected:

			/// Actually binds the texture attachment
		void bind();

			/// Actually unbinds the texture attachment
		void unbind();

		unsigned int buffer_ID;
	};








	class config
	{
	public:
		config();

			/// Destructor will NOT destroy attachments, since they might be used by another config
		~config();

			/// Binds the configuration to the FBO
			/// \note This function must ONLY be called by the FBO class, because it does NOT set up which FBO will be bound to (this is done by the FBO)
		unsigned int bind(unsigned int);

			/// Sets the attachment for a specified slot
			/// If the slot already has an attachment, this will be overridden
		void attach(slot,attachment*);

	protected:
			/// Map associating the attachment slots of this configuration with the attachments
		std::map<slot,attachment*> attachments;
	};




	
		/// The actual frame buffer object class
	class FBO
	{
	public:
	
		FBO();

			/// \bug Unfortunately, deleting a frame buffer object will unbind any FBOs that are currently bound. This is because there is no easy way to tell which FBO is currently bound.
		~FBO();

			/// Checks the validity of the frame buffer object
			/// \return false on error
		bool check();

			/// Binds the frame buffer object using the specified configuration
			/// \return In debug mode, this function will return whether the binding was successful
			///         In release mode, this function will always return true (unless passed in NULL) and fail silently
		bool bind(config*);

			/// Unbinds the frame buffer object
			/// \note It is not necessary to unbind an FBO before binding a new one. Only unbind if you want to go back to the default window system.
			/// This function actually unbinds all frame buffer objects (i.e. if this one is not bound, but unbind is called, it will unbind whichever one, if any, is attached)
		void unbind();

	protected:

			/// Encoding of the previous attachment
			/// Each attachment slot is associated with a bit
			/// When a configuration is bound, it stores which slots were bound
			/// When the next configuration is bound, it must unbind any slots that were previously bound, but no longer in use
		unsigned int previous_config_bits;

			/// The OpenGL ID of the FBO
		unsigned int FBO_ID;
	};

}


#endif // _fbo
