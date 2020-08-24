///-----------------------------------------------------------------------------------------------
/// \file ImageButton.h
/// \brief This is a very simple test widget. It's just a set of images for a set of states. It can be selected and pressed
/// If this is given a StateListDrawable, it will use its states
/// \author Leigh McLoughlin
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_GUISYSTEM_IMAGE_BUTTON__
#define __SHIVA_GUISYSTEM_IMAGE_BUTTON__

#include "GUI/Views/View.h"
#include "GUI/Views/ViewEventListener.h"
#include "GUI/Drawables/StateListDrawable.h"
#include "GUI/Drawables/LayeredImageDrawable.h"
#include "Audio/AudioClip.h"
#include "Utility/GPUProgram.h"

namespace ShivaGUI
{
	class ImageButton : public View
	{
	public:

		//----------------------------------------------------------------------------------
		/// \brief Default ctor
		//----------------------------------------------------------------------------------
		ImageButton();
		//----------------------------------------------------------------------------------
		/// \brief Dtor
		//----------------------------------------------------------------------------------
		virtual ~ImageButton();
		//----------------------------------------------------------------------------------
		/// \brief Gives the size of the View
		/// If this View has children, it is expected to work out the size and location of these and call Layout() on them too
		/// \param [in] _left
		/// \param [in] _top
		/// \param [in] _right
		/// \param [in] _bottom
		/// \param [in] _windowWidth
		/// \param [in] _windowHeight
		//----------------------------------------------------------------------------------
		virtual void Layout( int _left, int _top, int _right, int _bottom, int _windowWidth, int _windowHeight );
		//----------------------------------------------------------------------------------
		/// \brief Update method
		/// \param [in] _deltaTs Timestep
		/// \param [in] _guiController Link to GUI controller
		//----------------------------------------------------------------------------------
		virtual void Update( float _deltaTs, GUIController *_guiController );
		//----------------------------------------------------------------------------------
		/// \brief Draw method
		//----------------------------------------------------------------------------------
		virtual void Draw();
		//----------------------------------------------------------------------------------
		/// \brief Draw method
		/// \param [in] _context Current OGL context
		//----------------------------------------------------------------------------------
		virtual void Draw( unsigned int _context );
		//----------------------------------------------------------------------------------
		/// \brief Themes allow settings to be set for all Views of a given type
		/// The string returned from this function is the string that identifies the type in the theme xml file
		/// It is literally prefixed to normal inflation attributes
		/// \return "ImageButton_"
		//----------------------------------------------------------------------------------
		virtual std::string GetThemePrefix() { return "ImageButton_"; }
		//----------------------------------------------------------------------------------
		/// \brief For setting the View's attributes from xml
		/// \param [in] _xmlElement
		/// \param [in] _resources
		/// \param [in] _themePrefix
		/// \param [in] _rootNode
		//----------------------------------------------------------------------------------
		virtual void Inflate( TiXmlElement* _xmlElement, ResourceManager* _resources, std::string _themePrefix = "", bool _rootNode = false );
		//----------------------------------------------------------------------------------
		/// \brief For saving the View's attributes to xml
		/// \param [in] _resources
		/// \note This must be hierarchical and the element must include all child elements
		//----------------------------------------------------------------------------------
		virtual TiXmlElement* Deflate( ResourceManager *_resources );
		//----------------------------------------------------------------------------------
		/// \brief For setting the focus of this View
		/// If focussed, it is expected that the view will show this visually
		/// and then allow other forms of input to be received, e.g. 'enter' button pressed etc
		//----------------------------------------------------------------------------------
		virtual void SetFocus( bool );
		//----------------------------------------------------------------------------------
		/// \brief For setting the selection of this View
		//----------------------------------------------------------------------------------
		virtual void SetSelect( bool );
		//----------------------------------------------------------------------------------
		/// \brief Handle internal event
		/// \param [in] _eventIn 
		//----------------------------------------------------------------------------------
		virtual bool HandleEvent( InternalEvent* _eventIn );
		//----------------------------------------------------------------------------------
		/// \brief Set on click listener
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		void SetOnClickListener( ViewEventListener *_value ) { m_clickListener = _value; }
		//----------------------------------------------------------------------------------
		/// \brief Set content drawable
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		virtual void SetContent( Drawable *_value );
		//----------------------------------------------------------------------------------
		/// \brief Get content drawable
		//----------------------------------------------------------------------------------
		Drawable* GetContent() { return m_contentStateListDrawable == NULL ? m_contentGenDrawable : m_contentStateListDrawable; }
		//----------------------------------------------------------------------------------
		/// \brief Requested wrap pixel dimensions for layout (width)
		/// \note These are used when SetLayoutParams() is called, which will be after the View has been inflated
		//----------------------------------------------------------------------------------
		virtual int GetWrapWidth();
		//----------------------------------------------------------------------------------
		/// \brief Requested wrap pixel dimensions for layout (height)
		//----------------------------------------------------------------------------------
		virtual int GetWrapHeight();
		//----------------------------------------------------------------------------------


	protected:
		int m_left;
	    int m_right;
	    int m_top;
		int m_bottom;


		//----------------------------------------------------------------------------------
		/// \brief Button centre x-coord
		//----------------------------------------------------------------------------------
		int m_centreX;
		//----------------------------------------------------------------------------------
		/// \brief Button centre y-coord
		//----------------------------------------------------------------------------------
		int m_centreY;
		//----------------------------------------------------------------------------------
		/// \brief Button size along x
		//----------------------------------------------------------------------------------
		int m_sizeX;
		//----------------------------------------------------------------------------------
		/// \brief Button size along y
		//----------------------------------------------------------------------------------
		int m_sizeY;
		//----------------------------------------------------------------------------------
		/// \brief Used to check if view is focused
		//----------------------------------------------------------------------------------
		bool m_focussed;
		//----------------------------------------------------------------------------------
		/// \brief Used to check if pressed
		//----------------------------------------------------------------------------------
		bool m_pressed;
		//----------------------------------------------------------------------------------
		/// \brief Used to check if hovering
		//----------------------------------------------------------------------------------
		bool m_hover;
		//----------------------------------------------------------------------------------
		/// \brief Used to check if toggle is active
		//----------------------------------------------------------------------------------
		bool m_useToggleActive;
		//----------------------------------------------------------------------------------
		/// \brief Used to check toggle active state
		//----------------------------------------------------------------------------------
		bool m_toggleActiveState;
		//----------------------------------------------------------------------------------
		/// \brief General drawable
		//----------------------------------------------------------------------------------
		Drawable *m_generalDrawable;
		//----------------------------------------------------------------------------------
		/// \brief Statelist drawable
		//----------------------------------------------------------------------------------
		StateListDrawable *m_stateListDrawable;
		//----------------------------------------------------------------------------------
		/// \brief Content general drawable
		//----------------------------------------------------------------------------------
		Drawable *m_contentGenDrawable;
		//----------------------------------------------------------------------------------
		/// \brief Content statelist drawable
		//----------------------------------------------------------------------------------
		StateListDrawable *m_contentStateListDrawable;
		//----------------------------------------------------------------------------------
		// -1 = unset, 0 = false, 1 = true
		//----------------------------------------------------------------------------------
		/// \brief Used to set content scale up
		//----------------------------------------------------------------------------------
		int m_setContentScaleUp;
		//----------------------------------------------------------------------------------
		/// \brief Used to set content aspect ratio
		//----------------------------------------------------------------------------------
		int m_setContentAspectRatio;
		//----------------------------------------------------------------------------------
		/// \brief Keeps track of whether the drawables were loaded from a theme or not
		/// Mainly for use when deflating to xml
		//----------------------------------------------------------------------------------
		bool m_genDrawableFromTheme;
		//----------------------------------------------------------------------------------
		/// \brief To check if drawable is loaded from theme
		//----------------------------------------------------------------------------------
		bool m_contentDrawableFromTheme;
		//----------------------------------------------------------------------------------
		/// \brief Returns true if a mouse coord is on the button
		/// \param [in] _mouseX
		/// \param [in] _mouseY
		//----------------------------------------------------------------------------------
		bool MouseHit( int _mouseX, int _mouseY );
		/// \brief Returns true if a mouse coord is on the button
		/// \param [in] _mouseX
		/// \param [in] _mouseY
		//----------------------------------------------------------------------------------
		bool MouseOver( int _mouseX, int _mouseY );
		//----------------------------------------------------------------------------------
		/// \brief Sets the state of the StateListDrawable to match that of the button
		//----------------------------------------------------------------------------------
		void SetStateDrawable();
		//----------------------------------------------------------------------------------
		/// \brief Listener for click events
		//----------------------------------------------------------------------------------
		ViewEventListener *m_clickListener;
		//----------------------------------------------------------------------------------
		/// \brief Listener name
		//----------------------------------------------------------------------------------
		std::string m_clickListenerName;
		//----------------------------------------------------------------------------------
		/// \brief Called when changed to active
		//----------------------------------------------------------------------------------
		virtual void OnChangeActive() { SetStateDrawable(); }
		//----------------------------------------------------------------------------------
		/// \brief Must return whether the event is absorbed (i.e. whether it should be passed on to other events)
		//----------------------------------------------------------------------------------
		bool OnHoverEnter();
		/// \brief Must return whether the event is absorbed (i.e. whether it should be passed on to other events)
		//----------------------------------------------------------------------------------
		bool OnHoverOver();
		//----------------------------------------------------------------------------------
		// \brief Must return whether the event is absorbed (i.e. whether it should be passed on to other events)
		//----------------------------------------------------------------------------------
		bool OnHoverExit();
		//----------------------------------------------------------------------------------
		/// \brief Image name
		//----------------------------------------------------------------------------------
		std::string m_imageName;
		//----------------------------------------------------------------------------------
		
		// TEMPORARY CODE FOR LIMITED EYE-GAZE SUPPORT

		//----------------------------------------------------------------------------------
		/// \brief Draw the gaze indicator
		//----------------------------------------------------------------------------------
		void DrawGazeIndicator();
		//----------------------------------------------------------------------------------
		/// \brief Create vbos and bind to vao for gaze indicator
		//----------------------------------------------------------------------------------
		void BuildGazeIndicatorVBOs();
		//----------------------------------------------------------------------------------
		/// \brief Our gaze indicator VAO
		//----------------------------------------------------------------------------------
		GLuint m_gazeIndicatorVAO;
		//----------------------------------------------------------------------------------
		/// \brief Used to check if eye gaze is being used
		//----------------------------------------------------------------------------------
		bool m_usingEyeGaze;
		//----------------------------------------------------------------------------------
		/// \brief Flag used to check if gaze indicator is active
		//----------------------------------------------------------------------------------
		bool m_hasGaze;
		//----------------------------------------------------------------------------------
		/// \brief Gaze dwell timer
		//----------------------------------------------------------------------------------
		float m_gazeDwellTimer;
		//----------------------------------------------------------------------------------
		/// \brief Gaze dwell to select time
		//----------------------------------------------------------------------------------
		float m_gazeDwellToSelectTime;
		//----------------------------------------------------------------------------------
		/// \brief Gaze maximum wander time
		//----------------------------------------------------------------------------------
		float m_gazeMaxWanderTime;
		//----------------------------------------------------------------------------------
		/// \brief Gaze wander timer
		//----------------------------------------------------------------------------------
		float m_gazeWanderTimer;
		//----------------------------------------------------------------------------------
		/// \brief Used to check if gaze is pressing
		//----------------------------------------------------------------------------------
		bool m_gazeIsPressing;
		//----------------------------------------------------------------------------------
		/// \brief Gaze press time
		//----------------------------------------------------------------------------------
		float m_gazePressTime;
		//----------------------------------------------------------------------------------
		/// \brief Gaze dwell circle size
		//----------------------------------------------------------------------------------
		float m_gazeDwellCircleSize;
		//----------------------------------------------------------------------------------
		/// \brief Gaze dwell circle colour
		//----------------------------------------------------------------------------------
		std::vector< float > m_gazeColour;
		//----------------------------------------------------------------------------------
		/// \brief Used to check if gaze indicator needs to be repeated
		//----------------------------------------------------------------------------------
		bool m_gazeRepeat;
		//----------------------------------------------------------------------------------
		/// \brief The button enables / disables eye-gaze
		//----------------------------------------------------------------------------------
		bool m_gazeIsRestButton;		
		//----------------------------------------------------------------------------------
		/// \brief The state to enable/disable eye-gaze
		//----------------------------------------------------------------------------------
		bool m_gazeRestButtonToggle;	
		//----------------------------------------------------------------------------------
		/// \brief The rest button needs to issue a toggle to the gaze enable/disable state (it does this during Update())
		//----------------------------------------------------------------------------------
		bool m_gazeRestIssueEvent;	
		//----------------------------------------------------------------------------------
		/// \brief Used to check if parameter loaded from theme
		//----------------------------------------------------------------------------------
		bool m_gazeRepeatFromTheme;
		//----------------------------------------------------------------------------------
		/// \brief Shader for drawing gaze indicator
		//----------------------------------------------------------------------------------
		Utility::GPUProgram *m_gazeIndicatorShader;
		//----------------------------------------------------------------------------------
		/// \brief Load matrices to shader
		/// \param [in] _shaderID
		/// \param [in] _proj
		/// \param [in] _mv
		//----------------------------------------------------------------------------------
		void LoadMatricesToShader( GLuint _shaderID, cml::matrix44f_c _proj, cml::matrix44f_c _mv );
		//----------------------------------------------------------------------------------
		/// \brief Number of vertices to draw gaze indicator
		//----------------------------------------------------------------------------------
		unsigned int m_nVertices;
		//----------------------------------------------------------------------------------

		// AUDIO EVENTS

		//----------------------------------------------------------------------------------
		/// \brief An audio event to start playing when the button is pressed
		//----------------------------------------------------------------------------------
		AudioClip *m_audio_pressStart;
		//----------------------------------------------------------------------------------
		/// \brief Whether to stop playing when the button is released
		//----------------------------------------------------------------------------------
		bool m_audio_releaseStop;
		//----------------------------------------------------------------------------------
		/// \brief Audio event to play when hovering over button
		//----------------------------------------------------------------------------------
		AudioClip *m_audio_hoverEnter;
		//----------------------------------------------------------------------------------
		
	};
}

#endif
