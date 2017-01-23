#ifndef __SHIVA_GUISYSTEM_IMAGE_BUTTON__
#define __SHIVA_GUISYSTEM_IMAGE_BUTTON__

#include "GUI/Views/View.h"
#include "GUI/Views/ViewEventListener.h"
#include "GUI/Drawables/StateListDrawable.h"
#include "Audio/AudioClip.h"

namespace ShivaGUI
{

	/// This is a very simple test widget
	/// It's just a set of images for a set of states
	/// It can be selected and pressed
	/// If this is given a StateListDrawable, it will use its states

	class ImageButton : public View
	{
	public:

		//----------------------------------------------------------------------------------
		/// \brief Default ctor
		//----------------------------------------------------------------------------------
		ImageButton();
		//----------------------------------------------------------------------------------
		//ImageButton(ResourceSystem::BitmapDrawable *drawable);
		//ImageButton(ResourceSystem::StateListDrawable *drawable);
		//----------------------------------------------------------------------------------
		/// \brief Dtor
		//----------------------------------------------------------------------------------
		virtual ~ImageButton();
		//----------------------------------------------------------------------------------
		/// Gives the size of the View
		/// If this View has children, it is expected to work out the size and location of these and call Layout() on them too
		virtual void Layout( int left, int top, int right, int bottom, int windowWidth, int windowHeight );
		//----------------------------------------------------------------------------------
		virtual void Update( float deltaTs, GUIController *guiController );
		//----------------------------------------------------------------------------------
		/// \brief Draw method
		//----------------------------------------------------------------------------------
		virtual void Draw();
		//----------------------------------------------------------------------------------
		/// \brief Themes allow settings to be set for all Views of a given type
		/// The string returned from this function is the string that identifies the type in the theme xml file
		/// It is literally prefixed to normal inflation attributes
		/// \return "ImageButton_"
		//----------------------------------------------------------------------------------
		virtual std::string GetThemePrefix() { return "ImageButton_"; }
		//----------------------------------------------------------------------------------
		/// \brief For setting the View's attributes from xml
		//----------------------------------------------------------------------------------
		virtual void Inflate( TiXmlElement*, ResourceManager*, std::string themePrefix = "", bool rootNode = false );
		//----------------------------------------------------------------------------------
		/// \brief For saving the View's attributes to xml
		/// \param [in] resources
		/// \note This must be hierarchical and the element must include all child elements
		//----------------------------------------------------------------------------------
		virtual TiXmlElement* Deflate( ResourceManager *resources );
		//----------------------------------------------------------------------------------
		/// \brief For setting the focus of this View
		/// If focussed, it is expected that the view will show this visually
		/// and then allow other forms of input to be received, e.g. 'enter' button pressed etc
		//----------------------------------------------------------------------------------
		virtual void SetFocus( bool );
		//----------------------------------------------------------------------------------
		virtual void SetSelect( bool );
		//----------------------------------------------------------------------------------
		/// \brief Handle internal event
		/// \param [in] 
		//----------------------------------------------------------------------------------
		virtual bool HandleEvent( InternalEvent* );
		//----------------------------------------------------------------------------------
		/// \brief Set on click listener
		/// \param [in] value
		//----------------------------------------------------------------------------------
		void SetOnClickListener( ViewEventListener *value ) { _clickListener = value; }
		//----------------------------------------------------------------------------------
		/// \brief Set content drawable
		/// \param [in] value
		//----------------------------------------------------------------------------------
		void SetContent( Drawable *value );
		//----------------------------------------------------------------------------------
		/// \brief Get content drawable
		//----------------------------------------------------------------------------------
		Drawable* GetContent() { return _contentStateListDrawable == NULL ? _contentGenDrawable : _contentStateListDrawable; }
		//----------------------------------------------------------------------------------
		/// Requested wrap pixel dimensions for layout
		/// \note These are used when SetLayoutParams() is called, which will be after the View has been inflated
		virtual int GetWrapWidth();
		//----------------------------------------------------------------------------------
		virtual int GetWrapHeight();
		//----------------------------------------------------------------------------------


	protected:

		//----------------------------------------------------------------------------------
		int _centreX;
		//----------------------------------------------------------------------------------
		int _centreY;
		//----------------------------------------------------------------------------------
		int _sizeX, _sizeY;
		//----------------------------------------------------------------------------------
		bool _focussed;
		//----------------------------------------------------------------------------------
		bool _pressed;
		//----------------------------------------------------------------------------------
		bool _hover;
		//----------------------------------------------------------------------------------
		//bool _active;
		bool _useToggleActive, _toggleActiveState;
		//----------------------------------------------------------------------------------
		Drawable *_generalDrawable;
		//----------------------------------------------------------------------------------
		StateListDrawable *_stateListDrawable;
		//----------------------------------------------------------------------------------
		Drawable *_contentGenDrawable;
		//----------------------------------------------------------------------------------
		StateListDrawable *_contentStateListDrawable;
		//----------------------------------------------------------------------------------
		/// -1 = unset, 0 = false, 1 = true
		int _setContentScaleUp, _setContentAspectRatio;

		/// Keeps track of whether the drawables were loaded from a theme or not
		/// Mainly for use when deflating to xml
		bool _genDrawableFromTheme, _contentDrawableFromTheme;

		/// Returns true if a mouse coord is on the button
		bool MouseHit(int mouseX, int mouseY);

		/// Sets the state of the StateListDrawable to match that of the button
		void SetStateDrawable();

		/// Listener for click events
		ViewEventListener *_clickListener;

		std::string _clickListenerName;


		virtual void OnChangeActive() { SetStateDrawable(); }

		/// Must return whether the event is absorbed (i.e. whether it should be passed on to other events)
		//----------------------------------------------------------------------------------
		bool OnHoverEnter();
		//----------------------------------------------------------------------------------
		bool OnHoverExit();
		//----------------------------------------------------------------------------------


		// Temporary code for limited eye-gaze support
			void DrawGazeIndicator();
			bool _usingEyeGaze;
			bool _hasGaze;
			float _gazeDwellTimer;
			float _gazeDwellToSelectTime;
			float _gazeMaxWanderTime;
			float _gazeWanderTimer;
			bool _gazeIsPressing;
			float _gazePressTime;
			float _gazeDwellCircleSize;
			bool _gazeRepeat;
			bool _gazeIsRestButton;		// The button enables / disables eye-gaze
			bool _gazeRestButtonToggle;	// The state to enable/disable eye-gaze
			bool _gazeRestIssueEvent;	// The rest button needs to issue a toggle to the gaze enable/disable state (it does this during Update())

			bool _gazeRepeatFromTheme;

		//----------------------------------------------------------------------------------
		// Audio Events
		//----------------------------------------------------------------------------------
		/// \brief An audio event to start playing when the button is pressed
		//----------------------------------------------------------------------------------
		AudioClip *_audio_pressStart;
		//----------------------------------------------------------------------------------
		/// \brief Whether to stop playing when the button is released
		//----------------------------------------------------------------------------------
		bool _audio_releaseStop;
		//----------------------------------------------------------------------------------
		AudioClip *_audio_hoverEnter;
		//----------------------------------------------------------------------------------

		std::string _imageName;

	};
}

#endif
