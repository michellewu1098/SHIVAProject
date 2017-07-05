#include "GUI/Views/ColourSelector.h"
#include "GUIManager.h"

//----------------------------------------------------------------------------------

ShivaGUI::ColourSelector::ColourSelector()
{
	m_selectorBoundsLeft = m_selectorBoundsRight = m_selectorBoundsTop = m_selectorBoundsBottom = 0;
	m_sampleBoundsLeft = m_sampleBoundsRight = m_sampleBoundsTop = m_sampleBoundsBottom = 0;

	m_colourSelectorShader = new Utility::GPUProgram();
	m_colourSelectorShader->Create( "Resources/Shaders/ColourSelector", Utility::GPUProgram::VERTEX_AND_FRAGMENT );
	glBindAttribLocation( m_colourSelectorShader->GetProgramID(), 0, "vPosition" );
	glBindAttribLocation( m_colourSelectorShader->GetProgramID(), 1, "vUvs" );
	m_colourSelectorShader->LinkProgram();
	
	//std::cout << "ColourSelector shader created: " << m_colourSelectorShader->GetProgramID() << std::endl;
		
	m_colourSampleShader = new Utility::GPUProgram();
	m_colourSampleShader->Create( "Resources/Shaders/Colour", Utility::GPUProgram::VERTEX_AND_FRAGMENT );
	glBindAttribLocation( m_colourSampleShader->GetProgramID(), 0, "vPosition" );
	glBindAttribLocation( m_colourSampleShader->GetProgramID(), 1, "vUvs" );
	m_colourSampleShader->LinkProgram();

	m_sampleR = 1.0f;
	m_sampleG = 1.0f;
	m_sampleB = 1.0f;

	m_holdingHandle = false;

	m_handleHalfSize = 10.0f;
	m_handlePropPosX = 0.5f;
	m_handlePropPosY = 1.0f;

	m_handleBoundsLeft = m_handleBoundsRight = m_handleBoundsTop = m_handleBoundsBottom = 0.0f;

	m_selectorDrawable = NULL;
	m_selectorStateListDrawable = NULL;

	m_colourListener = NULL;

	HSLtoRGB( m_handlePropPosX, 1.0f, m_handlePropPosY, m_sampleR, m_sampleG, m_sampleB );
	SetHandleActualPos();

	m_projMat.identity();
	m_mvMat.identity();
}

//----------------------------------------------------------------------------------

ShivaGUI::ColourSelector::~ColourSelector()
{
	delete m_colourSelectorShader;
	delete m_colourSampleShader;
}

//----------------------------------------------------------------------------------

void ShivaGUI::ColourSelector::Layout( int _left, int _top, int _right, int _bottom, int _windowWidth, int _windowHeight )
{
	m_selectorBoundsLeft = ( float )_left + m_handleHalfSize;
	m_selectorBoundsRight = ( float )_right - m_handleHalfSize;
	m_selectorBoundsTop = ( float )_top + 30.f;
	m_selectorBoundsBottom = ( float )_bottom - m_handleHalfSize;

	m_sampleBoundsLeft = ( float )_left + m_handleHalfSize;
	m_sampleBoundsRight = ( float )_right - m_handleHalfSize;
	m_sampleBoundsTop = ( float )_top;
	m_sampleBoundsBottom = ( float )_top + 25.f;

	SetHandleActualPos();
}

//----------------------------------------------------------------------------------

void LoadMatricesToShader( GLuint _shaderID, cml::matrix44f_c _proj, cml::matrix44f_c _mv )
{
	// ModelView matrix
	GLint mvLoc = glGetUniformLocation( _shaderID, "u_ModelViewMatrix" );
	if( mvLoc != -1 ) { glUniformMatrix4fv( mvLoc, 1, GL_FALSE, _mv.data() ); }
	else { std::cerr << "ColourSelector: u_ModelViewMatrix not found in shader " << _shaderID << std::endl; }


	// Projection matrix	
	GLint pLoc = glGetUniformLocation( _shaderID, "u_ProjectionMatrix" );
	if( pLoc != -1 ) { glUniformMatrix4fv( pLoc, 1, GL_FALSE, _proj.data() ); }
	else { std::cerr << "ColourSelector: u_ProjectionMatrix not found in shader " << _shaderID << std::endl; }
}

//----------------------------------------------------------------------------------

void ShivaGUI::ColourSelector::BuildVBOs()
{
	float selectorVertices[ 4 * 2 ] = { m_selectorBoundsLeft, m_selectorBoundsBottom,
										m_selectorBoundsRight, m_selectorBoundsBottom,
										m_selectorBoundsRight, m_selectorBoundsTop,
										m_selectorBoundsLeft, m_selectorBoundsTop };

	float selectorUvs[ 4 * 2 ] = {	0.f, 0.f,
									1.f, 0.f,
									1.f, 1.f,
									0.f, 1.f };

	GLuint selectorIndices[ 6 ] = { 0, 1, 2, 0, 2, 3 };

	GLuint selectorVertsVBO, selectorUvsVBO;

	glGenBuffers( 1, &selectorVertsVBO );
	glBindBuffer( GL_ARRAY_BUFFER, selectorVertsVBO );
	glBufferData( GL_ARRAY_BUFFER, 4 * 2 * sizeof( GLfloat ), selectorVertices, GL_STATIC_DRAW );

	glGenBuffers( 1, &selectorUvsVBO );
	glBindBuffer( GL_ARRAY_BUFFER, selectorUvsVBO );
	glBufferData( GL_ARRAY_BUFFER, 4 * 2 * sizeof( GLfloat ), selectorUvs, GL_STATIC_DRAW );

	glGenVertexArrays( 1, &m_selectorVAO );
	glBindVertexArray( m_selectorVAO );

	glEnableVertexAttribArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, selectorVertsVBO );
	glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, ( GLfloat* )NULL );

	glEnableVertexAttribArray( 1 );
	glBindBuffer( GL_ARRAY_BUFFER, selectorUvsVBO );
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, ( GLfloat* )NULL );

	GLuint selectorIndicesVBO;
	glGenBuffers( 1, &selectorIndicesVBO );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, selectorIndicesVBO );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof( GLuint ), selectorIndices, GL_STATIC_DRAW );

	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );


	float sampleVertices[ 4 * 3 ] = {	m_sampleBoundsLeft, m_sampleBoundsBottom, 0.f, 
										m_sampleBoundsRight, m_sampleBoundsBottom, 0.f,
										m_sampleBoundsRight, m_sampleBoundsTop, 0.f, 
										m_sampleBoundsLeft, m_sampleBoundsTop, 0.f };

	GLuint sampleIndices[ 6 ] = { 0, 1, 2, 0, 2, 3 };

	GLuint sampleVertsVBO;

	glGenBuffers( 1, &sampleVertsVBO );
	glBindBuffer( GL_ARRAY_BUFFER, sampleVertsVBO );
	glBufferData( GL_ARRAY_BUFFER, 4 * 3 * sizeof( GLfloat ), sampleVertices, GL_STATIC_DRAW );

	glGenVertexArrays( 1, &m_sampleVAO );
	glBindVertexArray( m_sampleVAO );

	glEnableVertexAttribArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, sampleVertsVBO );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, ( GLfloat* ) NULL );

	GLuint sampleIndicesVBO;
	glGenBuffers( 1, &sampleIndicesVBO );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, sampleIndicesVBO );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof( GLuint ), sampleIndices, GL_STATIC_DRAW );

	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}

//----------------------------------------------------------------------------------

void ShivaGUI::ColourSelector::Draw()
{
	int viewport[4]; // Shouldn't really do this, but temporarily it's fine
	glGetIntegerv( GL_VIEWPORT, viewport );
	cml::matrix_orthographic_RH( m_projMat, 0.f, ( float )viewport[ 2 ], ( float )viewport[ 3 ], 0.f, -1.f, 1.f, cml::z_clip_neg_one );

	BuildVBOs();

	// Draw colour selector	spectrum
	//----------------------------------
	
	m_colourSelectorShader->Bind();

	LoadMatricesToShader( m_colourSelectorShader->GetProgramID(), m_projMat, m_mvMat );

	glBindVertexArray( m_selectorVAO );
		glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );
	glBindVertexArray( 0 );
	
	m_colourSelectorShader->Unbind();

	// Draw colour sample selected
	//----------------------------------

	m_colourSampleShader->Bind();
	LoadMatricesToShader( m_colourSampleShader->GetProgramID(), m_projMat, m_mvMat );

	glUniform4f( glGetUniformLocation( m_colourSampleShader->GetProgramID(), "u_Colour" ), m_sampleR, m_sampleG, m_sampleB, 1.0f );

	glBindVertexArray( m_sampleVAO );
		glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );
	glBindVertexArray( 0 );

	m_colourSampleShader->Unbind();

	// Draw colour selector
	//----------------------------------

	if( m_selectorStateListDrawable != NULL )
		m_selectorStateListDrawable->Draw();
	else if( m_selectorDrawable != NULL )
		m_selectorDrawable->Draw();
}

//----------------------------------------------------------------------------------

void ShivaGUI::ColourSelector::Draw( unsigned int _context )
{
	int viewport[4]; // Shouldn't really do this, but temporarily it's fine
	glGetIntegerv( GL_VIEWPORT, viewport );
	cml::matrix_orthographic_RH( m_projMat, 0.f, ( float )viewport[ 2 ], ( float )viewport[ 3 ], 0.f, -1.f, 1.f, cml::z_clip_neg_one );

	BuildVBOs();

	// Draw colour selector	spectrum
	//----------------------------------
	
	m_colourSelectorShader->Bind();

	LoadMatricesToShader( m_colourSelectorShader->GetProgramID(), m_projMat, m_mvMat );

	glBindVertexArray( m_selectorVAO );
		glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );
	glBindVertexArray( 0 );
	
	m_colourSelectorShader->Unbind();

	// Draw colour sample selected
	//----------------------------------

	m_colourSampleShader->Bind();
	LoadMatricesToShader( m_colourSampleShader->GetProgramID(), m_projMat, m_mvMat );

	glUniform4f( glGetUniformLocation( m_colourSampleShader->GetProgramID(), "u_Colour" ), m_sampleR, m_sampleG, m_sampleB, 1.0f );

	glBindVertexArray( m_sampleVAO );
		glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );
	glBindVertexArray( 0 );

	m_colourSampleShader->Unbind();

	// Draw colour selector
	//----------------------------------

	if( m_selectorStateListDrawable != NULL )
		m_selectorStateListDrawable->Draw();
	else if( m_selectorDrawable != NULL )
		m_selectorDrawable->Draw();
}

//----------------------------------------------------------------------------------

void ShivaGUI::ColourSelector::Inflate( TiXmlElement *_xmlElement, ResourceManager *_resources, std::string _themePrefix, bool _rootNode )
{
	if( _themePrefix.empty() )
		_themePrefix = "ColourSelector_";
	View::Inflate( _xmlElement, _resources, _themePrefix, _rootNode );

	for( TiXmlAttribute *currentAttribute = _xmlElement->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
	{
		if( ( std::string( "selectorDrawable" ) == currentAttribute->Name() ) || ( _themePrefix + "selectorDrawable" == currentAttribute->Name() ) )
		{
			std::string resourceName( currentAttribute->Value() );

			// Retrieve resource and store it as correct type
			Drawable *drawable = _resources->GetDrawable( resourceName );
			m_selectorStateListDrawable = dynamic_cast< StateListDrawable* >( drawable );
			if( m_selectorStateListDrawable == NULL )
				m_selectorDrawable = drawable;
		}
		else if( std::string( "onColourChange" ) == currentAttribute->Name() )
		{
			m_colourListenerName = std::string( currentAttribute->Value() );

			// Retrieve listener
			ViewEventListener *listener = _resources->GetListener( m_colourListenerName );
			if( listener != NULL )
				m_colourListener = listener;
		}
	}
}

//----------------------------------------------------------------------------------

TiXmlElement* ShivaGUI::ColourSelector::Deflate( ResourceManager *_resources )
{
	TiXmlElement *xmlNode = View::Deflate( _resources );
	xmlNode->SetValue( "ColourSelector" );

	if( m_selectorStateListDrawable != NULL )
		xmlNode->SetAttribute( "selectorDrawable", m_selectorStateListDrawable->GetFilename() );
	else if( m_selectorDrawable != NULL )
		xmlNode->SetAttribute( "selectorDrawable", m_selectorDrawable->GetFilename() );

	if( !m_colourListenerName.empty() )
		xmlNode->SetAttribute( "onColourChange", m_colourListenerName );

	return xmlNode;
}

//----------------------------------------------------------------------------------

void ShivaGUI::ColourSelector::SetFocus( bool )
{

}

//----------------------------------------------------------------------------------

void ShivaGUI::ColourSelector::SetSelect( bool )
{

}

//----------------------------------------------------------------------------------

bool ShivaGUI::ColourSelector::HandleEvent( InternalEvent *_currentEvent )
{
	if( m_active )
	{

		if( _currentEvent->GetType() == InternalEvent::POSITIONAL_SELECT )
		{
			int mouseX, mouseY;
			_currentEvent->GetPosition( mouseX, mouseY );
			float posX, posY;
			if( MouseHitSelector( mouseX, mouseY, posX, posY ) )
			{
				m_handlePropPosX = posX;
				m_handlePropPosY = posY;
				HSLtoRGB( posX, 1.0f, posY, m_sampleR, m_sampleG, m_sampleB );
				SetHandleActualPos();
				m_holdingHandle = true;
				return true;
			}
		}
		if( m_holdingHandle && _currentEvent->GetType() == InternalEvent::POSITIONAL_DRAG )
		{
			int mouseX, mouseY;
			_currentEvent->GetPosition( mouseX, mouseY );
			float posX, posY;
			if( MouseHitSelector( mouseX, mouseY, posX, posY ) )
			{
				m_handlePropPosX = posX;
				m_handlePropPosY = posY;
				HSLtoRGB( posX, 1.0f, posY, m_sampleR, m_sampleG, m_sampleB );
				SetHandleActualPos();
				if( m_colourListener != NULL )
					m_colourListener->HandleEvent( this );
				return true;
			}
		}
		if( m_holdingHandle && _currentEvent->GetType() == InternalEvent::POSITIONAL_DESELECT )
		{
			m_holdingHandle = false;
			if( m_colourListener != NULL )
				m_colourListener->HandleEvent( this );
			return true;
		}
	}
	return false;
}

//----------------------------------------------------------------------------------

void ShivaGUI::ColourSelector::SetHandlePosition( float _x, float _y )
{
	m_handlePropPosX = _x;
	m_handlePropPosY = _y;
	HSLtoRGB( _x, 1.0f, _y, m_sampleR, m_sampleG, m_sampleB );
	SetHandleActualPos();
}

//----------------------------------------------------------------------------------

bool ShivaGUI::ColourSelector::MouseHitSelector( int _mouseX, int _mouseY, float &_posX, float &_posY )
{
	if( m_holdingHandle || ( _mouseX > m_selectorBoundsLeft && _mouseX < m_selectorBoundsRight
		&& _mouseY > m_selectorBoundsTop && _mouseY < m_selectorBoundsBottom ) )
	{
		float widgetWidth = m_selectorBoundsRight - m_selectorBoundsLeft;
		float widgetHeight = m_selectorBoundsBottom - m_selectorBoundsTop;
		_posX = ( float )( _mouseX - m_selectorBoundsLeft ) / widgetWidth;
		_posY = ( float )( _mouseY - m_selectorBoundsTop ) / widgetHeight;

		if( _posX < 0.0f )
			_posX = 0.0f;
		else if( _posX > 1.0f )
			_posX = 1.0f;

		if( _posY < 0.0f )
			_posY = 0.0f;
		else if( _posY > 1.0f )
			_posY = 1.0f;

		return true;
	}
	return false;
}

//----------------------------------------------------------------------------------

void ShivaGUI::ColourSelector::HSLtoRGB( float _H, float _S, float _L, float &_R, float &_G, float &_B )
{
	/// This code is kinda from Bob Powell (http://www.bobpowell.net/RGBHSB.htm)
	/// But he got it from Foley and van Dam
	/// See also http://130.113.54.154/~monger/hsl-rgb.html
	/// (it's the exact same thing, variables same names etc)

	float temp2 = ( ( _L <= 0.5f ) ? _L * ( 1.0f + _S ) : _L + _S - ( _L * _S ) );
	float temp1 = 2.0f * _L - temp2;

	float temp3[ 3 ];
	temp3[ 0 ] = _H + 1.0f / 3.0f;
	temp3[ 1 ] = _H;
	temp3[ 2 ] = _H - 1.0f / 3.0f;

	float colour[ 3 ];
	colour[ 0 ] = 0.0f;
	colour[ 1 ] = 0.0f;
	colour[ 2 ] = 0.0f;

	for( int i = 0; i < 3; i++ )
	{
		if( temp3[ i ] < 0 )
			temp3[ i ] += 1.0f;
		else if( temp3[ i ] > 1 )
			temp3[ i ] -= 1.0f;

		if( 6.0f * temp3[ i ] < 1.0f )
			colour[ i ] = temp1 + ( temp2 - temp1 ) * temp3[ i ] * 6.0f;
		else if( 2.0f * temp3[ i ] < 1.0f )
			colour[ i ] = temp2;
		else if( 3.0f * temp3[ i ] < 2.0f )
			colour[ i ] = ( temp1 + ( temp2 - temp1 ) * ( ( 2.0f / 3.0f ) - temp3[ i ] ) * 6.0f );
		else
			colour[ i ] = temp1;
	}

	_R = colour[ 0 ];
	_G = colour[ 1 ];
	_B = colour[ 2 ];
}

//----------------------------------------------------------------------------------

void ShivaGUI::ColourSelector::SetHandleActualPos()
{
	float widgetWidth = m_selectorBoundsRight - m_selectorBoundsLeft;
	float widgetHeight = m_selectorBoundsBottom - m_selectorBoundsTop;
	float posX = ( m_handlePropPosX * widgetWidth) + m_selectorBoundsLeft;
	float posY = ( m_handlePropPosY * widgetHeight) + m_selectorBoundsTop;
	m_handleBoundsLeft = posX - m_handleHalfSize;
	m_handleBoundsRight = posX + m_handleHalfSize;
	m_handleBoundsTop = posY - m_handleHalfSize;
	m_handleBoundsBottom = posY + m_handleHalfSize;


	if( m_selectorStateListDrawable != NULL )
		m_selectorStateListDrawable->SetBounds( m_handleBoundsLeft, m_handleBoundsTop, m_handleBoundsRight, m_handleBoundsBottom );
	else if( m_selectorDrawable != NULL )
		m_selectorDrawable->SetBounds( m_handleBoundsLeft, m_handleBoundsTop, m_handleBoundsRight, m_handleBoundsBottom );
}

//----------------------------------------------------------------------------------

