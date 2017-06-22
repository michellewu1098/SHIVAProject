///-----------------------------------------------------------------------------------------------
/// \file VolView.h
/// \brief This is for displaying 3D objects
/// \author Leigh McLoughlin, Michelle Wu
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_GUISYSTEM_VOLVIEW__
#define __SHIVA_GUISYSTEM_VOLVIEW__

#include <cmath>

#include "GUI/Views/View.h"
#include "VolumeRenderer/GLSLRenderer.h"
#include "Totem/TotemController.h"
#include "Utility/GPUProgram.h"
#include "CommandManager.h"

class VolView : public ShivaGUI::View
{
public:

	//----------------------------------------------------------------------------------
	/// \brief Returns a volume view instance
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
	/// \param [in] _left
	/// \param [in] _top
	/// \param [in] _right
	/// \param [in] _bottom
	/// \param [in] _windowWidth
	/// \param [in] _windowHeight
	//----------------------------------------------------------------------------------
	virtual void Layout( int _left, int _top, int _right, int _bottom, int _windowWidth, int _windowHeight );
	//----------------------------------------------------------------------------------
	/// \brief For setting the View's attributes from xml
	/// \param [in] _xmlElement
	/// \param [in] _resources
	/// \param [in] _themePrefix
	/// \param [in] _rootNode
	//----------------------------------------------------------------------------------
	virtual void Inflate( TiXmlElement* _xmlElement, ShivaGUI::ResourceManager* _resources, std::string _themePrefix = "", bool _rootNode = false );
	//----------------------------------------------------------------------------------
	/// \brief
	/// \param [in] _resources
	//----------------------------------------------------------------------------------
	virtual TiXmlElement* Deflate( ShivaGUI::ResourceManager *_resources );
	//----------------------------------------------------------------------------------
	/// \brief Update method
	/// \param [in] _deltaTs
	/// \param [in] _guiController
	//----------------------------------------------------------------------------------
	virtual void Update( float _deltaTs, ShivaGUI::GUIController *_guiController );
	//----------------------------------------------------------------------------------
	/// \brief Draw method
	//----------------------------------------------------------------------------------
	virtual void Draw();
	//----------------------------------------------------------------------------------
	virtual void Draw( unsigned int _context );
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
	/// \param [in] _eventIn
	/// \return false if event is not absorbed (e.g. did not hit button etc)
	//----------------------------------------------------------------------------------
	virtual bool HandleEvent( ShivaGUI::InternalEvent* _eventIn );
	//----------------------------------------------------------------------------------
	/// \brief Set camera angle. Will probably need to rebuild the tree after changing this
	/// \param [in] _angleDeg
	//----------------------------------------------------------------------------------
	void SetCameraAngle( float _angleDeg );
	//----------------------------------------------------------------------------------
	/// \brief If true, short selection events will be used to select parts of the totem
	/// \param [in] _value Default is false
	//----------------------------------------------------------------------------------
	void AllowObjectClickSelection( bool _value ) { m_allowClickSelect = _value; }
	//----------------------------------------------------------------------------------
	/// \brief Get selection vector
	/// \param [in] _originX
	/// \param [in] _originY
	/// \param [in] _originZ
	/// \param [in] _dirX
	/// \param [in] _dirY
	/// \param [in] _dirZ
	//----------------------------------------------------------------------------------
	void GetSelectionVector( float &_originX, float &_originY, float &_originZ, float &_dirX, float &_dirY, float &_dirZ );
	//----------------------------------------------------------------------------------
	/// \brief Reset world rotation
	//----------------------------------------------------------------------------------
	void ResetWorldRotation();
	//----------------------------------------------------------------------------------
	/// \brief Add world rotation offset in degress
	/// \param [in] _rotX
	/// \param [in] _rotY
	/// \param [in] _rotZ
	//----------------------------------------------------------------------------------
	void AddWorldRotationOffsetDegs( float _rotX, float _rotY, float _rotZ );
	//----------------------------------------------------------------------------------
	/// \brief Add world rotation offset in radians
	/// \param [in] _rotX
	/// \param [in] _rotY
	/// \param [in] _rotZ
	//----------------------------------------------------------------------------------
	void AddWorldRotationOffsetRads( float _rotX, float _rotY, float _rotZ );
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
	/// \param [in] _r red
	/// \param [in] _g green
	/// \param [in] _b blue
	//----------------------------------------------------------------------------------
	void SetObjectColour( float _r, float _g, float _b );
	//----------------------------------------------------------------------------------
	// Overlays
	//----------------------------------------------------------------------------------
	/// \brief Show overlays
	/// \param [in] _show
	//----------------------------------------------------------------------------------
	void ShowCrosshairs( bool _show ) { m_showCrosshairs = _show; }
	//----------------------------------------------------------------------------------
	/// \brief When crosshairs are enabled they set the selection vector as well
	/// You can thus retrieve the crosshair location with GetSelectionVector()
	/// \param [in] _x
	/// \param [in] _y
	/// \return m_showCrosshairs
	//----------------------------------------------------------------------------------	
	bool GetCrosshairs( float &_x, float &_y ) { _x = m_crosshairX; _y = m_crosshairY; return m_showCrosshairs; }
	//----------------------------------------------------------------------------------
	/// \brief Moves the crosshairs by a small amount. Range between 0 and 1
	/// \param [in] _x
	/// \param [in] _y
	//----------------------------------------------------------------------------------
	void NudgeCrosshairs( float _x, float _y );
	//----------------------------------------------------------------------------------
	/// \brief Set overlay size
	/// \param [in] _value
	//----------------------------------------------------------------------------------
	void SetCrosshairSize( float _value ) { m_targetSize = _value; }
	//----------------------------------------------------------------------------------
	/// \brief Create vbo and vao for crosshair
	//----------------------------------------------------------------------------------
	void BuildCrosshairVBOs();
	//----------------------------------------------------------------------------------
	/// \brief Load matrices to shader
	/// \param [in] _shaderID Shader's id
	/// \param [in] _proj Projection matrix
	/// \param [in] _mv Modelview matrix 
	//----------------------------------------------------------------------------------
	void LoadMatricesToShader( GLuint _shaderID, cml::matrix44f_c _proj, cml::matrix44f_c _mv );
	//----------------------------------------------------------------------------------
	/// \brief Create vbo and vao for crosshair circle
	//----------------------------------------------------------------------------------
	void BuildCrosshairCircleVBOs();
	//----------------------------------------------------------------------------------

protected:

	//----------------------------------------------------------------------------------
	/// \brief Flag used for checking whether mouse is used
	//----------------------------------------------------------------------------------
	bool m_mousing;
	//----------------------------------------------------------------------------------
	/// \brief Mouse last pos (x-coord)
	//----------------------------------------------------------------------------------
	int m_mouseLastX;
	//----------------------------------------------------------------------------------
	/// \brief Mouse last pos (y-coord)
	//----------------------------------------------------------------------------------
	int m_mouseLastY;
	//----------------------------------------------------------------------------------
	/// \brief Mouse reference (x-coord)
	//----------------------------------------------------------------------------------
	int m_mouseRefX;
	//----------------------------------------------------------------------------------
	/// \brief Mouse reference (y-coord)
	//----------------------------------------------------------------------------------
	int m_mouseRefY;
	//----------------------------------------------------------------------------------
	/// \brief Rotation about x
	//----------------------------------------------------------------------------------
	float m_rotationX;
	//----------------------------------------------------------------------------------
	/// \brief Rotation about z
	//----------------------------------------------------------------------------------
	float m_rotationZ;
	//----------------------------------------------------------------------------------
	/// \brief Used to determine if an event is a click select or a drag rotate
	//----------------------------------------------------------------------------------
	float m_mousingTimer; 
	//----------------------------------------------------------------------------------
	/// \brief Time threshold
	//----------------------------------------------------------------------------------
	float m_clickSelectTimeThreshold;
	//----------------------------------------------------------------------------------
	/// \brief Flag to control click select. Default is false
	//----------------------------------------------------------------------------------
	bool m_allowClickSelect;
	//----------------------------------------------------------------------------------
	/// \brief Select vector origin x-coord
	//----------------------------------------------------------------------------------
	float m_selectVecOrigX;
	//----------------------------------------------------------------------------------
	/// \brief Select vector origin y-coord
	//----------------------------------------------------------------------------------
	float m_selectVecOrigY;
	//----------------------------------------------------------------------------------
	/// \brief Select vector origin z-coord
	//----------------------------------------------------------------------------------
	float m_selectVecOrigZ;
	//----------------------------------------------------------------------------------
	/// \brief Select vector direction x-coord
	//----------------------------------------------------------------------------------
	float m_selectVecDirX;
	//----------------------------------------------------------------------------------
	/// \brief Select vector direction y-coord
	//----------------------------------------------------------------------------------
	float m_selectVecDirY;
	//----------------------------------------------------------------------------------
	/// \brief Select vector direction z-coord
	//----------------------------------------------------------------------------------
	float m_selectVecDirZ;
	//----------------------------------------------------------------------------------
	// Bounds for the View's place in the GUI System
	//----------------------------------------------------------------------------------
	/// \brief Left bounds
	//----------------------------------------------------------------------------------
	int m_boundsLeft;
	//----------------------------------------------------------------------------------
	/// \brief Top bounds
	//----------------------------------------------------------------------------------
	int m_boundsTop;
	//----------------------------------------------------------------------------------
	/// \brief Right bounds
	//----------------------------------------------------------------------------------
	int m_boundsRight;
	//----------------------------------------------------------------------------------
	/// \brief Bottom bounds
	//----------------------------------------------------------------------------------
	int m_boundsBottom;
	//----------------------------------------------------------------------------------
	/// \brief Window width
	//----------------------------------------------------------------------------------
	int m_windowWidth;
	//----------------------------------------------------------------------------------
	/// \brief Window height
	//----------------------------------------------------------------------------------
	int m_windowHeight;
	//----------------------------------------------------------------------------------
	/// \brief Renderer
	//----------------------------------------------------------------------------------
	GLSLRenderer *m_renderer;
	//----------------------------------------------------------------------------------
	/// \brief Our main tree
	//----------------------------------------------------------------------------------
	VolumeTree::Tree *m_mainTree;
	//----------------------------------------------------------------------------------
	/// \brief Cache policy
	//----------------------------------------------------------------------------------
	VolumeTree::CachingPolicy *m_cachePolicy;
	//----------------------------------------------------------------------------------
	/// \brief Our totem controller
	//----------------------------------------------------------------------------------
	Totem::Controller *m_totemController;
	//----------------------------------------------------------------------------------
	/// \brief Calculate step size for positioning objects on pole
	//----------------------------------------------------------------------------------
	void CalcStepsize();
	//----------------------------------------------------------------------------------
	// Overlays
	//----------------------------------------------------------------------------------
	/// \brief Crosshairs with a centre dot
	//----------------------------------------------------------------------------------
	bool m_showCrosshairs;
	//----------------------------------------------------------------------------------
	// Dimensions are proportion of window size
	//----------------------------------------------------------------------------------
	/// \brief Crosshair x-dimension
	//----------------------------------------------------------------------------------
	float m_crosshairX;
	//----------------------------------------------------------------------------------
	/// \brief Crosshair y-dimension
	//----------------------------------------------------------------------------------
	float m_crosshairY;
	//----------------------------------------------------------------------------------
	/// \brief Target size
	//----------------------------------------------------------------------------------
	float m_targetSize;
	//----------------------------------------------------------------------------------
	/// \brief Crosshair vao
	//----------------------------------------------------------------------------------
	GLuint m_crosshairVAO;
	//----------------------------------------------------------------------------------
	/// \brief Crosshair circle vao
	//----------------------------------------------------------------------------------
	GLuint m_crosshairCircleVAO;
	//----------------------------------------------------------------------------------
	/// \brief Shader for drawing crosshair
	//----------------------------------------------------------------------------------
	Utility::GPUProgram* m_crosshairShader;
	//----------------------------------------------------------------------------------
	/// \brief Shader for drawing crosshair circle
	//----------------------------------------------------------------------------------
	Utility::GPUProgram* m_crosshairCircleShader;
	//----------------------------------------------------------------------------------
	/// \brief Reference to command manager
	//----------------------------------------------------------------------------------
	Totem::CommandManager* m_commandManager;
	//----------------------------------------------------------------------------------

};

#endif
