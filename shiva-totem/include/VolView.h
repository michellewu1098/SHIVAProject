///-----------------------------------------------------------------------------------------------
/// \file VolView.h
/// \brief This is for displaying 3D objects
/// \author Leigh McLoughlin
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_GUISYSTEM_VOLVIEW__
#define __SHIVA_GUISYSTEM_VOLVIEW__

#include <cml/cml.h>
#include <GL/GLee.h>
#include <GL/glu.h>
#include <cmath>
#include <iostream>

#include "GUI/Views/View.h"
#include "VolumeRenderer/GLSLRenderer.h"
#include "Totem\TotemController.h"

class VolView : public ShivaGUI::View
{
public:

	//----------------------------------------------------------------------------------
	static ShivaGUI::View* Factory() { return new VolView(); }
	//----------------------------------------------------------------------------------
	/// \brief Default ctor
	//----------------------------------------------------------------------------------
	VolView();
	//----------------------------------------------------------------------------------
	/// \brief Dtor
	//----------------------------------------------------------------------------------
	virtual ~VolView();
	//----------------------------------------------------------------------------------
	/// \brief Gives the size of the View
	/// If this View has children, it is expected to work out the size and location of these and call Layout() on them too
	/// \param [in] left
	/// \param [in] top
	/// \param [in] right
	/// \param [in] bottom
	/// \param [in] windowWidth
	/// \param [in] windowHeight
	//----------------------------------------------------------------------------------
	virtual void Layout( int left, int top, int right, int bottom, int windowWidth, int windowHeight );
	//----------------------------------------------------------------------------------
	/// \brief For setting the View's attributes from xml
	/// \param [in] xmlElement
	/// \param [in] resources
	/// \param [in] themePrefix
	/// \param [in] rootNode
	//----------------------------------------------------------------------------------
	virtual void Inflate( TiXmlElement*, ShivaGUI::ResourceManager*, std::string themePrefix = "", bool rootNode = false );
	//----------------------------------------------------------------------------------
	/// \brief
	/// \param [in] resources
	//----------------------------------------------------------------------------------
	virtual TiXmlElement* Deflate( ShivaGUI::ResourceManager *resources );
	//----------------------------------------------------------------------------------
	/// \brief Update method
	/// \param [in] deltaTs
	/// \param [in] guiController
	//----------------------------------------------------------------------------------
	virtual void Update( float deltaTs, ShivaGUI::GUIController *guiController );
	//----------------------------------------------------------------------------------
	/// \brief Draw method
	//----------------------------------------------------------------------------------
	virtual void Draw();
	//----------------------------------------------------------------------------------
	/// \brief Themes allow settings to be set for all Views of a given type
	/// The string returned from this function is the string that identifies the type in the theme xml file
	/// It is literally prefixed to normal inflation attributes
	/// \return "VolView_"
	//----------------------------------------------------------------------------------
	virtual std::string GetThemePrefix() { return "VolView_"; }
	//----------------------------------------------------------------------------------
	// This View does not accept focus
	//----------------------------------------------------------------------------------
	/// \brief Input event given to View, expected to filter down hierarchy
	/// \return false if event is not absorbed (e.g. did not hit button etc)
	//----------------------------------------------------------------------------------
	virtual bool HandleEvent( ShivaGUI::InternalEvent* );
	//----------------------------------------------------------------------------------
	/// \brief Set camera angle. Will probably need to rebuild the tree after changing this
	/// \param [in] angleDeg
	//----------------------------------------------------------------------------------
	void SetCameraAngle( float angleDeg );
	//----------------------------------------------------------------------------------
	/// \brief If true, short selection events will be used to select parts of the totem
	/// \param [in] value Default is false
	//----------------------------------------------------------------------------------
	void AllowObjectClickSelection( bool value ) { _allowClickSelect = value; }
	//----------------------------------------------------------------------------------
	/// \brief Get selection vector
	/// \param [in] originX
	/// \param [in] originY
	/// \param [in] originZ
	/// \param [in] dirX
	/// \param [in] dirY
	/// \param [in] dirZ
	//----------------------------------------------------------------------------------
	void GetSelectionVector( float &originX, float &originY, float &originZ, float &dirX, float &dirY, float &dirZ );
	//----------------------------------------------------------------------------------
	/// \brief Reset world rotation
	//----------------------------------------------------------------------------------
	void ResetWorldRotation();
	//----------------------------------------------------------------------------------
	/// \brief Add world rotation offset in degress
	/// \param [in] rotX
	/// \param [in] rotY
	/// \param [in] rotZ
	//----------------------------------------------------------------------------------
	void AddWorldRotationOffsetDegs( float rotX, float rotY, float rotZ );
	//----------------------------------------------------------------------------------
	/// \brief Add world rotation offset in radians
	/// \param [in] rotX
	/// \param [in] rotY
	/// \param [in] rotZ
	//----------------------------------------------------------------------------------
	void AddWorldRotationOffsetRads( float rotX, float rotY, float rotZ );
	//----------------------------------------------------------------------------------
/*
	void SetBlendParam(float value) {_blendValue=value;}
	void SetRotationSpeed(float value) {_rotationSpeed=value;}
	void AddRotationOffsetDegs(float rotX, float rotY, float rotZ);
	void AddRotationOffsetRads(float rotX, float rotY, float rotZ);
	void SetColour(float R, float G, float B) {_colourR=R;_colourG=G;_colourB=B;}
	void SetPauseRotation( bool value ) {_pauseRotation = value;}
	void SetQuality( float value );
*/
	//----------------------------------------------------------------------------------
	/// \brief Rebuilds the whole tree, sends it to the renderer
	//----------------------------------------------------------------------------------
	void RefreshTree();
	//----------------------------------------------------------------------------------
	/// \brief For when you're not adding/removing nodes from the tree but just changing parameter valuse
	/// This checks the renderer to see if parameters are still available for issue
	/// If not, it assumes that the tree doesn't have enough parameters so it rebuilds the whole tree
	//----------------------------------------------------------------------------------
	void RefreshTreeParams();
	//----------------------------------------------------------------------------------
	/// \brief Set object colour: values are expected between 0 and 1
	/// \param [in] R red
	/// \param [in] G green
	/// \param [in] B blue
	//----------------------------------------------------------------------------------
	void SetObjectColour( float R, float G, float B );
	//----------------------------------------------------------------------------------
	// Overlays
	//----------------------------------------------------------------------------------
	/// \brief Show overlays
	/// \param [in] show
	//----------------------------------------------------------------------------------
	void ShowCrosshairs( bool show ) { _showCrosshairs = show; }
	//----------------------------------------------------------------------------------
	/// \brief When crosshairs are enabled they set the selection vector as well
	/// You can thus retrieve the crosshair location with GetSelectionVector()
	/// \param [in] x
	/// \param [in] y
	/// \return _showCrosshairs
	//----------------------------------------------------------------------------------	
	bool GetCrosshairs( float &x, float &y ) { x = _crosshairX; y = _crosshairY; return _showCrosshairs; }
	//----------------------------------------------------------------------------------
	/// \brief Moves the crosshairs by a small amount. Range between 0 and 1
	/// \param [in] x
	/// \param [in] y
	//----------------------------------------------------------------------------------
	void NudgeCrosshairs( float x, float y );
	//----------------------------------------------------------------------------------
	/// \brief Set overlay size
	/// \param [in] value
	//----------------------------------------------------------------------------------
	void SetCrosshairSize( float value ) { _targetSize = value; }
	//----------------------------------------------------------------------------------

protected:

	//----------------------------------------------------------------------------------
	bool _mousing;
	//----------------------------------------------------------------------------------
	int _mouseLastX;
	//----------------------------------------------------------------------------------
	int _mouseLastY;
	//----------------------------------------------------------------------------------
	int _mouseRefX;
	//----------------------------------------------------------------------------------
	int _mouseRefY;
	//----------------------------------------------------------------------------------
	float _rotationX;
	//----------------------------------------------------------------------------------
	float _rotationZ;
	//----------------------------------------------------------------------------------
	/// \brief Used to determine if an event is a click select or a drag rotate
	//----------------------------------------------------------------------------------
	float _mousingTimer; 
	//----------------------------------------------------------------------------------
	float _clickSelectTimeThreshold;
	//----------------------------------------------------------------------------------
	/// \brief Flag to control click select. Default is false
	//----------------------------------------------------------------------------------
	bool _allowClickSelect;
	//----------------------------------------------------------------------------------
	float _selectVecOrigX;
	//----------------------------------------------------------------------------------
	float _selectVecOrigY;
	//----------------------------------------------------------------------------------
	float _selectVecOrigZ;
	//----------------------------------------------------------------------------------
	float _selectVecDirX;
	//----------------------------------------------------------------------------------
	float _selectVecDirY;
	//----------------------------------------------------------------------------------
	float _selectVecDirZ;
	//----------------------------------------------------------------------------------
	// Bounds for the View's place in the GUI System
	//----------------------------------------------------------------------------------
	/// \brief Left bounds
	//----------------------------------------------------------------------------------
	int _boundsLeft;
	//----------------------------------------------------------------------------------
	/// \brief Top bounds
	//----------------------------------------------------------------------------------
	int _boundsTop;
	//----------------------------------------------------------------------------------
	/// \brief Right bounds
	//----------------------------------------------------------------------------------
	int _boundsRight;
	//----------------------------------------------------------------------------------
	/// \brief Bottom bounds
	//----------------------------------------------------------------------------------
	int _boundsBottom;
	//----------------------------------------------------------------------------------
	/// \brief Window width
	//----------------------------------------------------------------------------------
	int _windowWidth;
	//----------------------------------------------------------------------------------
	/// \brief Window height
	//----------------------------------------------------------------------------------
	int _windowHeight;
	//----------------------------------------------------------------------------------
	/// \brief Renderer
	//----------------------------------------------------------------------------------
	GLSLRenderer *_renderer;
	//----------------------------------------------------------------------------------
	/// \brief Our main tree
	//----------------------------------------------------------------------------------
	VolumeTree::Tree *_mainTree;
	//----------------------------------------------------------------------------------
	/// \brief Cache policy
	//----------------------------------------------------------------------------------
	VolumeTree::CachingPolicy *_cachePolicy;
	//----------------------------------------------------------------------------------
	/// \brief Our totem controller
	//----------------------------------------------------------------------------------
	Totem::Controller *_totemController;
	//----------------------------------------------------------------------------------
	/// \brief Calculate step size for positioning objects on pole
	//----------------------------------------------------------------------------------
	void CalcStepsize();
	//----------------------------------------------------------------------------------
	// Overlays
	//----------------------------------------------------------------------------------
	/// \brief Crosshairs with a centre dot
	//----------------------------------------------------------------------------------
	bool _showCrosshairs;
	//----------------------------------------------------------------------------------
	// Dimensions are proportion of window size
	//----------------------------------------------------------------------------------
	float _crosshairX;
	//----------------------------------------------------------------------------------
	float _crosshairY;
	//----------------------------------------------------------------------------------
	float _targetSize;
	//----------------------------------------------------------------------------------

};

#endif
