
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef __SHIVA_GUISYSTEM_VOLVIEW__
#define __SHIVA_GUISYSTEM_VOLVIEW__
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#include <cml/cml.h>

//////////////////////////////////////////////////////////////////////////
#include "GUI/Views/View.h"
#include "VolumeRenderer/GLSLRenderer.h"
#include "Totem\TotemController.h"

//////////////////////////////////////////////////////////////////////////

/// This is for displaying 3D objects
class VolView : public ShivaGUI::View
{
public:
	static ShivaGUI::View* Factory() {return new VolView();}

	VolView();
	virtual ~VolView();

	/// Gives the size of the View
	/// If this View has children, it is expected to work out the size and location of these and call Layout() on them too
	virtual void Layout(int left, int top, int right, int bottom, int windowWidth, int windowHeight);

	/// For setting the View's attributes from xml
	virtual void Inflate(TiXmlElement*,ShivaGUI::ResourceManager*, std::string themePrefix = "", bool rootNode = false);

	virtual TiXmlElement* Deflate(ShivaGUI::ResourceManager *resources);

	virtual void Update(float deltaTs, ShivaGUI::GUIController *guiController);

	virtual void Draw();

	/// Themes allow settings to be set for all Views of a given type
	/// The string returned from this function is the string that identifies the type in the theme xml file
	/// It is literally prefixed to normal inflation attributes
	virtual std::string GetThemePrefix() {return "VolView_";}

	// This View does not accept focus

	/// Input event given to View, expected to filter down hierarchy
	/// \return false if event is not absorbed (e.g. did not hit button etc)
	virtual bool HandleEvent(ShivaGUI::InternalEvent*);

	/// Will probably need to rebuild the tree after changing this
	void SetCameraAngle( float angleDeg );

	/// If true, short selection events will be used to select parts of the totem
	/// Default is false
	void AllowObjectClickSelection(bool value) {_allowClickSelect = value;}

	void GetSelectionVector(float &originX,float &originY,float &originZ, float &dirX,float &dirY,float &dirZ);

	void ResetWorldRotation();

	void AddWorldRotationOffsetDegs(float rotX, float rotY, float rotZ);
	void AddWorldRotationOffsetRads(float rotX, float rotY, float rotZ);
/*
	void SetBlendParam(float value) {_blendValue=value;}

	void SetRotationSpeed(float value) {_rotationSpeed=value;}


	void AddRotationOffsetDegs(float rotX, float rotY, float rotZ);
	void AddRotationOffsetRads(float rotX, float rotY, float rotZ);

	void SetColour(float R, float G, float B) {_colourR=R;_colourG=G;_colourB=B;}



	void SetPauseRotation( bool value ) {_pauseRotation = value;}

	void SetQuality( float value );
*/

	/// Rebuilds the whole tree, sends it to the renderer
	void RefreshTree();
	/// For when you're not adding/removing nodes from the tree but just changing parameter valuse
	/// This checks the renderer to see if parameters are still available for issue
	/// If not, it assumes that the tree doesn't have enough parameters so it rebuilds the whole tree
	void RefreshTreeParams();

	/// Values are expected between 0 and 1
	void SetObjectColour(float R, float G, float B);


	// Overlays
		void ShowCrosshairs(bool show) {_showCrosshairs = show;}
		/// When crosshairs are enabled they set the selection vector as well
		/// You can thus retrieve the crosshair location with GetSelectionVector()
		bool GetCrosshairs(float &x, float &y) {x=_crosshairX;y=_crosshairY;return _showCrosshairs;}

		/// Moves the crosshairs by a small amount
		/// Range between 0 and 1
		void NudgeCrosshairs(float x, float y);

		void SetCrosshairSize(float value) {_targetSize = value;}

protected:

	bool _mousing;
	int _mouseLastX, _mouseLastY;
	int _mouseRefX, _mouseRefY;
	float _rotationX, _rotationZ;

	/// Used to determine if an event is a click select or a drag rotate
	float _mousingTimer, _clickSelectTimeThreshold;

	/// Default is false
	bool _allowClickSelect;

	float _selectVecOrigX,_selectVecOrigY,_selectVecOrigZ, _selectVecDirX,_selectVecDirY,_selectVecDirZ;

	/// Bounds for the View's place in the GUI System
	int _boundsLeft, _boundsTop, _boundsRight, _boundsBottom, _windowWidth, _windowHeight;

	GLSLRenderer *_renderer;
	VolumeTree::Tree *_mainTree;
	VolumeTree::CachingPolicy *_cachePolicy;
	Totem::Controller *_totemController;

	void CalcStepsize();

	// Overlays
		/// Crosshairs with a centre dot
		bool _showCrosshairs;
		/// Dimensions are proportion of window size
		float _crosshairX, _crosshairY;

		float _targetSize;

};

//////////////////////////////////////////////////////////////////////////
#endif
